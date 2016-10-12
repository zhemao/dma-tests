CC=riscv64-unknown-elf-gcc
CFLAGS=-Wall -O2
LDFLAGS=-static
OBJDUMP=riscv64-unknown-elf-objdump

PROGRAMS=simple matrix prefetch hello exception
EXECUTABLES=$(addsuffix .riscv,$(PROGRAMS))
DUMPS=$(addsuffix .dump,$(PROGRAMS))

all: $(EXECUTABLES)

dump: $(DUMPS)

%.dump: %.riscv
	$(OBJDUMP) -d $< > $@

%.riscv: %.o
	$(CC) $< $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(EXECUTABLES)
