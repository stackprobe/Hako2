#include "all.h"

void MeteorBeforeDraw(void)
{
	if(Gnd.GraphicalMode == GM_METEOR || Gnd.GraphicalMode == GM_METEOR_SOFT)
		GDc.Meteor.Frame++;
	else
		GDc.Meteor.Frame = 0;

	if(!GDc.Meteor.Frame) // ? not meteor mode
		return;

	if(rndPct(50))
	{
		const double SPEED = 3.0;
		const double Y_MARGIN = 200.0;
		double rate = 3.0 - dRnd() * 2.2;

		CEE.ModPicId = METEOR_D_MAX;
		CEE.PicIdFrmPerInc = 1;
		CEE.IgnoreCamera = 1;
		CEE.BlendAddOn = 1;
		AddCommonEffect(
			GDc.Meteor.EL,
			0,
			D_METEOR_00 + rnd(METEOR_P_MAX) * METEOR_D_MAX | DTP,
			dRnd() * (SCREEN_W + SCREEN_H) + Y_MARGIN,
			-Y_MARGIN,
			0.0,
			rate,
			0.7,
			SPEED * -rate,
			SPEED * rate
			);
		CEE_Reset();
	}

	ChangeDrawScreen(GDc.Meteor.WallScreen);
	ClearDrawScreen();
	GDc.Meteor.EL->ExecuteAllTask();
	ChangeDrawScreen(GDc.Meteor.ExWallScreen);

	int EXPAND_MARGIN_X = 80;
	int EXPAND_MARGIN_Y = 60;

	if(Gnd.GraphicalMode == GM_METEOR_SOFT)
	{
		EXPAND_MARGIN_X = 160;
		EXPAND_MARGIN_Y = 120;
	}
	DrawExtendGraph(
		-EXPAND_MARGIN_X,
		-EXPAND_MARGIN_Y,
		SCREEN_W + EXPAND_MARGIN_X,
		SCREEN_H + EXPAND_MARGIN_Y,
		GetHandle(GDc.Meteor.WallScreen),
		0
		);

	ChangeDrawScreen(GDc.Meteor.FieldScreen);
	ClearDrawScreen();
}
void MeteorAfterDraw(void)
{
	if(!GDc.Meteor.Frame) // ? not meteor mode
		return;

	RestoreDrawScreen();

	if(Gnd.GraphicalMode == GM_METEOR)
	{
		DrawGraph(0, 0, GetHandle(GDc.Meteor.WallScreen), 0);
		DrawGraph(0, 0, GetHandle(GDc.Meteor.FieldScreen), 1);

		GraphFilter(GetHandle(GDc.Meteor.ExWallScreen), DX_GRAPH_FILTER_GAUSS, 16, 1000);
		GraphBlend(
			GetHandle(GDc.Meteor.FieldScreen),
			GetHandle(GDc.Meteor.ExWallScreen),
			255,
			DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			DX_RGBA_SELECT_BLEND_R,
			DX_RGBA_SELECT_BLEND_G,
			DX_RGBA_SELECT_BLEND_B,
			DX_RGBA_SELECT_SRC_A
			);

		DPE_SetAlpha(0.7);
		DPE.GraphicHandleFlag = 1;
		DrawSimple(GetHandle(GDc.Meteor.FieldScreen), 0, 0);
		DPE_Reset();
	}
	else // ? GM_METEOR_SOFT
	{
		DrawGraph(0, 0, GetHandle(GDc.Meteor.WallScreen), 0);

		DPE_SetAlpha(0.9);
		DPE_SetBright(0, 0, 0);
		DrawRect(P_WHITEBOX, 0, 0, SCREEN_W, SCREEN_H);
		DPE_Reset();

		DrawGraph(0, 0, GetHandle(GDc.Meteor.FieldScreen), 1);

		GraphBlend(
			GetHandle(GDc.Meteor.FieldScreen),
			GetHandle(GDc.Meteor.ExWallScreen),
			255,
			DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			DX_RGBA_SELECT_BLEND_R,
			DX_RGBA_SELECT_BLEND_G,
			DX_RGBA_SELECT_BLEND_B,
			DX_RGBA_SELECT_SRC_A
			);

		DPE_SetAlpha(0.7);
		DPE.GraphicHandleFlag = 1;
		DrawSimple(GetHandle(GDc.Meteor.FieldScreen), 0, 0);
		DPE_Reset();
	}
}
