typedef struct PubMnuItem_st
{
	char *Line;
	int Type; // "FIDidR"

	union
	{
		struct
		{
			void (*Func)(void);
		}
		Func; // F

		struct
		{
			int *P_Value;
			int MinValue;
			int MaxValue;
			int Smallest; // 1 - ?
		}
		Int; // I

		struct
		{
			double *P_Value;
			double MinValue;
			double MaxValue;
			int Denom; // 1 - ?
		}
		Double; // D

		struct
		{
			int (*Getter)(void);
			void (*Adder)(int);
		}
		IntFunc; // i

		struct
		{
			double (*Getter)(void);
			void (*Adder)(int);
		}
		DoubleFunc; // d

		// R
	}
	u;
}
PubMnuItem_t;

void ReleasePubMnuItem(PubMnuItem_t *i);

class PublicMenu
{
private:
	char *Title;
	autoList<PubMnuItem_t *> *ItemList;

public:
	PublicMenu(char *title)
	{
		this->Title = strx(title);
		this->ItemList = new autoList<PubMnuItem_t *>();
	}
	PublicMenu(const PublicMenu &source)
	{
		error();
	}
	~PublicMenu()
	{
		memFree(this->Title);
		this->ItemList->Clear(ReleasePubMnuItem);
		delete this->ItemList;
	}

	void AddFunc(char *item, void (*func)(void))
	{
		errorCase(m_isEmpty(item));
		errorCase(!func);

		PubMnuItem_t *i = nb(PubMnuItem_t);

		i->Line = strx(item);
		i->Type = 'F';
		i->u.Func.Func = func;

		this->ItemList->AddElement(i);
	}
	void AddInt(char *item, int *p_value, int minval, int maxval, int smallest)
	{
		errorCase(m_isEmpty(item));
		errorCase(!p_value);
		errorCase(minval < 0 || IMAX < minval);
		errorCase(maxval < minval || IMAX < maxval);
		errorCase(smallest < 1 || IMAX < smallest);

		PubMnuItem_t *i = nb(PubMnuItem_t);

		i->Line = strx(item);
		i->Type = 'I';
		i->u.Int.P_Value = p_value;
		i->u.Int.MinValue = minval;
		i->u.Int.MaxValue = maxval;
		i->u.Int.Smallest = smallest;

		this->ItemList->AddElement(i);
	}
	void AddDouble(char *item, double *p_value, double minval, double maxval, int denom)
	{
		errorCase(m_isEmpty(item));
		errorCase(!p_value);
		errorCase(minval < 0.0 || (double)IMAX < minval);
		errorCase(maxval < minval || (double)IMAX < maxval);
		errorCase(denom < 1 || IMAX < denom);

		PubMnuItem_t *i = nb(PubMnuItem_t);

		i->Line = strx(item);
		i->Type = 'D';
		i->u.Double.P_Value = p_value;
		i->u.Double.MinValue = minval;
		i->u.Double.MaxValue = maxval;
		i->u.Double.Denom = denom;

		this->ItemList->AddElement(i);
	}
	void AddIntFunc(char *item, int (*getter)(void), void (*adder)(int))
	{
		errorCase(m_isEmpty(item));
		errorCase(!getter);
		errorCase(!adder);

		PubMnuItem_t *i = nb(PubMnuItem_t);

		i->Line = strx(item);
		i->Type = 'i';
		i->u.IntFunc.Getter = getter;
		i->u.IntFunc.Adder = adder;

		this->ItemList->AddElement(i);
	}
	void AddDoubleFunc(char *item, double (*getter)(void), void (*adder)(int))
	{
		errorCase(m_isEmpty(item));
		errorCase(!getter);
		errorCase(!adder);

		PubMnuItem_t *i = nb(PubMnuItem_t);

		i->Line = strx(item);
		i->Type = 'd';
		i->u.DoubleFunc.Getter = getter;
		i->u.DoubleFunc.Adder = adder;

		this->ItemList->AddElement(i);
	}

	void Show(void (*wallDrawer)(void), char *returnItem = "–ß‚é")
	{
		errorCase(!wallDrawer);

		// Add Return
		{
			PubMnuItem_t *i = nb(PubMnuItem_t);

			i->Line = strx(returnItem);
			i->Type = 'R';

			this->ItemList->AddElement(i);
		}

		errorCase(this->ItemList->GetCount() < 1); // 2bs

		int selectIndex = 0;
		int mouSelectIndex = 0;

		FreezeInput();

		for(; ; )
		{
			if(GetPound(INP_DIR_2))
				selectIndex++;

			if(GetPound(INP_DIR_8))
				selectIndex--;

			UpdateMousePos();
			mouSelectIndex = (MouseY - 8) / 32 - 1;

			m_range(selectIndex, 0, this->ItemList->GetCount() - 1);
			m_range(mouSelectIndex, 0, this->ItemList->GetCount() - 1);

			if(GetMouPound(MOUBTN_L))
				selectIndex = mouSelectIndex;

			if(GetPound(INP_A))
			{
				PubMnuItem_t *i = this->ItemList->GetElement(selectIndex);

				switch(i->Type)
				{
				case 'F':
					i->u.Func.Func();
					break;

				case 'i':
					i->u.IntFunc.Adder(0);
					break;

				case 'd':
					i->u.DoubleFunc.Adder(0);
					break;

				case 'R':
					goto endMenuLoop;

				default:
					break;
				}
			}
			if(GetPound(INP_B))
			{
				if(selectIndex == this->ItemList->GetCount() - 1)
					break;

				selectIndex = this->ItemList->GetCount() - 1;
			}
			int valRot = 0;

			if(GetPound(INP_DIR_4))
				valRot = -1;

			if(GetPound(INP_DIR_6))
				valRot = 1;

			if(MouseRot)
			{
				selectIndex = mouSelectIndex;
				valRot = MouseRot;
			}
			if(valRot)
			{
				if(GetKeyInput(KEY_INPUT_LCONTROL))
					valRot *= 10;

				if(GetKeyInput(KEY_INPUT_LSHIFT))
					valRot *= 100;

				PubMnuItem_t *i = this->ItemList->GetElement(selectIndex);

				switch(i->Type)
				{
				case 'I':
					i->u.Int.P_Value[0] += valRot * i->u.Int.Smallest;
					m_range(i->u.Int.P_Value[0], i->u.Int.MinValue, i->u.Int.MaxValue);
					break;

				case 'D':
					i->u.Double.P_Value[0] += valRot / i->u.Double.Denom;
					m_accur(i->u.Double.P_Value[0], i->u.Double.Denom);
					m_range(i->u.Double.P_Value[0], i->u.Double.MinValue, i->u.Double.MaxValue);
					break;

				case 'i':
					i->u.IntFunc.Adder(valRot);
					break;

				case 'd':
					i->u.DoubleFunc.Adder(valRot);
					break;

				default:
					break;
				}
			}
			wallDrawer();
			SetPrint(16, 16, 32);
			Print(this->Title);
			PrintRet();

			for(int index = 0; index < this->ItemList->GetCount(); index++)
			{
				PubMnuItem_t *i = this->ItemList->GetElement(index);

				Print_x(xcout(
					"%c%s%c %s"
					,index == mouSelectIndex ? '(' : '['
					,index == selectIndex ? "->" : "  "
					,index == mouSelectIndex ? ')' : ']'
					,i->Line
					));

				switch(i->Type)
				{
				case 'I':
					Print_x(xcout(" (%d)", i->u.Int.P_Value[0]));
					break;

				case 'D':
					Print_x(xcout(" (%f)", i->u.Double.P_Value[0]));
					break;

				case 'i':
					Print_x(xcout(" (%d)", i->u.IntFunc.Getter()));
					break;

				case 'd':
					Print_x(xcout(" (%f)", i->u.DoubleFunc.Getter()));
					break;

				default:
					break;
				}
				PrintRet();
			}
			EachFrame();
		}
endMenuLoop:
		FreezeInput();
	}
};
