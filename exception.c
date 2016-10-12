#include "dma-ext.h"
#include <stdlib.h>

int main(void)
{
	int src = 0;

	dma_write_cr(SEGMENT_SIZE, 4);
	dma_write_cr(NSEGMENTS, 1);
	dma_transfer(NULL, &src);

	return 0;
}
