#include "all.h"

int IsDeadTaskInfo(taskInfo_t *i)
{
	return i->Func == NULL;
}
