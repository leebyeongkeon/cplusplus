#pragma once
class CMyString
{
private:
	// 문자열을 저장하기 위해 동적 할당한 메모리를 가리키는 포인터
	char* m_pszData;
	// 저장된 문자열의 길이
	int m_nLength;
public:
	CMyString();
	explicit CMyString(const char*);//매개변수 1개 생성자인 경우 변환 생성자. 묵시적 변환 방지 예약어 explicit
	CMyString(const CMyString&);
	CMyString(CMyString&&);
	~CMyString();
	int SetString(const char*);
	const char* GetString() const;
	void Release();
	CMyString& operator=(const CMyString&);
	CMyString operator+(const CMyString&);
	CMyString& operator+=(const CMyString&);
	int operator==(const CMyString&);
	int operator!=(const CMyString&);
	char& operator[](int);
	char operator[](int)const;
	operator char*(void) const;
	int GetLength()const;
	int Append(const char*);
	virtual void OnSetString(char* pszData, int nLength);
	friend CMyString operator+(const char* pszParam, const CMyString& strParam);
};