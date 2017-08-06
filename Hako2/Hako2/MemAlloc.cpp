#include "all.h"

void *REAL_memAlloc(int size)
{
	void *block;

	errorCase(size < 0 || IMAX < size);
	block = malloc(m_max(size, 1));
	errorCase(!block);

	return block;
}
void *REAL_memRealloc(void *block, int size)
{
	errorCase(size < 0 || IMAX < size);
	block = realloc(block, m_max(size, 1));
	errorCase(!block);

	return block;
}
void REAL_memFree(void *block)
{
	free(block);
}

#define HUGEBLOCK_NUMMAX 1024

static void *HugeBlocks[HUGEBLOCK_NUMMAX];
static int HugeBlockCount;

static int GetHugeBlockIndex(void *block)
{
	for(int index = 0; index < HugeBlockCount; index++)
	{
		if(HugeBlocks[index] == block)
		{
			return index;
		}
	}
	return -1; // not found
}

#define K1 1024
#define K4 4096
#define K16 16384
#define K64 65536
#define K256 262144

static MBStock *Stock4;
static MBStock *Stock16;
static MBStock *Stock64;
static MBStock *Stock256;
static MBStock *Stock1K;
static MBStock *Stock4K;
static MBStock *Stock16K;
static MBStock *Stock64K;
static MBStock *Stock256K;

void memAlloc_INIT(void)
{
	Stock4    = new MBStock(4);
	Stock16   = new MBStock(16);
	Stock64   = new MBStock(64);
	Stock256  = new MBStock(256);
	Stock1K   = new MBStock(K1);
	Stock4K   = new MBStock(K4);
	Stock16K  = new MBStock(K16);
	Stock64K  = new MBStock(K64);
	Stock256K = new MBStock(K256);
}

#if 0 // test
void *memAlloc(int size)
{
	return REAL_memAlloc(size);
}
void *memRealloc(void *block, int size)
{
	return REAL_memRealloc(block, size);
}
void memFree(void *block)
{
	return REAL_memFree(block);
}
#else
void *memAlloc_NC(int size)
{
	if(size < 4)    return Stock4->GetBlock();
	if(size < 16)   return Stock16->GetBlock();
	if(size < 64)   return Stock64->GetBlock();
	if(size < 256)  return Stock256->GetBlock();
	if(size < K1)   return Stock1K->GetBlock();
	if(size < K4)   return Stock4K->GetBlock();
	if(size < K16)  return Stock16K->GetBlock();
	if(size < K64)  return Stock64K->GetBlock();
	if(size < K256) return Stock256K->GetBlock();

	errorCase(HUGEBLOCK_NUMMAX <= HugeBlockCount);

	void *block = REAL_memAlloc(size);

	HugeBlocks[HugeBlockCount] = block;
	HugeBlockCount++;

	return block;
}
void *memRealloc(void *block, int size)
{
	if(!block)
		return memAlloc(size);

	int bSize = 0;

	     if(Stock4->IsGivenBlock(block))    bSize = 4;
	else if(Stock16->IsGivenBlock(block))   bSize = 16;
	else if(Stock64->IsGivenBlock(block))   bSize = 64;
	else if(Stock256->IsGivenBlock(block))  bSize = 256;
	else if(Stock1K->IsGivenBlock(block))   bSize = K1;
	else if(Stock4K->IsGivenBlock(block))   bSize = K4;
	else if(Stock16K->IsGivenBlock(block))  bSize = K16;
	else if(Stock64K->IsGivenBlock(block))  bSize = K64;
	else if(Stock256K->IsGivenBlock(block)) bSize = K256;
	else
	{
		int index = GetHugeBlockIndex(block);
		errorCase(index == -1);

		block = REAL_memRealloc(block, size);
		HugeBlocks[index] = block;
		return block;
	}

	if(bSize < size)
	{
		void *newBlock = memAlloc_NC(size);
		memcpy(newBlock, block, bSize);
		memset((uchar *)newBlock + bSize, 0x00, size - bSize);
		memFree(block);
		block = newBlock;
	}
	return block;
}
void memFree(void *block)
{
	if(!block)
		return;

	if(Stock4->TryTakeBlock(block))    return;
	if(Stock16->TryTakeBlock(block))   return;
	if(Stock64->TryTakeBlock(block))   return;
	if(Stock256->TryTakeBlock(block))  return;
	if(Stock1K->TryTakeBlock(block))   return;
	if(Stock4K->TryTakeBlock(block))   return;
	if(Stock16K->TryTakeBlock(block))  return;
	if(Stock64K->TryTakeBlock(block))  return;
	if(Stock256K->TryTakeBlock(block)) return;

	int index = GetHugeBlockIndex(block);
	errorCase(index == -1);

	HugeBlockCount--;
	HugeBlocks[index] = HugeBlocks[HugeBlockCount];

	REAL_memFree(block);
}
#endif

void *memAlloc(int size)
{
	void *block = memAlloc_NC(size);

	memset(block, 0x00, size);
	return block;
}
void *memClone(void *block, int size)
{
	void *newBlock = memAlloc_NC(size);

	memcpy(newBlock, block, size);
	return newBlock;
}
