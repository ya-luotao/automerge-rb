# frozen_string_literal: true

require "minitest/autorun"
require "automerge"

class ChangeMetadataTest < Minitest::Test
  def test_decode_change_extracts_message_actor_timestamp
    doc = Automerge.init(actor_id: "deadbeef")
    doc.put(["k"], "v")
    hash = doc.commit(message: "first change", timestamp: 1_700_000_000)

    meta = Automerge.decode_change(doc.change_by_hash(hash))
    assert_equal hash, meta[:hash]
    assert_equal "first change", meta[:message]
    assert_equal 1_700_000_000, meta[:timestamp]
    assert_equal "deadbeef", meta[:actor_id]
    assert_equal 1, meta[:seq]
    refute meta[:empty]
    assert_empty meta[:deps]
  end

  def test_decode_change_extracts_deps_chain
    doc = Automerge.init(actor_id: "aaaaaaaa")
    doc.put(["a"], 1)
    h1 = doc.commit(message: "a")
    doc.put(["b"], 2)
    h2 = doc.commit(message: "b")

    meta1 = Automerge.decode_change(doc.change_by_hash(h1))
    meta2 = Automerge.decode_change(doc.change_by_hash(h2))
    assert_empty meta1[:deps]
    assert_equal [h1], meta2[:deps]
    assert_equal "b", meta2[:message]
    assert_equal 2, meta2[:seq]
  end

  def test_decode_change_nil_message_when_unset
    doc = Automerge.init
    doc.put(["k"], 1)
    hash = doc.commit
    meta = Automerge.decode_change(doc.change_by_hash(hash))
    assert_nil meta[:message]
  end

  def test_decode_change_recognizes_empty_change
    doc = Automerge.init
    doc.put(["a"], 1)
    doc.commit
    empty_hash = doc.empty_change(message: "ping")

    meta = Automerge.decode_change(doc.change_by_hash(empty_hash))
    assert meta[:empty]
    assert_equal "ping", meta[:message]
  end

  def test_decode_change_rejects_invalid_bytes
    assert_raises(Automerge::Error) { Automerge.decode_change("not bytes") }
    assert_raises(Automerge::Error) { Automerge.decode_change("") }
  end

  def test_decode_change_works_on_get_changes_output
    doc = Automerge.init(actor_id: "ffffffff")
    doc.put(["x"], "y")
    doc.commit(message: "label", timestamp: 42)

    changes = doc.get_changes
    assert_equal 1, changes.length
    meta = Automerge.decode_change(changes[0])
    assert_equal "label", meta[:message]
    assert_equal 42, meta[:timestamp]
    assert_equal "ffffffff", meta[:actor_id]
  end

  def test_decode_change_round_trips_with_load
    doc = Automerge.init(actor_id: "aaaabbbb")
    doc.put(["k"], "v")
    hash = doc.commit(message: "saved", timestamp: 100)
    bytes = doc.change_by_hash(hash)

    loaded = Automerge.load(doc.save)
    loaded_meta = Automerge.decode_change(loaded.change_by_hash(hash))
    original_meta = Automerge.decode_change(bytes)
    assert_equal original_meta, loaded_meta
  end
end
