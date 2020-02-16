#include "all.h"

MCell_t *CreateMCell(void)
{
	MCell_t *i = nb_(MCell_t);

	return i;
}
void ReleaseMCell(MCell_t *i)
{
	memFree(i);
}
