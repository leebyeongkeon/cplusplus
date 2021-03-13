#pragma once
#include "MyString.h"
class CMyStringEx :
    public CMyString
{

public:
    CMyStringEx();
    ~CMyStringEx();
    CMyStringEx(const char*);
    int Find(const char* pszParam);
    //int SetString(const char* pszParam);
    void OnSetString(char* pszData, int nLength);
};

