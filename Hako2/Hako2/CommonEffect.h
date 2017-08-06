typedef struct CommonEffectExtra_st
{
	int EndPicId; // PicId から EndPicId まで表示して終了する, 0 == 無効, PicIdFrmPerIncと連動
	int ModPicId; // PicId から (PicId + ModPicId - 1) までループする, 0 == 無効, PicIdFrmPerIncと連動
	int PicIdFrmPerInc;
	double SlideX;
	double SlideY;
	double SlideX_B;
	double SlideY_B;
	double R_B;
	double Z_B;
	double SlideX_F;
	double SlideY_F;
	double R_F;
	double Z_F;
	double SpeedRate;
	int IgnoreCamera;
	int BlendAddOn;

	struct
	{
		/*
			-2048 ～ 2047
		*/
		int L : 12;
		int T : 12;
		int R : 12;
		int B : 12;
	}
	ScreenArea;

	// wrapped by CEE_* -->

	int BrightOn;
	double Bright_R;
	double Bright_G;
	double Bright_B;
}
CommonEffectExtra_t;

extern CommonEffectExtra_t CEE;

extern double CameraX;
extern double CameraY;

void AddCommonEffect(
	taskList *tl,
	int topMode,
	int picId,
	double x, double y, double r = 0.0, double z = 1.0, double a = 1.0,
	double x_add = 0.0, double y_add = 0.0, double r_add = 0.0, double z_add = 0.0, double a_add = 0.0,
	double x_add2 = 0.0, double y_add2 = 0.0, double r_add2 = 0.0, double z_add2 = 0.0, double a_add2 = 0.0
	);

void CEE_SetBright(double r, double g, double b);
void CEE_Reset(void);

// ----

int IsInCamera(double x, double y, double margin);
