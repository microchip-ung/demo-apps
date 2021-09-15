#!/usr/bin/env ruby
#
#------------------------------------------------------------------------------
# Vitesse Switch API software.
#
# Copyright (c) 2002-2013 Vitesse Semiconductor Corporation "Vitesse". All
# Rights Reserved.
#
# Unpublished rights reserved under the copyright laws of the United States of
# America, other countries and international treaties. Permission to use, copy,
# store and modify, the software and its source code is granted. Permission to
# integrate into other products, disclose, transmit and distribute the software
# in an absolute machine readable format (e.g. HEX file) is also granted.  The
# source code of the software may not be disclosed, transmitted or distributed
# without the written permission of Vitesse. The software and its source code
# may only be used in products utilizing the Vitesse switch products.
#
# This copyright notice must appear in any copy, modification, disclosure,
# transmission or distribution of the software. Vitesse retains all ownership,
# copyright, trade secret and proprietary rights in the software.
#
# THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
# INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR USE AND NON-INFRINGEMENT.
#------------------------------------------------------------------------------
#
#
# Helper script for assembling ICLI command specs from the set of
# early files (prior to .icli file writing).
#
#

# Undocumented command line option: -e=someclass.debug=true
# Check classes below for @@debug member to see if they support it.

# Missing ICLI bug compatibility:
#
#   * "Multiple optional begin" - [ [x] y]


# Originally grabbed from vtss_appl/icli/base/icli_variable.c, then augmented
# with C types
TYPES = {
         "dpl"              => "u8",
         "pcp"              => "icli_unsigned_range_t",
         "dscp"             => "u8",

         "vcap_vr"          => "",

         "clock_id"         => "icli_clock_id_t",

         "oui"              => "icli_oui_t",

         "mac_addr"         => "vtss_mac_t",
         "mac_ucast"        => "vtss_mac_t",
         "mac_mcast"        => "vtss_mac_t",

         "ipv4_addr"        => "vtss_ip_t",
         "ipv4_netmask"     => "vtss_ip_t",
         "ipv4_wildcard"    => "vtss_ip_t",
         "ipv4_ucast"       => "vtss_ip_t",
         "ipv4_mcast"       => "vtss_ip_t",
         "ipv4_abc"         => "vtss_ip_t",
         "ipv4_subnet"      => "icli_ipv4_subnet_t",
         "ipv4_prefix"      => "u32",

         "ipv6_addr"        => "vtss_ipv6_t",
         "ipv6_netmask"     => "vtss_ipv6_t",
         "ipv6_ucast"       => "vtss_ipv6_t",
         "ipv6_mcast"       => "vtss_ipv6_t",
         "ipv6_linklocal"   => "vtss_ipv6_t",
         "ipv6_subnet"      => "icli_ipv6_subnet_t",
         "ipv6_prefix"      => "u32",

         "hostname"         => "char",

         "int"              => "i32",
         "int8"             => "i8",
         "int16"            => "i16",

         "uint"             => "u32",
         "uint8"            => "u8",
         "uint16"           => "u16",

         # "float"    # we don't expect that this should be used

         "range_list"       => "icli_range_t",

         "date"             => "icli_date_t",
         "time"             => "icli_time_t",
         "hhmm"             => "icli_time_t",

         "word"             => "char",
         "kword"            => "char",
         "cword"            => "char",
         "dword"            => "char",
         "fword"            => "char",

         "string"           => "char",

         "line"             => "char",

         "port_id"          => "icli_switch_port_range_t",
         "port_list"        => "icli_stack_port_range_t",
         "port_type"        => "icli_port_type_t",
         "port_type_id"     => "icli_switch_port_range_t",
         "port_type_list"   => "icli_stack_port_range_t",

         "range_list"       => "icli_range_t",

         "vlan_id"          => "u32",
         "vlan_list"        => "icli_unsigned_range_t"
        }

KnownSec = Struct.new(:section_name, :privilege, :mode_var, :mode)

KNOWN_SECTIONS = [KnownSec.new("user exec",                            0, nil,                          'ICLI_CMD_MODE_EXEC'),
                  KnownSec.new("privileged exec",                     15, nil,                          'ICLI_CMD_MODE_EXEC'),
                  KnownSec.new("global config",                       15, nil,                          'ICLI_CMD_MODE_GLOBAL_CONFIG'),
                  KnownSec.new("ipmc profile <profile_name:word16>",  15, ['profile_name'],             'ICLI_CMD_MODE_IPMC_PROFILE'),
                  KnownSec.new("snmp-server host <conf_name:word16>", 15, ['conf_name'],                'ICLI_CMD_MODE_SNMPS_HOST'),
                  KnownSec.new("rfc2544 profile <name:word32>",       15, ['profile_name'],             'ICLI_CMD_MODE_RFC2544_PROFILE'),
                  KnownSec.new("vlan <vlan_list>",                    15, ['vlist'],                    'ICLI_CMD_MODE_CONFIG_VLAN'),
                  KnownSec.new("interface common",                    15, nil,                          nil),
                  KnownSec.new("interface vlan <vlan_id>",            15, ['vlist'],                    'ICLI_CMD_MODE_INTERFACE_VLAN'),
                  KnownSec.new("interface ethernet <port_id>",        15, ['plist'],                    'ICLI_CMD_MODE_INTERFACE_PORT_LIST'),
                  KnownSec.new("spanning-tree aggregation",           15, nil,                          'ICLI_CMD_MODE_STP_AGGR'),
                  KnownSec.new("line",                                15, ['session_list', 'vty_list'], 'ICLI_CMD_MODE_CONFIG_LINE')
                 ]



def die(msg)
  $stderr.puts "ERROR: " + msg
  exit(1)
end



def print_message(filename, lineno, line, msg)
  $stderr.puts "#{msg}" if msg != nil
  $stderr.printf("%s:%d: %s\n", filename, lineno, line)
end



class Eof < Exception
end

class IcliSyntax < Exception
end



class Counters
  @@c     = Hash.new
  @@debug = false

  def Counters.debug=(val)
    @@debug = val
  end

  def Counters.inc(name, delta = 1)
    if @@c.has_key?(name)
      @@c[name] += delta
    else
      @@c[name] = delta
    end
  end

  def Counters.print
    keys = @@c.keys.find_all { |k| @@debug || !(k =~ /^Debug/) }.sort
    keys.each { |key|
      printf("!   %-40s: %5d\n", key, @@c[key])
    }
  end
end



class InputFile
  attr_reader :filename

  def initialize(fn)
    @filename = fn
    if fn == '-'
      @lines = $stdin.readlines
    else
      @lines = IO.readlines(fn)  ||  die("Cannot open input file #{fn}")
    end
    @lines.reverse!
    @count = @lines.length
    Counters.inc("Files total")
    Counters.inc("Lines total", @count)
  end

  def lineno
    @count - @lines.length
  end

  def next_line
    ln = @lines.pop
    raise Eof if ln == nil
    ln.rstrip!
    # printf("%5d %s\n", lineno, ln)
    if (ln =~ /^\s*!/) and !(ln =~ /^\s*!!/)
      return ""     # Kill comment with just one '!'
    else
      return ln
    end
  end

  def undo_next_line(ln)
    @lines.push(ln)
  end

  # Skip initial blank lines, then if the first line is indented, collect it
  # and all immediately following lines that are _more_ indented
  def next_cmd
    while (ln = next_line) == ""
    end
    res = ln

    i_first = indent(res)
    return res if i_first == 0

    i_next = i_first + 1   # to make while loop spin at least once
    begin
      while i_next > i_first
        ln = next_line
        i_next = indent(ln)
        if i_next > i_first
          res += " " + ln.strip
        else
          undo_next_line(ln)
        end
      end
    rescue Eof
      # EOF after first line is ok
    end
    res
  end

  def indent(s)
    s =~ /^(\s*)/
    $1 ? $1.length : 0
  end
end



class Cmd
  attr_reader :filename, :lineno, :vtss_cmdstr_list, :cmdstr, :tokens, :parsetree, :optional_keywords

  @@debug = false

  def Cmd.debug=(val)
    @@debug = val
  end

  def initialize(filename, lineno, vtss_cmdstr_list, cmdstr)
    vtss_cmdstr_list = ["(none)"] if vtss_cmdstr_list == []
    cmdstr = cmdstr.strip.gsub(/\s\s+/, ' ')
    @filename, @lineno, @vtss_cmdstr_list, @cmdstr = filename, lineno, vtss_cmdstr_list, cmdstr
    tokenize
    @parsetree = CmdParseTreeBuilder.new.parse(self)
    if @@debug
      puts "Parse tree for #{cmdstr}:"
      @parsetree.print
    end
    @optional_keywords = find_optional_keywords
  end

  def <=>(other)
    to_s_from_tokens <=> other.to_s_from_tokens
  end

  def to_s
    @cmdstr
  end

  def to_s_from_tokens
    @tokens.join(' ')
  end

  def to_s_all
    "#{@filename}: #{@vtss_cmdstr_list.join('  --and--  ')} => #{@cmdstr}"
  end

  # List of all tokens up to first '[' or '{'
  def prefix_list
    idx = 0
    @tokens.each { |t|
      if t == '{'  ||  t == '['
        return @tokens[0..idx-1]
      end
      idx += 1
    }
    return @tokens
  end

  def tokenize
    s = @cmdstr
    @tokens = []
    varnames = []   # So we can detect duplicates
    while s != ""
      s.lstrip!
      case s
      when /^([a-zA-Z][a-zA-Z0-9_\-:\/\*]*)(.*)/    # keywords, perhaps with variable in front (var:keyword or keyword:)
        t, s = $1, $2
	var, kwd = $1.split(':')
	kwd = var unless kwd
        raise IcliSyntax.new("Keywords must be lower-case: #{kwd}") if kwd.downcase != kwd
        raise IcliSyntax.new("Keywords must use '-' instead of '_': #{kwd}") if kwd.index('_') != nil
        @tokens << t
      when /^(\[|\]|\{|\}\*\d+|\}|\||\.\.\.|\*)(.*)/,   # [ ] { } }*n | ... *
           /^(\/[a-zA-Z][a-zA-Z0-9_\-]*)(.*)/,  # options  ('/force')
           /^(<[a-zA-Z0-9_\-~,:']+>)(.*)/,      # variables
           /^(\-?[0-9]+)(.*)/                   # integer numbers
        t, s = $1, $2
        @tokens << t
      else
        raise IcliSyntax.new("Unexpected input: #{s}")
      end
    end

    nest = []
    @prev_type == ""
    @tokens.each { |t|
      @prev_type == "" unless t =~ /</
      case t
      when '{'
        nest.push('}')
      when '['
        nest.push(']')
      when '}', ']'
        raise IcliSyntax.new("Unexpected '#{t}'") if nest == []
        raise IcliSyntax.new("Unmatched '#{t}' (depth #{nest.length}, expected '#{nest[-1]}')") if t != nest[-1]
        nest.pop
      when /\}\*(\d+)/
        n = $1.to_i
        raise IcliSyntax.new("Unexpected '#{t}'") if nest == []
        raise IcliSyntax.new("Unmatched '#{t}' (depth #{nest.length}, expected '#{nest[-1]}')") if '}' != nest[-1]
        raise IcliSyntax.new("Required-count must be > 0") if n < 1
        nest.pop
      when /<([a-zA-Z][a-zA-Z0-9_\-]*):([a-zA-Z0-9_\-~,]+)>/
        name, type = $1, $2
        if name =~ /\-/
            print_message(@filename, @lineno, @cmdstr, "WARNING: Variable names shouldn't contain '-'; use '_' instead: '#{name}'")
            Counters.inc("WARNING: '-' in variable name")
        end
        validate_type(type)
        raise IcliSyntax.new("Duplicate variable name '#{name}'") if varnames.include?(name)
        varnames << name
      when /<([a-zA-Z0-9_\-~,]+)>/
        type = $1
        validate_type(type)
      when /<.*[^>]$/
        raise IcliSyntax.new("Invalid variable, missing '>'")
      when /^([a-zA-Z][a-zA-Z0-9_\-]*):(.+)/
        name, keyword = $1, $2
        if name =~ /\-/
            print_message(@filename, @lineno, @cmdstr, "WARNING: Variable names shouldn't contain '-'; use '_' instead: '#{name}'")
            Counters.inc("WARNING: '-' in variable name")
        end
        raise IcliSyntax.new("Duplicate variable name '#{name}'") if varnames.include?(name)
        varnames << name
      when '|'
        raise IcliSyntax.new("First '|' isn't inside { } or [ ]") if nest == []
      else
        # The rest are valid; they were checked by the other case statement above
      end
    }
    raise IcliSyntax.new("Unmatched '#{nest[-1]}' (depth #{nest.length})") if nest != []
  end

  def validate_type(t)
    if @prev_type == "port_type"  &&  t == "port_list"
      print_message(@filename, @lineno, @cmdstr, "WARNING: '<port_type> <port_list>' should be replaced by '<port_type_list>'")
      $stderr.puts
      Counters.inc("WARNING: <port_type><port_list>")
    end
    @prev_type = t
    case t
    when /^(word|kword|string|line)(\d+)$/
      # ok
      Counters.inc("Debug:Range count")
    when /^(word|kword|string|line)(\d+)\-(\d+)/
      lo, hi = $2.to_i, ($3 ? $3.to_i : $2.to_i)
      raise IcliSyntax.new("#{t}: Low > High") if lo > hi
      Counters.inc("Debug:Range count")
    when /^[a-zA-Z]/
      raise IcliSyntax.new("Unknown type: #{t}") unless TYPES.has_key?(t)
    when /^(\-?[0-9a-fA-Foxb]+)~(\-?[0-9a-fA-Foxb]+)$/   # e.g. -10~-1 or 2~6
      lo, hi = s_to_i($1), s_to_i($2)
      raise IcliSyntax.new("Range #{t}: Low > High") if lo > hi
      Counters.inc("Debug:Range count")
    when /^[\-0-9]/
      validate_int_range(t)
      Counters.inc("Debug:Range count")
    else
      raise IcliSyntax.new("Error in type: #{t}")
    end
  end

  def validate_int_range(range)
    list = range.split(',')
    list.each { |r|
      case r
      when /^(\-?[0-9a-fA-Foxb]+)\-(\-?[0-9a-fA-Foxb]+)$/   # e.g. -10--1 or 2-6
        lo, hi = s_to_i($1), s_to_i($2)
        raise IcliSyntax.new("Range #{r}: Low > High") if lo > hi
      when /^\-?\d+$/
        # ok
      when /^'[A-Za-z0-9_]+'/
        # C symbolic constant; ok
      else
        raise IcliSyntax.new("Error in range type: #{t}")
      end
    }
  end

  def s_to_i(s)
    case s
    when /^0x([0-9a-fA-F]+)$/
      return $1.to_i(16)
    when /^0o([0-7])+$/
      return $1.to_i(8)
    when /^0b([01]+)$/
      return $1.to_i(2)
    when /^\-?\d+$/
      return s.to_i(10)
    else
      raise IcliSyntax.new("Invalid number format: #{s}")
    end
  end


  def find_optional_keywords
    puts "Finding optional keywords:" if @@debug
    dummy, acc = do_find_keywords_for_cmd_var(@parsetree, false, [], 0)
    acc
  end

  def do_find_keywords_for_cmd_var(node, include_first_leaf, accumulator, debug_indent)
    printf("%-30s %-20s %-14s res = %s\n", (" "*debug_indent*2)+"Check", node.token, include_first_leaf ? 'first_leaf' : '', accumulator.join(',')) if @@debug
    case node.token
    when :opt
      node.children.each { |c|
        dummy, accumulator = do_find_keywords_for_cmd_var(c, true, accumulator, debug_indent + 1)
      }
    when :seq
      node.children.each { |c|
        include_first_leaf, accumulator = do_find_keywords_for_cmd_var(c, include_first_leaf, accumulator, debug_indent + 1)
      }
    when :alt
      count = node.children.length
      node.children.each { |c|
        ## Optimization, but is it nice? Last element doesn't need one, really:
        ## dummy, accumulator = do_find_keywords_for_cmd_var(c, count > 1, accumulator, debug_indent + 1)
        dummy, accumulator = do_find_keywords_for_cmd_var(c, true, accumulator, debug_indent + 1)
        count = count - 1
      }
      include_first_leaf = false
    when /<(.+)>/
      # Variables can't be checked for presence
      include_first_leaf = false
    else
      accumulator << node.token if include_first_leaf
      include_first_leaf = false
    end

    printf("%-30s %-20s %-14s res = %s\n", (" "*debug_indent*2)+"Result", node.token, include_first_leaf ? 'first_leaf' : '', accumulator.join(',')) if @@debug
    return [include_first_leaf, accumulator]
  end

end



class Node
  include Enumerable

  attr_accessor :token, :children, :attributes

  def initialize(tok, chld = [], attributes = {})
    @token = tok
    @children = chld
    @attributes = attributes
  end

  def print(indent = 0)
    s = nil
    case @token
    when :opt
      puts " "*indent*2 + "["
      @children.each { |c| c.print(indent+1) }
      puts " "*indent*2 + "]"
    when :seq
      puts " "*indent*2 + "{"
      @children.each { |c| c.print(indent+1) }
      req_cnt = @attributes['required-count'] == nil ? '' : "*#{@attributes['required-count']}"
      puts " "*indent*2 + "}" + req_cnt
    when :alt
      print_or = false
      @children.each { |c|
        puts " "*indent*2 + "|" if print_or
        c.print(indent)
        print_or = true
      }
    else
      puts " "*indent*2 + @token
    end
  end

  def each(&block)
    block[self]
    @children.each { |c| c.each(&block) }
  end

  def is_leaf?
    return @children == []
  end

end



class CmdParseTreeBuilder
  attr_reader :root

  @@debug = false

  def CmdParseTreeBuilder.debug=(val)
    @@debug = val
  end

  def initialize
    @tokens = nil
    self
  end

  def parse(cmd)
    @tokens = cmd.tokens.clone
    root = expr
    @tokens = nil
    return root
  end

private
  def expr
    alternatives = [term]
    while @tokens[0] == '|'
      @tokens.shift
      alternatives << term
    end
    return (alternatives.length == 1) ? alternatives[0] : Node.new(:alt, alternatives)
  end

  def term
    list = [factor]
    while @tokens[0] != nil  &&  !(['}', ']', '|'].include?((@tokens[0][0]).chr))
      list << factor
    end
    return (list.length == 1) ? list[0] : Node.new(:seq, list)
  end

  def factor
    res = nil
    case @tokens[0]
    when nil
      raise IcliSyntax.new("Expected factor, got '#{@tokens[0]}'")

    when '{'
      @tokens.shift
      res = expr
      res = (res.token == :seq) ? res : Node.new(:seq, [res])

      case @tokens[0]
      when '}'
        # ok
      when /\}\*(\d+)/
        n = $1.to_i
        tmp = res.children
        cnt = tmp.length
        raise IcliSyntax.new("Required-count must be > 0 and <= #{cnt}") if n < 1 || n > cnt
        nonopt = tmp.find_all { |c| c.token != :opt }
        raise IcliSyntax.new("Sequence with required-count contains non-optionals (#{nonopt.map{|n|n.token}.join('/')}). Required form: { [a] [b] [c] }*n") if nonopt != []
        res.attributes['required-count'] = n
      else
        raise IcliSyntax.new("Expected '}' or '}*required-count'")
      end

      @tokens.shift

    when '['
      @tokens.shift
      res = Node.new(:opt, [expr])
      raise IcliSyntax.new("Expected ']'") unless @tokens[0] == ']'
      @tokens.shift

    else
      res = Node.new(@tokens[0])
      @tokens.shift
    end

    return res
  end

end



class Sections
  attr_reader :sections

  def initialize
    @sections = Hash.new
  end

  def [](idx)
    @sections[idx]
  end

  def add(sec, entry)
    if @sections.has_key?(sec)
      @sections[sec] << entry
    else
      @sections[sec] = [entry]
    end
  end

  # Return correct ordering of sections, but only those sections that hold data
  def section_order
    ks = KNOWN_SECTIONS.map { |s| s.section_name }
    secs = ks + (sections.keys.sort - ks.sort)
    return secs.find_all { |key| @sections.has_key?(key) }
  end

  def print(with_filenames = true)
    section_order.each { |key|
      puts key
      prev_filename = nil
      @sections[key].each { |cmd|
        if with_filenames  &&  (cmd.filename != prev_filename)
          puts "  ! File: #{cmd.filename}"
          prev_filename = cmd.filename
        end
        puts "  " + cmd.cmdstr
      }
      puts
    }
  end

  def print_vtss_table
    section_order.each { |key|
      puts "! #{key}"
      cmds = @sections[key].sort { |a, b|
        res = a.vtss_cmdstr_list[-1] <=> b.vtss_cmdstr_list[-1]
        #if res == 0
        #  res = a.cmdstr <=> b.cmdstr
        #end
        res
      }
      res = cmds.map { |cmd|
        cmd.vtss_cmdstr_list.map { |vtss_cmd|
          sprintf("!  %-37s\t=> %s\n", vtss_cmd, cmd.cmdstr)
        }
      }
      puts res.flatten.sort
      puts "!"
    }
  end

  def check_duplicates
    @sections.each { |key, cmds|
      sorted = cmds.sort
      c = sorted.shift
      while c != nil  &&  sorted != []
        if (sorted[0] <=> c) == 0
          spacing = sorted[0].cmdstr.length != c.cmdstr.length ? " (only spacing differences)" : ""
          print_message(c.filename, c.lineno, c.cmdstr, "WARNING: Duplicate command#{spacing}:")
          print_message(sorted[0].filename, sorted[0].lineno, sorted[0].cmdstr, nil)
          $stderr.puts
          sorted.shift
          Counters.inc("WARNING: Duplicate cmds")
        elsif sorted[0].prefix_list == c.prefix_list
          print_message(c.filename, c.lineno, c.cmdstr, "WARNING: Overlapping command prefix, ICLI won't be able to distinguish the commands: #{c.prefix_list.join(' ')}")
          print_message(sorted[0].filename, sorted[0].lineno, sorted[0].cmdstr, nil)
          $stderr.puts
          sorted.shift
          Counters.inc("WARNING: Overlapping cmds")
        else
          c = sorted.shift
        end
      end
    }
  end
end


class InputFileParser

  def initialize(filename, sections)
    input_file        = InputFile.new(filename)
    line              = nil
    current_sec       = nil
    current_vtss_cmd  = []
    loading_vtss_cmds = false
  
    begin
      while true
        begin
          line = input_file.next_cmd
          case line
          when /^[a-z]/      # Text at start of line: It's a section
            current_sec       = line
            current_vtss_cmd  = []
            loading_vtss_cmds = false
          when /^\s+!!\s*$/      # An empty '!!' comment: Clear current VTSS cmds
            current_vtss_cmd  = []
            loading_vtss_cmds = false
          when /^\s+!!\s*(.+)/   # A '!!' comment: A VTSS CLI command
            s = $1
            if loading_vtss_cmds
              current_vtss_cmd << s
            else
              current_vtss_cmd  = [s] # Only append VTSS cmds if they follow immediately after each other
            end
            loading_vtss_cmds = true
          when /\s+\S/   # Something indented: A command in a section
            loading_vtss_cmds = false
            cmd = Cmd.new(input_file.filename, input_file.lineno, current_vtss_cmd, line)
            sections.add(current_sec, cmd)
            Counters.inc("Commands total")
          else
            loading_vtss_cmds = false
            Counters.inc("ERROR: General syntax")
            raise IcliSyntax.new("ERROR: General syntax")
          end
        rescue IcliSyntax => msg
          print_message(input_file.filename, input_file.lineno, line, "ERROR: ICLI syntax: #{msg}")
          $stderr.puts
          Counters.inc("ERROR: ICLI syntax")
        end
      end
    rescue Eof
      # ignore; EOF is OK
    end
  end
end


class IcliGenerator
  attr_accessor :group

  def initialize(outfile, sections)
    @outfile, @sections = outfile, sections
    @special_cnt        = {}      # token => count
    @group              = false
  end

  def varname_to_byword(v)
    v.split('_').map { |word| word.capitalize }.join
  end

  def varname_cleanup(varname)
    expand_numbers(varname).gsub('/', 'option_').gsub(/[\-:]/, '_')
  end

  def expand_numbers(str)
    prefix = lo = hi = nil

    case str
    when /^([^\-\d]*)(\-?\d+)[\-~](\-?\d+)$/
      prefix, lo, hi = $1, $2.to_i, $3.to_i
    when /^([^\-\d]*)(\-?\d+)$/
      prefix, lo = $1, $2.to_i
    when '*'
      prefix = 'asterisk'
    else
      prefix = str
    end

    if lo
      prefix += "minus_" if lo < 0
      prefix += lo.abs.to_s
    end

    if hi
      prefix += "_to_"
      prefix += "minus_" if hi < 0
      prefix += hi.abs.to_s
    end

    return prefix
  end

  def inc_special_cnt(varname)
    return "" if varname == ""
    @special_cnt[varname] = 0 if @special_cnt[varname] == nil
    @special_cnt[varname] += 1
    return (@special_cnt[varname] == 1) ? '' : "_#{@special_cnt[varname]-1}"
  end

  def generate_one_icli_cmd(cmd, privilege, mode, mode_var, no_form, default_form)
    tokens            = cmd.tokens.clone
    bywords           = []
    cmdvars           = []
    out_tokens        = []
    @special_cnt      = {}
    optional_keywords = cmd.optional_keywords.clone

    while tokens != []
      varname = nil
      byword  = nil
      type    = nil
      t       = tokens.shift
      case t
      when /<(.+):(.+)>/
        varname, type = varname_cleanup($1), $2
        t = "<#{type}>"
        byword = "<" + varname_to_byword(varname) + " : #{type}>"
      when /<(.+)>/
        varname = "v_#{expand_numbers($1)}"
        byword = ''
      when /^([a-zA-Z].*):(.+)/
        varname, type = varname_cleanup($1), $2
        byword = "#{type} : Option"
        optional_keywords.shift if t == optional_keywords[0]
        t = type
      when /^\/?[a-zA-Z]/, /^\-?\d/, '*'
        if t == optional_keywords[0]
          varname = "has_#{varname_cleanup(t)}"
          byword = "#{t} : Option"
          optional_keywords.shift
        else
          varname = ""
        end
      else
        # pass unmodified
      end

      out_tokens << t
      if varname
        cmdvars << varname + inc_special_cnt(varname)
        bywords << byword
      end
    end

    @outfile.puts "!" + "="*78
    @outfile.puts

    @outfile.puts "CMD_BEGIN"
    @outfile.puts

    @outfile.puts "IF_FLAG = "
    @outfile.puts

    @outfile.puts "COMMAND = " + out_tokens.join(' ')
    @outfile.puts

    @outfile.puts "DOC_CMD_DESC    = "
    @outfile.puts "DOC_CMD_DEFAULT = "
    @outfile.puts "DOC_CMD_USAGE   = "
    @outfile.puts "DOC_CMD_EXAMPLE = "
    @outfile.puts

    if no_form
      @outfile.puts "NO_FORM_DOC_CMD_DESC    = "
      @outfile.puts "NO_FORM_DOC_CMD_DEFAULT = "
      @outfile.puts "NO_FORM_DOC_CMD_USAGE   = "
      @outfile.puts "NO_FORM_DOC_CMD_EXAMPLE = "
      @outfile.puts
    end

    if default_form
      @outfile.puts "DEFAULT_FORM_DOC_CMD_DESC    = "
      @outfile.puts "DEFAULT_FORM_DOC_CMD_DEFAULT = "
      @outfile.puts "DEFAULT_FORM_DOC_CMD_USAGE   = "
      @outfile.puts "DEFAULT_FORM_DOC_CMD_EXAMPLE = "
      @outfile.puts
    end

    @outfile.puts "FUNC_NAME = "
    @outfile.puts "FUNC_REUSE = "
    @outfile.puts

    props = []
    props << 'ICLI_CMD_PROP_GREP'    if cmd.tokens[0] == 'show'
    props << 'ICLI_CMD_PROP_LOOSELY' if (cmd.tokens[0] == 'no') || (cmd.tokens[0] == 'default')
    @outfile.puts "PRIVILEGE = ICLI_PRIVILEGE_#{privilege}"
    @outfile.puts "PROPERTY  = #{props.join(' | ')}"
    @outfile.puts

    @outfile.puts "CMD_MODE = #{mode}"
    (mode_var || ['']).each { |v|
      @outfile.puts "MODE_VAR = #{v}"
    }
    @outfile.puts

    words = out_tokens.find_all { |t| t =~ /^\-?[a-zA-Z0-9_<\/\*]/ }

    if @group then
        0.upto(words.length-1) { |i|
            @outfile.puts "! #{words[i]}"
            @outfile.puts "CMD_VAR = #{cmdvars[i]}"
            @outfile.puts "RUNTIME = "
            @outfile.puts "HELP    = "
            @outfile.puts "BYWORD  = #{bywords[i]}"
            @outfile.puts
        }
    else
        i = 1
        words.each { |w| @outfile.puts "! #{i}: #{w}"; i += 1 }
        @outfile.puts

        cmdvars.each { |v|
          @outfile.puts "CMD_VAR = #{v}"
        }
        @outfile.puts

        @outfile.puts "RUNTIME = \n" * words.length
        @outfile.puts

        @outfile.puts "HELP = \n" * words.length
        @outfile.puts

        bywords.each { |bw|
          @outfile.puts "BYWORD = #{bw}"
        }
        @outfile.puts
    end

    @outfile.puts "VARIABLE_BEGIN"
    @outfile.puts "VARIABLE_END"
    @outfile.puts

    if no_form
      @outfile.puts "NO_FORM_VARIABLE_BEGIN"
      @outfile.puts "NO_FORM_VARIABLE_END"
      @outfile.puts
    end

    if default_form
      @outfile.puts "DEFAULT_FORM_VARIABLE_BEGIN"
      @outfile.puts "DEFAULT_FORM_VARIABLE_END"
      @outfile.puts
    end

    @outfile.puts "CODE_BEGIN"
    @outfile.puts "CODE_END"
    @outfile.puts

    if no_form
      @outfile.puts "NO_FORM_CODE_BEGIN"
      @outfile.puts "NO_FORM_CODE_END"
      @outfile.puts
    end

    if default_form
      @outfile.puts "DEFAULT_FORM_CODE_BEGIN"
      @outfile.puts "DEFAULT_FORM_CODE_END"
      @outfile.puts
    end

    @outfile.puts "CMD_END"
    @outfile.puts
  end

  def generate_header
    @outfile.puts '''#
# Vitesse Switch software.
#
# Copyright (c) 2002-2012 Vitesse Semiconductor Corporation "Vitesse". All
# Rights Reserved.
#
# Unpublished rights reserved under the copyright laws of the United States of
# America, other countries and international treaties. Permission to use, copy,
# store and modify, the software and its source code is granted. Permission to
# integrate into other products, disclose, transmit and distribute the software
# in an absolute machine readable format (e.g. HEX file) is also granted.  The
# source code of the software may not be disclosed, transmitted or distributed
# without the written permission of Vitesse. The software and its source code
# may only be used in products utilizing the Vitesse switch products.
#
# This copyright notice must appear in any copy, modification, disclosure,
# transmission or distribution of the software. Vitesse retains all ownership,
# copyright, trade secret and proprietary rights in the software.
#
# THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
# INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR USE AND NON-INFRINGEMENT.
#

MODULE_IF_FLAG =

INCLUDE_BEGIN
INCLUDE_END

FUNCTION_BEGIN
FUNCTION_END

EXPORT_BEGIN
EXPORT_END

'''
  end

  def remove_match(tokens, cmd_list)
    res = cmd_list.find_all { |c| c.tokens != tokens }
    return [res.length != cmd_list.length, res]
  end

  # Only recognize pure no/default forms, not optional versions ([no],[no|default]) etc
  def generate
    generate_header
    @sections.section_order.each { |key|
      cmds = @sections[key].clone
      while cmds != []
        no_form      = false
        default_form = false
        c = cmds[0]

        # If we have a no/default form, try to find a matching command without no/default.
        # Then use it as basis for command generation later on
        if c.tokens[0] == 'no'  ||  c.tokens[0] == 'default' then
          pure_cmd = cmds.find { |other_c| other_c.tokens == c.tokens[1..-1] }
          c = pure_cmd if pure_cmd != nil
        end

        # Here, c is a no-no/default command. Remove it from cmds along with any matching no/default form(s)
        dummy, cmds        = remove_match(c.tokens, cmds)
        no_form, cmds      = remove_match(['no']+c.tokens, cmds)
        default_form, cmds = remove_match(['default']+c.tokens, cmds)

        if key == 'interface common' then
          generate_one_icli_cmd(c, 15, 'ICLI_CMD_MODE_INTERFACE_VLAN',      ['vlist'], no_form, default_form)
          generate_one_icli_cmd(c, 15, 'ICLI_CMD_MODE_INTERFACE_PORT_LIST', ['plist'], no_form, default_form)
        else
          ks = KNOWN_SECTIONS.find { |s| s.section_name == key }
          if ks != nil
            generate_one_icli_cmd(c, ks.privilege, ks.mode, ks.mode_var, no_form, default_form)
          else
            generate_one_icli_cmd(c, 15, '', nil, no_form, default_form)
          end
        end
      end
    }
  end
end



def usage
  $stderr.puts "Usage: asm_specs.rb [-o=output_filename.icli [-g]] [-e=ruby_code] file1 [ file2 ... ]"
  $stderr.puts "       '-' is an alias for standard input; useful for a quick test"
  $stderr.puts "       -g: Group per-keyword/variable settings in ICLI file"
  $stderr.puts "       -e: Evaluate ruby code. Only relevant for debugging, e.g."
  $stderr.puts "           -e=Cmd.debug=true"
  exit(1)
end


#
# Main
#

input_args  = []
outfile     = nil
outfilename = nil
group       = false

ARGV.each { |a|
  case a
  when /^\-o=(.+)/
    outfilename = $1
    if outfilename == '-'
      outfile = $stdout
      outfilename = "stdout"
    else
      outfile = File.new(outfilename, "w")  or  die("Cannot create output file #{outfilename}")
    end
  when /^\-e=(.+)/
    ruby_code = $1
    begin
      eval(ruby_code)
    rescue => msg
      die("Ruby code eval failed: #{msg}")
    end
  when '-g'
    group = true
  when /^\-.+/
    usage
  else
    input_args << a
  end
}

usage if input_args == []
usage if group and !outfile

sections = Sections.new

# Load file by file, checking for syntax problems and building the sections data structure
while (filename = input_args.shift) != nil
  InputFileParser.new(filename, sections)
end

# Load done, validate and output

sections.check_duplicates

puts
puts "! Loaded commands:"
puts "!"
sections.print
puts
puts
puts

puts "! VTSS CLI => ICLI command mapping:"
puts "!"
sections.print_vtss_table

puts
puts "! Statistics:"
Counters.print

if outfile
  puts "! Generating ICLI skeleton file #{outfilename}"
  gen = IcliGenerator.new(outfile, sections)
  gen.group = group
  gen.generate
end
