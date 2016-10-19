#include "dma-ext.h"
#include <stdio.h>
#include <string.h>

#define TEST_SIZE 400

int src_array[TEST_SIZE];
int dst_array[TEST_SIZE];

int main(void)
{
	int i;

	printf("Starting DMA test\n");

	for (i = 0; i < TEST_SIZE; i++)
		src_array[i] = (i << 16) | i;

#ifdef DMA
	dma_write_cr(SEGMENT_SIZE, TEST_SIZE * sizeof(int));
	dma_write_cr(NSEGMENTS, 1);
	dma_write_cr(SRC_STRIDE, 0);
	dma_write_cr(DST_STRIDE, 0);
	asm volatile ("fence");
	dma_set_cr(ACCEL_CTRL, CTRL_ALLOC_PAUSE);
	dma_transfer(dst_array, src_array);
	asm volatile ("fence");
	if (dma_read_cr(RESP_STATUS) != PAUSED) {
		fprintf(stderr, "unexpectedly not paused\n");
		return -1;
	}
	dma_clear_cr(ACCEL_CTRL, CTRL_ALLOC_PAUSE);
	dma_resume();
	asm volatile ("fence");
	if (dma_read_cr(RESP_STATUS) != NO_ERROR) {
		fprintf(stderr, "unexpectedly still paused\n");
		return -1;
	}
#else
	memcpy(dst_array, src_array, sizeof(dst_array));
#endif

	for (i = 0; i < TEST_SIZE; i++) {
		if (dst_array[i] != src_array[i]) {
			printf("Error: expected %d, got %d\n",
				src_array[i], dst_array[i]);
			return -1;
		}
	}

	printf("DMA test complete\n");

	return 0;
}
