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

# Problem #3
3.cpp

