require "mkmf"
require "fileutils"
require "open3"
require "rbconfig"

ROOT = File.expand_path("../..", __dir__)

ruby_api = RbConfig::CONFIG["ruby_version"][/\d+\.\d+/]
dlext = RbConfig::CONFIG["DLEXT"]
candidate_prebuilt = [
  File.join(ROOT, "lib", "automerge", ruby_api, "automerge_ext.#{dlext}"),
  File.join(ROOT, "lib", "automerge", "automerge_ext.#{dlext}"),
]
prebuilt_artifact = candidate_prebuilt.find { |p| File.file?(p) }

if prebuilt_artifact
  File.write("Makefile", <<~MAKE)
    .PHONY: all clean install
    all:
    clean:
    install:
  MAKE
  warn "automerge: using prebuilt extension at #{prebuilt_artifact}, skipping Rust build"
  return
end

source_dir = ENV["AUTOMERGE_SOURCE_DIR"] || File.join(ROOT, "vendor", "automerge-rust")
workspace_manifest = if File.file?(File.join(source_dir, "Cargo.toml"))
  File.join(source_dir, "Cargo.toml")
else
  File.join(source_dir, "rust", "Cargo.toml")
end
target_dir = ENV["AUTOMERGE_RB_CARGO_TARGET_DIR"] || File.join(ROOT, "tmp", "cargo")

abort "AUTOMERGE_SOURCE_DIR does not point at an Automerge Rust workspace: #{source_dir}" unless File.file?(workspace_manifest)

def run!(*cmd)
  puts cmd.join(" ")
  ok = system(*cmd)
  abort "command failed: #{cmd.join(" ")}" unless ok
end

def command_output(*cmd)
  stdout, _stderr, status = Open3.capture3(*cmd)
  status.success? ? stdout : ""
end

def rustc_minor
  version = command_output("rustc", "--version")
  match = version.match(/rustc\s+(\d+)\.(\d+)/)
  match && [match[1].to_i, match[2].to_i]
end

def have_cargo?
  command_output("cargo", "--version").start_with?("cargo")
end

unless have_cargo?
  abort <<~MSG
    automerge-rb could not find `cargo` on PATH, and no prebuilt extension is
    bundled for this platform / Ruby ABI (#{RUBY_PLATFORM} / #{RbConfig::CONFIG["ruby_version"]}).

    Either:
      * install Rust (https://rustup.rs) and retry, or
      * install a precompiled gem for your platform with:
          gem install automerge-rb --platform=#{Gem::Platform.local}
  MSG
end

toolchain = ENV["AUTOMERGE_RB_RUST_TOOLCHAIN"]
if toolchain.nil?
  minor = rustc_minor
  if minor && (minor[0] < 1 || (minor[0] == 1 && minor[1] < 89))
    toolchain = "1.89.0" if Dir.glob(File.expand_path("~/.rustup/toolchains/1.89.0*")).any?
  end
end

toolchain_dir = nil
if toolchain && !toolchain.empty?
  toolchain_dir = Dir.glob(File.expand_path("~/.rustup/toolchains/#{toolchain}*")).first
end

cargo = if toolchain_dir
  bin_dir = File.join(toolchain_dir, "bin")
  ENV["RUSTC"] = File.join(bin_dir, "rustc")
  ENV["PATH"] = "#{bin_dir}:#{ENV["PATH"]}"
  [File.join(bin_dir, "cargo")]
else
  ["cargo"]
end
profile_dir = ENV["AUTOMERGE_RB_DEBUG"] ? "debug" : "release"
profile_flag = ENV["AUTOMERGE_RB_DEBUG"] ? nil : "--release"

FileUtils.mkdir_p(target_dir)
cmd = cargo + [
  "build",
  "--manifest-path", workspace_manifest,
  "-p", "automerge-c",
  "--target-dir", target_dir,
  "--locked",
  "--ignore-rust-version",
]
cmd << profile_flag if profile_flag
run!(*cmd.compact)

lib = File.join(target_dir, profile_dir, RbConfig::CONFIG["LIBRUBY_SO"] =~ /mswin|mingw/ ? "automerge_core.lib" : "libautomerge_core.a")
abort "could not find built Automerge core library: #{lib}" unless File.file?(lib)

$CFLAGS << " -std=c99 -Wall -Wextra -Wno-missing-field-initializers"
if RbConfig::CONFIG["host_os"] =~ /darwin/
  $LDFLAGS << " -Wl,-force_load,#{lib}"
else
  $LDFLAGS << " -Wl,--whole-archive #{lib} -Wl,--no-whole-archive -lpthread -ldl -lm"
end

create_makefile("automerge_ext")
