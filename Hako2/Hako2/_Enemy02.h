// ---- TRAP ----

void EachFrameTrap(Enemy_t *i);
void DrawTrap(Enemy_t *i);

// ---- ASHIBA ----

extern int Ashiba_Disabled;

int GetAshibaFrame(int kind);
int AshibaIsActive(int kind);

void EachFrameAshiba(Enemy_t *i);
void DrawAshiba(Enemy_t *i);

// ---- ENEMY_WALL ----

void EachFrameEnemyWall(Enemy_t *i);
void DrawEnemyWall(Enemy_t *i);

// ---- PLAYER_WALL ----

void EachFramePlayerWall(Enemy_t *i);
void DrawPlayerWall(Enemy_t *i);

// ----
