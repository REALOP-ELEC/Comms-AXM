namespace eval ::axsdb {
    variable axsdb_termecho
    variable axsdb_stop

    proc inforeg {} {
	set pc   [read_pc]
	if {[chan read cpustat 1] ne "H"} {
	    puts [format "PC %04X" ${pc}]
	    return;
	}
	set acc  [read_mem --sfr 0xe0]
	set b    [read_mem --sfr 0xf0]
	set psw  [read_mem --sfr 0xd0]
	set sp   [read_mem --sfr 0x81]
	set ie   [read_mem --sfr 0xa8]
	set dps  [read_mem --sfr 0x86]
	set dpl0 [read_mem --sfr 0x82]
	set dph0 [read_mem --sfr 0x83]
	set dpl1 [read_mem --sfr 0x84]
	set dph1 [read_mem --sfr 0x85]
	set r00  [read_mem --direct 0x00]
	set r01  [read_mem --direct 0x01]
	set r02  [read_mem --direct 0x02]
	set r03  [read_mem --direct 0x03]
	set r04  [read_mem --direct 0x04]
	set r05  [read_mem --direct 0x05]
	set r06  [read_mem --direct 0x06]
	set r07  [read_mem --direct 0x07]
	set r10  [read_mem --direct 0x08]
	set r11  [read_mem --direct 0x09]
	set r12  [read_mem --direct 0x0a]
	set r13  [read_mem --direct 0x0b]
	set r14  [read_mem --direct 0x0c]
	set r15  [read_mem --direct 0x0d]
	set r16  [read_mem --direct 0x0e]
	set r17  [read_mem --direct 0x0f]
	set r20  [read_mem --direct 0x10]
	set r21  [read_mem --direct 0x11]
	set r22  [read_mem --direct 0x12]
	set r23  [read_mem --direct 0x13]
	set r24  [read_mem --direct 0x14]
	set r25  [read_mem --direct 0x15]
	set r26  [read_mem --direct 0x16]
	set r27  [read_mem --direct 0x17]
	set r30  [read_mem --direct 0x18]
	set r31  [read_mem --direct 0x19]
	set r32  [read_mem --direct 0x1a]
	set r33  [read_mem --direct 0x1b]
	set r34  [read_mem --direct 0x1c]
	set r35  [read_mem --direct 0x1d]
	set r36  [read_mem --direct 0x1e]
	set r37  [read_mem --direct 0x1f]
	puts [format "PC %04X SP %02X PSW %02X ACC %02X B %02X IE %02X DPS %02X DPTR %04X DPTR1 %04X" ${pc} ${sp} ${psw} ${acc} ${b} ${ie} ${dps} [expr ${dpl0}|(${dph0}<<8)] [expr ${dpl1}|(${dph1}<<8)] ]
	puts [format "BANK0: R0 %02X R1 %02X R2 %02X R3 %02X R4 %02X R5 %02X R6 %02X R7 %02X" ${r00} ${r01} ${r02} ${r03} ${r04} ${r05} ${r06} ${r07} ]
	puts [format "BANK1: R0 %02X R1 %02X R2 %02X R3 %02X R4 %02X R5 %02X R6 %02X R7 %02X" ${r10} ${r11} ${r12} ${r13} ${r14} ${r15} ${r16} ${r17} ]
	puts [format "BANK2: R0 %02X R1 %02X R2 %02X R3 %02X R4 %02X R5 %02X R6 %02X R7 %02X" ${r20} ${r21} ${r22} ${r23} ${r24} ${r25} ${r26} ${r27} ]
	puts [format "BANK3: R0 %02X R1 %02X R2 %02X R3 %02X R4 %02X R5 %02X R6 %02X R7 %02X" ${r30} ${r31} ${r32} ${r33} ${r34} ${r35} ${r36} ${r37} ]
	set d [disass [read_pc]]
	puts [format "%04X %-6s %s" [lindex $d 0] [string toupper [lindex $d 1]] [lindex $d 4]]
    }

    proc waitcpustate {{states "H"}} {
	variable ::axsdb::axsdb_stop
	set ::axsdb::axsdb_stop 0
	variable ::axsdb::axsdb_termecho
	set ::axsdb::axsdb_termecho ${states}
	chan event cpustat readable {
	    variable ::axsdb::axsdb_stop
	    variable ::axsdb::axsdb_termecho
	    set st [chan read cpustat 1]
	    if {[string match -nocase ${::axsdb::axsdb_termecho} ${st}]} {
		set ::axsdb::axsdb_stop 1
	    }
	}
	set st [chan read cpustat 1]
	if {[string match -nocase ${::axsdb::axsdb_termecho} ${st}]} {
	    set ::axsdb::axsdb_stop 1
	}
	if {$::axsdb::axsdb_stop == 0} {
	    vwait ::axsdb::axsdb_stop
	}
	chan event cpustat readable {}
    }

    proc waitcpustopped {} {
	waitcpustate "H"
    }

    proc waitcpurunning {} {
	waitcpustate "R"
    }

    proc ir {} {
	stop
	waitcpustopped
	inforeg
    }

    proc sr {} {
	ir
	variable ::axsdb::axsdb_stop
	set ::axsdb::axsdb_stop 0
	set st [chan read cpustat 1]
	chan event cpustat readable {
	    variable ::axsdb::axsdb_stop
	    set st [chan read cpustat 1]
	    if {$st eq "H"} {
		set ::axsdb::axsdb_stop 1
	    }
	}
	step
	vwait ::axsdb::axsdb_stop
	chan event cpustat readable {}
	ir
    }

    proc clrdbglink {} {
	set x [chan pending input dbglink]
	if {$x > 0} {
	    chan read dbglink $x
	}
    }

    proc rst {} {
	stop
	waitcpustopped
	clrdbglink
	write_pc 0
	write_mem --sfr 0xe0 0x00
	write_mem --sfr 0xf0 0x00
	write_mem --sfr 0xd0 0x00
	write_mem --sfr 0x81 0x07
	write_mem --sfr 0xa8 0x00
	write_mem --sfr 0x86 0x00
	write_mem --sfr 0x82 0x00
	write_mem --sfr 0x83 0x00
	write_mem --sfr 0x84 0x00
	write_mem --sfr 0x85 0x00
	write_mem --sfr 0x97 0xD7
    }

    proc rstf {} {
	rst
	write_mem --sfr 0x97 0xD0
    }

    proc toASCII { char } {
	scan $char %c value
	return $value
    }

    proc toChar { value } {
	return [format %c $value]
    }

    if { [ string equal $tcl_platform(platform) "unix" ] } {
	set axsdb_tracedir "/tmp"

	proc saveRaw {{channel stdin}} {
	    return [exec /bin/stty -g <@$channel]
	}

	proc restoreRaw {{channel stdin} rest} {
	    exec /bin/stty $rest <@$channel
	}

	proc enableRaw {{channel stdin}} {
	    exec /bin/stty raw -echo <@$channel
	}

	proc disableRaw {{channel stdin}} {
	    exec /bin/stty -raw echo <@$channel
	}
    } else {
	set axsdb_tracedir "C:/temp"

	package require twapi_base

	proc saveRaw {{channel stdin}} {
	    set console_handle [twapi::GetStdHandle -10]
	    set oldmode [twapi::GetConsoleMode $console_handle]
	    return [expr {$oldmode & 6}]
	}

	proc restoreRaw {{channel stdin} rest} {
	    set console_handle [twapi::GetStdHandle -10]
	    set oldmode [twapi::GetConsoleMode $console_handle]
	    set newmode [expr {($oldmode & ~6) | ($rest & 6)}]
	    twapi::SetConsoleMode $console_handle $newmode
	}

	proc enableRaw {{channel stdin}} {
	    set console_handle [twapi::GetStdHandle -10]
	    set oldmode [twapi::GetConsoleMode $console_handle]
	    set newmode [expr {$oldmode & ~6}] ;# Turn off the echo and line-editing bits
	    twapi::SetConsoleMode $console_handle $newmode
	}

	proc disableRaw {{channel stdin}} {
	    set console_handle [twapi::GetStdHandle -10]
	    set oldmode [twapi::GetConsoleMode $console_handle]
	    set newmode [expr {$oldmode | 6}] ;# Turn on the echo and line-editing bits
	    twapi::SetConsoleMode $console_handle $newmode
	}
    }

    if {[file isdirectory ${axsdb_tracedir}]} {
	traceio --normal "${axsdb_tracedir}/axsdbtrace.log"
	#traceio --poll "${axsdb_tracedir}/axsdbtrace.log"
	#traceio --lowlevelio "${axsdb_tracedir}/axsdbtrace.log"
    }

    proc ri {{echo 0}} {
	set st [chan read cpustat 1]
	if {${st} eq "D"} {
	    connect
	}
	set st [chan read cpustat 1]
	if {${st} eq "L"} {
	    return
	}
	inforeg
	fconfigure stdin -buffering none
	fconfigure stdout -buffering none
	fconfigure dbglink -buffering none
	set savestdin [saveRaw stdin]
	enableRaw stdin
	variable ::axsdb::axsdb_termecho
	set ::axsdb::axsdb_termecho $echo
	variable ::axsdb::axsdb_stop
	set ::axsdb::axsdb_stop 0
	chan event stdin readable {
	    variable ::axsdb::axsdb_stop
	    variable ::axsdb::axsdb_termecho
	    set ch [chan read stdin 1]
	    set cha [::axsdb::toASCII $ch]
	    if {$cha == 1 || $cha == 2 || $cha == 3} {
		set ::axsdb::axsdb_stop $cha
#	    } elseif {$cha == 5} {
#		chan puts stdout ""
#		::axsdb::test
#		chan puts stdout "Test"
	    } else {
		if {$::axsdb::axsdb_termecho != 0} {
		    chan puts -nonewline stdout "$ch"
		}
		chan puts -nonewline dbglink "$ch"
	    }
	}
	chan event dbglink readable {
	    set ch [chan read dbglink 1]
	    chan puts -nonewline stdout "$ch"
	}
	chan event cpustat readable {
	    variable ::axsdb::axsdb_stop
	    set st [chan read cpustat 1]
	    if {(${st} eq "H") || (${st} eq "D")} {
		set ::axsdb::axsdb_stop 1
	    }
	}
	run
	set ::axsdb::axsdb_stop 0
	puts "Microcontroller running; terminal mode; press CTRL-B to stop CPU, CTRL-A to abort terminal"
	vwait ::axsdb::axsdb_stop
	set do_stop 0
	if {$::axsdb::axsdb_stop == 2 || $::axsdb::axsdb_stop == 3} {
	    set do_stop 1
	    stop
	}
	chan event stdin readable {}
	chan event dbglink readable {}
	restoreRaw stdin $savestdin
	fconfigure stdin -buffering line
	fconfigure stdout -buffering line
	if {$do_stop} {
	    set ::axsdb::axsdb_stop 0
	    set st [chan read cpustat 1]
	    if {${st} eq "H"} {
		set ::axsdb::axsdb_stop 1
	    }
	    if {$::axsdb::axsdb_stop == 0} {
		vwait ::axsdb::axsdb_stop
	    }
	}
	chan event cpustat readable {}
	inforeg
    }

    proc aload {fn} {
	stop
	waitcpustopped
	set fne [file extension "${fn}"]
	set fnb [string range "${fn}" 0 end-[string length "${fne}"] ]
	if {"${fne}" eq "" && ![file isfile "${fn}"]} {
	    if [file isfile "${fnb}.omf"] {
		set fn "${fnb}.omf"
		set fne ".omf"
	    } elseif [file isfile "${fnb}.hex"] {
		set fn "${fnb}.hex"
		set fne ".hex"
	    } elseif [file isfile "${fnb}.ihx"] {
		set fn "${fnb}.ihx"
		set fne ".ihx"
	    }
	}
	set fmt "--omf"
	switch -exact -- "${fne}" {
	    ""     {set fmt "--omf"}
	    ".omf" {set fmt "--omf"}
	    ".hex" {set fmt "--hex"}
	    ".ihx" {set fmt "--ihx"}
	}
	puts "loading file ${fn}"
	load_mem ${fmt} "${fn}"
	if [file isfile "${fnb}.cdb"] {
	    puts "loading symbol file ${fnb}.cdb"
	    load_mem --cdb "${fnb}.cdb"
	}
    }

    proc sload {fn} {
	set fne [file extension "${fn}"]
	set fnb [string range "${fn}" 0 end-[string length "${fne}"] ]
	if {"${fne}" eq "" && ![file isfile "${fn}"]} {
	    if [file isfile "${fnb}.omf"] {
		set fn "${fnb}.omf"
		set fne ".omf"
	    } elseif [file isfile "${fnb}.hex"] {
		set fn "${fnb}.hex"
		set fne ".hex"
	    } elseif [file isfile "${fnb}.ihx"] {
		set fn "${fnb}.ihx"
		set fne ".ihx"
	    }
	}
	set fmt "--omf --symbols"
	switch -exact -- "${fne}" {
	    ""     {set fmt "--omf --symbols"}
	    ".omf" {set fmt "--omf --symbols"}
	    ".hex" {set fmt ""}
	    ".ihx" {set fmt ""}
	}
	if {"${fmt}" ne ""} {
	    puts "loading file ${fn}"
	    load_mem ${fmt} "${fn}"
	}
	if [file isfile "${fnb}.cdb"] {
	    puts "loading symbol file ${fnb}.cdb"
	    load_mem --cdb "${fnb}.cdb"
	}
    }

    proc rload {fn} {
	stop
	waitcpustopped
	rstf
	aload ${fn}
	writeback
    }

    proc flashfill {} {
	for {set x 0} {$x<0x10000} {incr x} {
	    write_mem --flash $x 0x8F
	}
    }

    proc berase {} {
	variable ::axsdb::axsdb_stop
	set ::axsdb::axsdb_stop 0
	chan event cpustat readable {
	    variable ::axsdb::axsdb_stop
	    set st [chan read cpustat 1]
	    if {($st eq "H") || ($st eq "R")} {
		set ::axsdb::axsdb_stop 1
	    }
	    if {($st eq "D") || ($st eq "L")} {
		set ::axsdb::axsdb_stop 2
	    }
	}
	bulkerase
	vwait ::axsdb::axsdb_stop
	chan event cpustat readable {}
	if {$::axsdb::axsdb_stop == 2} {
	    clrdbglink
	    return "failed"
	}
	stop
	waitcpustopped
	rstf
	return "done"
    }

    proc axsdb_exit {args} {
	disconnect_target
    }

    fconfigure cpustat -buffering none
    set targs [lsearch -all -inline -not -glob -nocase [list_targets] sim* ]
    puts "Targets: ${targs}"
    trace add execution exit enter axsdb_exit

    if {${::axsdb::serial} ne ""} {
	connect_target ${::axsdb::serial}
	cpustate
	connect
    } elseif {[llength ${targs}] == 1} {
	connect_target [lindex ${targs} 0]
	cpustate
	connect
    }

    #puts [expr {[cpustate --last] ne "halt"}]
    #puts [cpustate --last]

    # core commands
    namespace export traceio list_targets disconnect_target connect_simulator connect_target target_serial
    namespace export read_mem write_mem load_mem read_pc write_pc cpustate connect disconnect
    namespace export hwreset run stop reset step stepline stepinto stepout writeback bulkerase writekey writeprotect eraseprotect
    namespace export breakpoint disass modules sourcelines symbols print quit ctype cexpr
    namespace export registers chips pinemul cputrace profile
    # variables
    namespace export compilervendor
    # channels
    namespace export cpustat dbglink
    # convenience commands
    namespace export inforeg ir sr rst rstf ri aload sload rload flashfill berase clrdbglink
    namespace export waitcpustate waitcpustopped waitcpurunning
}

namespace import axsdb::*
