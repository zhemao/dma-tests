CC=riscv64-unknown-elf-gcc
CFLAGS=-Wall -O2
LDFLAGS=-static

PROGRAMS=simple matrix prefetch
EXECUTABLES=$(addsuffix .riscv,$(PROGRAMS))

all: $(EXECUTABLES)

%.riscv: %.o
	$(CC) $< $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(EXECUTABLES)
