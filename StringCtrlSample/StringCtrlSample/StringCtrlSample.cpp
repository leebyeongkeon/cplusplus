#include"stdafx.h"
#include"MyStringEx.h"
void TestFunc(const CMyString&);
int _tmain(int argc, _TCHAR* argv[])
{
	CMyString b("World"), c;
	c = "Hello" + b;
	cout << c << endl;
	return 0;
}
void TestFunc(const CMyString& strParam) {
	cout << strParam[0] << endl;
	cout << strParam[strParam.GetLength() - 1] << endl;
}
CMyString operator+(const char* pszParam, const CMyString& strParam) {
	CMyString strResult(pszParam);
	strResult.Append(strParam.m_pszData);
	return strResult;
}