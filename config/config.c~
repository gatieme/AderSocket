


#include "config.h"

int XSocketPrintf(const char* format, ...)
{
	va_list	argPtr;
	int		count;

	va_start(argPtr, format);				// 获取可变参数列表的地址
	count = vprintf(format, argPtr);		// 将信息输出到输出设备
	va_end(argPtr);							// 可变参数的结束

	return count;
}



