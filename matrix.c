#include "dma-ext.h"
#include <stdio.h>
#include <string.h>

#define LARGE_SIZE 25
#define SMALL_SIZE 10

#define ROW_START 10
#define COL_START 5

int large_mat[LARGE_SIZE * LARGE_SIZE];
int small_mat[SMALL_SIZE * SMALL_SIZE];

void check_copy(int r, int c)
{
	int expected = large_mat[(ROW_START + r) * LARGE_SIZE + COL_START + c];
	int actual = small_mat[r * SMALL_SIZE + c];
	if (actual != expected) {
		printf("(%d, %d): Expected %d, got %d\n",
			r, c, expected, actual);
	}
}

void copy_data(int *dst, int *src, int dst_cols, int src_cols, int nrows)
{
#ifdef DMA
	dma_write_cr(SEGMENT_SIZE, dst_cols * sizeof(int));
	dma_write_cr(NSEGMENTS, nrows);
	dma_write_cr(SRC_STRIDE, (src_cols - dst_cols) * sizeof(int));
	dma_write_cr(DST_STRIDE, 0);

	asm volatile ("fence");
	dma_transfer(dst, src);
	asm volatile ("fence");
#else
	for (int r = 0; r < nrows; r++)
		memcpy(dst + dst_cols * r, src + src_cols * r, dst_cols * sizeof(int));
#endif
}

int main(void)
{
	int *src, *dst;
	int firstrow = 0, lastrow = 10;
	int i, r, c;

	printf("Starting Matrix test\n");

	for (i = 0; i < LARGE_SIZE * LARGE_SIZE; i++)
		large_mat[i] = i;

	src = &large_mat[(ROW_START + firstrow) * LARGE_SIZE + COL_START];
	dst = &small_mat[firstrow * SMALL_SIZE];

	copy_data(dst, src, SMALL_SIZE, LARGE_SIZE, lastrow - firstrow);

	for (r = firstrow; r < lastrow; r++) {
		printf("Scanning row %d (src: %p, dst: %p)\n", r,
			&large_mat[(ROW_START + r) * LARGE_SIZE + COL_START],
			&small_mat[r * SMALL_SIZE]);
		for (c = 0; c < SMALL_SIZE; c++)
			check_copy(r, c);
	}

	printf("Finished Matrix test\n");

	return 0;
}
