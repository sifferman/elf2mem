
GCC = mips-linux-gnu-gcc
GCC_FLAGS = -std=gnu99 -O3 -march=mips2 -mfp32 -nostdinc -nostdlib -ffreestanding

%.o: %.c
	$(GCC) $(GCC_FLAGS) $*.c -o $@

%.s: %.c
	$(GCC) -S $(GCC_FLAGS) $*.c -o $@

clean:
	rm -rf *.o *.s
