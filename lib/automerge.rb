# frozen_string_literal: true

require_relative "automerge/version"

module Automerge
  class Error < StandardError; end

  class Scalar
    attr_reader :value

    def initialize(value)
      @value = value
    end

    def ==(other)
      other.class == self.class && other.value == value
    end
  end

  class Counter < Scalar; end
  class Timestamp < Scalar; end
  class Uint < Scalar; end

  class Bytes < Scalar
    def initialize(value)
      @value = String(value).b
    end
  end

  class Text < Scalar
    def initialize(value = "")
      @value = String(value)
    end
  end
end

require "automerge_ext"

module Automerge
  class Document
    def self.from(value, actor_id: nil)
      doc = new(actor_id: actor_id)
      unless value.is_a?(Hash)
        raise ArgumentError, "Automerge document root must be a Hash"
      end

      value.each do |key, child|
        doc.put([key], child)
      end
      doc
    end

    def [](path)
      get(path)
    end

    def []=(path, value)
      put(path, value)
    end

    def change(message: nil, timestamp: nil)
      if block_given?
        begin
          yield self
          commit(message, timestamp)
        rescue Exception
          rollback
          raise
        end
      else
        commit(message, timestamp)
      end
      self
    end

    def to_h
      get([])
    end

    alias to_hash to_h

    def sync_state
      SyncState.new
    end
  end

  def self.init(actor_id: nil)
    Document.new(actor_id: actor_id)
  end

  def self.from(value, actor_id: nil)
    Document.from(value, actor_id: actor_id)
  end

  def self.load(bytes)
    Document.load(bytes)
  end
end
