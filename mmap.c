#include "dma-ext.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PAGESHIFT 12

static inline void pagein(unsigned long vpn)
{
	void *ptr = (void *)(vpn << PAGESHIFT);
	int size = 1 << PAGESHIFT;
	printf("paging in %p\n", ptr);
	mlock(ptr, size);
	//munlock(ptr, size);
}

int main(int argc, char *argv[])
{
	int fd;
	void *dst;
	int *src;
	int size, n, res;

	size = sysconf(_SC_PAGESIZE) * 4;

	if (argc < 2) {
		fprintf(stderr, "Usage: mmap <filename>\n");
		return -1;
	}

	src = malloc(size);
	if (!src) {
		fprintf(stderr, "Could not allocate memory\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR|O_CREAT);
	if (fd < 0) {
		fprintf(stderr, "Could not open file %s\n", argv[1]);
		return -1;
	}

	if (ftruncate(fd, size)) {
		perror("ftruncate");
		return -1;
	}

	dst = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (dst == MAP_FAILED) {
		perror("mmap");
		return -1;
	}

	n = size / sizeof(int);
	for (int i = 0; i < n; i++)
		src[i] = i * 2;

	printf("copying from %p to %p\n", src, dst);

#ifdef DMA
	dma_write_cr(SEGMENT_SIZE, size);
	dma_write_cr(NSEGMENTS, 1);
	dma_transfer(dst, src);

	for (;;) {
		int status;
		unsigned long vpn;
		asm volatile ("fence");
		status = dma_read_cr(RESP_STATUS);
		if (status == NO_ERROR)
			break;
		if (status != SRC_PAGE_FAULT && status != DST_PAGE_FAULT) {
			fprintf(stderr, "Unhandleable error %d", status);
			abort();
		}
		vpn = dma_read_cr(RESP_VPN);
		pagein(vpn);
		dma_resume();
	}
#else
	memcpy(dst, src, size);
#endif

	munmap(dst, size);
	close(fd);

	fd = open(argv[1], O_RDONLY);
	for (int i = 0; i < n; i++) {
		read(fd, &res, sizeof(int));
		if (res != src[i])
			fprintf(stderr, "Bad data %d != %d\n", res, src[i]);
	}
	close(fd);

	return 0;
}
