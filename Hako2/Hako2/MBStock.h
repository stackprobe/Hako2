#define CLUSTER_SIZE 0x400000

class MBStock
{
private:
	uchar *Cluster;
	uchar **StockList;
	uchar *GivenList;
	int StockCount;
	int BlockSize;
	MBStock *Next;

	int GetBlockCount()
	{
		return CLUSTER_SIZE / this->BlockSize;
	}
	int Block2Index(uchar *block)
	{
		errorCase(block < this->Cluster);
		errorCase(this->Cluster + CLUSTER_SIZE <= block);

		int offset = (int)((uint)block - (uint)this->Cluster);

		errorCase(offset % this->BlockSize != 0);

		return offset / this->BlockSize;
	}
	uchar *Index2Block(int index)
	{
		errorCase(index < 0);
		errorCase(this->GetBlockCount() <= index);

		uchar *block = this->Cluster + index * this->BlockSize;

		return block;
	}

public:
	MBStock(int blockSize)
	{
LOG("[MBS.ctor] %d\n", blockSize);
		errorCase(blockSize < 1);
		errorCase(CLUSTER_SIZE < blockSize);
		errorCase(CLUSTER_SIZE % blockSize != 0);

		int stockCount = CLUSTER_SIZE / blockSize;

		this->Cluster = (uchar *)REAL_memAlloc(CLUSTER_SIZE);
		this->StockList = (uchar **)REAL_memAlloc(stockCount * sizeof(uchar *));
		this->GivenList = (uchar *)REAL_memAlloc(stockCount);
		this->StockCount = stockCount;
		this->BlockSize = blockSize;
		this->Next = NULL;

		for(int index = 0; index < stockCount; index++)
		{
			this->StockList[index] = this->Index2Block(index);
			this->GivenList[index] = 0;
		}
	}
	MBStock(const MBStock &source)
	{
		error();
	}
	~MBStock()
	{
		1; // 貸し出し中のブロック未回収のまま開放出来ない。memAlloc() とかどこで呼ばれるか分からない。-> 開放しない。
	}

	void *GetBlock()
	{
		uchar *block;

		if(this->StockCount)
		{
			this->StockCount--;
			block = this->StockList[this->StockCount];
			this->GivenList[this->Block2Index(block)] = 1;
		}
		else
		{
			if(!this->Next)
				this->Next = new MBStock(this->BlockSize);

			block = (uchar *)this->Next->GetBlock();
		}
		return (void *)block;
	}
	int IsGivenBlock(void *v_block, int tryTake)
	{
		uchar *block = (uchar *)v_block;

		if(block < this->Cluster)
			goto notGiven;

		if(this->Cluster + CLUSTER_SIZE <= block)
			goto notGiven;

		int index = this->Block2Index(block);

		if(this->GivenList[index] == 0) // ? 貸し出し中ではない。
			error(); // goto notGiven;

		if(tryTake)
		{
			this->StockList[this->StockCount] = block;
			this->StockCount++;
			this->GivenList[this->Block2Index(block)] = 0;
		}
		return 1;

notGiven:
		if(this->Next)
			return this->Next->IsGivenBlock(v_block, tryTake);

		return 0;
	}
	int IsGivenBlock(void *v_block)
	{
		return this->IsGivenBlock(v_block, 0);
	}
	int TryTakeBlock(void *v_block)
	{
		return this->IsGivenBlock(v_block, 1);
	}
};
