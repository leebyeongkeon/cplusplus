#include "stdafx.h"
#include "MyStringEx.h"
CMyStringEx::CMyStringEx() {

}
CMyStringEx::CMyStringEx(const char* pszParam)
	:CMyString(pszParam) {
}

CMyStringEx::~CMyStringEx() {

}
int CMyStringEx::Find(const char* pszParam)
{
	// TODO: 여기에 구현 코드 추가.
	if (pszParam == NULL || GetString() == NULL)
		return -1;
	const char* result = strstr(GetString(), pszParam);
	if (result == NULL)
		return -1;	
	return result - GetString();
	
}


/*
int CMyStringEx::SetString(const char* pszParam)
{
	// TODO: 여기에 구현 코드 추가.
	int nResult;
	if (strcmp(pszParam, "멍멍이아들") == 0)
		nResult = CMyString::SetString("착한 사람");
	else
		nResult = CMyString::SetString(pszParam);
	return nResult;
}
*/

void CMyStringEx::OnSetString(char* pszData, int nLength)
{
	// TODO: 여기에 구현 코드 추가.
	if (strcmp(pszData, "멍멍이아들") == 0)
		strcpy(pszData, "*착한아들*");
}
