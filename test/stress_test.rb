# frozen_string_literal: true

require "minitest/autorun"
require "automerge"

class StressTest < Minitest::Test
  def test_repeated_fork_and_merge
    doc = Automerge.init(actor_id: "aaaaaaaa")
    doc.put(["counter"], Automerge::Counter.new(0))
    doc.commit

    50.times do |i|
      fork = doc.fork
      fork.actor_id = i.to_s.rjust(8, "f")[0, 8]
      fork.increment(["counter"], 1)
      fork.commit
      doc.merge(fork)
    end

    assert_equal Automerge::Counter.new(50), doc.get(["counter"])
  end

  def test_large_list_operations
    doc = Automerge.init
    doc.put(["items"], [])
    doc.commit

    500.times do |i|
      doc.insert(["items"], i, "item-#{i}")
    end
    doc.commit

    assert_equal 500, doc.length(["items"])
    assert_equal "item-0", doc.get(["items", 0])
    assert_equal "item-499", doc.get(["items", 499])

    # Round trip preserves all items
    loaded = Automerge.load(doc.save)
    assert_equal 500, loaded.length(["items"])
    assert_equal "item-250", loaded.get(["items", 250])
  end

  def test_large_text_splice
    doc = Automerge.from("text" => Automerge::Text.new(""))
    chunk = "lorem ipsum dolor sit amet, "
    100.times { |i| doc.splice_text(["text"], i * chunk.length, 0, chunk) }
    doc.commit

    text = doc.get(["text"]).value
    assert_equal chunk.length * 100, text.length
    assert_equal chunk + chunk, text[0, chunk.length * 2]
  end

  def test_deeply_nested_structure
    doc = Automerge.init
    current_path = []
    20.times do |i|
      key = "level_#{i}"
      doc.put(current_path + [key], {})
      current_path << key
    end
    doc.put(current_path + ["leaf"], "found")
    doc.commit

    full_path = (0...20).map { |i| "level_#{i}" } + ["leaf"]
    assert_equal "found", doc.get(full_path)

    loaded = Automerge.load(doc.save)
    assert_equal "found", loaded.get(full_path)
  end

  def test_many_actors_converge
    actors = (0...10).map { |i| "%016x" % i }
    docs = actors.map { |id| Automerge.init(actor_id: id) }

    # Each actor adds a key
    docs.each_with_index do |doc, i|
      doc.put(["key_#{i}"], i)
      doc.commit
    end

    # Merge all into doc 0
    docs.each_with_index do |doc, i|
      next if i == 0
      docs[0].merge(doc)
    end

    # All keys present
    (0...10).each do |i|
      assert_equal i, docs[0].get(["key_#{i}"]), "missing key_#{i}"
    end

    # Merging in reverse direction yields same final state via save round-trip
    docs[1..].each do |doc|
      doc.merge(docs[0])
      assert_equal docs[0].to_h, doc.to_h
    end
  end

  def test_sync_protocol_with_large_history
    src = Automerge.init(actor_id: "aaaaaaaa")
    100.times do |i|
      src.put(["log_#{i}"], "entry-#{i}")
      src.commit
    end

    target = Automerge.init(actor_id: "bbbbbbbb")
    state_src = Automerge::SyncState.new
    state_tgt = Automerge::SyncState.new

    iterations = 0
    loop do
      iterations += 1
      raise "sync did not converge in 20 iterations" if iterations > 20

      message = src.generate_sync_message(state_src)
      target.receive_sync_message(state_tgt, message) if message

      reply = target.generate_sync_message(state_tgt)
      src.receive_sync_message(state_src, reply) if reply

      break if message.nil? && reply.nil?
    end

    assert_equal src.to_h, target.to_h
    assert_equal 100, target.keys.length
  end

  def test_save_incremental_aligns_with_full_save
    doc = Automerge.init(actor_id: "aaaaaaaa")
    50.times do |i|
      doc.put(["k_#{i}"], i)
      doc.commit
    end

    # save_incremental after partial work
    snapshot = doc.save
    doc.put(["new_key"], "new_value")
    doc.commit

    incremental = doc.save_incremental
    rebuilt = Automerge.load(snapshot)
    rebuilt.load_incremental(incremental)
    assert_equal doc.to_h, rebuilt.to_h
  end

  def test_conflicting_concurrent_edits_converge
    base = Automerge.init(actor_id: "00000000")
    base.put(["counter"], Automerge::Counter.new(0))
    base.commit

    forks = (0...5).map do |i|
      fork = base.clone
      fork.actor_id = "%08x" % (i + 1)
      fork.increment(["counter"], i + 1)
      fork.commit
      fork
    end

    result = base
    forks.each { |f| result.merge(f) }

    expected_sum = (1..5).sum
    assert_equal Automerge::Counter.new(expected_sum), result.get(["counter"])
  end

  def test_round_trip_with_unicode
    text = "你好世界 🚀 mañana café"
    doc = Automerge.from("greeting" => text, "body" => Automerge::Text.new(text))
    loaded = Automerge.load(doc.save)
    assert_equal text, loaded.get(["greeting"])
    assert_equal Automerge::Text.new(text), loaded.get(["body"])
  end

  def test_repeated_save_load_is_stable
    doc = Automerge.from("k" => 1, "list" => [1, 2, 3], "map" => {"a" => "b"})
    bytes1 = doc.save
    10.times { |_| bytes1 = Automerge.load(bytes1).save }
    bytes2 = Automerge.load(bytes1).save
    assert_equal bytes1, bytes2
  end

  def test_get_changes_pagination
    doc = Automerge.init
    doc.put(["a"], 1)
    h1 = doc.commit
    doc.put(["b"], 2)
    h2 = doc.commit
    doc.put(["c"], 3)
    h3 = doc.commit

    assert_equal 3, doc.get_changes.length
    assert_equal 2, doc.get_changes([h1]).length
    assert_equal 1, doc.get_changes([h2]).length
    assert_equal 0, doc.get_changes([h3]).length
  end

  def test_apply_changes_resolves_dependencies
    src = Automerge.init(actor_id: "aaaaaaaa")
    src.put(["a"], 1)
    src.commit
    src.put(["b"], 2)
    src.commit
    src.put(["c"], 3)
    src.commit

    # Apply in correct order on a fresh target
    target = Automerge.init
    changes = src.get_changes
    target.apply_changes(changes)

    assert_equal({"a" => 1, "b" => 2, "c" => 3}, target.to_h)
  end
end
