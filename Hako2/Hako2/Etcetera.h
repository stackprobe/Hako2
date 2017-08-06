enum
{
	ETC_SINE90TXT,
	ETC_DATSTRINGS,

	// app >

	ENUM_RANGE(ETC_FLOOR_00, FLOOR_NUM)
	ETC_FLOOR_DUMMY,

	// < app

	ETC_MAX, // num of member
};

oneObjectProto(resCluster<autoList<uchar> *>, GetEtcRes);
