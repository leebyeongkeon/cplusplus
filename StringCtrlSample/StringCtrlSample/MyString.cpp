#include "stdafx.h"
#include "MyString.h"
CMyString::CMyString()
	:m_pszData(NULL), m_nLength(0)
{

}
CMyString::CMyString(const CMyString& cParam) : m_pszData(NULL), m_nLength(0) {
	
	this->SetString(cParam.GetString());
}
CMyString::CMyString(const char* strParam) : m_pszData(NULL), m_nLength(0) {
	this->SetString(strParam);
}
CMyString::CMyString(CMyString&& rhs) : m_pszData(NULL), m_nLength(0) {
	cout << "이동 생성자 호출" << endl;
	m_nLength = rhs.m_nLength;
	m_pszData = rhs.m_pszData;
	rhs.m_nLength = 0;
	rhs.m_pszData = NULL;
}
CMyString::~CMyString()
{
	Release();
}
CMyString& CMyString::operator=(const CMyString& cParam) {
	
	if(this!=&cParam)
		this->SetString(cParam.GetString());
	return *this;
}
CMyString CMyString::operator+(const CMyString& rhs) {
	
	CMyString strResult(m_pszData);
	strResult.Append(rhs.GetString());
	return strResult;
	/*CMyString temp;
	//char* result = new char[m_nLength + rhs.m_nLength + 1];
	//strcpy(result, m_pszData);
	//strcpy(result + m_nLength*sizeof(char), rhs.m_pszData);
	//temp.SetString(result);
	//delete[] result;
	return temp;
	*/
}
CMyString& CMyString::operator+=(const CMyString& rhs) {
	Append(rhs.GetString());
	return *this;
	/*char* temp = new char[m_nLength + rhs.m_nLength + 1];
	strcpy(temp, m_pszData);
	strcpy(temp + m_nLength, rhs.m_pszData);
	Release();
	m_pszData = temp;
	m_nLength = strlen(m_pszData);
	*/
	
}

int CMyString::SetString(const char* pszParam)
{
	// TODO: 여기에 구현 코드 추가.
	Release();
	if (pszParam == NULL)return 0;
	int nLength = strlen(pszParam);
	if (nLength == 0) return 0;
	
	m_pszData = new char[nLength + 1];
	
	strcpy(m_pszData, pszParam);
	m_nLength = nLength;
	OnSetString(m_pszData, m_nLength);
	return nLength;
}


const char* CMyString::GetString() const
{
	// TODO: 여기에 구현 코드 추가.
	return m_pszData;
}

CMyString::operator char*(void) const {
	return m_pszData;
}
char& CMyString::operator[](int nIndex) {
	return m_pszData[nIndex];
}
char CMyString::operator[](int nIndex)const {
	return m_pszData[nIndex];
}
void CMyString::Release()
{
	// TODO: 여기에 구현 코드 추가.
	if(m_pszData!=nullptr)
		delete[] m_pszData;
	m_pszData = NULL;
	m_nLength = 0;
}
int CMyString::GetLength()const {
	return m_nLength;
}
int CMyString::operator==(const CMyString& rhs) {
	if (m_pszData != NULL && rhs.m_pszData != NULL)
		if (strcmp(m_pszData, rhs.m_pszData) == 0)
			return 1;
	return 0;
}
int CMyString::operator!=(const CMyString& rhs) {
	if (m_pszData != NULL && rhs.m_pszData != NULL)
		if (strcmp(m_pszData, rhs.m_pszData) == 0)
			return 0;
	return 1;
}
int CMyString::Append(const char* pszParam) {
	if (pszParam == NULL)
		return 0;
	int nLenParam = strlen(pszParam);
	if (nLenParam == 0)
		return 0;
	if (m_pszData == NULL) {
		SetString(pszParam);
		return m_nLength;
	}
	int nLenCur = m_nLength;
	char* pszResult = new char[nLenCur + nLenParam + 1];
	strcpy(pszResult, m_pszData);
	strcpy(pszResult + sizeof(char) * nLenCur, pszParam);
	Release();
	m_pszData = pszResult;
	m_nLength = nLenCur + nLenParam;
	return m_nLength;
}

void CMyString::OnSetString(char* pszData, int nLength)
{
	// TODO: 여기에 구현 코드 추가.

}
