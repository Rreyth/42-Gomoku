#include <iostream>
#include <vector>
#include <unistd.h>
#include <random>
#include <ctime>
#include <map>
#include <unordered_map>
#include <algorithm>

// #define NB_TEST 361
#define NB_TEST_START	10
#define NB_TEST_END		100
#define NB_TEST_INCR	10
#define NB_TEST_LOOP	1

int	rand_int(int start, int end)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(start, end);

	return dist(rng);
}

void	printVector(std::vector<std::string> &vec)
{
	std::cout << "[";
	for (int i = 0; i < vec.size(); i++)
	{
		if (i > 0)
			std::cout << ", ";
		std::cout << vec[i];
	}
	std::cout << "]" << std::endl;
}


int	getData(std::map<std::string, int> *memory, std::string str)
{
	return (memory->at(str));
}


void	bubbleSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int					i, size, leftVal, rightVal, eval;
	std::string			tmp;
	std::vector<int>	evaluations;

	i = 1;
	size = vec.size();
	if (size < 2)
		return ;

	for (int k = 0; k < size; k++)
		evaluations.push_back(getData(memory, vec[k]));

	while (i < size)
	{
		leftVal = evaluations[i - 1];
		rightVal = evaluations[i];

		if (leftVal < rightVal)
		{
			tmp = vec[i];
			vec[i] = vec[i - 1];
			vec[i - 1] = tmp;

			eval = evaluations[i];
			evaluations[i] = evaluations[i - 1];
			evaluations[i - 1] = eval;

			i = 1;
		}
		else
			i++;
	}
}


void	optiBubbleSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int					i, size, leftVal, rightVal, eval;
	std::string			tmp;
	std::vector<int>	evaluations;

	i = 1;
	size = vec.size();
	if (size < 2)
		return ;

	for (int k = 0; k < size; k++)
		evaluations.push_back(getData(memory, vec[k]));

	leftVal = evaluations[0];
	rightVal = evaluations[1];

	while (1)
	{
		if (leftVal < rightVal)
		{
			tmp = vec[i];
			vec[i] = vec[i - 1];
			vec[i - 1] = tmp;

			eval = evaluations[i];
			evaluations[i] = evaluations[i - 1];
			evaluations[i - 1] = eval;

			i = 1;

			leftVal = evaluations[0];
			rightVal = evaluations[1];
		}
		else
		{
			i++;
			if (i >= size)
				break;
			leftVal = rightVal;
			rightVal = evaluations[i];
		}
	}
}


void	insertionSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int					size, value, j;
	std::string			tmp;
	std::vector<int>	evaluations;

	size = vec.size();
	if (size < 2)
		return ;

	for (int k = 0; k < size; k++)
		evaluations.push_back(getData(memory, vec[k]));

	for (int i = 1; i < size; ++i) {
		tmp = vec[i];
		value = evaluations[i];
		j = i - 1;

		while (j >= 0 && evaluations[j] < value) {
			vec[j + 1] = vec[j];
			evaluations[j + 1] = evaluations[j];
			j = j - 1;
		}
		vec[j + 1] = tmp;
		evaluations[j + 1] = value;
	}
}


void	shellSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int					size, value, j;
	std::string			tmp;
	std::vector<int>	evaluations;

	size = vec.size();
	if (size < 2)
		return ;

	for (int k = 0; k < size; k++)
		evaluations.push_back(getData(memory, vec[k]));

	for (int gap = size / 2; gap > 0; gap /= 2)
	{
		for (int i = gap; i < size; i++)
		{
			tmp = vec[i];
			value = evaluations[i];

			for (j = i; j >= gap && evaluations[j - gap] > value; j -= gap)
			{
				vec[j] = vec[j - gap];
				evaluations[j] = evaluations[j - gap];
			}

			vec[j] = tmp;
			evaluations[j] = value;
		}
	}
}


void	gnomeSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int					size, j, eval;
	std::string			tmp;
	std::vector<int>	evaluations;

	size = vec.size();
	if (size < 2)
		return ;

	for (int k = 0; k < size; k++)
		evaluations.push_back(getData(memory, vec[k]));

	j = 0;
	while (j < size)
	{
		if (j == 0)
			j++;

		if (evaluations[j] >= evaluations[j - 1])
			j++;
		else
		{
			tmp = vec[j];
			vec[j] = vec[j - 1];
			vec[j - 1] = tmp;

			eval = evaluations[j];
			evaluations[j] = evaluations[j - 1];
			evaluations[j - 1] = eval;

			j--;
		}
	}
}


void	quickSort(std::vector<std::string> &vec,
					std::vector<int> &evaluations,
					int low, int high)
{
	int					i, pivot, eval;
	std::string			tmp;

	if (low >= high)
		return ;

	// partition
	pivot = evaluations[high];
	i = low - 1;

	for (int j = low; j < high; j++)
	{
		if (evaluations[j] < pivot)
		{
			i++;

			tmp = vec[j];
			vec[j] = vec[i];
			vec[i] = tmp;

			eval = evaluations[j];
			evaluations[j] = evaluations[i];
			evaluations[i] = eval;
		}
	}

	i++;

	tmp = vec[i];
	vec[i] = vec[high];
	vec[high] = tmp;

	eval = evaluations[i];
	evaluations[i] = evaluations[high];
	evaluations[high] = eval;

	// Recursion
	quickSort(vec, evaluations, low, i - 1);
	quickSort(vec, evaluations, i + 1, high);
}


void	heapify(std::vector<std::string> &vec,
					std::vector<int> &evaluations,
					int size, int i)
{
	int			largest, l, r, value;
	std::string	tmp;

	largest = i;

	l = 2 * i + 1;
	r = l + 1;

	value = evaluations[largest];

	if (l < size && evaluations[l] > value)
		largest = l;

	if (r < size && evaluations[r] > value)
		largest = r;

	if (largest != i)
	{
		tmp = vec[i];
		vec[i] = vec[largest];
		vec[largest] = tmp;

		value = evaluations[i];
		evaluations[i] = evaluations[largest];
		evaluations[largest] = value;

		heapify(vec, evaluations, size, largest);
	}
}


void	heapSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int					size, j;
	std::string			tmp;
	std::vector<int>	evaluations;

	size = vec.size();
	if (size < 2)
		return ;

	for (int k = 0; k < size; k++)
		evaluations.push_back(getData(memory, vec[k]));

	for (int i = size / 2 - 1; i >= 0; i--)
		heapify(vec, evaluations, size, i);

	for (int i = size - 1; i > 0; i--)
	{
		tmp = vec[0];
		vec[0] = vec[i];
		vec[i] = tmp;

		heapify(vec, evaluations, i, 0);
	}
}

void	stdSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int			size;
	std::string	tmp;

	size = vec.size();
	if (size < 2)
		return ;

	std::sort(vec.begin(), vec.end(), [&](std::string &a, std::string &b)
	{
		return (getData(memory, a) < getData(memory, b));
	});
}


int	main(void)
{
	std::vector<std::string>	datas, tmpDatas;
	int							timeTook, tmpValue, i, total;
	std::clock_t				start;
	std::map<std::string, int>	memory;
	std::vector<int>			timesBubbles,
								timesBubblesOpti,
								timesInsertion,
								timesShell,
								timesGnome,
								timesQuick,
								timesHeap,
								timesStd;

	printf("Sort time algorithm is us\n");
	printf(" list size  ");
	printf("     Bubble ");
	printf(" BubbleOpti ");
	printf("  Insertion ");
	printf("      Shell ");
	printf("      Gnome ");
	printf("      Quick ");
	printf("       Heap ");
	printf("        std ");
	printf("\n");

	i = 0;
	for (int nbTest = NB_TEST_START; nbTest <= NB_TEST_END; nbTest +=  NB_TEST_INCR)
	{

		// Generate Data
		// printf("Generate %i test data\n", nbTest);
		start = std::clock();
		for (int j = 0; j < nbTest; j++)
		{
			int			value;
			std::string	str;

			// Get random int value
			value = rand_int(-100, 100);

			// Transform this value to bites string
			str = std::to_string(value);

			memory.insert(std::pair<std::string, int>(str, value));
			datas.push_back(str);
		}
		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		// printf("Done in %i us\n\n", timeTook);
		// printVector(datas);

		// ====================================================================
		// Bubble sort
		// printf("Bubble sort :\n");
		tmpDatas = datas;
		start = std::clock();

		bubbleSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesBubbles.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// Opti bubble sort
		// printf("Opti bubble sort :\n");
		tmpDatas = datas;
		start = std::clock();

		optiBubbleSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesBubblesOpti.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// Insertion sort
		// printf("Insertion sort :\n");
		tmpDatas = datas;
		start = std::clock();

		insertionSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesInsertion.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// Shell sort
		// printf("Shell sort :\n");
		tmpDatas = datas;
		start = std::clock();

		shellSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesShell.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// Gnome sort
		// printf("\nGnome sort :\n");
		tmpDatas = datas;
		start = std::clock();

		gnomeSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesGnome.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// Quick sort
		// printf("\nQuick sort :\n");
		tmpDatas = datas;
		start = std::clock();

		std::vector<int>	evaluations;
		for (int k = 0; k < tmpDatas.size(); k++)
			evaluations.push_back(getData(&memory, tmpDatas[k]));

		quickSort(tmpDatas, evaluations, 0, datas.size() - 1);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesQuick.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// Heap sort
		// printf("\nHeap sort :\n");
		tmpDatas = datas;
		start = std::clock();

		heapSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesHeap.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		// ====================================================================
		// std sort
		// printf("\nstd sort :\n");
		tmpDatas = datas;
		start = std::clock();

		stdSort(tmpDatas, &memory);

		timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		timesStd.push_back(timeTook);
		// printf(" - took %i us\n", timeTook);
		// printVector(tmpDatas);

		printf(" %7i :  ", nbTest);
		printf(" %10i ", timesBubbles[i]);
		printf(" %10i ", timesBubblesOpti[i]);
		printf(" %10i ", timesInsertion[i]);
		printf(" %10i ", timesShell[i]);
		printf(" %10i ", timesGnome[i]);
		printf(" %10i ", timesQuick[i]);
		printf(" %10i ", timesHeap[i]);
		printf(" %10i ", timesStd[i]);
		printf("\n");
		i++;
	}

	printf(" Average :  ");

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesBubbles[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesBubblesOpti[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesInsertion[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesShell[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesGnome[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesQuick[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesHeap[j];
	printf(" %10.2f ", (float)total / i);

	total = 0;
	for (int j = 0; j < i; j++)
		total += timesStd[j];
	printf(" %10.2f ", (float)total / i);

	printf("\n");

	return (0);
}
