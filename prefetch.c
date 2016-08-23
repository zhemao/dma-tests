#include "dma-ext.h"
#include <stdlib.h>
#include <stdio.h>

#define TEST_SIZE 4096

char rd_section[2 * TEST_SIZE];
char wr_section[2 * TEST_SIZE];

void read_block(char *data)
{
	char value;

	for (int i = 0; i < TEST_SIZE; i++) {
		value = data[i];
		if (value != 0)
			return;
	}
}

void write_block(char *data)
{
	for (int i = 0; i < TEST_SIZE; i++)
		data[i] = (i & 0xff);
}

int main(void)
{
	dma_write_cr(SEGMENT_SIZE, TEST_SIZE);
	dma_write_cr(NSEGMENTS, 1);
	dma_write_cr(SRC_STRIDE, 0);
	dma_write_cr(DST_STRIDE, 0);

	dma_read_prefetch(rd_section);
	asm volatile ("fence");
	read_block((char *) rd_section + TEST_SIZE);
	read_block((char *) rd_section);

	dma_write_prefetch(wr_section);
	asm volatile ("fence");
	write_block(wr_section + TEST_SIZE);
	write_block(wr_section);

	return 0;
}
