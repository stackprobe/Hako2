enum
{
	ENUM_RANGE(D_DUMMY_00, 4)

	// app >

	ENUM_RANGE(D_STAR_W_00, 5 * 3) // White
	ENUM_RANGE(D_STAR_K_00, 5 * 3) // blacK
	ENUM_RANGE(D_STAR_M_00, 5 * 3) // Magenta
	ENUM_RANGE(D_STAR_G_00, 5 * 3) // Green
	ENUM_RANGE(D_STAR_B_00, 5 * 3) // Blue
	ENUM_RANGE(D_STAR_C_00, 5 * 3) // Cyan
	ENUM_RANGE(D_STAR_R_00, 5 * 3) // Red
	ENUM_RANGE(D_STAR_P_00, 5 * 3) // Pink
	ENUM_RANGE(D_STAR_O_00, 5 * 3) // Orange
	ENUM_RANGE(D_STAR_Y_00, 5 * 3) // Yellow

	ENUM_RANGE(D_FIRE_O_00, 10) // Orange
	ENUM_RANGE(D_FIRE_R_00, 10) // Red
	ENUM_RANGE(D_FIRE_B_00, 10) // Blue
	ENUM_RANGE(D_FIRE_G_00, 10) // Green
	ENUM_RANGE(D_FIRE_Y_00, 10) // Yellow
	ENUM_RANGE(D_FIRE_P_00, 10) // Pink
	ENUM_RANGE(D_FIRE_M_00, 10) // Magenta
	ENUM_RANGE(D_FIRE_K_00, 10) // blacK
	ENUM_RANGE(D_FIRE_W_00, 10) // White

	ENUM_RANGE(D_KIGOU_Y_00, 10 * 2)
	ENUM_RANGE(D_KIGOU_W_00, 10 * 2)
	ENUM_RANGE(D_KIGOU_DW_00, 10 * 2)

	ENUM_RANGE(D_MAHOUJIN_00, 14)

	ENUM_RANGE(D_WINDOW_Y_00, 16)
	ENUM_RANGE(D_WINDOW_B_00, 16)
	ENUM_RANGE(D_WINDOW_G_00, 16)

	ENUM_RANGE(D_BLUEEXPLOSION_00, 7)

	ENUM_RANGE(D_MAPCHIP_A1_MINI_00, 32 * 24)
	ENUM_RANGE(D_MAPCHIP_A4_MINI_00, 32 * 32)
	ENUM_RANGE(D_MAPCHIP_A5_00, 8 * 16)
	ENUM_RANGE(D_MAPCHIP_B_00, 16 * 16)
	ENUM_RANGE(D_MAPCHIP_D_00, 16 * 16)
	ENUM_RANGE(D_MAPCHIP_E_00, 16 * 16)

	ENUM_RANGE(D_METEOR_00, METEOR_P_MAX * METEOR_D_MAX)

	ENUM_RANGE(D_RESPAWN_POINT_00, 5 * 2)

	// < app

	D_MAX, // num of member
};

typedef struct DerInfo_st
{
	int ParentPicId;
	int X;
	int Y;
	int W;
	int H;
}
DerInfo_t;

int Der(resCluster<PicInfo_t *> *resclu, int derId);
int Der_W(int derId);
int Der_H(int derId);

void UnloadAllDer(resCluster<PicInfo_t *> *resclu);
