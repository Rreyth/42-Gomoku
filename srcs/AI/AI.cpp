#include <AI/AI.hpp>
#include <utils/Functions.hpp>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

AI::AI(void)
{
	this->parallelRun.running = true;
	this->parallelRun.needCompute = false;
	this->parallelRun.computeDone = false;
	this->parallelRun.move = sf::Vector2i(-1, -1);
	this->timer = 0;
	this->totalTimer = 0;
}


AI::~AI()
{
	this->destroyThread();
}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

void	AI::setDifficulty(AI_difficulty difficulty)
{
	this->difficulty = difficulty;
}


AI_difficulty	AI::getDifficulty(void)
{
	return (this->difficulty);
}


int	AI::getTimer(void)
{
	return (this->timer);
}


int	AI::getTotalTimer(void)
{
	return (this->totalTimer);
}


void	AI::setAI(
			Grid *grid, AI_difficulty difficulty,
			PlayerInfo *player, PlayerInfo *opponent)
{
	this->difficulty = difficulty;
	this->timer = 0;
	this->totalTimer = 0;

	this->destroyThread();
	this->startThread(grid, player, opponent);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	AI::startTurn(void)
{
	this->mutex.lock();
	this->parallelRun.needCompute = true;
	this->mutex.unlock();
}


sf::Vector2i	AI::getNextMove(bool *moveDone)
{
	sf::Vector2i	move;

	move = sf::Vector2i(-1, -1);
	this->mutex.lock();
	if (this->parallelRun.computeDone)
	{
		this->parallelRun.needCompute = false;
		this->parallelRun.computeDone = false;
		move = this->parallelRun.move;
		this->timer = this->parallelRun.timeLastMove;
		this->totalTimer += this->timer;
		*moveDone = true;
	}
	this->mutex.unlock();

	return (move);
}


void	AI::reset(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	this->timer = 0;
	this->totalTimer = 0;
	this->destroyThread();
	this->startThread(grid, player, opponent);
}

////////////////////////////////////////////////////////////////////////////////
// Privbate methods
////////////////////////////////////////////////////////////////////////////////

void	AI::startThread(
			Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	// Setup parallel variable
	this->mutex.lock();
	this->parallelRun.running = true;
	this->parallelRun.timeLastMove = 0;
	this->mutex.unlock();

	// Fill struct params
	this->threadParams.aiDifficulty = this->difficulty;
	this->threadParams.grid = grid;
	this->threadParams.mutex = &this->mutex;
	this->parallelRun.needCompute = false;
	this->parallelRun.computeDone = false;
	this->threadParams.parallelRun = &this->parallelRun;
	this->threadParams.player = player;
	this->threadParams.opponent = opponent;

	// Start thread
	this->thread = std::thread(aiThreadCore, &this->threadParams);
}


void	AI::destroyThread(void)
{
	if (!this->thread.joinable())
		return ;
	this->mutex.lock();
	this->parallelRun.running = false;
	this->mutex.unlock();
	this->thread.join();
}

////////////////////////////////////////////////////////////////////////////////
// Fonctions
////////////////////////////////////////////////////////////////////////////////

void	aiThreadCore(ThreadParams *threadParams)
{
	int				diff;
	bool			running, needCompute;
	Grid			grid;
	PlayerInfo		player, opponent;
	sf::Vector2i	move;
	std::clock_t	start;
	std::mutex		*mutex;
	Evaluation		evaluator;
	ParallelRun		*parallelRun;
	AI_difficulty	aiDifficulty;
	std::unordered_map<std::size_t, int>				memoryEval;
	std::unordered_map<std::size_t, std::vector<Move>>	memoryMoves;

	// Get params from struct
	mutex = threadParams->mutex;
	parallelRun = threadParams->parallelRun;
	aiDifficulty = threadParams->aiDifficulty;

	initRandom();

	// Main thread loop
	running = true;
	needCompute = false;
	while (running)
	{
		if (needCompute)
		{
			move = sf::Vector2i(-1, -1);
			start = std::clock();

			grid = *threadParams->grid;
			player = *threadParams->player;
			opponent = *threadParams->opponent;

			if (aiDifficulty == RANDOM)
				move = getRandomMove(&grid, &player, &opponent);
			else if (aiDifficulty == BETTER_RANDOM)
				move = getBetterRandom(&grid, &player, &opponent);
			else if (aiDifficulty == EASY)
				move = getEasyMove(&grid, &player, &opponent, &evaluator);
			else if (aiDifficulty == BETTER_EASY)
				move = getBetterEasyMove(&grid, &player, &opponent, &evaluator);
			else if (aiDifficulty == MEDIUM)
				move = getMediumMove(&memoryMoves, &memoryEval, &grid, &player,
										&opponent, &evaluator);
			else if (aiDifficulty == HARD)
				move = getHardMove(&memoryMoves, &memoryEval, &grid, &player,
										&opponent, &evaluator);

			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;

			// Give result to main thread
			mutex->lock();
			parallelRun->needCompute = false;
			parallelRun->computeDone = true;
			parallelRun->move = move;
			parallelRun->timeLastMove = diff;
			mutex->unlock();
		}

		// Wait 100 ms before check the mutex
		usleep(100000);

		// Check if the thread need to stop or compute a new move
		mutex->lock();
		running = parallelRun->running;
		needCompute = parallelRun->needCompute;
		mutex->unlock();
	}

	memoryEval.clear();
	memoryMoves.clear();
}
