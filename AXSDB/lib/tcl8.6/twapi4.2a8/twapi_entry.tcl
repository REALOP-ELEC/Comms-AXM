# -*- tcl -*-
namespace eval twapi {
    variable version
    set version(twapi)     4.2a8
    variable patchlevel    4.2a8
    variable package_name  twapi
    variable dll_base_name twapi[string map {. {}} 4.2a8]
    variable scriptdir     [file dirname [info script]]
}

source [file join $twapi::scriptdir twapi.tcl]
