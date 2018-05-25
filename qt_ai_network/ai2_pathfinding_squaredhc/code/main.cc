#include "FindPath.h"
#include <stdio.h>
#include <chrono>

int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY, const unsigned char* pMap, const int nMapWidth, const int nMapHeight, int* pOutBuffer, const int nOutBufferSize);

int main()
{

	unsigned char pMap0[] = {
		1, 1,
		1, 1 };

	int pOutBuffer0[12];
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;
	start = std::chrono::high_resolution_clock::now();
	printf("0 len: %d\n", FindPath(0, 0, 1, 1, pMap0, 2, 2, pOutBuffer0, 12));
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());
	for (int i = 0; i < 12; i++)
	{
		printf("%d\n", pOutBuffer0[i]);
	}


	unsigned char pMap[] = { 
		1, 1, 1, 1, 
		0, 1, 0, 1, 
		0, 1, 1, 1 };
	int pOutBuffer[12];

	start = std::chrono::high_resolution_clock::now();
	printf("1 len: %d\n", FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12));
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());
	for (int i = 0; i < 12; i++)
	{
		printf("%d\n", pOutBuffer[i]);
	}

	unsigned char pMap2[] = { 
		0, 0, 1, 
		0, 1, 1, 
		1, 0, 1 };
	int pOutBuffer2[7];
	start = std::chrono::high_resolution_clock::now();
	printf("\n2 len: %d\n", FindPath(2, 0, 0, 2, pMap2, 3, 3, pOutBuffer2, 7));
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());
	for (int i = 0; i < 7; i++)
	{
		printf("%d\n", pOutBuffer2[i]);
	}

	unsigned char pMap3[] = {
		1, 1, 1,
		0, 1, 1,
		1, 1, 1 };
	int pOutBuffer3[10];
	start = std::chrono::high_resolution_clock::now();
	printf("\n3 len: %d\n", FindPath(2, 0, 0, 2, pMap3, 3, 3, pOutBuffer3, 10));
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());
	for (int i = 0; i < 10; i++)
	{
		printf("%d\n", pOutBuffer3[i]);
	}

	unsigned char pMap5[] = {
		1, 1, 0,
		1, 1, 1,
		1, 1, 1 };
	int pOutBuffer5[10];
	start = std::chrono::high_resolution_clock::now();
	printf("\n4 len: %d\n", FindPath(2, 2, 0, 0, pMap5, 3, 3, pOutBuffer5, 10));
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());
	for (int i = 0; i < 10; i++)
	{
		printf("%d\n", pOutBuffer5[i]);
	}

	unsigned char pMap6[] = {
		1, 1, 1, 1, 1,
		1, 0, 1, 0, 1,
		1, 0, 1, 1, 0,
		1, 0, 1, 0, 1,
		1, 1, 1, 1, 1,
	};

	unsigned char testas[] = {
		0,  1,  2,  3,  4,
		5,  6,  7,  8,  9,
		10, 11, 12, 13, 14,
		15, 16, 17, 18, 19,
		20, 21, 22, 23, 24,
	};
	int pOutBuffer6[10];
	start = std::chrono::high_resolution_clock::now();
	printf("\n5 len: %d\n", FindPath(0, 2, 2, 2, pMap6, 5, 5, pOutBuffer6, 10));
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());
	for (int i = 0; i < 10; i++)
	{
		printf("%d\n", pOutBuffer6[i]);
	}

	unsigned char pMap4[262144];
	memset(pMap4, 1, sizeof(char) * 262144);
	int pOutBuffer4[4000];
	start = std::chrono::high_resolution_clock::now();
	int len = FindPath(0, 0, 511, 511, pMap4, 512, 512, pOutBuffer4, 4000);
	end = std::chrono::high_resolution_clock::now();
	printf("\n6 len: %d\n", len);
	elapsed_seconds = end - start;
	printf("time passed: %f\n", elapsed_seconds.count());

	printf("%d\n", pOutBuffer4[0]);
	printf("%d\n", pOutBuffer4[len-1]);
	printf("correct connections: %d", ((511-1)*512)+((512-1)*511));

	return 0; 
}
