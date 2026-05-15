require_relative "lib/automerge/version"

Gem::Specification.new do |spec|
  spec.name = "automerge-rb"
  spec.version = Automerge::VERSION
  spec.authors = ["Automerge Ruby contributors"]
  spec.email = ["maintainers@example.com"]

  spec.summary = "Ruby bindings for the Automerge CRDT core"
  spec.description = "A production-oriented Ruby SDK for Automerge backed by the upstream Rust/C core."
  spec.homepage = "https://github.com/automerge/automerge"
  spec.license = "MIT"
  spec.required_ruby_version = ">= 3.0"
  spec.required_rubygems_version = ">= 3.3.22"

  spec.files = Dir.chdir(__dir__) do
    files = Dir[
      "README.md",
      "LICENSE.txt",
      "lib/**/*.rb",
      "ext/**/*.{c,h,rb}",
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
  spec.metadata["rubygems_mfa_required"] = "true"

  spec.add_development_dependency "rake", "~> 13.0"
  spec.add_development_dependency "minitest", "~> 5.16"
  spec.add_development_dependency "rake-compiler", "~> 1.2"
  spec.add_development_dependency "rake-compiler-dock", "~> 1.5"
end
