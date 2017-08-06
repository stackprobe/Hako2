class bitList
{
private:
	autoList<uint> *List;

public:
	bitList()
	{
		this->List = new autoList<uint>();
	}
	bitList(const bitList &source)
	{
		error();
	}
	~bitList()
	{
		delete this->List;
	}

	void Clear()
	{
		this->List->Clear();
	}

	void PutBit(int index, int value)
	{
		errorCase(index < 0);
		uint c = this->List->RefElement(index / 32, 0);

		if(value)
			c |= 1 << (index % 32);
		else
			c &= ~(1 << (index % 32));

		this->List->PutElement(index / 32, c, 0);
	}
	int RefBit(int index)
	{
		errorCase(index < 0);
		return (this->List->RefElement(index / 32, 0) >> (index % 32)) & 1;
	}

	autoList<uint> *GetList_DIRECT()
	{
		return this->List;
	}
	void Overwrite(bitList *src)
	{
		errorCase(!src);
		this->List->Overwrite(src->List);
	}
};
