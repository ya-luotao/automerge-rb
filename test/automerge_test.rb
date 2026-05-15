# frozen_string_literal: true

require "minitest/autorun"
require "automerge"

class AutomergeTest < Minitest::Test
  def test_nested_documents_save_load_and_merge
    doc1 = Automerge.from("cards" => [])
    doc1.insert(["cards"], 0, "title" => "Rewrite everything in Ruby", "done" => false)
    doc1.change(message: "Add card")

    doc2 = Automerge.load(doc1.save)
    doc1.put(["cards", 0, "done"], true)
    doc1.change(message: "Mark done")

    doc2.delete(["cards", 0])
    doc2.change(message: "Delete card")

    doc1.merge(doc2)
    assert_equal [], doc1.get(["cards"])
    assert_equal doc1.to_h, Automerge.load(doc1.save).to_h
  end

  def test_changes_can_be_requested_and_applied
    source = Automerge.from("key" => "before")
    target = Automerge.load(source.save)
    heads = target.heads

    source.put(["key"], "after")
    source.change(message: "Update key")

    changes = source.get_changes(heads)
    assert_equal 1, changes.length

    target.apply_changes(changes)
    assert_equal "after", target.get(["key"])
    assert_equal source.heads, target.heads
  end

  def test_sync_protocol_round_trips_between_peers
    doc1 = Automerge.from("items" => [])
    doc2 = Automerge.init
    state1 = doc1.sync_state
    state2 = doc2.sync_state

    doc1.insert(["items"], 0, "name" => "alpha")
    doc1.change(message: "Add item")

    10.times do
      message = doc1.generate_sync_message(state1)
      doc2.receive_sync_message(state2, message) if message

      reply = doc2.generate_sync_message(state2)
      doc1.receive_sync_message(state1, reply) if reply
      break if message.nil? && reply.nil?
    end

    assert_equal doc1.to_h, doc2.to_h
    assert_equal [{"name" => "alpha"}], doc2.get(["items"])
  end

  def test_automerge_scalar_types
    doc = Automerge.from(
      "count" => Automerge::Counter.new(10),
      "when" => Automerge::Timestamp.new(1_735_689_600_000),
      "blob" => Automerge::Bytes.new("\x00\x01".b),
      "uint" => Automerge::Uint.new(42),
      "body" => Automerge::Text.new("hello")
    )

    doc.increment(["count"], 5)
    doc.splice_text(["body"], 5, 0, " world")

    assert_equal Automerge::Counter.new(15), doc.get(["count"])
    assert_equal Automerge::Timestamp.new(1_735_689_600_000), doc.get(["when"])
    assert_equal Automerge::Bytes.new("\x00\x01".b), doc.get(["blob"])
    assert_equal Automerge::Uint.new(42), doc.get(["uint"])
    assert_equal Automerge::Text.new("hello world"), doc.get(["body"])
  end

  def test_conflicts_change_lookup_missing_deps_and_incremental_load
    doc1 = Automerge.init(actor_id: "aaaaaaaa")
    doc1.put(["name"], "base")
    doc1.change(message: "Base")

    doc2 = Automerge.load(doc1.save)
    doc2.actor_id = "bbbbbbbb"

    before = doc2.heads
    doc1.put(["name"], "left")
    doc1.change(message: "Left")
    doc2.put(["name"], "right")
    doc2.change(message: "Right")

    changes = doc1.get_changes(before)
    assert_equal doc1.change_by_hash(doc1.heads.first), changes.first

    loaded = Automerge.load(doc2.save)
    assert_equal 1, loaded.load_incremental(doc1.save_incremental)
    assert_equal [], loaded.missing_deps(doc1.heads + doc2.heads)

    loaded.merge(doc1)
    assert_equal ["left", "right"], loaded.get_all(["name"]).sort
  end

  def test_cursors_and_marks_on_text
    doc = Automerge.from("body" => Automerge::Text.new("hello"))
    doc.change(message: "Create text")

    cursor = doc.cursor(["body"], 2)
    assert_equal 2, doc.cursor_position(["body"], cursor)

    doc.mark(["body"], 0, 5, "strong", true, :both)
    assert_equal [{:name => "strong", :start => 0, :end => 5, :value => true}], doc.marks(["body"])

    doc.unmark(["body"], 0, 5, "strong", :both)
    assert_equal [], doc.marks(["body"])
  end
end
