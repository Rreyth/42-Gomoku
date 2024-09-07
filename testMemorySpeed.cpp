#include <iostream>
#include <vector>
#include <unistd.h>
#include <random>
#include <ctime>
#include <map>
#include <unordered_map>

#define NB_TEST 1000000

typedef struct t_testData
{
	std::string	id;
	int			value;
}	TestData;


int	rand_int(int start, int end)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(start, end);

	return dist(rng);
}


int	computeData(TestData *data)
{
	usleep(10);
	return (data->value);
}


int	main(void)
{
	std::vector<TestData>	datas;
	int						nbEvaluation, timeTook, tmpValue;
	std::clock_t			start;

	// Generate Data
	printf("Generate %i test data\n", NB_TEST);
	start = std::clock();
	for (int i = 0; i < NB_TEST; i++)
	{
		TestData	testData;

		// Get random int value
		testData.value = rand_int(-10000, 10000);

		// Transform this value to bites string
		testData.id = "";
		for (int tkt = 0; tkt < 1; tkt++)
		{
			for (int j = 0; j < 32; j++)
			{
				int	bitCheck = 1 << (31 - j);

				if (testData.value & bitCheck)
					testData.id += '1';
				else
					testData.id += '0';
			}
			testData.id += '-';
		}

		datas.push_back(testData);
	}
	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf("Done in %i us\n\n", timeTook);

	// Classic test

	printf("Classic evaluation :\n");
	nbEvaluation = 0;
	start = std::clock();

	for (int i = 0; i < NB_TEST; i++)
	{
		TestData	data = datas[i];

		nbEvaluation++;
		tmpValue = computeData(&data);

		if (tmpValue != data.value)
			printf("ERROR\n");
	}

	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf(" - took %i us for %i evaluations\n", timeTook, nbEvaluation);

	// Map memory test
	std::map<std::string, int>	mapMemory;

	printf("Map memory evaluation :\n");
	nbEvaluation = 0;
	start = std::clock();

	for (int i = 0; i < NB_TEST; i++)
	{
		TestData	data = datas[i];

		try
		{
			tmpValue = mapMemory.at(data.id);
		}
		catch(const std::exception& e)
		{
			nbEvaluation++;
			tmpValue = computeData(&data);
			mapMemory.insert(std::pair<std::string, int>(data.id, tmpValue));
		}

		if (tmpValue != data.value)
			printf("ERROR\n");
	}

	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf(" - took %i us for %i evaluations\n", timeTook, nbEvaluation);

	// HashMap memory test
	std::unordered_map<std::string, int>	UnordermapMemory;

	printf("UnorderMap memory evaluation :\n");
	nbEvaluation = 0;
	start = std::clock();

	for (int i = 0; i < NB_TEST; i++)
	{
		TestData	data = datas[i];

		try
		{
			tmpValue = UnordermapMemory.at(data.id);
		}
		catch(const std::exception& e)
		{
			nbEvaluation++;
			tmpValue = computeData(&data);
			UnordermapMemory.insert(std::pair<std::string, int>(data.id, tmpValue));
		}

		if (tmpValue != data.value)
			printf("ERROR\n");
	}

	timeTook = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	printf(" - took %i us for %i evaluations\n", timeTook, nbEvaluation);

	return (0);
}
