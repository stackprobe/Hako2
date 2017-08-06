#include "all.h"

typedef struct CommonEffect_st
{
	int Frame;
	int PicId;
	CommonEffectExtra_t Extra;
	double X;
	double Y;
	double R;
	double Z;
	double A;
	double XAdd;
	double YAdd;
	double RAdd;
	double ZAdd;
	double AAdd;
	double XAdd2;
	double YAdd2;
	double RAdd2;
	double ZAdd2;
	double AAdd2;
	int OutOfScreenFrame;
}
CommonEffect_t;

double CameraX; // px
double CameraY; // px

static int CommonEffectFunc(CommonEffect_t *i)
{
	int picId = i->PicId;

	if(i->Extra.EndPicId)
	{
		m_range(i->Extra.PicIdFrmPerInc, 1, IMAX);
		picId += i->Frame / i->Extra.PicIdFrmPerInc;

		if(i->Extra.EndPicId < picId)
			return 0;
	}
	else if(i->Extra.ModPicId)
	{
		m_range(i->Extra.PicIdFrmPerInc, 1, IMAX);
		picId += (i->Frame / i->Extra.PicIdFrmPerInc) % i->Extra.ModPicId;
	}

	i->Frame++;

	double drawX = i->X;
	double drawY = i->Y;

	if(!i->Extra.IgnoreCamera)
	{
		drawX -= CameraX;
		drawY -= CameraY;
	}

	// ? out of screen
	if(
		drawX < i->Extra.ScreenArea.L || i->Extra.ScreenArea.R < drawX ||
		drawY < i->Extra.ScreenArea.T || i->Extra.ScreenArea.B < drawY
		)
	{
		if(20 < i->OutOfScreenFrame) return 0;
		i->OutOfScreenFrame++;
		goto endDraw;
	}
	i->OutOfScreenFrame = 0;

	if(!i->Extra.BlendAddOn)
		DPE_SetAlpha(i->A);
	else
		DPE_SetBlendAdd(i->A);

	if(i->Extra.BrightOn)
		DPE_SetBright(i->Extra.Bright_R, i->Extra.Bright_G, i->Extra.Bright_B);

	DrawBegin(picId, drawX, drawY);

	DrawSlide(i->Extra.SlideX_B, i->Extra.SlideY_B);
	DrawRotate(i->Extra.R_B);
	DrawZoom(i->Extra.Z_B);

	DrawSlide(i->Extra.SlideX, i->Extra.SlideY);
	DrawRotate(i->R);
	DrawZoom(i->Z);

	DrawSlide(i->Extra.SlideX_F, i->Extra.SlideY_F);
	DrawRotate(i->Extra.R_F);
	DrawZoom(i->Extra.Z_F);

	DrawEnd();
	DPE_Reset();

endDraw:
	i->X += i->XAdd;
	i->Y += i->YAdd;
	i->R += i->RAdd;
	i->Z += i->ZAdd;
	i->A += i->AAdd;

	i->XAdd += i->XAdd2;
	i->YAdd += i->YAdd2;
	i->RAdd += i->RAdd2;
	i->ZAdd += i->ZAdd2;
	i->AAdd += i->AAdd2;

	i->XAdd *= i->Extra.SpeedRate;
	i->YAdd *= i->Extra.SpeedRate;

	return 0.0 < i->A;
}
static void CommonEffectReleaseParam(CommonEffect_t *i)
{
	memFree(i);
}
CommonEffectExtra_t CEE;

void AddCommonEffect(
	taskList *tl,
	int topMode,
	int picId,
	double x, double y, double r, double z, double a,
	double x_add, double y_add, double r_add, double z_add, double a_add,
	double x_add2, double y_add2, double r_add2, double z_add2, double a_add2
	)
{
	CommonEffect_t *i = (CommonEffect_t *)memAlloc_NC(sizeof(CommonEffect_t));

	i->Frame = 0;
	i->PicId = picId;
	i->Extra = CEE;

	i->X = x;
	i->Y = y;
	i->R = r;
	i->Z = z;
	i->A = a;

	i->XAdd = x_add;
	i->YAdd = y_add;
	i->RAdd = r_add;
	i->ZAdd = z_add;
	i->AAdd = a_add;

	i->XAdd2 = x_add2;
	i->YAdd2 = y_add2;
	i->RAdd2 = r_add2;
	i->ZAdd2 = z_add2;
	i->AAdd2 = a_add2;

	i->OutOfScreenFrame = 0;

	AddTask(tl, topMode, CommonEffectFunc, i, CommonEffectReleaseParam);
}

void CEE_SetBright(double r, double g, double b)
{
	CEE.BrightOn = 1;
	CEE.Bright_R = r;
	CEE.Bright_G = g;
	CEE.Bright_B = b;
}
void CEE_Reset(void)
{
	memset(&CEE, 0x00, sizeof(CommonEffectExtra_t));

	CEE.Z_B = 1.0;
	CEE.Z_F = 1.0;
	CEE.SpeedRate = 1.0;
	CEE.ScreenArea.L = -SCREEN_W;
	CEE.ScreenArea.T = -SCREEN_H;
	CEE.ScreenArea.R = SCREEN_W * 2;
	CEE.ScreenArea.B = SCREEN_H * 2;
}

// ----

int IsInCamera(double x, double y, double margin)
{
	errorCase(margin < 0.0);

	return
		m_isRange(x - CameraX, -margin, SCREEN_W + margin) &&
		m_isRange(y - CameraY, -margin, SCREEN_H + margin);
}
