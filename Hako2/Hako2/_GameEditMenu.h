enum // EditMenuClicked_
{
	EMC_NONE, // 何もクリックしていない。

	EMC_EMPTY, // 何も無いところをクリックした。
	EMC_MENUPOS,
	EMC_CELLTYPE,
	EMC_EXIT,

	EMC_MAX, // num of item
};

void EditMenuEachFrame(void);
void DrawEditMenuEntity(void);
void DrawEditCell(int cellType, int dr_x, int dr_y, int allFlag = 0);
