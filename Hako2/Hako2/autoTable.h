template <class Element_t>
class autoTable
{
private:
	autoList<autoList<Element_t> *> *Rows;
	Element_t (*CreateCellFunc)(void);
	void (*ReleaseCellFunc)(Element_t);

public:
	autoTable(Element_t (*createCellFunc)(void), void (*releaseCellFunc)(Element_t), int w = 1, int h = 1)
	{
		errorCase(!createCellFunc);
		errorCase(!releaseCellFunc);

		this->Rows = new autoList<autoList<Element_t> *>();
		this->CreateCellFunc = createCellFunc;
		this->ReleaseCellFunc = releaseCellFunc;
		this->Resize(w, h);
	}
	autoTable(const autoTable &source)
	{
		error();
	}
	~autoTable()
	{
		while(this->Rows->GetCount())
		{
			autoList<Element_t> *row = this->Rows->UnaddElement();

			while(row->GetCount())
			{
				this->ReleaseCellFunc(row->UnaddElement());
			}
			delete row;
		}
		delete this->Rows;
	}

	void SetCallBack(Element_t (*createCellFunc)(void), void (*releaseCellFunc)(Element_t) = NULL)
	{
		if(createCellFunc)
			this->CreateCellFunc = createCellFunc;

		if(releaseCellFunc)
			this->ReleaseCellFunc = releaseCellFunc;
	}
	void Change(autoTable<Element_t> *otherTable, int withCallBack = 0)
	{
		m_swap(this->Rows, otherTable->Rows, void *);

		if(withCallBack)
		{
			m_swap(this->CreateCellFunc, otherTable->CreateCellFunc, void *);
			m_swap(this->ReleaseCellFunc, otherTable->ReleaseCellFunc, void *);
		}
	}

	void Resize(int w, int h)
	{
		errorCase(w < 1 || IMAX < w);
		errorCase(h < 1 || IMAX / w < h);

		while(h < this->Rows->GetCount())
		{
			autoList<Element_t> *row = this->Rows->UnaddElement();

			while(row->GetCount())
			{
				this->ReleaseCellFunc(row->UnaddElement());
			}
			delete row;
		}
		while(this->Rows->GetCount() < h)
		{
			this->Rows->AddElement(new autoList<Element_t>());
		}
		for(int rowidx = 0; rowidx < h; rowidx++)
		{
			autoList<Element_t> *row = this->Rows->GetElement(rowidx);

			while(w < row->GetCount())
			{
				this->ReleaseCellFunc(row->UnaddElement());
			}
			while(row->GetCount() < w)
			{
				row->AddElement(this->CreateCellFunc());
			}
		}
	}

	int GetWidth()
	{
		return this->Rows->GetCount() ? this->Rows->GetElement(0)->GetCount() : 0;
	}
	int GetHeight()
	{
		return this->Rows->GetCount();
	}

	Element_t *CellAt(int x, int y)
	{
		errorCase(x < 0 || this->GetWidth() <= x);
		errorCase(y < 0 || this->GetHeight() <= y);

		return this->Rows->GetElement(y)->ElementAt(x);
	}
	Element_t GetCell(int x, int y)
	{
		return *this->CellAt(x, y);
	}
	void SetCell(int x, int y, Element_t e)
	{
		*this->CellAt(x, y) = e;
	}

	void SetMinimal(int w, int h)
	{
		m_maxim(w, this->GetWidth());
		m_maxim(h, this->GetHeight());

		this->Resize(w, h);
	}
	Element_t *RefCellAt(int x, int y)
	{
		this->SetMinimal(x + 1, y + 1);
		return this->CellAt(x, y);
	}
	Element_t RefCell(int x, int y, Element_t dummyElement)
	{
		if(
			x < 0 || this->GetWidth() <= x ||
			y < 0 || this->GetHeight() <= y
			)
		{
			return dummyElement;
		}
		return this->GetCell(x, y);
	}
	Element_t RefCellEx(int x, int y)
	{
		this->SetMinimal(x + 1, y + 1);
		return this->GetCell(x, y);
	}
	void PutCell(int x, int y, Element_t e)
	{
		this->SetMinimal(x + 1, y + 1);
		return this->SetCell(x, y, e);
	}

	void Reset(int x, int y)
	{
		this->ReleaseCellFunc(this->GetCell(x, y));
		this->SetCell(x, y, this->CreateCellFunc());
	}
	void Reset(int l, int t, int w, int h)
	{
		for(int x = 0; x < w; x++)
		for(int y = 0; y < h; y++)
		{
			this->Reset(l + x, t + y);
		}
	}
	void Reset()
	{
		this->Reset(0, 0, this->GetWidth(), this->GetHeight());
	}
	void Clear(int w, int h)
	{
		this->Resize(w, h);
		this->Reset(
			0,
			0,
			m_min(w, this->GetWidth()),
			m_min(h, this->GetHeight())
			);
	}

	void VTurn()
	{
		this->Rows->Reverse();
	}
};
