void *REAL_memAlloc(int size);
void *REAL_memRealloc(void *block, int size);
void REAL_memFree(void *block);

void memAlloc_INIT(void);

void *memAlloc_NC(int size);
void *memRealloc(void *block, int size);
void memFree(void *block);
void *memAlloc(int size);
void *memClone(void *block, int size);

#define na(type_t, count) \
	((type_t *)memAlloc(sizeof(type_t) * (count)))

#define nb(type_t) \
	(na(type_t, 1))
