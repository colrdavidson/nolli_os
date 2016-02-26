set architecture i8086
set disassembly-flavor intel
target remote :1234
br *0x7c00
br *0x7e00
