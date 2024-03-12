#
# Tcl package index file
#

namespace eval twapi {
    variable scriptdir
    proc set_scriptdir dir {variable scriptdir ; set scriptdir $dir}
}

package ifneeded twapi_base 4.2a8 \
  [list load [file join $dir twapi42a8.dll] twapi_base]
package ifneeded twapi_com 4.2a8 \
  {load {} twapi_com}
package ifneeded metoo 4.2a8 \
  [list source [file join $dir metoo.tcl]]
package ifneeded twapi_com 4.2a8 \
  {load {} twapi_com}
package ifneeded twapi_msi 4.2a8 \
  [list source [file join $dir msi.tcl]]
package ifneeded twapi_power 4.2a8 \
  [list source [file join $dir power.tcl]]
package ifneeded twapi_printer 4.2a8 \
  [list source [file join $dir printer.tcl]]
package ifneeded twapi_synch 4.2a8 \
  [list source [file join $dir synch.tcl]]
package ifneeded twapi_security 4.2a8 \
  {load {} twapi_security}
package ifneeded twapi_account 4.2a8 \
  {load {} twapi_account}
package ifneeded twapi_apputil 4.2a8 \
  {load {} twapi_apputil}
package ifneeded twapi_clipboard 4.2a8 \
  {load {} twapi_clipboard}
package ifneeded twapi_console 4.2a8 \
  {load {} twapi_console}
package ifneeded twapi_crypto 4.2a8 \
  {load {} twapi_crypto}
package ifneeded twapi_device 4.2a8 \
  {load {} twapi_device}
package ifneeded twapi_etw 4.2a8 \
  {load {} twapi_etw}
package ifneeded twapi_eventlog 4.2a8 \
  {load {} twapi_eventlog}
package ifneeded twapi_mstask 4.2a8 \
  {load {} twapi_mstask}
package ifneeded twapi_multimedia 4.2a8 \
  {load {} twapi_multimedia}
package ifneeded twapi_namedpipe 4.2a8 \
  {load {} twapi_namedpipe}
package ifneeded twapi_network 4.2a8 \
  {load {} twapi_network}
package ifneeded twapi_nls 4.2a8 \
  {load {} twapi_nls}
package ifneeded twapi_os 4.2a8 \
  {load {} twapi_os}
package ifneeded twapi_pdh 4.2a8 \
  {load {} twapi_pdh}
package ifneeded twapi_process 4.2a8 \
  {load {} twapi_process}
package ifneeded twapi_rds 4.2a8 \
  {load {} twapi_rds}
package ifneeded twapi_resource 4.2a8 \
  {load {} twapi_resource}
package ifneeded twapi_service 4.2a8 \
  {load {} twapi_service}
package ifneeded twapi_share 4.2a8 \
  {load {} twapi_share}
package ifneeded twapi_shell 4.2a8 \
  {load {} twapi_shell}
package ifneeded twapi_storage 4.2a8 \
  {load {} twapi_storage}
package ifneeded twapi_ui 4.2a8 \
  {load {} twapi_ui}
package ifneeded twapi_input 4.2a8 \
  {load {} twapi_input}
package ifneeded twapi_winsta 4.2a8 \
  {load {} twapi_winsta}
package ifneeded twapi_wmi 4.2a8 \
  {load {} twapi_wmi}

package ifneeded twapi 4.2a8 [subst {
  twapi::set_scriptdir $dir
  package require twapi_base 4.2a8
  source [file join $dir twapi_entry.tcl]
  package require metoo 4.2a8
  package require twapi_com 4.2a8
  package require twapi_msi 4.2a8
  package require twapi_power 4.2a8
  package require twapi_printer 4.2a8
  package require twapi_synch 4.2a8
  package require twapi_security 4.2a8
  package require twapi_account 4.2a8
  package require twapi_apputil 4.2a8
  package require twapi_clipboard 4.2a8
  package require twapi_console 4.2a8
  package require twapi_crypto 4.2a8
  package require twapi_device 4.2a8
  package require twapi_etw 4.2a8
  package require twapi_eventlog 4.2a8
  package require twapi_mstask 4.2a8
  package require twapi_multimedia 4.2a8
  package require twapi_namedpipe 4.2a8
  package require twapi_network 4.2a8
  package require twapi_nls 4.2a8
  package require twapi_os 4.2a8
  package require twapi_pdh 4.2a8
  package require twapi_process 4.2a8
  package require twapi_rds 4.2a8
  package require twapi_resource 4.2a8
  package require twapi_service 4.2a8
  package require twapi_share 4.2a8
  package require twapi_shell 4.2a8
  package require twapi_storage 4.2a8
  package require twapi_ui 4.2a8
  package require twapi_input 4.2a8
  package require twapi_winsta 4.2a8
  package require twapi_wmi 4.2a8
  package provide twapi 4.2a8
}]
