
# MIPS ELF File to Verilog Memfile

This tool loads a MIPS ELF file, and creates two Verilog memfiles to load into TLB and physical memory.

## Building

Run `make` to create the `"elf2mem"` executable.

## Running

Run `./elf2mem <elf_file>` (ex. `./elf2mem "elfgen/example.o"`)

## Reset Sequence

`"elf2mem.cpp"` adds the following reset sequence to the MIPS reset vector location (`0xbfc00000`):

```mips
        lui $sp, 0x8000
        la $fp, <entry point>
        jr $fp
```
