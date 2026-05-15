# automerge-rb

Ruby bindings for the upstream Automerge Rust core.

This gem builds a small MRI native extension and statically links the vendored
`automerge-c` core from `vendor/automerge-rust` by default. Set
`AUTOMERGE_SOURCE_DIR` to point at a different Automerge Rust workspace or at a
full Automerge checkout.

## Build

```sh
bundle exec rake compile
```

The Automerge checkout currently requires Rust 1.89 or newer. If your default
toolchain is older, install one with:

```sh
rustup toolchain install 1.89.0
```

The extension builds the Rust core in release mode by default. Use
`AUTOMERGE_RB_DEBUG=1 bundle exec rake compile` for a faster local debug build.

## Test alignment

The default test suite includes Ruby ports of upstream Automerge C/Rust
conformance cases and binary fixture checks:

```sh
bundle exec rake test
```

To also run the vendored upstream Rust core test suite, use:

```sh
bundle exec rake test:upstream_core
```

To run the upstream C API suite, install CMake and cmocka, then use:

```sh
bundle exec rake test:upstream_c
```

For a full local gate, run all three:

```sh
bundle exec rake conformance
```

If multiple Rust toolchains are installed, the Rake tasks prefer
`AUTOMERGE_RB_RUST_TOOLCHAIN` and otherwise use `1.89.0` when present so `cargo`
and `rustc` stay on the same toolchain.

## Usage

```ruby
require "automerge"

doc = Automerge.from("cards" => [])
doc.insert(["cards"], 0, "title" => "Rewrite everything in Ruby", "done" => false)
doc.change(message: "Add card")

bytes = doc.save
loaded = Automerge.load(bytes)
loaded.to_h
```

The Ruby API is path-oriented. A path is an array of map keys and list indexes:

```ruby
doc.put(["cards", 0, "done"], true)
doc.get(["cards", 0, "done"]) #=> true
```

Supported Automerge-specific scalar wrappers:

```ruby
Automerge::Counter.new(1)
Automerge::Timestamp.new(1_735_689_600_000)
Automerge::Bytes.new("\x00\x01".b)
Automerge::Text.new("mutable text")
Automerge::Uint.new(42)
```

Implemented document operations include:

- map/list/text reads and writes through path arrays
- commits, empty changes, rollback, actor IDs, heads, and forks
- save/load, incremental save/load, change lookup, missing deps, and applying changes
- merge conflict reads with `get_all`
- the stateful sync protocol with `SyncState`
- text cursors and text marks

## Historical reads

Every read accepts an optional `heads:` keyword argument naming a vector of
change hashes (typically returned by `Document#heads`). When supplied, the read
resolves against that historical snapshot instead of the current HEAD:

```ruby
doc = Automerge.init
doc.put(["k"], "v1"); doc.commit
h1 = doc.heads
doc.put(["k"], "v2"); doc.commit

doc.get(["k"])               #=> "v2"
doc.get(["k"], heads: h1)    #=> "v1"
doc.keys([], heads: h1)      #=> ["k"]
doc.length(["list"], heads: h1)
doc.cursor_position(["text"], cursor, heads: h1)
doc.marks(["text"], heads: h1)
doc.get_all(["k"], heads: h1)
```

## Change metadata

`Automerge.decode_change(bytes)` unpacks a change-bytes blob (from
`Document#change_by_hash` or `Document#get_changes`) into a hash describing the
change. Useful for audit logs and replication tooling:

```ruby
hash = doc.commit(message: "Add card", timestamp: Time.now.to_i)
meta = Automerge.decode_change(doc.change_by_hash(hash))
meta[:message]    # => "Add card"
meta[:actor_id]   # => "deadbeef"
meta[:timestamp]  # => 1735689600
meta[:seq]        # => 4
meta[:deps]       # => [<change hash>, ...]
meta[:empty]      # => false
```

## Thread safety

`Automerge::Document` instances are not thread-safe. Two threads sharing the
same `Document` must coordinate externally; concurrent mutations or even
concurrent reads against an in-progress writer will trigger undefined behavior
in the underlying Rust core. Sharing across threads is safe only after a
`save`/`load` round-trip or `fork` produces independent documents.
