#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define SIZE 20

int TestFunc(int *);
int main()
{
	int i = 0, nCount = 0;
	int aList[SIZE] = { 0 };
	srand(time(NULL));
	for (i = 0; i < SIZE; i++)
	{
		aList[i] = rand() % 20 + 1;
		printf("%d\n", aList[i]);
	}
	printf("최대 최소 구하기 그리고 비교횟수\n");
	nCount = TestFunc(aList);
	printf("비교 횟수는 총 %d 회\n", nCount);
	return 0;
}
int TestFunc(int *nList)
{
	int nCount = 0;//비교 횟수
	int i = 0, j = 0, max, min;
	int Smalln[SIZE / 2] = { 0 };							//원래 배열의 크기의 1/2만큼
	int Bign[SIZE - sizeof(Smalln) / sizeof(int)] = { 0 };//원래 배열의 크기에서 Smalln을 뺀 만큼
	if (sizeof(Bign) / sizeof(int) % 2 == 1) {
		Bign[sizeof(Bign) / sizeof(int) - 1] = nList[SIZE - 1];
	}
														//Bign이 홀수배열일 경우 원 배열의 끝값을 Bign의 끝에 저장

	for (i = 1; i < SIZE; i += 2)						//원 배열의 원소들을 i-1와 i 두 묶음으로 나누어 
														//큰 값을 Bign에 저장하고 작은 값들을 Smalln에 저장
	{

		if (nList[i] >= nList[i - 1])
		{
			Bign[i / 2] = nList[i];
			Smalln[i / 2] = nList[i - 1];
			nCount++;
		}
		else
		{
			Smalln[i / 2] = nList[i];
			Bign[i / 2] = nList[i - 1];
			nCount++;
		}
	}
	max = Bign[0];
	min = Smalln[0];
	for (i = 1; i < SIZE / 2; i++)//원래 배열이 홀수 배열인 경우 
	{							//맨 끝 원소는 앞에서 이미 할당이 끝나있음
		if (Bign[i] > max)
			max = Bign[i];
		nCount++;
		if (Smalln[i] < min)
			min = Smalln[i];
		nCount++;
	}
	if (SIZE % 2 == 1)
	{													//원래 배열의 크기가 홀수일때만
		if (Bign[sizeof(Bign) / sizeof(int) - 1] > max)		//마지막 원소는 한번도 비교하지 않은 								
			max = Bign[sizeof(Bign) / sizeof(int) - 1];//상태이므로 각각 한번씩만 수행해준다.
		nCount++;
		if (Bign[sizeof(Bign) / sizeof(int) - 1] < min)
			min = Bign[sizeof(Bign) / sizeof(int) - 1];
		nCount++;
	}
	printf("MAX = %d,\t MIN = %d\n", max, min);

	return nCount;//입력크기 n에 대하여 단위연산 수행횟수 nCount는
}					//최초에 배열을 둘로 나누어 저장할때 n/2회, Smalln의 크기(n/2)에서 -1회

					//원래 배열이 짝수일 경우 Bign의 크기(n/2)에서 -1회 수행으로
					// n/2 + (n/2-1) + (n/2-1) 이고 정리하면
					//총 1.5n-2회

					//원래 배열이 홀수일 경우 Bign의 크기(n/2+1)에서 
					//-2회 수행 후에 마지막 원소로 2회를 수행하면
					// n/2 + (n/2-1) + (n/2+1-2) + 2 이고 정리하면
					//총 1.5n회
					//둘다 1.5n회를 넘기지 않습니다.