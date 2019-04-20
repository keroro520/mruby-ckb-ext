include CKB

module CKB
  def assert(bool, message)
    raise message unless bool
  end

  def hex_to_bin(hex)
    if hex.start_with?("0x")
      hex = hex[2..-1]
    end
    [hex].pack("H*")
  end

  def self.require(binary_hash)
    eval_by_hash(binary_hash, Source::DEP)
  end

  def self.eval_by_index(index, source)
    self.eval(index, source)
  end

  def self.eval_by_hash(binary_hash, source)
    hash = hex_to_bin(binary_hash)
    cells =
      case source
      when Source::INPUT
        CKB.load_tx["inputs"]
      when Source::OUTPUT
        CKB.load_tx["outputs"]
      when Source::DEP
        CKB.load_tx["deps"]
      else
        raise "unexpected source"
      end
    indexes = cells.each_index.select { |i|
      hash == CellField.new(source, i, CKB::CellField::DATA_HASH).readall
    }
    assert(!indexes.empty?, "not found target cell")

    return eval_by_index(indexes[0], source)
  end
end
