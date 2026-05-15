# frozen_string_literal: true

require "minitest/autorun"
require "automerge"

class UpstreamConformanceTest < Minitest::Test
  ROOT = File.expand_path("..", __dir__)
  UPSTREAM = File.join(ROOT, "vendor", "automerge-rust")

  UPSTREAM_TEST_SOURCES = [
    "automerge-c/test/ported_wasm/basic_tests.c",
    "automerge-c/test/ported_wasm/cursor_tests.c",
    "automerge-c/test/ported_wasm/sync_tests.c",
    "automerge-c/test/doc_tests.c",
    "automerge-c/test/map_tests.c",
    "automerge-c/test/list_tests.c",
    "automerge-c/test/mark_tests.c",
    "automerge/tests/test.rs",
    "automerge/tests/text_encoding.rs",
    "automerge/tests/test_save_load_orphans.rs"
  ].freeze

  def test_upstream_conformance_sources_are_vendored
    missing = UPSTREAM_TEST_SOURCES.reject { |path| File.file?(upstream(path)) }
    assert_empty missing
  end

  def test_ported_wasm_create_clone_empty_change_and_missing_get
    doc = Automerge.init(actor_id: "aabbcc")
    clone = doc.clone

    assert_equal doc, clone
    assert_nil doc.get(["missing"])
    assert_kind_of String, doc.empty_change
    assert_equal 0, doc.pending_ops
    assert_equal doc.to_h, Automerge.load(doc.save).to_h
  end

  def test_ported_wasm_simple_scalars_subobjects_and_lists
    doc = Automerge.init(actor_id: "aabbcc")
    doc.put(["hello"], "world")
    doc.put(["number1"], Automerge::Uint.new(5))
    doc.put(["number2"], 5)
    doc.put(["number3"], 5.5)
    doc.put(["bool"], true)
    doc.put(["time1"], Automerge::Timestamp.new(1000))
    doc.put(["bytes"], Automerge::Bytes.new([10, 11, 12].pack("C*")))
    doc.put(["submap"], {"number" => Automerge::Uint.new(6)})
    doc.put(["numbers"], [])
    doc.insert(["numbers"], 0, "a")
    doc.insert(["numbers"], 1, "b")
    doc.insert(["numbers"], 2, "c")
    doc.insert(["numbers"], 0, "z")
    doc.put(["numbers", 2], "b v2")
    doc.put(["null"], nil)

    assert_equal "world", doc.get(["hello"])
    assert_equal Automerge::Uint.new(5), doc.get(["number1"])
    assert_equal 5, doc.get(["number2"])
    assert_equal 5.5, doc.get(["number3"])
    assert_equal true, doc.get(["bool"])
    assert_equal Automerge::Timestamp.new(1000), doc.get(["time1"])
    assert_equal Automerge::Bytes.new([10, 11, 12].pack("C*")), doc.get(["bytes"])
    assert_equal({"number" => Automerge::Uint.new(6)}, doc.get(["submap"]))
    assert_equal ["z", "a", "b v2", "c"], doc.get(["numbers"])
    assert_nil doc.get(["null"])
  end

  def test_ported_wasm_list_insert_set_splice_and_timestamp
    doc = Automerge.from("letters" => [])
    doc.insert(["letters"], 0, "a")
    doc.insert(["letters"], 0, "b")

    assert_equal ["b", "a"], doc.get(["letters"])

    doc.insert(["letters"], doc.length(["letters"]), "c")
    heads = doc.heads
    doc.insert(["letters"], doc.length(["letters"]), Automerge::Timestamp.new(3))
    doc.splice(["letters"], 1, 1, ["d", "e", "f"])
    doc.put(["letters", 0], "z")

    assert_equal ["z", "d", "e", "f", "c", Automerge::Timestamp.new(3)], doc.get(["letters"])
    assert_equal 6, doc.length(["letters"])
    assert_equal({"letters" => ["b", "a", "c"]}, doc.fork(heads).to_h)
  end

  def test_ported_wasm_delete_and_counter_semantics
    doc = Automerge.init
    doc.put(["foo"], "bar")
    doc.put(["bip"], "bap")
    doc.change

    assert_equal ["bip", "foo"], doc.keys
    doc.delete(["foo"])
    doc.delete(["baz"])
    doc.change

    assert_equal ["bip"], doc.keys
    assert_nil doc.get(["foo"])

    doc.put(["counter"], Automerge::Counter.new(10))
    doc.increment(["counter"], 10)
    doc.increment(["counter"], -5)

    assert_equal Automerge::Counter.new(15), doc.get(["counter"])
  end

  def test_ported_wasm_save_all_or_incrementally
    doc = Automerge.init
    doc.put(["foo"], 1)
    save1 = doc.save

    doc.put(["bar"], 2)
    save_midway = doc.clone.save
    save2 = doc.save_incremental

    doc.put(["baz"], 3)
    save3 = doc.save_incremental

    save_all = doc.save
    save_chunks = save1 + save2 + save3

    refute_equal save_all, save_chunks

    doc_a = Automerge.load(save_all)
    doc_b = Automerge.load(save_chunks)
    doc_c = Automerge.load(save_midway)
    assert_equal 1, doc_c.load_incremental(save3)

    assert_equal doc_a.to_h, doc_b.to_h
    assert_equal doc_a.to_h, doc_c.to_h
    assert_equal doc_a.save, doc_b.save
    assert_equal doc_a.save, doc_c.save
  end

  def test_ported_wasm_local_increment_updates_visible_map_counters
    doc1 = Automerge.init(actor_id: "aaaa")
    doc1.put(["hello"], "world")
    doc2 = Automerge.load(doc1.save)
    doc2.actor_id = "bbbb"
    doc3 = Automerge.load(doc1.save)
    doc3.actor_id = "cccc"
    heads = doc1.heads

    doc1.put(["cnt"], 20)
    doc2.put(["cnt"], Automerge::Counter.new(0))
    doc3.put(["cnt"], Automerge::Counter.new(10))
    doc1.apply_changes(doc2.get_changes(heads))
    doc1.apply_changes(doc3.get_changes(heads))

    assert_equal [[Automerge::Counter, 0], [Automerge::Counter, 10], [Integer, 20]],
                 sorted_value_signatures(doc1.get_all(["cnt"]))

    doc1.increment(["cnt"], 5)

    assert_equal [[Automerge::Counter, 5], [Automerge::Counter, 15]],
                 sorted_value_signatures(doc1.get_all(["cnt"]))
    assert_equal doc1.save, Automerge.load(doc1.save).save
  end

  def test_ported_wasm_local_increment_updates_visible_sequence_counters
    doc1 = Automerge.init(actor_id: "aaaa")
    doc1.put(["seq"], ["hello"])
    doc2 = Automerge.load(doc1.save)
    doc2.actor_id = "bbbb"
    doc3 = Automerge.load(doc1.save)
    doc3.actor_id = "cccc"
    heads = doc1.heads

    doc1.put(["seq", 0], 20)
    doc2.put(["seq", 0], Automerge::Counter.new(0))
    doc3.put(["seq", 0], Automerge::Counter.new(10))
    doc1.apply_changes(doc2.get_changes(heads))
    doc1.apply_changes(doc3.get_changes(heads))

    assert_equal [[Automerge::Counter, 0], [Automerge::Counter, 10], [Integer, 20]],
                 sorted_value_signatures(doc1.get_all(["seq", 0]))

    doc1.increment(["seq", 0], 5)

    assert_equal [[Automerge::Counter, 5], [Automerge::Counter, 15]],
                 sorted_value_signatures(doc1.get_all(["seq", 0]))
    assert_equal doc1.save, Automerge.load(doc1.save).save
  end

  def test_ported_wasm_fetch_changes_by_hash
    doc1 = Automerge.init(actor_id: "aaaa")
    doc2 = Automerge.init(actor_id: "bbbb")
    doc1.put(["a"], "b")
    doc2.put(["b"], "c")

    head1 = doc1.heads.first
    head2 = doc2.heads.first

    assert_equal doc1.get_changes.first, doc1.change_by_hash(head1)
    assert_raises(Automerge::Error) { doc1.change_by_hash(head2) }
  end

  def test_ported_wasm_recursive_sets_and_empty_objects_are_preserved
    doc = Automerge.init(actor_id: "aaaa")
    doc.put(["list"], [{"zip" => ["a", "b"]}, {"foo" => "bar"}, [1, 2, 3]])
    doc.put(["info1"], Automerge::Text.new("hello world"))
    doc.put(["info2"], "hello world")
    doc.put(["info3"], Automerge::Text.new("hello world"))
    doc.put(["a"], {})
    doc.put(["b"], {})
    doc.put(["c"], {"d" => "dd"})

    loaded = Automerge.load(doc.save)

    assert_equal [{"zip" => ["a", "b"]}, {"foo" => "bar"}, [1, 2, 3]], loaded.get(["list"])
    assert_equal Automerge::Text.new("hello world"), loaded.get(["info1"])
    assert_equal "hello world", loaded.get(["info2"])
    assert_equal Automerge::Text.new("hello world"), loaded.get(["info3"])
    assert_equal({}, loaded.get(["a"]))
    assert_equal({}, loaded.get(["b"]))
    assert_equal({"d" => "dd"}, loaded.get(["c"]))
  end

  def test_ported_wasm_fork_at_heads
    doc = Automerge.init(actor_id: "aaaaaa")
    doc.put(["key1"], "val1")
    doc.put(["key2"], "val2")
    heads1 = doc.heads

    fork = doc.fork
    fork.actor_id = "bbbbbb"
    doc.put(["key3"], "val3")
    fork.put(["key4"], "val4")
    doc.merge(fork)
    heads2 = doc.heads

    doc.put(["key5"], "val5")

    assert_equal({"key1" => "val1", "key2" => "val2"}, doc.fork(heads1).to_h)
    assert_equal({"key1" => "val1", "key2" => "val2", "key3" => "val3", "key4" => "val4"},
                 doc.fork(heads2).to_h)
  end

  def test_ported_wasm_merging_text_conflicts_then_saving_and_loading
    doc_a = Automerge.init(actor_id: "aabbcc")
    doc_a.put(["text"], Automerge::Text.new("hello"))
    doc_b = doc_a.fork

    assert_equal doc_a.get(["text"]), doc_b.get(["text"])

    doc_b.splice_text(["text"], 4, 1, "")
    doc_b.splice_text(["text"], 4, 0, "!")
    doc_b.splice_text(["text"], 5, 0, " ")
    doc_b.splice_text(["text"], 6, 0, "world")

    doc_a.merge(doc_b)
    loaded = Automerge.load(doc_a.save)

    assert_equal Automerge::Text.new("hell! world"), loaded.get(["text"])
  end

  def test_ported_wasm_cursor_tracks_position_across_text_edits
    doc = Automerge.from("text" => Automerge::Text.new("the sly fox jumped over the lazy dog"))
    cursor = doc.cursor(["text"], 12)

    assert_equal 12, doc.cursor_position(["text"], cursor)

    doc.splice_text(["text"], 0, 3, "Has the")

    assert_equal Automerge::Text.new("Has the sly fox jumped over the lazy dog"), doc.get(["text"])
    assert_equal 16, doc.cursor_position(["text"], cursor)
    assert_equal 0, doc.cursor_position(["text"], doc.cursor(["text"], 0))
  end

  def test_c_mark_round_trip_and_unicode_indexing
    doc = Automerge.from("text" => Automerge::Text.new("hello world"))
    doc.mark(["text"], 0, 5, "bold", true, :both)
    doc.splice_text(["text"], 5, 0, "cool ")
    doc.unmark(["text"], 1, 6, "bold", :both)

    assert_equal [
      {:name => "bold", :start => 0, :end => 1, :value => true},
      {:name => "bold", :start => 6, :end => 10, :value => true}
    ], doc.marks(["text"])

    unicode = Automerge.from("text" => Automerge::Text.new("🇬🇧🇩🇪"))
    unicode_mark = begin
      unicode.mark(["text"], 8, 16, "bold", true, :none)
      unicode.splice_text(["text"], 0, 8, "hi ")
      {:name => "bold", :start => 3, :end => 11, :value => true}
    rescue Automerge::Error
      unicode = Automerge.from("text" => Automerge::Text.new("🇬🇧🇩🇪"))
      unicode.mark(["text"], 2, 4, "bold", true, :none)
      unicode.splice_text(["text"], 0, 2, "hi ")
      {:name => "bold", :start => 3, :end => 5, :value => true}
    end

    assert_equal Automerge::Text.new("hi 🇩🇪"), unicode.get(["text"])
    assert_equal [unicode_mark], unicode.marks(["text"])
  end

  def test_c_doc_loads_brave_ape_fixture
    doc = Automerge.load(File.binread(upstream("automerge-c/test/files/brave-ape-49.automerge")))

    assert_equal ["assets", "data", "dom", "meta"], doc.keys.sort
  end

  def test_rust_storage_fixtures
    assert_raises(Automerge::Error) { load_rust_fixture("counter_value_has_incorrect_meta.automerge") }
    assert_raises(Automerge::Error) { load_rust_fixture("counter_value_is_overlong.automerge") }
    assert_kind_of Automerge::Document, load_rust_fixture("counter_value_is_ok.automerge")

    %w[
      two_change_chunks.automerge
      two_change_chunks_compressed.automerge
      two_change_chunks_out_of_order.automerge
    ].each do |name|
      doc = load_rust_fixture(name)
      assert_equal({"a" => {"a" => "b"}}, doc.to_h)
    end

    %w[
      64bit_obj_id_change.automerge
      64bit_obj_id_doc.automerge
    ].each do |name|
      assert_loads_or_rejects_64bit_obj_id_fixture(name)
    end
  end

  def test_rust_fuzz_crashers_are_rejected
    Dir.children(upstream("automerge/tests/fuzz-crashers")).sort.each do |name|
      assert_raises(Automerge::Error, "expected #{name} to be rejected") do
        Automerge.load(File.binread(upstream("automerge/tests/fuzz-crashers", name)))
      end
    end
  end

  private

  def upstream(*segments)
    File.join(UPSTREAM, *segments)
  end

  def load_rust_fixture(name)
    Automerge.load(File.binread(upstream("automerge/tests/fixtures", name)))
  end

  def assert_loads_or_rejects_64bit_obj_id_fixture(name)
    doc = load_rust_fixture(name)
    assert_kind_of Hash, doc.get(["a"])
  rescue Automerge::Error
    pass
  end

  def sorted_value_signatures(values)
    values.map { |value| [value.class, value.respond_to?(:value) ? value.value : value] }
          .sort_by { |klass, value| [klass.name, value] }
  end
end
