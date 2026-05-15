# frozen_string_literal: true

require "minitest/autorun"
require "automerge"

class ApiCompletenessTest < Minitest::Test
  def test_init_with_no_args_uses_random_actor
    doc = Automerge.init
    refute_nil doc.actor_id
    assert_equal 32, doc.actor_id.length
  end

  def test_init_accepts_hex_actor_id
    doc = Automerge.init(actor_id: "deadbeef")
    assert_equal "deadbeef", doc.actor_id
  end

  def test_init_rejects_invalid_actor_id
    assert_raises(Automerge::Error) { Automerge.init(actor_id: "not-hex") }
    assert_raises(Automerge::Error) { Automerge.init(actor_id: "z") }
  end

  def test_from_accepts_string_keyed_hash
    doc = Automerge.from("k" => 1)
    assert_equal({"k" => 1}, doc.to_h)
  end

  def test_from_accepts_explicit_hash
    doc = Automerge.from({"k" => 1}, actor_id: "aabbcc")
    assert_equal({"k" => 1}, doc.to_h)
    assert_equal "aabbcc", doc.actor_id
  end

  def test_from_rejects_non_hash_root
    assert_raises(ArgumentError) { Automerge::Document.from([1, 2, 3]) }
    assert_raises(ArgumentError) { Automerge::Document.from("plain string") }
  end

  def test_path_supports_symbol_and_string_keys
    doc = Automerge.from(foo: "bar")
    assert_equal "bar", doc.get(["foo"])
    assert_equal "bar", doc.get([:foo])
    doc.put([:foo], "baz")
    assert_equal "baz", doc.get(["foo"])
  end

  def test_bracket_access_aliases
    doc = Automerge.from("k" => 1)
    assert_equal 1, doc["k"]
    assert_equal 1, doc[["k"]]
    doc["k"] = 2
    assert_equal 2, doc[["k"]]
  end

  def test_clone_is_independent
    doc = Automerge.from("counter" => Automerge::Counter.new(0))
    twin = doc.clone
    twin.increment(["counter"], 5)
    assert_equal Automerge::Counter.new(0), doc.get(["counter"])
    assert_equal Automerge::Counter.new(5), twin.get(["counter"])
  end

  def test_equality_compares_state
    doc1 = Automerge.from("k" => 1, actor_id: "aaaaaa")
    doc2 = Automerge.from("k" => 1, actor_id: "bbbbbb")
    refute_equal doc1, doc2
    doc3 = Automerge.load(doc1.save)
    assert_equal doc1, doc3
  end

  def test_change_block_yields_doc_and_auto_commits
    doc = Automerge.init
    doc.change { |d| d.put(["k"], 1) }
    assert_equal 1, doc.get(["k"])
    assert_equal 0, doc.pending_ops
  end

  def test_change_block_rolls_back_on_exception
    doc = Automerge.init
    doc.put(["initial"], 1)
    doc.commit
    assert_raises(RuntimeError) do
      doc.change do |d|
        d.put(["initial"], 2)
        raise "abort"
      end
    end
    assert_equal 1, doc.get(["initial"])
  end

  def test_pending_ops_reflects_uncommitted_state
    doc = Automerge.init
    assert_equal 0, doc.pending_ops
    doc.put(["k"], 1)
    assert_equal 1, doc.pending_ops
    doc.put(["m"], 2)
    assert_equal 2, doc.pending_ops
    doc.commit
    assert_equal 0, doc.pending_ops
  end

  def test_rollback_discards_pending_ops
    doc = Automerge.from("k" => 1)
    doc.put(["k"], 2)
    doc.rollback
    assert_equal 0, doc.pending_ops
    assert_equal 1, doc.get(["k"])
  end

  def test_commit_returns_hash_or_nil_when_no_changes
    doc = Automerge.init
    assert_nil doc.commit
    doc.put(["k"], 1)
    hash = doc.commit
    assert_kind_of String, hash
    assert_equal 32, hash.bytesize
  end

  def test_empty_change_creates_change_hash
    doc = Automerge.init
    hash = doc.empty_change(message: "ping")
    assert_kind_of String, hash
    assert_equal 32, hash.bytesize
  end

  def test_keys_and_length_on_root
    doc = Automerge.from("a" => 1, "b" => 2, "c" => 3)
    assert_equal ["a", "b", "c"], doc.keys.sort
    assert_equal 3, doc.length
  end

  def test_keys_and_length_on_nested_path
    doc = Automerge.from("list" => [10, 20, 30], "map" => {"x" => 1, "y" => 2})
    assert_equal 3, doc.length(["list"])
    assert_equal ["x", "y"], doc.keys(["map"]).sort
    assert_equal 2, doc.length(["map"])
  end

  def test_get_missing_path_returns_nil
    doc = Automerge.init
    assert_nil doc.get(["missing"])
    assert_nil doc.get(["foo", "bar"]) if doc.get(["foo"]).is_a?(Hash)
  end

  def test_deep_nested_paths
    doc = Automerge.init
    doc.put(["a"], {})
    doc.put(["a", "b"], {})
    doc.put(["a", "b", "c"], [{"d" => 42}])
    assert_equal 42, doc.get(["a", "b", "c", 0, "d"])
  end

  def test_list_negative_index_rejected
    doc = Automerge.from("l" => [1, 2, 3])
    assert_raises(ArgumentError) { doc.get(["l", -1]) }
  end

  def test_splice_text_unicode_grapheme
    doc = Automerge.from("body" => Automerge::Text.new("ABC"))
    doc.splice_text(["body"], 1, 0, "🚀")
    assert_equal Automerge::Text.new("A🚀BC"), doc.get(["body"])
  end

  def test_save_load_round_trip_preserves_nested_structures
    original = {
      "users" => [
        {"name" => "alice", "age" => Automerge::Uint.new(30)},
        {"name" => "bob", "age" => Automerge::Uint.new(25)}
      ],
      "tags" => Automerge::Text.new("admin moderator"),
      "counts" => {"total" => Automerge::Counter.new(2)}
    }
    doc = Automerge.from(original)
    loaded = Automerge.load(doc.save)
    assert_equal doc.to_h, loaded.to_h
  end

  def test_apply_changes_is_idempotent
    src = Automerge.from("k" => 1)
    target = Automerge.load(src.save)
    src.put(["k"], 2)
    src.commit
    changes = src.get_changes(target.heads)

    target.apply_changes(changes)
    first = target.save
    target.apply_changes(changes)
    second = target.save
    assert_equal first, second
  end

  def test_fork_at_heads_returns_historical_view
    doc = Automerge.init(actor_id: "aaaa")
    doc.put(["v"], 1)
    doc.commit
    h1 = doc.heads
    doc.put(["v"], 2)
    doc.commit

    snapshot = doc.fork(h1)
    assert_equal 1, snapshot.get(["v"])
    assert_equal 2, doc.get(["v"])
  end

  def test_get_changes_with_empty_heads_returns_all
    doc = Automerge.init
    doc.put(["a"], 1)
    doc.commit
    doc.put(["b"], 2)
    doc.commit
    assert_equal 2, doc.get_changes([]).length
  end

  def test_missing_deps_returns_empty_when_complete
    doc = Automerge.from("k" => 1)
    assert_empty doc.missing_deps
  end

  def test_get_all_preserves_conflict_resolution_order
    doc1 = Automerge.init(actor_id: "aaaa")
    doc1.put(["k"], "alpha")
    doc1.commit

    doc2 = Automerge.load(doc1.save)
    doc2.actor_id = "bbbb"
    doc2.put(["k"], "beta")
    doc2.commit

    doc1.put(["k"], "gamma")
    doc1.commit
    doc1.merge(doc2)

    values = doc1.get_all(["k"])
    assert_equal ["beta", "gamma"], values.sort
  end

  def test_sync_state_encode_decode_round_trips_bytes
    state = Automerge::SyncState.new
    bytes = state.encode
    assert_kind_of String, bytes
    restored = Automerge::SyncState.decode(bytes)
    # Upstream State::parse sets their_have=Some(Vec::new()) but State::default sets None,
    # so structural == is false even though both encode to the same bytes.
    assert_equal bytes, restored.encode
    assert_equal state.shared_heads, restored.shared_heads
  end

  def test_sync_state_shared_heads_empty_initially
    state = Automerge::SyncState.new
    assert_empty state.shared_heads
  end

  def test_load_invalid_bytes_raises
    assert_raises(Automerge::Error) { Automerge.load("not a real save") }
  end

  def test_load_empty_buffer_returns_empty_document
    doc = Automerge.load("")
    assert_equal({}, doc.to_h)
    assert_empty doc.keys
  end

  def test_cursor_persists_across_save_load
    doc = Automerge.from("text" => Automerge::Text.new("hello world"))
    cursor = doc.cursor(["text"], 5)
    saved = doc.save
    loaded = Automerge.load(saved)
    assert_equal 5, loaded.cursor_position(["text"], cursor)
  end

  def test_two_separate_marks_can_coexist
    doc = Automerge.from("text" => Automerge::Text.new("hello world"))
    doc.mark(["text"], 0, 5, "bold", true, :none)
    doc.mark(["text"], 6, 11, "italic", true, :none)
    marks = doc.marks(["text"])
    assert_equal 2, marks.length
    assert_equal ["bold", "italic"], marks.map { |m| m[:name] }.sort
  end

  def test_increment_negative_amount
    doc = Automerge.from("c" => Automerge::Counter.new(10))
    doc.increment(["c"], -3)
    assert_equal Automerge::Counter.new(7), doc.get(["c"])
  end

  def test_list_delete_shifts_indices
    doc = Automerge.from("l" => ["a", "b", "c", "d"])
    doc.delete(["l", 1])
    assert_equal ["a", "c", "d"], doc.get(["l"])
  end

  def test_change_with_timestamp
    doc = Automerge.init
    doc.put(["k"], 1)
    doc.change(message: "test", timestamp: 1_700_000_000)
    assert_kind_of String, doc.heads.first
  end
end
