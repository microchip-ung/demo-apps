This is an introduction and guide for the initial specification of commands
for the new VTSS ICLI.


Background
==========

We have a large number of commands from the existing VTSS CLI that must be
re-designed for the new ICLI. Ultimately these commands must be implemented
via .icli files, complete with help texts, properties, bywords, etc.

However, before we reach that stage we need to get an overview of the new
command structure and get the syntax right -- that's what this is about.

For each major module, you create a simple file. In it you write the new
commands, but without help texts or all the other things that are necessary
for ICLI. You group your commands into the modes where they belong, perhaps
creating new modes along the way.

Then, when we have completed and reviewed the command design, we begin the
real .icli command implementation work (at which time these files are
obsoleted).



Getting Started
===============

Absolutely:

0. READ THIS ENTIRE FILE FIRST, PLEASE!

0. READ THIS ENTIRE FILE FIRST, PLEASE!!

0. READ THIS ENTIRE FILE FIRST, PLEASE!!!    :-)

0. READ http://twiki.dk.vitesse.com/twiki/cgi-bin/view/Software/IcliAndIcfgBestPractices

Then roughly:

1. Go to soft02:/proj/sw/icli-cmd-specs/ and create a new file based on
   icli-cmd-spec-template.txt and put it in the 'cmd' subdir.

2. Open old-style-vtss-cli-cmds.txt and copy all the old-style VTSS CLI
   commands you're interested in into your new file.

   PLEASE NOTE: The old-style commands must *NOT* be reformatted in any way,
                shape or form -- that way we'll easily be able to check if
                all commands from old-style-vtss-cli-cmds.txt are accounted
                for in the new-style files.

   (Hint: You could leave the full list of old-style commands in the top of
          the file, and then use shorter-form versions when you do the actual
          specification of the new-style versions.)

3. Start with the easy commands and leave the hard ones for later, until
   you're more experienced designing new-style commands.

4. Run soft02:/proj/sw/icli-cmd-specs/bin/asm_specs.rb on your file frequently
   to catch most problems.

5. Run asm_specs.rb on cmd/*.txt before review to see if there are conflicts
   with other commands. See if you can resolve them yourself, or in cooperation
   with the designer of the conflicting command.

6. Send mail to martine@vitesse.com when your work is ready for review.

7. When you're ready to begin implementation: Use asm_specs.rb to generate a
    skeleton file: asm_specs.rb -o=your-output-file.icli your-input-file.txt


Hint: Consult early. If you have a hard time getting started, please don't
      hesitate contacting VICLICS!  :-)

Hint: Share often. This allows us (and MartinE in particular) to have an idea
      about how we're doing, plus it lets us catch problems early. (Ideally,
      let the file live in the above-mentioned directory.)



"VICLICS" -- Vitesse ICLI Consulting Service
============================================

If you have questions about your command design you can contact Martin E,
martine@vitesse.com. I will probably not have all the answers, but I may
occasionally have useful input -- or annoying questions (the first of which
is almost guaranteed to be: Did you read the entire file? :-)



File Layout
===========

A spec file is made up of command mode sections: The first line in a section
indicates the command mode, the following lines (which must be indented)
indicate commands under that command mode.

Commands that span multiple lines must be further indented, like:

global config
  this is a very long command line with even
    more command words
  and this is the second command under global
    config

Comments must be on their own line and begin with '!' as first non-
whitespace character.

If a comment is indented and begins with '!!', it indicates that the text
is an old-style VTSS CLI command; this is used to have a tool, asm_specs.rb,
output a table of old => new command mappings. See the example below.

An empty text (i.e. line = '  !!') clears the old-style command. Thus, it
is possible to write one old-style command that spans several new-style
commands. Please note that the '!!' comment must be indented to the same
level as the command, and that the feature doesn't apply for modes.

Please refer to AN1047, section 3.1, for details about the command
syntax; and appendix A for details about the supported data types.

This file extends the type syntax a bit in the hope that it'll make the
command variables easier to understand:

   ICLI specifies variable types with this syntax: '<' type-name '>', e.g.
   '<vlan_id>' or '<0-100>'.

   In this file* you can prepend the type-name with a variable name, like:
   '<new_tag:vlan_id>', or '<percent:0-100>'. It makes the command more
   readable, plus it helps asm_specs.rb to generate more useful ICLI skeleton
   code.

NOTE: It is valid to introduce new modes when necessary, but don't
      do it lightly.



File Placement and Naming
=========================

Since these files are short-lived, we won't put them inside each module.
Instead, please:

  - Put your file(s) in soft02:/proj/sw/icli-cmd-specs/cmd/
  - Name them as: modulename.txt -- e.g. vlan.txt. If you have several files
    for one module, name them like this: modulename-something-specific.txt

The directory tree:

    /proj/sw/icli-cmd-specs
    |
    |-- bin
    |   `-- asm_specs.rb             # a tool, see below
    |-- cmd
    |   |-- auth.txt                 # a command file
    |   |-- ...                      # ... and more command files...
    |   `-- ...
    |-- example.txt                  # an example, see below
    |-- icli-cmd-spec-README.txt     # this file
    |-- icli-cmd-spec-template.txt   # template command file
    `-- old-style-vtss-cli-cmds.txt  # list of old-style VTSS CLI commands



Tools
=====

A tool is available in soft02:/proj/sw/icli-cmd-specs in the bin directory:

  bin/asm_specs.rb [-o=filename.icli] cmd/filename1.txt [ cmd/filename2.txt ... ]

will load one or more input files (usually from the cmd dir), assemble the
various sections, do some simple syntax validation, output the result and
also output the old-style => new-style command map. If you add the -o=... option
it will also generate a skeleton .icli file that's a good starting point for
your implementation.



"no" and "default" Forms
========================

Many configuration commands have a corresponding "no" and/or "default" form.
"no" either removes configuration or resets it to its default settings;
"default", if present, always resets to default.

Depending on the command, the syntax for "no"/"default" forms may be
different; the "no" form often doesn't need the full set of parameters
that the configuration command needs, but may allow them nonetheless.
Same for "default". See "ip address" in the example below for such a case.

See the Twiki page for more details about this subject.



Modes
=====

The following modes are pre-defined:

  - User exec: Unprivileged commands; usually only monitoring commands
  - Privileged exec: Privileged commands; again mainly monitoring
  - Global config: System-wide configuration (system-wide = not in a sub-
    mode)
  - Several config sub-modes: Contains commands for interfaces, VLANs,
    terminal lines.



Debug Commands
==============

For the first releases we'll include both VTSS CLI and ICLI, so the existing
debug commands will be available there. Therefore, don't bother porting them
at this time.

If new debug commands are needed, they should be put under privileged exec,
first word 'debug', but try to avoid them for now; we may need to develop our
debugging model a bit before we're ready for migration to ICLI.

NOTE: The common industry debug model is different from VTSS' model: Our debug
      commands are used to display tables, change internal settings, erase
      flash, enable tracing and what have you.

      Usually, 'debug' commands on other equipment turn on logging of certain
      event-related output, for example the reception of a particular kind of
      STP PDU, or transitions of the 802.3ah OAM state machine.

      To turn specific debugging off a "no" form is used, or the "undebug all"
      or "no debug all" commands for removal of all debugging.



Example
=======

(This file is in the above-mentioned directory under the name 'example.txt')

!==============================================================================
! We begin with global commands in user exec mode. They are unprivileged, i.e.
! any user who can log in can execute them. Note the old-style-VTSS-cmd
! comment.

user exec
  !! System Version
  show version

!==============================================================================
! Then commands for a privileged user ("super user"). Fine tuning of privi-
! lege level is done per command in the .icli file. Again, note the old-style
! command comments; in particular how the second clears the comment.

privileged exec
  !! System Config All
  show running-config
  !!
  clear counters <port_type> <port_list>

!==============================================================================
! Then the global configuration section. It contains all configuration that
! doesn't belong in a vlan/interface/line section.

global config
  ip http server
  no ip http server

!==============================================================================
! The VLAN section. It is used for defining VLAN attributes and enabling/
! disabling specific VLANs.

vlan <vlan_id>
  shutdown
  no shutdown
  mtu <576-18190>
  no mtu

!==============================================================================
! Common interface commands. There are several commands that are relevant both
! to VLAN interfaces and to Ethernet interfaces -- they go here.
! Again, the old-style command comments: Here they span several new-style
! commands.

interface common
  !! IP Setup [<ip_addr>] [<ip_mask>] [<ip_router>] [<vid>]
  ip address <ip:ipv4_ucast> <netmask:ipv4_netmask>
  no ip address [ <ip:ipv4_ucast> <netmask:ipv4_netmask> ]
  default ip address [ <ip:ipv4_ucast> <netmask:ipv4_netmask> ]
  !! Port State [<port_list>] [enable|disable]
  shutdown
  no shutdown

!==============================================================================
! If there are any VLAN-specific commands that aren't covered by the common
! case above, put them here

interface vlan <vlan_id>

!==============================================================================
! Same for Ethernet interface commands.

interface ethernet <port_id>
  speed { 10 | 100 | 1000 | 2500 | auto }
  duplex { full | half | auto }

!==============================================================================
! Then it's line settings (for console, telnet, ssh)

line
  speed <baudrate:uint>
  exec-timeout <minutes:0-30000> <seconds:0-2147483>

!==============================================================================
! Finally, an example of how to define a new configuration sub-mode.

my own sub-mode
  my own command

!==============================================================================
! End of example.
!==============================================================================


And here's an example of running it:

martine@soft02:~ $ cd /proj/sw/icli-cmd-specs/
martine@soft02:/proj/sw/icli-cmd-specs $ bin/asm_specs.rb example.txt

! Loaded commands:
!
global config
  ! File: example.txt
  ip http server
  no ip http server

interface common
  ! File: example.txt
  ip address <ip:ipv4_ucast> <netmask:ipv4_netmask>
  no ip address [ <ip:ipv4_ucast> <netmask:ipv4_netmask> ]
  default ip address [ <ip:ipv4_ucast> <netmask:ipv4_netmask> ]
  shutdown
  no shutdown

interface ethernet <port_id>
  ! File: example.txt
  speed { 10 | 100 | 1000 | 2500 | auto }
  duplex { full | half | auto }

line
  ! File: example.txt
  speed <baudrate:uint>
  exec-timeout <minutes:0-30000> <seconds:0-2147483>

my own sub-mode
  ! File: example.txt
  my own command

privileged exec
  ! File: example.txt
  show running-config
  clear counters <port_type> <port_list>

user exec
  ! File: example.txt
  show version

vlan <vlan_id>
  ! File: example.txt
  shutdown
  no shutdown
  mtu <576-18190>
  no mtu




! VTSS CLI => ICLI command mapping:
!
! global config
!  (none)                                => ip http server
!  (none)                                => no ip http server
!
! interface common
!  IP Setup [<ip_addr>] [<ip_mask>] [<ip_router>] [<vid>] => ip address <ip:ipv4_ucast> <netmask:ipv4_netmask>
!  IP Setup [<ip_addr>] [<ip_mask>] [<ip_router>] [<vid>] => no ip address [ <ip:ipv4_ucast> <netmask:ipv4_netmask> ]
!  IP Setup [<ip_addr>] [<ip_mask>] [<ip_router>] [<vid>] => default ip address [ <ip:ipv4_ucast> <netmask:ipv4_netmask> ]
!  Port State [<port_list>] [enable|disable] => shutdown
!  Port State [<port_list>] [enable|disable] => no shutdown
!
! interface ethernet <port_id>
!  (none)                                => speed { 10 | 100 | 1000 | 2500 | auto }
!  (none)                                => duplex { full | half | auto }
!
! line
!  (none)                                => speed <baudrate:uint>
!  (none)                                => exec-timeout <minutes:0-30000> <seconds:0-2147483>
!
! my own sub-mode
!  (none)                                => my own command
!
! privileged exec
!  (none)                                => clear counters <port_type> <port_list>
!  System Config All                     => show running-config
!
! user exec
!  System Version                        => show version
!
! vlan <vlan_id>
!  (none)                                => shutdown
!  (none)                                => no shutdown
!  (none)                                => mtu <576-18190>
!  (none)                                => no mtu
!

! Statistics:
!   Commands total      :    19
!   Files total         :     1
!   Lines total         :    82



And here's an example if a file contains errors (some output omitted):

martine@soft02:/proj/sw/icli-cmd-specs $ bin/asm_specs.rb errors.txt
ERROR: ICLI syntax: Unmatched ']' (depth 2)
errors.txt: Line     7:   nest_err{one{two{three[four|five]}]six}seven
ERROR: ICLI syntax: Unknown type: no-such-type
errors.txt: Line     9:   type_err <v:no-such-type>
ERROR: ICLI syntax: Unexpected input: <foo
errors.txt: Line    10:   var_err <foo
...
! Statistics:
!   Commands total      :     4
!   ERROR: ICLI syntax  :     4      <<==== Note!
!   Files total         :     1
!   Lines total         :    20

End of file.
