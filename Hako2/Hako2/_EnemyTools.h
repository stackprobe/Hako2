extern int EnemyAnaIsWall;

int IsEnemyWall(MCell_t *cell);
void AddEnemyCrash(fPInt_t cent_x, fPInt_t cent_y);
void AddEnemyCrash(Enemy_t *i);
int IsEnemyInWall(Enemy_t *i);
void AdjustEnemyPos(Enemy_t *i);
int GetEnemyOrderWeight(Enemy_t *i);

// ---- extra tools ----

void CookieBornEffect(fPInt_t enemy_x, fPInt_t enemy_y, double cR, double cG, double cB, double scale = 1.0);
void BulletDeadEffect(fPInt_t enemy_x, fPInt_t enemy_y, double cR, double cG, double cB);

// ----
