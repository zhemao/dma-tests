CC=riscv64-unknown-elf-gcc
CFLAGS=-Wall -O2 -DDMA
LDFLAGS=-static
OBJDUMP=riscv64-unknown-elf-objdump
LNX_CC=riscv64-unknown-linux-gnu-gcc

PROGRAMS=simple matrix prefetch hello
LNX_PROGRAMS=mmap exception simple matrix
EXECUTABLES=$(addsuffix .pk.riscv,$(PROGRAMS)) $(addsuffix .lnx.riscv,$(LNX_PROGRAMS))
DUMPS=$(addsuffix .dump,$(PROGRAMS))

all: $(EXECUTABLES)

dump: $(DUMPS)

%.dump: %.riscv
	$(OBJDUMP) -d $< > $@

%.pk.riscv: %.pk.o
	$(CC) $< $(LDFLAGS) -o $@

%.lnx.riscv: %.lnx.o
	$(LNX_CC) $< $(LDFLAGS) -o $@

%.pk.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.lnx.o: %.c
	$(LNX_CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.riscv *.dump
