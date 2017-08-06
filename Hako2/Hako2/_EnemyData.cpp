#include "all.h"

Enemy_t *CreateEnemy(int enemyType, fPInt_t x, fPInt_t y)
{
	errorCase(enemyType < 0 || CT_MAX <= enemyType);
	errorCase(x < -IMAX || IMAX < x);
	errorCase(y < -IMAX || IMAX < y);

	Enemy_t *i = nb(Enemy_t);

	i->EnemyType = enemyType;
	i->X = x;
	i->Y = y;

	i->Frame = -1; // -1 == uninited

	return i;
}
void ReleaseEnemy(Enemy_t *i)
{
	memFree(i);
}
