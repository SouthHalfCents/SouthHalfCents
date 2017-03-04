#pragma once
#include "Database.h"

//工具类
class CUtils
{
public:
	CUtils();
	~CUtils();

	static int TranDayweekToInt(int nDayofWeek, int & nDayValue);			//把dayofweek转为DAY_MONDAY的值
};

