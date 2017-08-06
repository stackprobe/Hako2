typedef struct DrawPicExtra_st
{
	taskList *TL;
	resCluster<PicInfo_t *> *PicRes;
	int MosaicOn;
	int IntPosOn;
	int IgnoreError;
	int GraphicHandleFlag;
	i2D_t GraphicSize; // GraphicHandleFlag != 0 ‚Ì‚Æ‚« DrawCenter(), DrawBegin() ‚Å•K—vB

	// wrapped by DPE_* -->

	double A;
	int AlphaOn;
	int BlendAddOn;
	int BlendInvOn;
	double R;
	double G;
	double B;
	int BrightOn;
}
DrawPicExtra_t;

extern DrawPicExtra_t DPE;

extern int ScreenShift_X;
extern int ScreenShift_Y;

void DrawFree(int picId, double ltx, double lty, double rtx, double rty, double rbx, double rby, double lbx, double lby);
void DrawRect_LTRB(int picId, double l, double t, double r, double b);
void DrawRect(int picId, double l, double t, double w, double h);
void DrawSimple(int picId, double x, double y);
void DrawCenter(int picId, double x, double y);
void DrawBegin(int picId, double x, double y);
void DrawSlide(double x, double y);
void DrawRotate(double rot);
void DrawZoom_X(double z);
void DrawZoom_Y(double z);
void DrawZoom(double z);
void DrawEnd(void);

void DPE_SetAlpha(double a);
void DPE_SetBlendAdd(double a);
void DPE_SetBright(double cR, double cG, double cB);
void DPE_Reset(void);
