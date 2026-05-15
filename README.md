# automerge-rb

Ruby bindings for the upstream Automerge Rust core.

## Install

```sh
gem install automerge-rb
```

Precompiled native gems are published for the following platforms, so end users
do **not** need Rust or a C toolchain installed:

- `arm64-darwin` (Apple Silicon macOS)
- `x86_64-darwin` (Intel macOS)
- `x86_64-linux` (glibc)
- `aarch64-linux` (glibc)
- `x64-mingw-ucrt` (Windows)

Each native gem ships a fat binary covering Ruby 3.0–3.4. RubyGems auto-selects
the matching artifact for the host's platform and Ruby ABI at install time.

On any other platform `gem install automerge-rb` falls back to building from
source, which requires Rust (1.89+) and a C compiler. The source build also
runs whenever you set `--platform=ruby` explicitly.

## Build from source

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

By default `extconf.rb` skips the Rust step if a prebuilt
`lib/automerge/<ruby-abi>/automerge_ext.<dlext>` already exists (i.e. inside a
native gem); set `AUTOMERGE_SOURCE_DIR` to point at a different Automerge Rust
workspace if you want to recompile from a custom checkout.

## Cross-compiling native gems

Maintainers can build the published binary set with rake-compiler-dock:

```sh
bundle exec rake gem:native
```

This invokes the same docker-backed cross toolchain that the release workflow
uses, producing `pkg/automerge-rb-<version>-<platform>.gem` for every supported
platform.

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
