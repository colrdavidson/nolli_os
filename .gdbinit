set architecture i8086
set disassembly-flavor intel
target remote :1234
br *0x7eb0
set architecture auto
