#include<iostream>
//#include<Windows.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include<conio.h>
#include<vector>
#include<sstream>
#include<string>
#include<fstream>
#include "cursor.h"

using namespace std;
using namespace sql;

//vector컨테이너는 표준 템플릿 라이브러리(STL)에서 제공함. 배열을 대체할 수 있음.
//vector는 동적으로 요소를 추가할 수 있음.
//tokenize_getline()함수는 문자열과 구분자를 매개변수로 받아서 문자열을 나누고 그렇게 만들어진 문자열의 배열을 반환하는 함수
//이 함수는 https://psychoria.tistory.com/666에 있는 코드를 참고하였음.
vector<string> tokenize_getline(const string& data, const char delimiter) {
	vector<string> result;		
	string token;							//임시 변수인 토큰
	stringstream ss(data);					//stringstream은 주어진 문자열에서 필요한 정보를 빼내기 위해 사용함.
	while (getline(ss, token, delimiter)) {	//스트링스트림에 있는 문자열을 구분자가 나올 때까지 읽어서 token변수에 저장함.
		result.push_back(token);			//구분자는 버리고 다음 구분자가 나올 때까지 또 읽어서 token에 저장하고
	}										//token은 result에 추가함.
	return result;
}
class MyDBHelper
{
private:
	Connection* con;//사용할 db에 연결할 setSchema()함수가 있음. 
	Statement *stmt;//sql구문을 실행하는 역할.
	ResultSet* res;	//쿼리 결과 데이터를 관리함.
	Driver* driver;	//db서버에 연결하는 connect()함수가 포함되어 있음.
public:
	MyDBHelper();								//DB서버에 연결
	~MyDBHelper();
	vector<string> selOpponent(string&, string&);//계좌이체 상대의 DB정보를 가져오는 함수
	vector<string> selBanks();					//은행 DB를 가져오는 함수
	bool updateCharge(string&, int);			//카드번호로 잔액을 업데이트(ATM을 이용중인 고객) 
	bool updateCharge(int, string&);			//계좌번호로 잔액을 업데이트(계좌이체 상대 고객)
	string selectCard(string&);					//카드번호로 해당 고객 DB를 가져오는 함수
};
class Human
{
protected:
	char pwd[5];//고객과 관리자 둘다 비밀번호를 사용하므로. 
public:			//ATM의 비밀번호는 숫자만 입력하지만 앞자리가 0일수도 있으니 문자열로 함.
	bool checkPwd(char*);		//비밀번호 검사 함수.
	virtual char* inpPwd() = 0;	//비밀번호 입력용 함수를 순수가상함수로 선언함.
								//순수가상함수를 멤버 함수로 가질 경우 클래스는 추상클래스이고 인스턴스를 생성할 수 없음.
};								//순수 가상함수는 파생 클래스에서 반드시 재정의 해야 함.	
class Manager :public Human
{
private:
	char secondPwd[7];	//관리자용의 2차 비밀번호
	ifstream *ifs;		//파일 입력 스트림 클래스.
public:
	Manager();					//관리자 비밀번호를 저장한 파일로부터 비밀번호를 읽어옴
	~Manager();
	vector<string>readMyFile();	//관리자 파일 내용을 읽어옴.
	void printManager();		//관리자 메뉴를 출력
	char* inpPwd();				//비밀번호 입력 함수
	char* getSecondPwd();		//2차 비밀번호 입력 함수
	bool checkPwdSec(char*);	//2차 비밀번호 검사 함수
	void changePwd();			//변경할 비밀번호를 입력받아 인스턴스 변수들에 복사함. 입력받을 때는 길이를 검사함.
	void rewritePwd();			//변경한 비밀번호로 파일 데이터를 업데이트함.
};

class Customer :public Human
{
private:
	char cardNum[17];	//카드번호 16자리
	char name[10];		//고객 이름
	char accountNum[16];//계좌번호 15자리
	int charge;			//잔액
	MyDBHelper *db;		//Customer클래스 내에서만 db접근하도록 함.
public:
	bool exist;			//카드번호를 입력한 결과를 담음.
	Customer(string&);	//카드번호를 매개변수로 받아 인스턴스를 초기화함.
	~Customer();		//생성자에서 db인스턴스를 동적으로 생성한 것을 해제해야 함.
	char* inpPwd();		//비밀번호를 입력받음
	bool transCharge(int, int, string&);//계좌이체 함수.
	string bankCodes();					//은행코드 목록을 가져와서 출력한 후에 사용자로부터 계좌이체할 상대방의 은행코드를 입력받고 은행이름과 함께 반환함.
	vector<string> selByAccount(string&, string&);//송금할 상대방의 계좌번호와 은행코드로 상대의 정보를 가져옴.
	int getOutCharge(int);				//출금 동작.
	bool putInCharge(int);				//입금 동작.
	void getOutResult(int);				//입출금 시의 결과를 출력해줌.
	void getOutResult();				//계좌 잔액 조회 결과
	void getOutResult(int, string&);	//계좌이체 결과를 출력해줌.
};
class Machine
{
public:
	virtual void printMenu() = 0;	//순수가상함수. Machine은 추상클래스가 되고 인스턴스를 생성할 수 없음.
	virtual bool inputCard() = 0;
	virtual string inputCardNum() = 0;
};
class ATM:public Machine
{
	//메뉴는 5개 이상
private:
	Customer *guest;	//카드번호가 제대로 입력되었을 경우에만 고객 인스턴스를 생성해서 사용하도록 함.
public:
	static int b10000;	//지폐 잔량을 관리하는 정적 멤버 변수.
	ATM();
	void reWriteBill();	//지폐 잔량을 파일에 업데이트함.
	char* readBill();	//지폐 잔량을 파일로부터 읽어서 정적 변수에 저장.
	void printMenu();	//메인메뉴 출력
	int clickMenu();	//메뉴 클릭 동작.

	bool inputCard();				//메뉴를 클릭한 후 카드를 넣기 전에 취소할 경우도 고려함.
	string inputCardNum();			//카드번호를 입력받음.
	void printTransInfo(string, string, string, int); //계좌이체 메뉴 이용 시 상대의 정보와 송금액을 확인하는 함수.
	string inputAccount();			//계좌이체 메뉴에서 상대방의 계좌번호를 입력받음.
	void printSelCharge();			//출금 시 금액 목록을 출력해줌. 직접 입력할 수도 있음.
	int getSelCharge();				//금액을 선택함.
	int putCharge();				//입금동작.
	void removeCard();				//카드 제거 명령
	void cancel();					//화면 지운 후 취소 2초간 출력
	void okay();					//확인 버튼을 대신하기 함. 정보를 보여준 상태에서 아무 버튼이나 클릭하면 확인한 것으로 생각함
	void managerMenu(Manager&);		//관리자 메뉴 출력
	int switchMoney(int);			//사용자 
	void manaSwitch(int, Manager&);	//관리자 메뉴 클릭에 따라 분기함.
	bool cerifiAccount();			//계좌인증함수.
	void switchMenu(int);			//메인메뉴에서 클릭한 버튼에 따라 기능을 수행하는 함수. 프로그램의 실질적인 몸체임.
};
MyDBHelper::MyDBHelper() {
	try {
		driver = get_driver_instance();	//드라이버 인스턴스를 생성하고
		con = driver->connect("tcp://127.0.0.1:3306", "root", "1234");//드라이버로 db에 연결을 할때 연결할 db의 호스트 주소, db계정, 비밀번호를 넘겨야 함.
		con->setSchema("bank_db");		//프로젝트를 위해 생성한 db.
		stmt = con->createStatement();	//sql문을 db에 전달하기 위한 statement객체를 생성함.
		res = NULL;						//쿼리 결과 데이터 관리용 인스턴스를 가리키는 포인터 변수인데 아직 사용하지 않으므로 NULL로 함.
	}
	catch (SQLException& e) {
		cout << "에러코드 : " << e.getErrorCode() << endl;
		cout << "에러 : " << e.what() << endl;
		cout << "SQL상태 : " << e.getSQLState() << endl;
		exit(1);
	}
}
MyDBHelper::~MyDBHelper() {
	delete res;
	delete stmt;
	delete con;
}
vector<string> MyDBHelper::selOpponent(string& account, string& splitedBank) {
	try {
		vector<string> opponent;				//상대의 계좌정보를 반환할 변수. 이름과 잔액을 배열로 반환할 것임.
		string query = "SELECT name, deposit FROM account WHERE accountNum='" + account + "' AND bankCode='" + splitedBank + "';";
		res = stmt->executeQuery(query.c_str());//쿼리 실행
		int deposit; string strDeposit;
		if (res->next()) {										//실행 결과 행이 여러개인 경우 while로.
			opponent.push_back(res->getString("name").c_str());	//결과 행에서 name과 필드명이 일치하는 값을 가져옴.
			deposit = res->getInt("deposit");					//결과 행에서 deposit과 필드명이 일치하는 값을 가져옴.
			strDeposit = to_string(deposit);					//잔액은 정수이지만 이름과 함께 반환하려면 형 변환을 해야 함.
			opponent.push_back(strDeposit);						
		}
		else {
			system("cls");
			cout << "해당 계좌가 존재하지 않음";
			delay(2000);
		}//결과가 없는 상태로 그대로 반환하겠음.
		return opponent;
	}
	catch (SQLException& e) {
		cout << "에러코드 : " << e.getErrorCode() << endl;
		cout << "에러 : " << e.what() << endl;
		cout << "SQL상태 : " << e.getSQLState() << endl;
		exit(1);
	}
}

vector<string> MyDBHelper::selBanks() {
	try {
		vector<string> bankArray;							//반환 결과용 변수
		string query = "SELECT * FROM bank";				//bank테이블에서 모든 행을 가져오는 쿼리
		res = stmt->executeQuery(query.c_str());			//실행
			
		while (res->next()) {								//이번에는 결과가 여러개 있을 것이므로 while로 함. 행이 1개라도 while은 어차피 똑같이 동작할것임.
			bankArray.push_back(res->getString(1).c_str());	//은행이름과 코드를 계속해서 추가함.
			bankArray.push_back(res->getString(2).c_str());
		}
		return bankArray;
	}
	catch (SQLException& e) {
		cout << "에러코드 : " << e.getErrorCode() << endl;
		cout << "에러 : " << e.what() << endl;
		cout << "SQL상태 : " << e.getSQLState() << endl;
		exit(1);
	}
}
bool MyDBHelper::updateCharge(string& cardNum, int uCharge) {
	try {
		string query = "UPDATE account SET deposit=" + to_string(uCharge) + " WHERE cardNum='" + cardNum + "';";
		stmt->executeUpdate(query.c_str());//카드번호가 일치하는 계좌의 잔액을 업데이트하는 쿼리를 실행함. ATM 이용하는 고객 잔액 업데이트용
		return true;
	}//에러가 생기면 예외처리 코드가 실행되고 false가 반환. 성공적으로 업데이트 되면 true반환.
	catch (SQLException& e) {
		cout << "에러코드 : " << e.getErrorCode() << endl;
		cout << "에러 : " << e.what() << endl;
		cout << "SQL상태 : " << e.getSQLState() << endl;
		delay(2000);
		return false;
	}
}
bool MyDBHelper::updateCharge(int uCharge, string& account) {
	try {
		string query = "UPDATE account SET deposit=" + to_string(uCharge) + " WHERE accountNum='" + account + "';";
		stmt->executeUpdate(query.c_str());//계좌번호가 일치하는 계좌의 잔액을 업데이트함. 계좌이체 상대의 잔액 업데이트용 함수.
		return true;
	}
	catch (SQLException& e) {
		cout << "에러코드 : " << e.getErrorCode() << endl;
		cout << "에러 : " << e.what() << endl;
		cout << "SQL상태 : " << e.getSQLState() << endl;
		delay(2000);
		return false;
	}
}

string MyDBHelper::selectCard(string& cardNum) {
	try {
		string result = "";
		string query = "SELECT * FROM account WHERE cardNum='" + cardNum + "';";
		res = stmt->executeQuery(query.c_str());//카드번호로 일치하는 고객의 계좌 정보를 가져옴. Customer클래스 인스턴스 변수들 초기화용.
		if (res->next()) {
			string accountNum = res->getString(3).c_str();	//계좌번호
			string pwd = res->getString(5).c_str();			//비밀번호
			string name = res->getString(6).c_str();		//이름
			int deposit = res->getInt(7);					//잔액
			result += accountNum + "," + cardNum + "," + pwd + "," + name + "," + to_string(deposit);
		}//결과를 문자열 하나에 구분자를 넣어서 반환함.
		else {
			system("cls");
			cout << "등록된 카드가 아님";
			delay(2000);
		}
		return result;
	}
	catch (SQLException& e) {
		cout << "에러코드 : " << e.getErrorCode() << endl;
		cout << "에러 : " << e.what() << endl;
		cout << "SQL상태 : " << e.getSQLState() << endl;
		exit(1);
	}
}
bool Human::checkPwd(char* getPwd) {	//비밀번호 검사용 함수
	int cmp = strcmp(pwd, getPwd);		//strcpm는 문자열 비교 함수. 일치하는 경우 0반환
	if (cmp == 0)
		return true;
	else
		return false;
}
Manager::Manager() {
	vector<string> pwds = readMyFile();					//파일에서 비밀번호들을 읽어오기
	vector<string>pwd1 = tokenize_getline(pwds[0], ':');//1차 2차 비밀번호를 구분자로 분리하고
	vector<string>pwd2 = tokenize_getline(pwds[1], ':');
	strcpy_s(pwd, sizeof(pwd), pwd1[1].c_str());		//각각의 인스턴스 변수에 복사.
	strcpy_s(secondPwd, sizeof(secondPwd), pwd2[1].c_str());

}
Manager::~Manager() {
	delete ifs;
}
vector<string> Manager::readMyFile() {
	ifs = new ifstream("manager.txt");				//파일 출력 스트림 클래스로 관리자 파일 열기.
	char line[100];									//읽어들일 임시 변수
	vector<string> pwds;							//반환할 배열
	if (ifs->is_open()) {							//파일이 열렸으면 파일 출력 스트림으로 줄 단위로 읽어서 line에 저장하고
		while (ifs->getline(line, sizeof(line))) {	
			pwds.push_back(line);					//배열에 line을 추가함.
		}
		ifs->close();
	}
	else {
		system("cls");
		cout << "파일 읽기 실패";
		exit(1);
	}
	return pwds;
}
void Manager::printManager() {	//관리자 메뉴 출력용 함수
	system("cls");
	cout << "1.현금 수량 확인\n\n\n2.현금 충전\n\n\n3.관리자 비밀번호 변경" << endl;
}

char* Manager::inpPwd() {		//관리자 1차 비밀번호 입력받기
	system("cls");
	char pwd[5] = "";
	cout << "1차 비밀번호 : ";
	cin >> pwd;
	return pwd;
}
char* Manager::getSecondPwd() {	//관리자 2차 비밀번호 입력
	system("cls");
	char secPwd[7] = "";
	cout << "2차 비밀번호 : ";
	cin >> secPwd;
	return secPwd;
}
bool Manager::checkPwdSec(char* getPwd) {	//관리자 2차 비밀번호 검사 함수
	int cmp = strcmp(secondPwd, getPwd);
	if (cmp == 0)
		return true;
	else
		return false;
}
void Manager::changePwd() {				//관리자 비밀번호 변경용 함수
	string temp1, temp2;				//입력값을 저장할 임시 변수
	system("cls");
	cout << "<비밀번호 변경>\n" << endl;
	do {
		gotoxy(0, 2);
		cout << "1차 비밀번호 입력 : ";
		cin >> temp1;
		if (temp1.length() + 1 != sizeof(pwd))//길이를 검사한 후 4자가 아니니면 재입력받음. 
			cout << "4자로만 입력바람" << endl;
	} while (temp1.length() + 1 != sizeof(pwd));//length()함수는 string클래스의 함수인데 문자열의 길이를 반환함. char[]는 배열의 마지막에 NULL문자 이므로 실제로 길이를 맞추려면 +1
	do {
		gotoxy(0, 3);
		cout << "2차 비밀번호 입력 : ";
		cin >> temp2;
		if (temp2.length() + 1 != sizeof(secondPwd))//2차 비밀번호도 길이 검사를 받음
			cout << "6자로만 입력바람" << endl;
	} while (temp2.length() + 1 != sizeof(secondPwd));
	strcpy_s(pwd, sizeof(pwd), temp1.c_str());				//길이 검사 후에 둘 다 인스턴스 변수에 복사함.
	strcpy_s(secondPwd, sizeof(secondPwd), temp2.c_str());
}
void Manager::rewritePwd() {
	ofstream ofs("manager.txt");					//관리자 파일을 파일 출력 스트림으로 열어서
	if (ofs.is_open()) {
		ofs << "1st password:" << pwd << endl;		//파일에 1차 비밀번호와 2차 비밀번호를 기록함.
		ofs << "2nd password:" << secondPwd << endl;
		ofs.close();
	}
	else {
		system("cls");
		cout << "파일 쓰기 실패";
		exit(1);
	}
}
Customer::Customer(string& cardNumber) {	//카드번호로 고객 인스턴스 값들을 초기화하는 생성자
	db = new MyDBHelper();					//동적으로 db클래스의 인스턴스를 생성하고
	string getData;							//db값을 받아와서 저장할 문자열
	getData = db->selectCard(cardNumber);	//카드번호로 db값을 불러옴

	if (getData.length() == 0) {	//결과 길이가 0인 경우는 테이블에 입력한 카드번호에 해당하는 계좌가 없다는 것인데
		exist = false;				//등록되지 않은 카드인 것으로 생각함
		return;						//생성자는 return문을 사용하지 않지만 아래 코드들을 실행하지 않기 위해 여기서 종료.
	}
	exist = true;					//결과가 0이 아니면 카드번호에 해당하는 계좌가 있다는 것임.
	vector<string> result = tokenize_getline(getData, ',');//구분자로 회원정보를 나눠서 배열로 만듦.
	string account = result[0];
	string card = result[1];
	string accountPwd = result[2];
	string sName = result[3];
	string sCharge = result[4];
	//결과값들을 담을 임시변수를 선언하고, 임시변수에서 인스턴스 변수들에 값을 복사
	strcpy_s(accountNum, sizeof(accountNum), account.c_str());
	strcpy_s(cardNum, sizeof(cardNum), card.c_str());
	strcpy_s(pwd, sizeof(pwd), accountPwd.c_str());
	strcpy_s(name, sizeof(name), sName.c_str());
	charge = atoi(result[4].c_str());	//atoi는 문자열을 바로 정수로 바꿔주는 함수. c언어 표준 라이브러리인 stdlib.h에 정의
}
Customer::~Customer() {
	delete db;
}
char* Customer::inpPwd() {	//Human클래스에서 순수가상함수로 선언된 비밀번호 입력함수. Customer에서 재정의함.
	system("cls");
	char pwd[5] = "";
	cout << "비밀번호 : ";
	cin >> pwd;
	return pwd;
}
bool Customer::transCharge(int getCharge, int oppoCharge, string& oppoAccount) {
	if (getCharge > charge) {				//계좌이체함수. 송금할 금액이 소지액보다 적다면 실패.
		return false;
	}
	charge -= getCharge;						//송금 후의 잔액 계산
	string card(cardNum);						//카드번호로 string인스턴스 생성.
	db->updateCharge(card, charge);				//내 계좌에서 송금액만큼 마이너스

	oppoCharge += getCharge;					//송금받은 후의 상대방 잔액
	db->updateCharge(oppoCharge, oppoAccount);	//송금받은 후의 잔액으로 상대방의 DB업데이트
	return true;
}
string Customer::bankCodes() {				//은행 목록을 DB에서 받아와서 출력하고 코드를 입력받는 함수 
	vector<string> banks = db->selBanks();	//은행목록을 보면서 은행코드를 입력하고 입력받은 은행의 코드와 이름을 가져옴.
	int size = banks.size();				//vector의 사이즈는 vector배열의 길이를 알 수 있는 함수임.
	string selected; int i;
	system("cls");
	for (i = 0; i < size; i += 2) {			//배열의 크기 절반만큼의 반복횟수. 은행코드와 은행명을 각각 출력.
		cout << banks[i + 1].c_str() << "\t:\t" << banks[i] << "\n" << endl;
	}										//db에 등록되어 있는 은행목록을 출력한다.
	cout << "\n\n은행코드 입력 : ";
	string code;
	cin >> code;
	i = 0;
	while (i < size) {						//은행코드에 해당하는 배열 요소들(짝수 인덱스 : 0,2,4,...) 중에서 일치하는 값을 찾기
		if (code == banks[i])
			break;
		i += 2;
	}
	if (i == size)							//반복을 마칠 때까지 일치하는 코드를 찾지 못했다는 것이므로
		return selected;					//입력한 은행코드가 은행목록 DB에 없는 경우.

	selected = banks[i] + "," + banks[i + 1];
	return selected;						//은행코드와 은행명에 구분자를 넣어서 같이 반환함.
}
vector<string> Customer::selByAccount(string& account, string& splitedBank) {
	vector<string> opponent = db->selOpponent(account, splitedBank);
	return opponent;						//계좌이체 시에 사용. 고객이 입력한 계좌번호와 은행코드에 일치하는 계좌정보 가져오기.
}											
int Customer::getOutCharge(int getCharge) {	//출금함수.
	if (getCharge > ATM::b10000 * 10000)	//ATM기기의 지폐 잔량이 출금액보다 적을 경우는 실패.
		return -2;							
	if (charge < getCharge)					//고객의 잔액이 출금액보다 적을 경우 실패.
		return -1;							
	charge -= getCharge;					//고객 인스턴스 정보 업데이트
	ATM::b10000 -= getCharge / 10000;		//기기에서도 현금이 인출됨
	string card(cardNum);					
	if (!db->updateCharge(card, charge)) {	//고객 db값 업데이트.
		cout << "DB업데이트 실패" << endl;
		return 1;
	}
	return 0;
}
bool Customer::putInCharge(int getCharge) {	//입금함수.
	charge += getCharge;					//인스턴스 변수 값 업데이트.
	string card(cardNum);					//기기 현금이나 고객 잔액이 마이너스가 될 일은 없으므로 검사는 하지 않음.
	if (!db->updateCharge(card, charge)) {	//db값도 업데이트.
		cout << "DB업데이트 실패" << endl;
		return false;
	}
	return true;
}
void Customer::getOutResult(int getCharge) {				//출금 시에 고객 정보와 잔액 출력. 
	system("cls");											//입금 결과도 같은 정보를 보여줄 것이기 때문에 같은 함수로 처리하기로 함.
	cout << "계좌번호 : " << accountNum << endl;				
	cout << "\n거래금액 : " << getCharge << " 원" << endl;
	cout << "\n거래 후 잔액 : " << charge << " 원" << endl;
	//cout << "\n수수료 : " << 0 << " 원" << endl;
	delay(2000);
}
void Customer::getOutResult() {								//예금조회. 다른 매개변수가 필요하지 않음
	system("cls");
	cout << "계좌번호 : " << accountNum << endl;
	cout << "\n잔액 : " << charge << " 원" << endl;
	delay(2000);
}
void Customer::getOutResult(int tranCharge, string& destAccount) {	//계좌이체 결과. 대상 고객의 정보를 인자로.
	system("cls");
	cout << "계좌번호 : " << destAccount << endl;					//상대 계좌번호	
	cout << "\n거래금액 : " << tranCharge << "원" << endl;			//송금액
	cout << "\n거래 후 잔액 : " << charge << "원" << endl;			//고객 본인의 잔액
}
ATM::ATM() {
	guest = NULL;											//guest는 고객이 카드를 꽂기 전까진 NULL로 함.
	char* bill = readBill();								//파일로부터 지폐 잔량 읽어들임.
	vector<string>billInfo = tokenize_getline(bill, ':');	//구분자 ':'로 문자열을 나눠 문자열 배열을 반환받고
	b10000 = atoi(billInfo[1].c_str());						//배열의 2번째 요소를 정수로 변환하여 정적변수 b10000에 저장함. 기계가 켜질때 지폐 수량을 초기화함.
}														
void ATM::reWriteBill() {
	ofstream ofs("bill.txt");								//파일 출력 스트림 클래스로 지폐 정보 파일을 열어서
	if (ofs.is_open()) {									//파일이 열리면 아래와 같은 형식으로 파일 쓰기를 함.
		ofs << "count:" << b10000 << endl;
		ofs.close();
	}
	else {
		system("cls");
		cout << "파일 쓰기 실패";
		exit(1);
	}
}
char* ATM::readBill() {					//파일 입력 스트림 클래스. 지폐 잔량을 기록하는 파일을 열기
	ifstream ifs("bill.txt");			
	char line[100];						//파일로 읽은 값을 반환할 변수.
	if (ifs.is_open()) {
		ifs.getline(line, sizeof(line));//파일에서 줄 단위로 읽어들이기
		ifs.close();
	}
	else {
		system("cls");
		cout << "파일 읽기 실패";
		exit(1);						//지폐 잔량을 알 수 없어서 ATM을 운용할 수도 없음.
	}
	return line;
}
void ATM::printMenu() {					//4개의 고객 기능이 있고 5번은 관리자 메뉴로 들어가게 됨.
	system("cls");
	cout << "1.예금출금\n\n\n2.계좌이체\n\n\n3.예금조회\n\n\n4.입금하기" << endl;//\t\t5.관리자
}
int ATM::clickMenu() {					
	int btnNum = _getch();				//버퍼를 사용하지 않고 사용자로부터 값을 입력받는 _getch()함수. 클릭한 문자에 해당하는 아스키 코드 값을 반환함.
	if (btnNum == 'q' || btnNum == 'Q')	//원래 q나 Q키의 경우 clickMenu()에서 -1로 바꿔서 반환했으나 
		return -100;					//'0'을 뺐을때 실제로 -1이 되는 '/'를 누르면 종료가 되는 문제가 생겨 아스키코드에서 '0'값을 빼도 나올 수 없는 범위의 값으로 바꾸기로 함.
	btnNum -= '0';						//0의 아스키 코드값을 빼면 숫자들은 해당하는 아스키 코드값이 아닌 원래 정수값을 갖는다. 0은 48, 1은 49이므로
	return btnNum;
}

bool ATM::inputCard() {					//카드를 넣는지 취소하는지에 따라 bool값 반환
	system("cls");
	cout << "카드를 넣어주십시오.\t투입(c 또는 C를 제외한 키)\t취소(c 또는 C)" << endl;
	int okCancel = _getch();
	if (okCancel == 'c' || okCancel == 'C')
		return false;					//메뉴를 입력한 상태에서 카드 투입 단계에서 취소한 것으로 생각함.
	return true;						//카드를 정상적으로 꽂은 것으로 생각함.
}
string ATM::inputCardNum() {
	string cardNum;
	do {
		system("cls");
		cout << "카드번호 : ";			//실제로는 카드를 투입하면 자동으로 읽겠지만 
		cin >> cardNum;					//진짜 ATM기계는 아니므로 카드번호를 콘솔에서 입력받겠음
		if (cardNum.length() != 16) {	//카드번호는 16자로 고정되어 있는데 입력한 번호의 길이가 맞지 않다면 
			system("cls");				//카드를 재입력 받도록 함.
			cout << "카드를 다시 꽂아주십시오.";
			delay(2000);
		}
	} while (cardNum.length() != 16);	//카드번호 길이가 안맞는 것은 잘못 꽂은 것으로 생각하고 다시 입력함.
	return cardNum;
}
void ATM::printTransInfo(string account, string name, string bankName, int transCharge) {
	system("cls");
	cout << "입금은행 : " << bankName << endl;			//송금 시에 거래 정보 출력. 상대의 은행 이름
	cout << "\n계좌번호 : " << account << endl;			//상대방 계좌번호
	cout << "\n성명 : " << name << endl;					//상대방 성명
	cout << "\n금액 : " << transCharge << "원" << endl;	//송금액
}
string ATM::inputAccount() {
	string account;
	do {
		system("cls");
		cout << "받으실 분의 계좌번호 : ";		//계좌이체 시에 상대방의 계좌번호를 입력받음.
		cin >> account;
		if (account.length() != 15) {			//길이를 검사하여 15자가 아닌 경우 다시 입력받음.
			system("cls");
			cout << "15자로 입력하십시오.";
			delay(2000);
		}
	} while (account.length() != 15);
	return account;
}						
void ATM::printSelCharge() {					//출금 금액 출력 함수

	system("cls");
	cout << "1.1 만원\t2.2 만원\n" << endl;
	cout << "3.3 만원\t4.5 만원\n" << endl;
	cout << "5.7 만원\t6.10만원\n" << endl;
	cout << "7.15만원\t8.20만원\n" << endl;
	cout << "\t\t9.기타\n\n취소(c)" << endl;
}
int ATM::getSelCharge() {						//출금할 금액을 선택하는 함수. 위에서 8번까지는 선택한 번호에 해당하는 금액으로, 9번은 직접 입력받을 것
	int getNum;
	do {
		getNum = _getch();
		if (getNum == 'c' || getNum == 'C')		//c 또는 C는 취소키로 사용
			return 0;							//취소키를 누른 경우 중간 반환하고
		getNum -= '0';							
	} while (getNum < 1 || getNum>9);			//숫자로 변환했을 때 1부터 9까지의 수가 아니라면 재입력한다.

	if (getNum == 9) {							//기타는 출금할 금액을 직접 입력하는 것으로 함. 단위는 만원
		cout << "\n직접 입력(단위: 만 원) : ";
		cin >> getNum;
	}
	else {
		getNum = switchMoney(getNum);			//직접 입력이 아니라 1~8번까지 중에서 고른 경우 해당 금액으로 바꿔주는 함수.
	}
	return getNum * 10000;						//만 원 단위로 했으므로 곱해줌.
}
int ATM::putCharge() {							//입금동작.
	int getNum;
	do {										//지폐를 넣는 동작.
		system("cls");
		cout << "입금할 돈을 넣어주십시오(단위는 만 원, 입금 취소는 0) : ";
		cin >> getNum;
		if (getNum > 100)
			cout << "1회 한도는 100만원입니다." << endl;
	} while (getNum < 0 || getNum>100);			//0미만인 경우 잘못 입력한 것으로 생각하고 그냥 재입력.
	return getNum * 10000;						//0은 입금 취소인데 10000을 곱해도 어차피 0이 반환되므로 반환받는 함수에서 처리함.
}
void ATM::removeCard() {						//사용 종료 후 카드 제거. 카드 제거 동작은 아무 키나 입력받는 것으로 대신함.

	system("cls");
	cout << "카드를 제거해주십시오(아무 키나 입력)";
	_getch();									//여기서는 딱히 쓸 일이 없으므로 반환값은 그냥 무시함.
}
void ATM::cancel() {							//중간에 취소 버튼을 누를 경우

	system("cls");								//2초간 취소한다는 내용을 콘솔에 출력함.
	cout << "거래 취소" << endl;
	delay(2000);
}
void ATM::okay() {
	cout << "\n확인(아무 키나 입력)";				//확인 버튼을 누르는 동작을 대체함.
	_getch();
}
void ATM::managerMenu(Manager& m) {				//관리자용 메뉴.
	for (;;) {
		m.printManager();						//메뉴 출력.
		int clicked = clickMenu();				//메인 메뉴를 클릭할 때 처리해주는 함수와 동일함.
		if (clicked == -100) {					//q또는 Q입력
			system("cls");
			cout << "관리자용 메뉴 종료" << endl;
			delay(2000);
			break;
		}
		else if (clicked < 1 || clicked>3) continue;
												//클릭한 메뉴가 1부터 3까지의 정수가 아니면 이번 반복을 넘기고 다시 출력함.
		else {
			manaSwitch(clicked, m);				//1,2,3입력시
		}
	}
}
int ATM::switchMoney(int param) {				//출금 메뉴에서 고른 번호를 해당 금액으로 바꿔주는 단순한 함수임.
	int result = 0;
	switch (param) {
	case 1:
		result = 1;
		break;
	case 2:
		result = 2;
		break;
	case 3:
		result = 3;
		break;
	case 4:
		result = 5;
		break;
	case 5:
		result = 7;
		break;
	case 6:
		result = 10;
		break;
	case 7:
		result = 15;
		break;
	case 8:
		result = 20;
		break;
	}
	return result;
}
void ATM::manaSwitch(int clicked, Manager& manager) {
	system("cls");
	switch (clicked) {	//클릭한 번호를 인자로 받아서 분기함.
	case 1:				//지폐 잔량 확인
		{				//정적 변수b10000은 기기에 남아있는 지폐의 수량임.
			cout << "기기의 남은 지폐 : " << b10000 << "장" << endl;
			okay();
			break;
		}
	case 2:				//지폐 충전
		{				//관리자는 지폐를 충전하고 지폐 파일을 최신화함.
			int bill;
			cout << "지폐를 넣어주십시오(단위 : 만 원) : ";
			cin >> bill;
			b10000 += bill;
			reWriteBill();
			break;
		}
	case 3:
		{				//비밀번호를 바꾸는 경우. 1차와 2차를 모두 입력하고 파일에 기록.
			manager.changePwd();
			manager.rewritePwd();
			break;
		}
	}
}
bool ATM::cerifiAccount() {			//카드 투입시 계좌 인증 함수.
	if (inputCard() == false) {		//카드를 투입 시 취소를 누른 경우 
		cancel();
		return false;
	}
	string cardNum;
	cardNum = inputCardNum();				//카드 번호를 입력받음.
	guest = new Customer(cardNum);			//카드를 투입하면 카드번호로 고객정보를 가져올 것이고 이용중인 고객 1명을 가리키는 포인터
	if (!guest->exist)						//등록되지 않은 카드의 경우 false를 반환하고 거래 실패.
		return false;
	if (!guest->checkPwd(guest->inpPwd())) {//4자리의 비밀번호를 입력하는데 틀릴 경우
		system("cls");						//거래가 실패하며 false를 반환하며 처음 메뉴로 돌아감.
		cout << "비밀번호 틀림" << endl;
		delay(2000);
		cancel();
		return false;
	}
	return true;							//카드 투입 후에 카드정보를 제대로 읽어들여 계좌가 인증된 경우.
}
void ATM::switchMenu(int button) {
	switch (button) {
	case 1://예금출금
	{
		if (!cerifiAccount())				//계좌인증 실패할 경우
			break;
		printSelCharge();					//출금액 메뉴 출력
		int getCharge = getSelCharge();		//출금할 금액을 입력받고
		if (getCharge == 0) {				//금액 선택 메뉴에서 취소를 누른 경우
			cancel();
			break;
		}									//출금하는 함수. DB업데이트 과정도 포함시킴.
		int getChResult = guest->getOutCharge(getCharge);
		if (getChResult == 0) {				//결과가 0이 반환되면 출금에 성공한 것이고
			reWriteBill();					//출금한 만큼 지폐 잔량도 최신화해야 함.
			system("cls");
			cout << "출금 성공";
			delay(2000);

			guest->getOutResult(getCharge);	//거래결과를 출력해줌
			okay();							//확인버튼 대체
			system("cls");
			cout << "거래 종료";
			delay(2000);
		}
		else if (getChResult == -1) {		//-1을 반환하는 경우는 고객 잔액 부족
			cout << "잔액 부족";
			delay(2000);
			cancel();
		}
		else if (getChResult == -2) {		//-2는 기기의 지폐 부족
			cout << "기기 현금 부족";
			delay(2000);
			cancel();
		}
		else {//1
			cancel();
		}
		break;
	}
	case 2://계좌이체
	{
		if (!cerifiAccount())				//계좌인증
			break;
		string bankInfo;
		do {
			bankInfo = guest->bankCodes();	//은행정보를 출력하고 은행코드를 입력받고 해당 결과를 받아옴
			if (bankInfo.length() == 0) {	//받아온 결과의 길이가 0이라는 것은 목록에 없는 코드이므로 잘못 입력한 것.
				cout << "\n목록에 있는 은행 코드만을 입력하여 주십시오.";
				delay(2000);
			}

		} while (bankInfo.length() == 0);
		string account = inputAccount();		//상대방 계좌번호
												//은행정보를 구분자로 나눠서
		vector<string>splitedBank = tokenize_getline(bankInfo, ',');
												//계좌번호와 은행코드를 넘겨서 상대방의 잔액과 이름을 받아옴.
		vector<string> opponent = guest->selByAccount(account, splitedBank[0]);
		if (opponent.size() == 0)				//결과의 size()가 0이면 해당하는 상대 계좌가 존재하지 않는 것임.
			break;
		string str = opponent[1].c_str();		//상대방의 잔액
		int oppoCharge = atoi(str.c_str());
		printSelCharge();
		int getCharge = getSelCharge();			//송금할 금액을 입력받고
		if (getCharge == 0) {					//금액 선택 메뉴에서 취소를 누른 경우
			cancel();
			break;
		}

		string name = opponent[0].c_str();					//송금받을 예금주
		string bankName = splitedBank[1].c_str();			//은행이름
		printTransInfo(account, name, bankName, getCharge);	//거래 내역 확인
		okay();
		
		//위에서 확인을 누른 후에 이 단계에서 송금됨. db업데이트 과정 포함.
		if (!guest->transCharge(getCharge, oppoCharge, account)) {	
			cout << "금액 부족" << endl;
			cancel();
			break;
		}

		guest->getOutResult(getCharge, account);			//결과 보여주기(상대방 은행, 계좌번호, 이름, 금액)
		okay();
		break;
	}
	case 3:										//예금조회
	{	
		if (!cerifiAccount())
			break;
		guest->getOutResult();					//회원 정보 조회.
		okay();

		system("cls");

		cout << "거래 종료";
		delay(2000);

		break;
	}
	case 4://입금하기
	{
		if (!cerifiAccount())
			break;
		int putCharge = 0; putCharge = this->putCharge();
		if (putCharge == 0) {								//금액 투입 과정에서 취소를 누른 경우
			cancel();
			break;
		}
		b10000 += putCharge / 10000;
		reWriteBill();										//입금할때 지폐를 넣는 것도 파일에 기록함.
		bool getChResult = guest->putInCharge(putCharge);	//입금액이 계좌에 반영됨.
		if (getChResult == true) {

			system("cls");
			cout << "입금 성공";
			delay(2000);

			guest->getOutResult(putCharge);					//결과를 출력.
			okay();

			system("cls");
			cout << "거래 종료";
			delay(2000);
			break;
		}
	}
	case 5://관리자용
	{//관리자 인스턴스 생성. 생성자에서 파일 데이터를 읽어 인스턴스 변수인 비밀번호들을 초기화하고
		Manager manager;
		if (!manager.checkPwd(manager.inpPwd())) {			//비밀번호를 입력받아서 검사함
			system("cls");
			cout << "1차 비밀번호 틀림" << endl;
			delay(2000);
			break;
		}
		if (!manager.checkPwdSec(manager.getSecondPwd())) {	//2차 비밀번호 입력 후 검사
			system("cls");
			cout << "2차 비밀번호 틀림" << endl;
			delay(2000);
			break;
		}
		managerMenu(manager);								//관리자 메뉴로 이동.
		break;
	}

	}//swtich
	if (guest != NULL) {
		removeCard();	//취소할때 작성할 수도 있으나 거래가 성공해도 카드는 제거하므로 마지막에 한 번만 호출하기로함.
		delete guest;
		guest = NULL;	//=NULL을 하지 않으면 1회 거래 후에 다시 거래를 시도하다가 카드를 넣기 전에 취소하는 경우 에러. 동적메모리가 해제되었어도 guest가 NULL은 아니기 때문에
	}					//해당 경우에 위 조건문에 안들어오기 위해 다시 NULL.
}
int ATM::b10000;		//ATM의 정적 멤버 변수. 지폐 잔량을 관리하기 위해 사용함.
int main()
{
	ATM A;							//ATM은 24시간 켜져있으니 프로그램 시작부터 끝까지 인스턴스가 계속 1개 있고
	for (;;) {						//이용자 한명이 이용을 마쳐도 처음 메뉴를 출력하면서 계속 켜져있음
		A.printMenu();				//메뉴출력
		int clicked=A.clickMenu();	//사용자가 메뉴를 클릭
		if (clicked == -100) {		//clickMenu함수에서 q 또는 Q키를 입력한 경우 반복을 종료함.
			system("cls");			 
			break;					
		}
		else if (clicked >= 1 && clicked <= 4) {
			A.switchMenu(clicked);	//클릭한 값으로 해당하는 기능을 이용할 수 있음.
		}
		else if (clicked == -21) {	//ESC키를 누른 경우 관리자 메뉴로 넘어감. 시작메뉴에는 보여지지 않음.
			clicked = 5;			//ESC의 아스키 코드값이 27이고 0의 아스키 코드값이 48인데 0의 값을 빼서 반환해서 -21이 됨.
			A.switchMenu(clicked);
		}
		else
			continue;				//클릭한 메뉴가 1부터 4까지의 정수이거나 ESC키가 아니면 다시 처음 메뉴를 출력함.
	}
	cout << "종료" << endl;
	return 0;
}