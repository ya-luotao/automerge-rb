Gem::Specification.new do |spec|
  spec.name = "automerge"
  spec.version = "0.1.0"
  spec.authors = ["Automerge Ruby contributors"]
  spec.email = ["maintainers@example.com"]

  spec.summary = "Ruby bindings for the Automerge CRDT core"
  spec.description = "A production-oriented Ruby SDK for Automerge backed by the upstream Rust/C core."
  spec.homepage = "https://github.com/automerge/automerge"
  spec.license = "MIT"
  spec.required_ruby_version = ">= 2.6"

  spec.files = Dir.chdir(__dir__) do
    files = Dir[
      "README.md",
      "LICENSE.txt",
      "lib/**/*.rb",
      "ext/**/*.{c,h,rb}",
      "test/**/*.rb",
      "vendor/**/*",
    ]
    files.reject do |path|
      path.start_with?("vendor/automerge-rust/target/") ||
        path == "vendor/automerge-rust/target" ||
        path.end_with?(".gem")
    end
  end
  spec.extensions = ["ext/automerge_ext/extconf.rb"]
  spec.require_paths = ["lib"]

  spec.metadata["allowed_push_host"] = "https://rubygems.org"
  spec.metadata["source_code_uri"] = "https://github.com/automerge/automerge"
  spec.metadata["changelog_uri"] = "https://github.com/automerge/automerge/blob/main/rust/CHANGELOG.md"

  spec.add_development_dependency "rake", "~> 13.0"
  spec.add_development_dependency "minitest", "~> 5.16"
  spec.add_development_dependency "rake-compiler", "~> 1.2"
end
