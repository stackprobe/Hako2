#define IsPlayerWall(cellType) \
	((cellType) == CT_WALL || \
	m_isRange((cellType), ET_COOKIE_FIRST, ET_COOKIE_LAST) || \
	m_isRange((cellType), ET_GUNNER_00, ET_GUNNER_00 + GUNNER_MAX - 1) || \
	m_isRange((cellType), ET_DAIKAITEN_00, ET_DAIKAITEN_00 + DAIKAITEN_MAX - 1) || \
	m_isRange((cellType), ET_ASHIBA_00, ET_ASHIBA_00 + ASHIBA_MAX - 1) && AshibaIsActive((cellType) - ET_ASHIBA_00) || \
	(cellType) == ET_PLAYER_WALL)

#define EDIT_WIN_W 240
#define EDIT_WIN_H ((SCREEN_H / 16) * 16)

void GameMain(void);
void Pub_CreateRespawnPoint(fPInt_t x, fPInt_t y, int forceMode = 0);
