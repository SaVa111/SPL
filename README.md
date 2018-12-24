Special Programming Languages

# Problem #1
Quick sort algorithm:

```c++
template<typename FUNCTOR, typename IT>
void QuickSort(IT first, IT last, FUNCTOR cmp)
{
	if (first == last)
		return;

	IT left = first;
	IT right = last;
	IT pivot = left++;
	while (left != right)
	{
		if (cmp(*left, *pivot))
		{
			++left;
		}
		else
		{
			while ((left != --right) && cmp(*pivot, *right));
			std::iter_swap(left, right);
		}
	}
	--left;
	std::iter_swap(first, left);
	QuickSort(first, left, cmp);
	QuickSort(right, last, cmp);
}
```
# Problem #2

CreateSpiralMatrix fill matrix with nimbers by spiral
```c++
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
```
If run with argument "test", it runs test cases

![](/Screenshot_3.png)

# Problem #3

This app indexing text files in directory and save it.
You can search which files include the words you search.
It shows you sorted result.

In program commands:
For searching just write text.
!index - recalculate cache.
!load - to reload cache.
!cd - change directory.
!exit - close application.

### Indexing:

![](/Screenshot_1.png)

### Finding words:
![](/Screenshot_2.png)
