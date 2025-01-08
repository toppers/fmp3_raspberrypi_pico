require 'optparse'

def write_file(name, buf)
  File.open(name, "wb") { |f| f.write(buf) }
  puts "Wrote #{buf.bytesize} bytes to #{name}"
end

def main
  appstartaddr = nil

  def error(msg)
    puts msg
    exit(1)
  end

  options = {
    divid: "0x2000",
    output: nil
  }

  OptionParser.new do |opts|
    opts.banner = "Usage: script.rb [options] INPUT"

    opts.on('-d', '--divid DIVID', 'Set divid address of application for BIN format (default: 0x2000)') do |divid|
      options[:divid] = divid
    end

    opts.on('-o', '--output FILE', 'Write output to named file; defaults to "flash.uf2" or "flash.bin" where sensible') do |output|
      options[:output] = output
    end
  end.parse!(into: options)

  input = ARGV[0]
  error("Need input file") if input.nil?

  appstartaddr = Integer(options[:divid])

  inpbuf = File.open(input, "rb") do |f|
    f.seek(appstartaddr)
    f.read
  end

  outbuf = inpbuf

  write_file(options[:output], outbuf) if options[:output]
end

main
