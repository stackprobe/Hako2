enum
{
	P_DUMMY, // as 0
	P_WHITEBOX,
	P_WHITECIRCLE,

	// app >

	ENUM_RANGE(P_STAR_00, 10)
	ENUM_RANGE(P_FIRE_00, 9)

	P_KIGOU_Y,
	P_KIGOU_W,
	P_KIGOU_DW,

	P_BIKKURI,
	P_MAHOUJIN,

	P_WINDOW_Y_OLD,
	P_WINDOW_B_OLD,
	P_WINDOW_G,
	P_WINDOW_Y,
	P_WINDOW_B,

	P_BLUEEXPLOSION,

	P_ALLFLOORCLEARED,
	P_CONGRATULATION,
	P_THANKYOUFORPLAYING,

	P_MAPCHIP_A1,
	P_MAPCHIP_A4,
	P_MAPCHIP_A5,
	P_MAPCHIP_B,
	P_MAPCHIP_D,
	P_MAPCHIP_E,

	ENUM_RANGE(P_METEOR_00, METEOR_P_MAX)

	P_CLOUD,

	P_WBOX24,
	P_WBOX32,
	P_TITLE_WALL,
	P_WALL_FLOOR_INTERVAL,
	P_WALL_FLOOR_DEFAULT,
	P_RESPAWN_POINT,
	P_WALL_FLOOR_01,
	P_WALL_FLOOR_01B,
	P_WALL_FLOOR_01C,
	P_WALL_FLOOR_02,

	ENUM_RANGE(P_WALL_FLOOR_02_00, FLOOR_02_WALL_MAX)

	P_WALL_FLOOR_03,
	P_WALL_FLOOR_03B,
	P_WALL_FLOOR_03C,

	P_ENDING,
	P_ENDING_01,
	P_ENDING_02,

	// < app

	P_MAX, // num of member
};

typedef struct PicInfo_st
{
	int Handle;
	int W;
	int H;
}
PicInfo_t;

// Pic_ >

int Pic_FileData2SoftImage(autoList<uchar> *fileData);
int Pic_SoftImage2GraphicHandle(int si_h);
PicInfo_t *Pic_GraphicHandle2PicInfo(int handle);
void Pic_ReleasePicInfo(PicInfo_t *i);

void Pic_GetSoftImageSize(int si_h, int &w, int &h);
void Pic_GetGraphicHandleSize(int handle, int &w, int &h);

extern int SI_R;
extern int SI_G;
extern int SI_B;
extern int SI_A;

void Pic_GetSIPixel(int si_h, int x, int y);
void Pic_SetSIPixel(int si_h, int x, int y);

// < Pic_

resCluster<PicInfo_t *> *CreatePicRes(PicInfo_t *(*picLoader)(autoList<uchar> *), void (*picUnloader)(PicInfo_t *));
void UnloadAllPicResHandle(void);

void SetPicRes(resCluster<PicInfo_t *> *resclu);
resCluster<PicInfo_t *> *GetPicRes(void);
void ResetPicRes(void);

#define DTP 0x40000000

int Pic(int picId);
int Pic_W(int picId);
int Pic_H(int picId);
