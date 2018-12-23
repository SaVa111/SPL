#include <functional>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>

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

template<typename IT>
inline void QuickSort(IT first, IT last)
{
	QuickSort(first, last,
		std::less_equal< typename std::iterator_traits<IT>::value_type >());
}

int main()
{
	std::vector<int> a = {3, 4, 2, 1, 7, 9, 2, 5, 23, 4, 12, 22};
	QuickSort(a.begin(), a.end());
	for (size_t i = 0; i < a.size(); ++i)
	{
		std::cout << a[i] << std::endl;
	}

	system("pause");
	return 0;
}
