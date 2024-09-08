#include <iostream>
#include <vector>
#include <unistd.h>
#include <random>
#include <ctime>
#include <map>
#include <unordered_map>

#define NB_TEST 289
// #define NB_TEST 100

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
	int			i, size, leftVal, rightVal;
	std::string	tmp;

	i = 1;
	size = vec.size();
	if (size < 2)
		return ;

	while (i < size)
	{
		leftVal = getData(memory, vec[i - 1]);
		rightVal = getData(memory, vec[i]);

		if (leftVal < rightVal)
		{
			tmp = vec[i];
			vec[i] = vec[i - 1];
			vec[i - 1] = tmp;
			i = 1;
		}
		else
			i++;
	}
}


void	optiBubbleSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int			i, size, leftVal, rightVal;
	std::string	tmp;

	i = 1;
	size = vec.size();
	if (size < 2)
		return ;

	leftVal = getData(memory, vec[0]);
	rightVal = getData(memory, vec[1]);

	while (1)
	{
		if (leftVal < rightVal)
		{
			tmp = vec[i];
			vec[i] = vec[i - 1];
			vec[i - 1] = tmp;
			i = 1;
			leftVal = getData(memory, vec[0]);
			rightVal = getData(memory, vec[1]);
		}
		else
		{
			i++;
			if (i >= size)
				break;
			leftVal = rightVal;
			rightVal = getData(memory, vec[i]);
		}
	}
}


void insertionSort(std::vector<std::string> &vec,
					std::map<std::string, int> *memory)
{
	int			size, value, j;
	std::string	tmp;

	size = vec.size();
	if (size < 2)
		return ;

	for (int i = 1; i < size; ++i) {
		tmp = vec[i];
		value = getData(memory, vec[i]);
		j = i - 1;

		while (j >= 0 && getData(memory, vec[j]) < value) {
			vec[j + 1] = vec[j];
			j = j - 1;
		}
		vec[j + 1] = tmp;
	}
}


int	main(void)
{
	std::vector<std::string>	datas, tmpDatas;
	int							timeTook, tmpValue;
	std::clock_t				start;
	std::map<std::string, int>	memory;

	// Generate Data
	printf("Generate %i test data\n", NB_TEST);
	start = std::clock();
	for (int i = 0; i < NB_TEST; i++)
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
	printf("Done in %i us\n\n", timeTook);
	// printVector(datas);

	// ====================================================================
	// Bubble sort
	printf("Bubble sort :\n");
	tmpDatas = datas;
	start = std::clock();

	bubbleSort(tmpDatas, &memory);

	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf(" - took %i us\n", timeTook);
	// printVector(tmpDatas);

	// ====================================================================
	// Opti bubble sort
	printf("Opti bubble sort :\n");
	tmpDatas = datas;
	start = std::clock();

	optiBubbleSort(tmpDatas, &memory);

	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf(" - took %i us\n", timeTook);
	// printVector(tmpDatas);

	// ====================================================================
	// Opti bubble sort
	printf("Insertion sort :\n");
	tmpDatas = datas;
	start = std::clock();

	insertionSort(tmpDatas, &memory);

	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf(" - took %i us\n", timeTook);
	// printVector(tmpDatas);

	return (0);
}
