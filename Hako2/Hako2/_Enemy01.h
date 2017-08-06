// ---- STOKER ----

void EachFrameStoker(Enemy_t *i, int kind);
void DrawStoker(Enemy_t *i);

// ---- GUNNER ----

int GetGunnerDir(int cellType);
int Gunner_IsFastBullet(int cellType);
int Gunner_IsRapidFire(int cellType);

void EachFrameGunner(Enemy_t *i);
void DrawGunner(Enemy_t *i);

// ---- BULLET ----

int GetBulletDir(int cellType);
int IsFastBullet(int cellType);

void EachFrameBullet(Enemy_t *i);
void DrawBullet(Enemy_t *i);

// ---- DAIKAITEN ----

int GetDaikaitenRotStartPos(int cellType);
int GetDaikaitenRotDir(int cellType);

void EachFrameDaikaiten(Enemy_t *i);
void DrawDaikaiten(Enemy_t *i);

// ----
