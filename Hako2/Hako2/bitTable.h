class bitTable
{
private:
	bitList *List;
	int Width;

public:
	bitTable(int w)
	{
		errorCase(w < 1 || IMAX < w);

		this->List = new bitList();
		this->Width = w;
	}
	bitTable(const bitTable &source)
	{
		error();
	}
	~bitTable()
	{
		delete this->List;
	}

	void Clear()
	{
		this->List->Clear();
	}

	void PutBit(int x, int y, int value)
	{
		errorCase(x < 0);
		errorCase(y < 0);
		errorCase(this->Width <= x);
		errorCase(IMAX / this->Width <= y);

		return this->List->PutBit(x + y * this->Width, value);
	}
	int RefBit(int x, int y)
	{
		errorCase(x < 0);
		errorCase(y < 0);
		errorCase(this->Width <= x);
		errorCase(IMAX / this->Width <= y);

		return this->List->RefBit(x + y * this->Width);
	}
};
