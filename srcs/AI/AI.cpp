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


void	AI::setAI(
			Grid *grid, AI_difficulty difficulty,
			PlayerInfo *player, PlayerInfo *opponent)
{
	this->difficulty = difficulty;
	this->timer = 0;

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
		*moveDone = true;
	}
	this->mutex.unlock();

	return (move);
}


void	AI::reset(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	this->timer = 0;
	this->destroyThread();
	this->startThread(grid, player, opponent);
}

////////////////////////////////////////////////////////////////////////////////
// Privbate methods
////////////////////////////////////////////////////////////////////////////////

void	AI::startThread(
			Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	printf("START ORDER\n");
	// Setup parallel variable
	this->mutex.lock();
	this->parallelRun.running = true;
	this->parallelRun.timeLastMove = 0;
	this->mutex.unlock();

	// Fill struct params
	this->threadParams.aiDifficulty = this->difficulty;
	this->threadParams.grid = grid;
	this->threadParams.mutex = &this->mutex;
	this->threadParams.parallelRun = &this->parallelRun;
	this->threadParams.player = player;
	this->threadParams.opponent = opponent;

	// Start thread
	this->thread = std::thread(aiThreadCore, &this->threadParams);
}


void	AI::destroyThread(void)
{
	printf("DISTROY ORDER CALL\n");
	if (!this->thread.joinable())
		return ;
	printf("DISTROY ORDER\n");
	this->mutex.lock();
	this->parallelRun.running = false;
	this->mutex.unlock();
	this->thread.join();
}

////////////////////////////////////////////////////////////////////////////////
// Fonctions
////////////////////////////////////////////////////////////////////////////////

// TODO: REMOVE
static void	resetTracker(Tracker &tracker);
static void	printTracker(
			Tracker &tracker, AI_difficulty &aiDifficulty, int timer);

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
	std::unordered_map<int, std::vector<Move>>	memoryMoves;
	std::unordered_map<int, int>				memoryEval;

	printf("ohayo thread-san\n");

	// Get params from struct
	mutex = threadParams->mutex;
	parallelRun = threadParams->parallelRun;
	aiDifficulty = threadParams->aiDifficulty;

	// TODO: REMOVE
	Tracker	tracker;
	resetTracker(tracker);

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
			else if (aiDifficulty == MEDIUM)
				move = getMediumMove(&memoryMoves, &memoryEval, &grid, &player,
										&opponent, &evaluator, &tracker);
			// else if (aiDifficulty == HARD)
				// move = mtd(f);

			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;

			// TODO: REMOVE
			printTracker(tracker, aiDifficulty, diff);
			resetTracker(tracker);

			// printf("MOVE %i %i\n", move.x, move.y);

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

	memoryMoves.clear();
	memoryEval.clear();
	printf("janee thread-kun\n");
}


// TODO: REMOVE
static void	resetTracker(Tracker &tracker)
{
	tracker.nbEvaluations = 0;
	tracker.evaluationTime = 0;
	tracker.nbComputeEval = 0;
	tracker.computeEvalTime = 0;
	tracker.nbMemoryEval = 0;

	tracker.getSortMoveNumber = 0;
	tracker.getMoveTime = 0;
	tracker.sortMoveTime = 0;
	tracker.sortSizeTotal = 0;
	tracker.sortSizeMin = GRID_NB_INTER + 1;
	tracker.sortSizeMax = -1;

	tracker.checkStoneNumber = 0;
	tracker.putStoneTime = 0;

	tracker.checkWinNumber = 0;
	tracker.checkWinTime = 0;
}

// TODO: REMOVE
static void	printTracker(
				Tracker &tracker, AI_difficulty &aiDifficulty, int timer)
{
	setlocale(LC_NUMERIC, "");
	if (aiDifficulty == RANDOM)
	{
		printf("\n\n==================== RANDOM AI ====================\n");
		printf("Compute move in %'i us\n", timer);
	}
	else if (aiDifficulty == BETTER_RANDOM)
	{
		printf("\n\n================= BETTER RANDOM AI ================\n");
		printf("Compute move in %'i us\n", timer);
	}
	else if (aiDifficulty == EASY)
	{
		printf("\n\n===================== EASY AI =====================\n");
		printf("Compute move in %'i us\n", timer);
	}
	else if (aiDifficulty == MEDIUM)
	{
		printf("\n\n==================== MEDIUM AI ====================\n");
		printf("Compute move in %'i us\n", timer);
		printf("  evaluate position\n");
		printf("   - number of call : %'i\n", tracker.nbEvaluations);
		printf("   - number of compute call : %'i\n", tracker.nbComputeEval);
		printf("   - number of memory call : %'i\n", tracker.nbMemoryEval);
		printf("   - TIME ALL %'i us\n", tracker.evaluationTime);
		printf("   - TIME COMPUTE %'i us\n", tracker.computeEvalTime);
		printf("   - about %'f us per call\n", (float)tracker.evaluationTime / tracker.nbEvaluations);
		printf("   - about %'f us per compute call\n", (float)tracker.computeEvalTime / tracker.nbComputeEval);
		printf("  get sort moves\n");
		printf("   - number of call : %'i\n", tracker.getSortMoveNumber);
		printf("   - get moves TIME %'i us\n", tracker.getMoveTime);
		printf("   - about %'f us per call\n", (float)tracker.getMoveTime / tracker.getSortMoveNumber);
		printf("   - sort moves TIME %'i us\n", tracker.sortMoveTime);
		printf("   - about %'f us per call\n", (float)tracker.sortMoveTime / tracker.getSortMoveNumber);
		printf("   - sort moves min size %'i\n", tracker.sortSizeMin);
		printf("   - sort moves max size %'i\n", tracker.sortSizeMax);
		printf("   - sort moves avg size %'f\n", (float)tracker.sortSizeTotal / tracker.getSortMoveNumber);
		printf("  check stone\n");
		printf("   - number of call : %'i\n", tracker.checkStoneNumber);
		printf("   - put stone TIME %'i us\n", tracker.putStoneTime);
		printf("   - about %'f us per call\n", (float)tracker.putStoneTime / tracker.checkStoneNumber);
		printf("  check win\n");
		printf("   - number of call : %'i\n", tracker.checkWinNumber);
		printf("   - check win TIME %'i us\n", tracker.checkWinTime);
		printf("   - about %'f us per call\n", (float)tracker.checkWinTime / tracker.checkWinNumber);
	}
	else if (aiDifficulty == HARD)
	{
		printf("\n\n===================== HARD AI =====================\n");
		printf("Compute move in %'i us\n", timer);
		printf("  evaluate position\n");
		printf("   - number of call : %'i\n", tracker.nbEvaluations);
		printf("   - TIME %'i us\n", tracker.evaluationTime);
		printf("   - about %'f us per call\n", (float)tracker.evaluationTime / tracker.nbEvaluations);
		printf("  get sort moves\n");
		printf("   - number of call : %'i\n", tracker.getSortMoveNumber);
		printf("   - get moves TIME %'i us\n", tracker.getMoveTime);
		printf("   - about %'f us per call\n", (float)tracker.getMoveTime / tracker.getSortMoveNumber);
		printf("   - sort moves TIME %'i us\n", tracker.sortMoveTime);
		printf("   - about %'f us per call\n", (float)tracker.sortMoveTime / tracker.getSortMoveNumber);
		printf("   - sort moves min size %'i\n", tracker.sortSizeMin);
		printf("   - sort moves max size %'i\n", tracker.sortSizeMax);
		printf("   - sort moves avg size %'f\n", (float)tracker.sortSizeTotal / tracker.getSortMoveNumber);
		printf("  check stone\n");
		printf("   - number of call : %'i\n", tracker.checkStoneNumber);
		printf("   - put stone TIME %'i us\n", tracker.putStoneTime);
		printf("   - about %'f us per call\n", (float)tracker.putStoneTime / tracker.checkStoneNumber);
		printf("  check win\n");
		printf("   - number of call : %'i\n", tracker.checkWinNumber);
		printf("   - check win TIME %'i us\n", tracker.checkWinTime);
		printf("   - about %'f us per call\n", (float)tracker.checkWinTime / tracker.checkWinNumber);
	}
}
