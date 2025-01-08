# Translated from python by ChatGPT.
# https://github.com/microsoft/uf2/blob/master/utils/uf2conv.py
require 'json'
require 'optparse'
require 'rexml/document'
require 'fileutils'
require 'stringio'
require 'open3'
require 'pathname'

UF2_MAGIC_START0 = 0x0A324655 # "UF2\n"
UF2_MAGIC_START1 = 0x9E5D5157 # Randomly selected
UF2_MAGIC_END    = 0x0AB16F30 # Ditto

INFO_FILE = "/INFO_UF2.TXT"

$appstartaddr = 0x2000
$familyid = 0x0

def is_uf2(buf)
  w = buf[0, 8].unpack("II")
  w[0] == UF2_MAGIC_START0 && w[1] == UF2_MAGIC_START1
end

def is_hex(buf)
  begin
    w = buf[0, 30].force_encoding("UTF-8")
  rescue Encoding::UndefinedConversionError
    return false
  end
  w[0] == ':' && buf.match(/^[:0-9a-fA-F\r\n]+$/)
end

def convert_from_uf2(buf)
  numblocks = buf.size / 512
  curraddr = nil
  currfamilyid = nil
  families_found = {}
  prev_flag = nil
  all_flags_same = true
  outp = []
  
  numblocks.times do |blockno|
    ptr = blockno * 512
    block = buf[ptr, 512]
    hd = block[0, 32].unpack("IIIIIIII")
    
    if hd[0] != UF2_MAGIC_START0 || hd[1] != UF2_MAGIC_START1
      puts "Skipping block at " + ptr + "; bad magic"
      next
    end
    if hd[2] & 1 != 0
      # NO-flash flag set; skip block
      next
    end
    
    datalen = hd[4]
    raise "Invalid UF2 data size at #{ptr}" if datalen > 476
    
    newaddr = hd[3]
    if (hd[2] & 0x2000) != 0 && currfamilyid.nil?
      currfamilyid = hd[7]
    end

    if curraddr.nil? || ((hd[2] & 0x2000) != 0 && hd[7] != currfamilyid)
      currfamilyid = hd[7]
      curraddr = newaddr
      $appstartaddr = newaddr if $familyid == 0x0 || $familyid == hd[7]
    end

    padding = newaddr - curraddr
    raise "Block out of order at #{ptr}" if padding < 0
    raise "More than 10M of padding needed at #{ptr}" if padding > 10 * 1024 * 1024
    raise "Non-word padding size at #{ptr}" if padding % 4 != 0

    while padding > 0
      padding -= 4
      outp << "\x00\x00\x00\x00".b
    end

    outp << block[32, datalen] if $familyid == 0x0 || ((hd[2] & 0x2000) != 0 && $familyid == hd[7])
    curraddr = newaddr + datalen
    if (hd[2] & 0x2000) != 0
      families_found[hd[7]] = [families_found[hd[7]], newaddr].compact.min
    end

    prev_flag = hd[2] if prev_flag.nil?
    all_flags_same &&= (prev_flag == hd[2])
  end
  if blockno == (numblocks - 1)
    puts "--- UF2 File Header Info ---"
    families = load_families()
    families_found.keys.each do |family_hex|
      family_short_name = ""
      families.each do |name, value|
        if value == family_hex
          family_short_name = name
        end
      end
      puts "Family ID is #{family_short_name}, hex value is 0x#{family_hex.to_s(16).rjust(8, '0')}"
      puts "Target Address is 0x#{families_found[family_hex].to_s(16).rjust(8, '0')}"
    end
    if all_flags_same
      puts "All block flag values consistent, 0x#{hd[2].to_s(16).rjust(4, '0')}"
    else
      puts "Flags were not all the same"
    end
    puts "----------------------------"
    if families_found.size > 1 && familyid == 0x0
      outp = []
      appstartaddr = 0x0
    end
  end
  
  outp.join
end

def convert_to_carray(file_content)
  outp = "const unsigned long bindata_len = #{file_content.size};\n"
  outp += "const unsigned char bindata[] __attribute__((aligned(16))) = {"
  file_content.each_byte.each_with_index do |byte, i|
    outp += "\n" if i % 16 == 0
    outp += format("0x%02x, ", byte)
  end
  outp += "\n};\n"
  outp
end

def convert_to_uf2(file_content)
  datapadding = ""
  datapadding += "\x00\x00\x00\x00" while datapadding.bytesize < 512 - 256 - 32 - 4
  numblocks = (file_content.bytesize + 255) / 256
  outp = []
  
  numblocks.times do |blockno|
    ptr = 256 * blockno
    chunk = file_content[ptr, 256] || ""
    flags = 0x0
    flags |= 0x2000 if $familyid
    hd = [
      UF2_MAGIC_START0, UF2_MAGIC_START1,
      flags, ptr + $appstartaddr, 256, blockno, numblocks, $familyid
    ].pack("V*")
    chunk += "\x00" * (256 - chunk.bytesize) if chunk.bytesize < 256
    block = hd + chunk + datapadding + [UF2_MAGIC_END].pack("V")
    raise "Block size error" unless block.bytesize == 512
    outp << block
  end
  
  outp.join
end

class Block
  attr_accessor :addr, :bytes

  def initialize(addr)
    @addr = addr
    @bytes = "\x00" * 256
  end

  def encode(blockno, numblocks)
    flags = 0x0
    flags |= 0x2000 if $familyid
    hd = [
      UF2_MAGIC_START0, UF2_MAGIC_START1,
      flags, @addr, 256, blockno, numblocks, $familyid
    ].pack("V*")
    hd += @bytes[0, 256]
    hd += "\x00" * (512 - 4 - hd.bytesize) if hd.bytesize < 512 - 4
    hd + [UF2_MAGIC_END].pack("V")
  end
end

def convert_from_hex_to_uf2(buf)
  appstartaddr = nil
  upper = 0
  currblock = nil
  blocks = []

  buf.each_line do |line|
    next if line[0] != ":"

    rec = []
    i = 1
    while i < line.length - 1
      rec << line[i, 2].to_i(16)
      i += 2
    end

    tp = rec[3]
    case tp
    when 4
      upper = ((rec[4] << 8) | rec[5]) << 16
    when 2
      upper = ((rec[4] << 8) | rec[5]) << 4
    when 1
      break
    when 0
      addr = upper + ((rec[1] << 8) | rec[2])
      appstartaddr ||= addr
      i = 4
      while i < rec.length - 1
        if currblock.nil? || (currblock.addr & ~0xff) != (addr & ~0xff)
          currblock = Block.new(addr & ~0xff)
          blocks << currblock
        end
        currblock.bytes[addr & 0xff] = rec[i]
        addr += 1
        i += 1
      end
    end
  end

  numblocks = blocks.length
  resfile = "".b
  blocks.each_with_index do |block, i|
    resfile << block.encode(i, numblocks)
  end

  resfile
end

def to_str(binary)
  binary.force_encoding("UTF-8")
end

def get_drives
  drives = []
  if RUBY_PLATFORM.include?("mswin") || RUBY_PLATFORM.include?("mingw")
    stdout, _stderr, _status = Open3.capture3("wmic PATH Win32_LogicalDisk get DeviceID, VolumeName, FileSystem, DriveType")
    to_str(stdout).each_line do |line|
      words = line.split(/\s+/)
      if words.size >= 3 && words[1] == "2" && words[2] == "FAT"
        drives << words[0]
      end
    end
  else
    searchpaths = if RUBY_PLATFORM.include?("darwin")
                    ["/Volumes"]
                  elsif RUBY_PLATFORM.include?("linux")
                    ["/media/#{ENV['USER']}", "/run/media/#{ENV['USER']}"]
                  else
                    []
                  end

    searchpaths.each do |rootpath|
      if Dir.exist?(rootpath)
        Dir.foreach(rootpath) do |d|
          drive_path = File.join(rootpath, d)
          drives << drive_path if File.directory?(drive_path)
        end
      end
    end
  end

  drives.select { |d| has_info(d) }
end

def has_info(drive)
  File.file?(File.join(drive, INFO_FILE))
rescue
  false
end

def board_id(path)
  file_content = File.read(File.join(path, INFO_FILE))
  file_content[/Board-ID: ([^\r\n]*)/, 1]
end

def list_drives
  get_drives.each do |drive|
    puts "#{drive} #{board_id(drive)}"
  end
end

def write_file(name, buf)
  File.open(name, "wb") { |file| file.write(buf) }
  puts "Wrote #{buf.bytesize} bytes to #{name}"
end

def load_families
  filename = File.join(__dir__, "uf2families.json")
  raw_families = JSON.parse(File.read(filename))
  families = {}
  raw_families.each { |family| families[family["short_name"]] = Integer(family["id"]) }
  families
end

def main
  options = {}
  args = OptionParser.new do |opts|
    opts.banner = "Usage: uf2conv.rb [options]\nINPUT Input file (HEX, BIN or UF2)"

    opts.on('-b', '--base BASE', 'Set base address of application for BIN format') { |v| options[:base] = Integer(v) }
    opts.on('-f', '--family FAMILY', 'Specify familyID') { |v| options[:family] = v }
    opts.on('-o', '--output FILE', 'Write output to named file') { |v| options[:output] = v }
    opts.on('-c', '--convert', 'Do not flash, just convert') { options[:convert] = true }
    opts.on('-d', '--device DEVICE_PATH', 'select a device path to flash') { |v| options[:device_path] = v }
    opts.on('-l', '--list', 'list connected devices') { |v| options[:list] = v }
    opts.on('-c', '--convert', 'do not flash, just convert') { |v| options[:convert] = v }
    opts.on('-D', '--deploy', 'just flash, do not convert') { |v| options[:deploy] = v }
    opts.on('-w', '--wait', 'wait for device to flash') { |v| options[:deploy] = v }
    opts.on('-C', '--carray', 'convert binary file to a C array, not UF2') { |v| options[:carray] = v }
    opts.on('-i', '--info', 'display header information from UF2, do not convert') { |v| options[:info] = v }
  end.parse!
  options[:input] = args.size > 0 ? args[0] : nil

  $appstartaddr = options[:base] if options[:base]
  
  families = load_families
  $familyid = families[options[:family].upcase] || Integer(options[:family])

  if options[:input]
    inpbuf = File.binread(options[:input])
    from_uf2 = is_uf2(inpbuf)
    ext = "uf2"
    if options[:deploy]
      outbuf = inpbuf
    elsif from_uf2 && !options[:info]
      outbuf = convert_from_uf2(inpbuf)
      ext = "bin"
    elsif from_uf2 && options[:info]
      outbuf = ""
      convert_from_uf2(inpbuf)
    elsif is_hex(inpbuf)
      outbuf = convert_from_hex_to_uf2(inpbuf)
    elsif options[:carray]
      outbuf = convert_to_carray(inpbuf)
      ext = "h"
    else
      outbuf = convert_to_uf2(inpbuf)
    end
    if !options[:deploy] and !options[:info]
      puts "Converted to #{ext}, output size: #{outbuf.size}, start address: 0x#{$appstartaddr.to_s(16)}"
    end
    if options[:convert] || ext != "uf2"
      if options[:output].empty?
        options[:output] = "flash." + ext
      end
    end
    if options[:output]
      write_file(options[:output], outbuf)
    end
    if ext == "uf2" && !options[:convert] && !options[:info]
      drives = get_drives
      if drives.empty?
        if options[:wait]
          puts "Waiting for drive to deploy..."
          while drives.empty?
            sleep(0.1)
            drives = get_drives
          end
        elsif !options[:output]
          error("No drive to deploy.")
        end
      end
      drives.each do |d|
        puts "Flashing #{d} (#{board_id(d)})"
        write_file("#{d}/NEW.UF2", outbuf)
      end
    end
  else
    puts "Need input file"
    exit 1
  end
end

main
