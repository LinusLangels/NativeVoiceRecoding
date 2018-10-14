#include "stdafx.h"
#include "debug_utilities.h"



void consolePrintInt(char *test, int parameter)
{
	char str[512];
	sprintf_s(str, test, parameter);

	LPCWSTR string = convertCharArrayToLPCWSTR(str);
	OutputDebugString(string);
}
void consolePrintFloat(char *test, float parameter)
{
	char str[512];
	sprintf_s(str, test, parameter);

	LPCWSTR string = convertCharArrayToLPCWSTR(str);
	OutputDebugString(string);
}
void consolePrintString(char *test, char *parameter)
{
	char str[512];
	sprintf_s(str, test, parameter);

	LPCWSTR string = convertCharArrayToLPCWSTR(str);
	OutputDebugString(string);
}


wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}