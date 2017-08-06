#include "all.h"

void ReleasePubMnuItem(PubMnuItem_t *i)
{
	memFree(i->Line);
	memFree(i);
}
