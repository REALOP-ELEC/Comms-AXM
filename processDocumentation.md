example using config file psoc4.cfg
```source [find interface/cmsis-dap.cfg]

transport select swd

source [find target/psoc4.cfg]

reset_config srst_only

gdb_memory_map disable
```

then run `openocd -f pscoc4test.cfg`

in a separate terminal window/tab, run `telnet localhost 4444`
within that terminal window, run
```
flash banks
init
reset init
psoc4 mass_erase 0
flash write_image ux2d1minicast1.elf
flash verify_image ux2d1minicast1.elf
reset run

```


note that many of the above lines are for redundancy and sanity checking, the strict minimum needed is the following
```
init
reset init
flash write_image ux2d1minicas1.elf
reset run

```
