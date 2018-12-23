#pragma once
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>

void CreateSpiralMatrix(std::vector<std::vector<int> >& matrix, size_t idx = 0)
{
	int m = matrix.size() - idx * 2;
	int n = matrix[0].size() - idx * 2;

	int i = idx;
	int j = idx;

	if (i > m && j > n)
		return;

	int nextNumber = idx ? matrix[i][j - 1] + 1 : 1;

	for (int lastNumber = n == 1 && m == 1 ? nextNumber + n * 2 + m * 2 - 3 : nextNumber + n * 2 + m * 2 - 4;
		nextNumber < lastNumber;
		++nextNumber)
	{
		matrix[i][j] = nextNumber;

		i == idx && j < idx + n - 1 ? ++j :
		j == idx + n - 1 && i < idx + m - 1 ? ++i :
		i == idx + m - 1 && j > idx ? --j : --i;
	}
	CreateSpiralMatrix(matrix, idx + 1);
}

void RunTest(
	const size_t n, const size_t m,
	const std::vector<std::vector<int> >& correct)
{
	std::cout << "Test n = " << n << " m = " << m << std::endl;
	std::vector<std::vector<int> > result(n, std::vector<int>(m, 0));
	CreateSpiralMatrix(result);

	for (size_t i = 0; i < result.size(); ++i)
	{
		for (size_t j = 0; j < result[0].size(); ++j)
		{
			printf("%d ", result[i][j]);
		}
		printf("\n");
	}

	if (result == correct)
		std::cout << "OK" << std::endl;
	else
		std::cout << "FAIL" << std::endl;
}

void RunTests()
{
	std::vector<std::vector<int> > correct;
	
	correct =
	{
		{1, 2, 3, 4},
		{10, 11, 12, 5},
		{9, 8, 7, 6}
	};
	RunTest(3, 4, correct);

	correct =
	{
		{1, 2},
		{4, 3}
	};
	RunTest(2, 2, correct);

	correct =
	{
		{1, 2, 3, 4, 5, 6},
		{20, 21, 22, 23, 24, 7},
		{19, 32, 33, 34, 25, 8},
		{18, 31, 36, 35, 26, 9},
		{17, 30, 29, 28, 27, 10},
		{16, 15, 14, 13, 12, 11}
	};
	RunTest(6, 6, correct);

	system("pause");
}


int main(size_t argc, char* args[])
{
	if (argc > 1 && std::string(args[1]) == "test")
	{
		RunTests();
		return 0;
	}

	size_t m, n;

	std::cin >> n >> m;
	std::vector<std::vector<int> > matrix(n, std::vector<int>(m, 0));

	CreateSpiralMatrix(matrix);

	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix[0].size(); ++j)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	system("pause");
	return 0;
}
