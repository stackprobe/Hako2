typedef struct taskInfo_st
{
	int (*Func)(void *);
	void *Param;
	void (*ReleaseParam)(void *);
}
taskInfo_t;

int IsDeadTaskInfo(taskInfo_t *i);

class taskList
{
private:
	autoList<taskInfo_t> *List;
	int LastFrame;

public:
	taskList()
	{
		this->List = new autoList<taskInfo_t>();
		this->LastFrame = -1;
	}
	taskList(const taskList &source)
	{
		error();
	}
	~taskList()
	{
		this->Clear();
		delete this->List;
	}

	int GetCount()
	{
		return this->List->GetCount();
	}

	void AddTask(taskInfo_t ti)
	{
		this->List->AddElement(ti);
	}
	void AddTopTask(taskInfo_t ti)
	{
		this->List->InsertElement(0, ti);
	}
	void ExecuteAllTask(int oncePerFrame = 1)
	{
		if(oncePerFrame)
		{
			if(ProcFrame <= this->LastFrame)
				return;

			this->LastFrame = ProcFrame;
		}
		int dead = 0;

		for(int index = 0; index < this->List->GetCount(); index++)
		{
			taskInfo_t *ti = this->List->ElementAt(index);

			if(!ti->Func(ti->Param)) // ? â–½
			{
				ti = this->List->ElementAt(index); // Func() ‚Å‚±‚Ì tl ‚É’Ç‰Á‚³‚ê‚éê‡‚ð‘z’è

				if(ti->ReleaseParam)
					ti->ReleaseParam(ti->Param);

				ti->Func = NULL;
				dead = 1;
			}
		}
		if(dead)
			this->List->MultiDiscard(IsDeadTaskInfo);
	}
	void Clear()
	{
		while(this->List->GetCount())
		{
			taskInfo_t ti = this->List->UnaddElement();

			if(ti.ReleaseParam)
				ti.ReleaseParam(ti.Param);
		}
	}
};

template <class Param_t>
void AddTask(taskList *tl, int topMode, int (*tf)(Param_t *), Param_t *tp = NULL, void (*tr)(Param_t *) = NULL)
{
	errorCase(!tl);
	errorCase(!tf);

	taskInfo_t ti;

	ti.Func = (int (*)(void *))tf;
	ti.Param = (void *)tp;
	ti.ReleaseParam = (void (*)(void *))tr;

	if(topMode)
		tl->AddTopTask(ti);
	else
		tl->AddTask(ti);
}
