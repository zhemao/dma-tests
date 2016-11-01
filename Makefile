CC=riscv64-unknown-elf-gcc
CFLAGS=-Wall -O2 -DDMA
LDFLAGS=-static
OBJDUMP=riscv64-unknown-elf-objdump
LNX_CC=riscv64-unknown-linux-gnu-gcc
LNX_OBJDUMP=riscv64-unknown-linux-gnu-objdump

PROGRAMS=simple matrix prefetch hello
LNX_PROGRAMS=mmap exception simple matrix
EXECUTABLES=$(addsuffix .pk.riscv,$(PROGRAMS)) $(addsuffix .lnx.riscv,$(LNX_PROGRAMS))
DUMPS=$(addsuffix .pk.dump,$(PROGRAMS)) $(addsuffix .lnx.dump,$(LNX_PROGRAMS))

all: $(EXECUTABLES)

dump: $(DUMPS)

%.pk.dump: %.pk.riscv
	$(OBJDUMP) -d $< > $@

%.lnx.dump: %.lnx.riscv
	$(LNX_OBJDUMP) -d $< > $@

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
