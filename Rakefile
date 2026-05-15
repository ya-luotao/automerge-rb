require "rake/testtask"
require "rake/extensiontask"
require "fileutils"
require "rubygems/package_task"

ROOT = __dir__

GEMSPEC = Gem::Specification.load(File.join(ROOT, "automerge.gemspec"))

CROSS_PLATFORMS = %w[
  arm64-darwin
  x86_64-darwin
  x86_64-linux
  aarch64-linux
  x64-mingw-ucrt
].freeze

CROSS_RUBY_VERSIONS = %w[3.0 3.1 3.2 3.3 3.4].freeze

def automerge_source_dir
  ENV["AUTOMERGE_SOURCE_DIR"] || File.join(ROOT, "vendor", "automerge-rust")
end

def automerge_rust_workspace_dir
  source_dir = automerge_source_dir
  if File.file?(File.join(source_dir, "Cargo.toml"))
    source_dir
  else
    File.join(source_dir, "rust")
  end
end

def automerge_workspace_manifest
  File.join(automerge_rust_workspace_dir, "Cargo.toml")
end

def rust_toolchain_bin
  toolchain = ENV["AUTOMERGE_RB_RUST_TOOLCHAIN"] || "1.89.0"
  Dir.glob(File.expand_path("~/.rustup/toolchains/#{toolchain}*/bin")).first
end

def cargo_env_and_command
  bin_dir = rust_toolchain_bin
  return [{}, "cargo"] unless bin_dir

  [
    {
      "RUSTC" => File.join(bin_dir, "rustc"),
      "PATH" => "#{bin_dir}:#{ENV["PATH"]}",
    },
    File.join(bin_dir, "cargo"),
  ]
end

def upstream_c_cmake_args
  args = ["-DCMAKE_BUILD_TYPE=Debug"]

  if RUBY_PLATFORM.include?("darwin")
    [["/opt/homebrew", "/opt/homebrew"], ["/usr/local", "/usr/local"]].each do |prefix, cmake_prefix|
      next unless File.file?(File.join(prefix, "include", "cmocka.h"))

      args << "-DCMAKE_PREFIX_PATH=#{cmake_prefix}"
      args << "-DCMAKE_C_FLAGS=-I#{File.join(prefix, "include")}"
      args << "-DCMAKE_EXE_LINKER_FLAGS=-L#{File.join(prefix, "lib")}"
      break
    end
  end

  if ENV["AUTOMERGE_RB_UPSTREAM_CMAKE_ARGS"]
    args.concat(ENV["AUTOMERGE_RB_UPSTREAM_CMAKE_ARGS"].split)
  end

  args
end

Rake::ExtensionTask.new("automerge_ext", GEMSPEC) do |ext|
  ext.ext_dir = "ext/automerge_ext"
  ext.lib_dir = "lib/automerge"
  ext.cross_compile = true
  ext.cross_platform = CROSS_PLATFORMS
  ext.cross_compiling do |spec|
    spec.files.reject! { |f| f.start_with?("vendor/", "ext/") }
    spec.extensions = []
    spec.dependencies.reject! { |d| d.name == "rake-compiler-dock" }
  end
end

Rake::TestTask.new(:test => :compile) do |t|
  t.libs << "lib"
  t.libs << "ext/automerge_ext"
  t.pattern = "test/**/*_test.rb"
end

namespace :test do
  desc "Run the vendored upstream Automerge Rust core test suite"
  task :upstream_core do
    abort "missing Automerge Rust workspace: #{automerge_workspace_manifest}" unless File.file?(automerge_workspace_manifest)

    env, cargo = cargo_env_and_command
    sh(
      env,
      cargo,
      "test",
      "--manifest-path", automerge_workspace_manifest,
      "-p", "automerge",
      "--target-dir", ENV["AUTOMERGE_RB_CARGO_TARGET_DIR"] || File.join(ROOT, "tmp", "cargo"),
      "--locked",
      "--ignore-rust-version"
    )
  end

  desc "Run the vendored upstream Automerge C test suite through CMake"
  task :upstream_c do
    workspace_dir = automerge_rust_workspace_dir
    upstream_c_dir = File.join(workspace_dir, "automerge-c")
    abort "missing upstream C suite: #{upstream_c_dir}" unless File.file?(File.join(upstream_c_dir, "CMakeLists.txt"))

    source_dir = File.join(ROOT, "tmp", "upstream-c-src")
    build_dir = File.join(ROOT, "tmp", "upstream-c")
    FileUtils.rm_rf [source_dir, build_dir]
    FileUtils.mkdir_p source_dir
    FileUtils.cp_r File.join(workspace_dir, "."), source_dir

    env, = cargo_env_and_command
    cmake_source_dir = File.join(source_dir, "automerge-c")
    sh(env, "cmake", "-S", cmake_source_dir, "-B", build_dir, *upstream_c_cmake_args)
    sh(env, "cmake", "--build", build_dir, "--target", "automerge_test")
  end
end

desc "Run Ruby API tests plus upstream Rust and C conformance tests"
task :conformance => [:test, "test:upstream_core", "test:upstream_c"]

namespace :gem do
  desc "Build precompiled gems for all cross platforms via rake-compiler-dock"
  task :native do
    require "rake_compiler_dock"
    CROSS_PLATFORMS.each do |platform|
      RakeCompilerDock.sh(
        "bundle install && rake native:#{platform} gem RUBY_CC_VERSION=#{CROSS_RUBY_VERSIONS.join(":")}",
        platform: platform,
      )
    end
  end

  desc "Build a single-ABI native gem for the host platform (testing only)"
  task :host => :compile do
    dlext = RbConfig::CONFIG["DLEXT"]
    abi = RbConfig::CONFIG["ruby_version"][/\d+\.\d+/]
    compiled = "lib/automerge/automerge_ext.#{dlext}"
    raise "rake compile did not produce #{compiled}" unless File.file?(compiled)

    versioned_rel = "lib/automerge/#{abi}/automerge_ext.#{dlext}"
    FileUtils.mkdir_p File.dirname(versioned_rel)
    FileUtils.cp compiled, versioned_rel

    spec = Gem::Specification.load(File.join(ROOT, "automerge.gemspec")).dup
    spec.platform = Gem::Platform.local.to_s.sub(/(darwin)\d+/, '\1')
    spec.files = spec.files.reject { |f| f.start_with?("vendor/", "ext/") } + [versioned_rel]
    spec.extensions = []

    FileUtils.mkdir_p "pkg"
    gem_filename = Gem::Package.build(spec)
    FileUtils.mv gem_filename, File.join("pkg", gem_filename)
    puts "Built pkg/#{gem_filename}"
  end
end

task :clean do
  FileUtils.rm_f Dir["ext/**/*.bundle", "ext/**/*.so", "ext/**/*.o", "ext/**/Makefile", "*.gem"]
  FileUtils.rm_rf Dir["ext/**/*.dSYM"]
  FileUtils.rm_rf ["tmp", "pkg", "vendor/automerge-rust/target"]
  FileUtils.rm_rf Dir["lib/automerge/[0-9]*"]
end

task :default => :test
