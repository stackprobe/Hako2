enum
{
	MOUBTN_L,
	MOUBTN_M,
	MOUBTN_R,

	MOUBTN_MAX, // num of member
};

extern int MouseRot;
extern int MouseX;
extern int MouseY;
extern int MouseMoveX;
extern int MouseMoveY;

void MouseEachFrame(void);
int GetMouInput(int mouBtnId);
int GetMouPound(int mouBtnId);
void UpdateMousePos(void);
void ApplyMousePos(void);
void UpdateMouseMove(void);
