
elf2mem: elf2mem.cpp
	g++ -I lib/ELFIO $^ -o $@

clean:
	rm -rf elf2mem *.mem
