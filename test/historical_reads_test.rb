# frozen_string_literal: true

require "minitest/autorun"
require "automerge"

class HistoricalReadsTest < Minitest::Test
  def test_get_at_past_heads
    doc = Automerge.init(actor_id: "aaaaaaaa")
    doc.put(["k"], "v1")
    doc.commit
    h1 = doc.heads

    doc.put(["k"], "v2")
    doc.commit
    h2 = doc.heads

    assert_equal "v1", doc.get(["k"], heads: h1)
    assert_equal "v2", doc.get(["k"], heads: h2)
    assert_equal "v2", doc.get(["k"])  # default = HEAD
  end

  def test_get_root_at_past_heads
    doc = Automerge.init
    doc.put(["a"], 1)
    doc.commit
    h1 = doc.heads
    doc.put(["b"], 2)
    doc.commit

    assert_equal({"a" => 1}, doc.get([], heads: h1))
    assert_equal({"a" => 1, "b" => 2}, doc.get([]))
  end

  def test_keys_at_past_heads
    doc = Automerge.init
    doc.put(["a"], 1)
    doc.commit
    h1 = doc.heads
    doc.put(["b"], 2)
    doc.commit

    assert_equal ["a"], doc.keys([], heads: h1)
    assert_equal ["a", "b"], doc.keys.sort
  end

  def test_length_at_past_heads
    doc = Automerge.init
    doc.put(["list"], [])
    doc.commit
    doc.insert(["list"], 0, "a")
    doc.commit
    h1 = doc.heads
    doc.insert(["list"], 1, "b")
    doc.commit

    assert_equal 1, doc.length(["list"], heads: h1)
    assert_equal 2, doc.length(["list"])
  end

  def test_get_all_at_past_heads
    base = Automerge.init(actor_id: "aaaa")
    base.put(["x"], "a")
    base.commit

    other = Automerge.load(base.save)
    other.actor_id = "bbbb"

    base.put(["x"], "from-a")
    base.commit
    other.put(["x"], "from-b")
    other.commit

    h_before_merge = base.heads
    base.merge(other)

    # before merge there's only one value at HEAD-of-base
    assert_equal ["from-a"], base.get_all(["x"], heads: h_before_merge)
    # after merge there are conflicts
    assert_equal ["from-a", "from-b"].sort, base.get_all(["x"]).sort
  end

  def test_marks_at_past_heads
    doc = Automerge.from("text" => Automerge::Text.new("hello"))
    doc.mark(["text"], 0, 5, "bold", true, :none)
    doc.commit
    h_with_mark = doc.heads

    doc.unmark(["text"], 0, 5, "bold", :none)
    doc.commit

    assert_empty doc.marks(["text"])
    assert_equal 1, doc.marks(["text"], heads: h_with_mark).length
  end

  def test_cursor_position_at_past_heads
    doc = Automerge.from("text" => Automerge::Text.new("hello world"))
    cursor = doc.cursor(["text"], 6)
    h1 = doc.heads

    doc.splice_text(["text"], 0, 0, "PREFIX ")
    doc.commit

    assert_equal 6, doc.cursor_position(["text"], cursor, heads: h1)
    assert_equal 13, doc.cursor_position(["text"], cursor)
  end

  def test_heads_param_works_through_nested_paths
    doc = Automerge.init
    doc.put(["users"], [{"name" => "alice", "age" => Automerge::Uint.new(30)}])
    doc.commit
    h1 = doc.heads

    doc.put(["users", 0, "age"], Automerge::Uint.new(31))
    doc.commit

    assert_equal Automerge::Uint.new(30), doc.get(["users", 0, "age"], heads: h1)
    assert_equal Automerge::Uint.new(31), doc.get(["users", 0, "age"])
  end

  def test_heads_with_empty_array_returns_latest
    # Upstream automerge-c convention: empty heads list resolves to HEAD.
    doc = Automerge.init
    doc.put(["k"], 1)
    doc.commit
    doc.put(["k"], 2)
    doc.commit

    snapshot = doc.get([], heads: [])
    assert_equal({"k" => 2}, snapshot)
  end

  def test_heads_kwarg_does_not_break_default_behavior
    doc = Automerge.from("k" => 1)
    assert_equal 1, doc.get(["k"])
    assert_equal 1, doc.get(["k"], heads: nil)
    assert_equal ["k"], doc.keys
  end
end
