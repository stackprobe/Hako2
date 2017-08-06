template <class Element_t>
class endlessQueue
{
private:
	autoList<Element_t> *List;
	int RIndex;

	void Init(autoList<Element_t> *list)
	{
		this->List = list;
		this->RIndex = 0;
	}

public:
	endlessQueue()
	{
		this->Init(new autoList<Element_t>());
	}
	endlessQueue(autoList<Element_t> *list_bind)
	{
		this->Init(list_bind);
	}
	endlessQueue(const endlessQueue &source)
	{
		error();
	}
	~endlessQueue()
	{
		delete this->List;
	}

	void Enqueue(Element_t e)
	{
		this->List->AddElement(e);
	}
	Element_t Dequeue()
	{
		Element_t e = this->List->GetElement(this->RIndex); // this->List->GetCount() == 0 ‚È‚ç‚±‚±‚ÅƒGƒ‰[‚É‚È‚éB
		this->RIndex = (this->RIndex + 1) % this->List->GetCount();
		return e;
	}

	autoList<Element_t> *GetList_DIRECT()
	{
		return this->List;
	}
};

template <class Element_t>
void releaseQueue(endlessQueue<Element_t> *queue, void (*func)(Element_t e))
{
	errorCase(!queue);
	errorCase(!func);

	queue->GetList_DIRECT()->CallAllElement(func);
	delete queue;
}
