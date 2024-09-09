#include <AI/AI.hpp>
#include <utils/Functions.hpp>
#include <game/Player.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////
AI::AI(void)
{

}


AI::~AI()
{
}


////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

AI_difficulty	AI::getDifficulty(void)
{
	return (this->difficulty);
}

void	AI::setDifficulty(AI_difficulty difficulty)
{
	this->difficulty = difficulty;
}

double	AI::getTimer(void)
{
	return (this->timer);
}

void	AI::setTimer(double timer)
{
	this->timer = timer;
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	AI::setAI(AI_difficulty difficulty)
{
	this->difficulty = difficulty;
	this->timer = 0;
	this->memory.clear();
}

sf::Vector2i	AI::getNextMove(
					Grid *grid, Player *player, Player *opponent,
					Evaluation *evaluator)
{
	sf::Vector2i	move(-1, -1);
	std::clock_t	start, diff;

	// TODO : REMOVE TRACKER
	Tracker	tracker;
	tracker.nbEvaluations = 0;
	tracker.evaluationTime = 0;
	tracker.getSortMoveNumber = 0;
	tracker.getMoveTime = 0;
	tracker.sortMoveTime = 0;
	tracker.checkStoneNumber = 0;
	tracker.putStoneTime = 0;
	tracker.removeStoneTime = 0;
	tracker.checkWinTime = 0;
	tracker.reverseStoneCaptureNumber = 0;
	tracker.reverseStoneCaptureTime = 0;

	// Start timer
	start = std::clock();

	// Get move to play
	if (this->difficulty == RANDOM)
		move = this->getRandomMove(grid, player, opponent);
	else if (this->difficulty == BETTER_RANDOM)
		move = this->getBetterRandom(grid, player, opponent);
	else if (this->difficulty == EASY)
		move = this->getEasyMove(grid, player, opponent, evaluator);
	else if (this->difficulty == MEDIUM)
		move = this->getMediumMove(grid, player, opponent, evaluator, &tracker);
	//TODO: Implement other difficulties

	// Compute time taken to choose ai
	diff = std::clock() - start;
	this->timer = ((double)diff / CLOCKS_PER_SEC) * 1000000;

	if (this->difficulty == MEDIUM)
	{
		setlocale(LC_NUMERIC, "");
		printf("\n\nGet move in %'i us\n", (int)this->timer);
		printf("  evaluate position\n");
		printf("   - number of call : %'i\n", tracker.nbEvaluations);
		printf("   - took %'i us\n", tracker.evaluationTime);
		if (tracker.nbEvaluations > 0)
			printf("   - about %'f us per call\n", (float)tracker.evaluationTime / tracker.nbEvaluations);
		else
			printf("   - about 0 us per call\n");
		printf("  get sort moves\n");
		printf("   - number of call : %'i\n", tracker.getSortMoveNumber);
		printf("   - get moves took %'i us\n", tracker.getMoveTime);
		if (tracker.getSortMoveNumber > 0)
			printf("   - about %'f us per call\n", (float)tracker.getMoveTime / tracker.getSortMoveNumber);
		else
			printf("   - about 0 us per call\n");
		printf("   - sort moves took %'i us\n", tracker.sortMoveTime);
		if (tracker.getSortMoveNumber > 0)
			printf("   - about %'f us per call\n", (float)tracker.sortMoveTime / tracker.getSortMoveNumber);
		else
			printf("   - about 0 us per call\n");
		printf("  check stone\n");
		printf("   - number of call : %'i\n", tracker.checkStoneNumber);
		printf("   - put stone took %'i us\n", tracker.putStoneTime);
		if (tracker.checkStoneNumber > 0)
			printf("   - about %'f us per call\n", (float)tracker.putStoneTime / tracker.checkStoneNumber);
		else
			printf("   - about 0 us per call\n");
		printf("   - remove stone took %'i us\n", tracker.removeStoneTime);
		if (tracker.checkStoneNumber > 0)
			printf("   - about %'f us per call\n", (float)tracker.removeStoneTime / tracker.checkStoneNumber);
		else
			printf("   - about 0 us per call\n");
		printf("   - check win took %'i us\n", tracker.checkWinTime);
		if (tracker.checkStoneNumber > 0)
			printf("   - about %'f us per call\n", (float)tracker.checkWinTime / tracker.checkStoneNumber);
		else
			printf("   - about 0 us per call\n");
		printf("  reverse capture\n");
		printf("   - number of call : %'i\n", tracker.reverseStoneCaptureNumber);
		printf("   - took %'i us\n", tracker.reverseStoneCaptureTime);
		if (tracker.reverseStoneCaptureNumber > 0)
			printf("   - about %'f us per call\n", (float)tracker.reverseStoneCaptureTime / tracker.reverseStoneCaptureNumber);
		else
			printf("   - about 0 us per call\n");
	}
	else
	{
		printf("Compute move in %'i us\n", (int)this->timer);
	}
	return (move);
}


void	AI::reset(void)
{
	this->memory.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

sf::Vector2i	AI::getRandomMove(Grid *grid, Player *player, Player *opponent)
{
	std::vector<sf::Vector2i>	legalMoves;

	// Get all legal moves
	legalMoves = grid->getLegalMoves(player, opponent);
	if (legalMoves.size() == 0)
		return (sf::Vector2i(-1, -1));

	// Choose move to play
	int index = rand_int(0, legalMoves.size() - 1);
	return (legalMoves[index]);
}


sf::Vector2i	AI::getBetterRandom(Grid *grid, Player *player, Player *opponent)
{
	std::vector<sf::Vector2i>	interestingMoves;

	// Get interesting move
	interestingMoves = grid->getInterestingMoves(player, opponent);
	if (interestingMoves.size() == 0)
	{
		if (player->getMoves() + opponent->getMoves() == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Find move to play
	int index = rand_int(0, interestingMoves.size() - 1);
	return (interestingMoves[index]);
}


sf::Vector2i	AI::getEasyMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator)
{
	int							maxEval, tmpEval, plCapture, opCapture;
	sf::Vector2i				move;
	std::vector<sf::Vector2i>	interestingMoves;
	inter_type					plType, opType;

	// Get interesting move
	interestingMoves = grid->getInterestingMoves(player, opponent);
	if (interestingMoves.size() == 0)
	{
		if (player->getMoves() + opponent->getMoves() == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Compute variables for evaluation
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();

	// Find move to play
	move = sf::Vector2i(-1, -1);
	maxEval = -1;
	for (int i = 0; i < interestingMoves.size(); i++)
	{
		tmpEval = evaluator->evaluationPosition(
								grid, plType, opType, plCapture, opCapture,
								interestingMoves[i].x, interestingMoves[i].y);
		if (tmpEval > maxEval)
		{
			maxEval = tmpEval;
			move = interestingMoves[i];
		}
	}

	return (move);
}

# define DEPTH 3

static int	mediumMiniMax(std::unordered_map<std::string, int> *memory,
				Grid *grid, Player *player, Player *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker)
{
	int							maxEval, tmpEval, plCapture, opCapture,
								nbMoves, plMoves, opMoves;
	bool						someoneWin;
	std::string					boardState;
	std::vector<sf::Vector2i>	moves;
	inter_type					plType, opType;
	sf::Vector2i				bboxUL, bboxDR;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// Compute variables for evaluation and put stone
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	boardState = grid->getCurrentBoardState();

	if (depth <= 0)
	{
		// Check in memory if we already compute the evaluation of this grid
		try
		{
			// If yes, just get it from memory
			tmpEval = memory->at(boardState);
		}
		catch (std::exception e)
		{
			// If not, compute it and store it in memory
			tracker->nbEvaluations++;
			start = std::clock();
			tmpEval = evaluator->evaluateGrid(
								grid, plType, opType, plCapture, opCapture);
			memory->insert(std::pair<std::string, int>(boardState, tmpEval));
			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
			tracker->evaluationTime += diff;
		}
		return (tmpEval);
	}

	// Get interesting moves
	if (maximizingEval)
		moves = grid->getInterestingMovesSorted(player, opponent, evaluator, true, tracker);
	else
		moves = grid->getInterestingMovesSorted(opponent, player, evaluator, false, tracker);
	if (moves.size() == 0)
		return (0);

	// Compute variables put stone and reset players
	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;
	grid->saveBbox(&bboxUL, &bboxDR);

	std::vector<sf::Vector2i>	leftWinPos, rightWinPos;
	grid->saveWinPos(&leftWinPos, &rightWinPos);

	// Find move
	if (maximizingEval)
		maxEval = -1000000001;
	else
		maxEval = 1000000001;

	for (int i = 0; i < moves.size(); i++)
	{
		if (i > 0)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->loadWinPos(&leftWinPos, &rightWinPos);
			grid->removeStone(&moves[i - 1], tracker);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState, tracker);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);
		}

		start = std::clock();
		// Simulate new move
		if (maximizingEval)
		{
			if (!grid->putStone(&moves[i], nbMoves, player, opponent))
				continue;
			player->addMove();
		}
		else
		{
			if (!grid->putStone(&moves[i], nbMoves, opponent, player))
				continue;
			opponent->addMove();
		}
		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->putStoneTime += diff;
		tracker->checkStoneNumber++;

		start = std::clock();
		// Check victory
		if (maximizingEval)
			someoneWin = grid->checkWinCondition(player, opponent, moves[i]);
		else
			someoneWin = grid->checkWinCondition(opponent, player, moves[i]);
		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->checkWinTime += diff;
		if (someoneWin)
		{
			// Player wins
			if (player->getWinState() != WIN_STATE_NONE)
			{
				// Reset grid
				grid->loadBbox(&bboxUL, &bboxDR);
				grid->loadWinPos(&leftWinPos, &rightWinPos);
				grid->removeStone(&moves[i], tracker);
				if (player->getCaptured() != plCapture
					|| opponent->getCaptured() != opCapture)
					grid->resetGridByBoardState(boardState, tracker);

				// Reset players
				player->setWinState(WIN_STATE_NONE);
				player->setMoves(plMoves);
				player->setCaptured(plCapture);
				opponent->setMoves(opMoves);
				opponent->setCaptured(opCapture);

				return (1000000000); // Nothing is better than a victory
			}
			// Opponent wins
			else if (opponent->getWinState() != WIN_STATE_NONE)
			{
				// Reset grid
				grid->loadBbox(&bboxUL, &bboxDR);
				grid->loadWinPos(&leftWinPos, &rightWinPos);
				grid->removeStone(&moves[i], tracker);
				if (player->getCaptured() != plCapture
					|| opponent->getCaptured() != opCapture)
					grid->resetGridByBoardState(boardState, tracker);

				// Reset players
				opponent->setWinState(WIN_STATE_NONE);
				player->setMoves(plMoves);
				player->setCaptured(plCapture);
				opponent->setMoves(opMoves);
				opponent->setCaptured(opCapture);

				return (-1000000000); // Nothing is worst than a defeat
			}
		}

		// Get evaluation for this move
		tmpEval = mediumMiniMax(memory, grid,
						player, opponent, evaluator,
						!maximizingEval, alpha, beta,
						depth - 1, tracker);

		// For maximizing
		if (maximizingEval)
		{
			// Update score
			if (tmpEval > maxEval)
				maxEval = tmpEval;

			// Update alpha
			if (tmpEval > alpha)
				alpha = tmpEval;
		}
		// For minimizing
		else
		{
			// Update score
			if (tmpEval < maxEval)
				maxEval = tmpEval;

			// Update beta
			if (tmpEval < beta)
				beta = tmpEval;
		}

		// If we already have already better move, stop searching by pruning
		if (beta <= alpha)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->removeStone(&moves[i], tracker);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState, tracker);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);

			return (maxEval);
		}
	}

	// Reset grid
	grid->loadBbox(&bboxUL, &bboxDR);
	grid->loadWinPos(&leftWinPos, &rightWinPos);
	grid->removeStone(&moves[moves.size() - 1], tracker);
	if (player->getCaptured() != plCapture
		|| opponent->getCaptured() != opCapture)
		grid->resetGridByBoardState(boardState, tracker);

	// Reset players
	player->setMoves(plMoves);
	player->setCaptured(plCapture);
	opponent->setMoves(opMoves);
	opponent->setCaptured(opCapture);

	return (maxEval);
}


sf::Vector2i	AI::getMediumMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator, Tracker *tracker)
{
	int							maxEval, tmpEval, plCapture, opCapture,
								nbMoves, plMoves, opMoves;
	std::vector<sf::Vector2i>	moves;
	sf::Vector2i				move, bboxUL, bboxDR;
	inter_type					plType, opType;
	std::string					boardState;

	// Compute variables for evaluation and put stone
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;
	grid->saveBbox(&bboxUL, &bboxDR);
	boardState = grid->getCurrentBoardState();

	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->getMoves() + opponent->getMoves() == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Find move to play
	move = sf::Vector2i(-1, -1);
	maxEval = -1000000001;
	for (int i = 0; i < moves.size(); i++)
	{
		if (i > 0)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->removeStone(&moves[i - 1], NULL);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState, NULL);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);
		}

		// Simulate new move
		if (!grid->putStone(&moves[i],
								nbMoves, player, opponent))
			continue;
		player->addMove();

		// Get evaluation for this move
		tmpEval = mediumMiniMax(&this->memory, grid,
					player, opponent, evaluator, false, -1000000001, 1000000001,
					DEPTH, tracker);

		// If we detect a victory, stop it
		if (tmpEval >= 1000000000)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->removeStone(&moves[i], NULL);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState, NULL);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);

			return (moves[i]);
		}

		// Keep the higher scored move
		if (tmpEval > maxEval)
		{
			maxEval = tmpEval;
			move = moves[i];
		}

	}

	// Reset grid
	grid->loadBbox(&bboxUL, &bboxDR);
	grid->removeStone(&moves[moves.size() - 1], NULL);
	if (player->getCaptured() != plCapture
		|| opponent->getCaptured() != opCapture)
		grid->resetGridByBoardState(boardState, NULL);

	// Reset players
	player->setMoves(plMoves);
	player->setCaptured(plCapture);
	opponent->setMoves(opMoves);
	opponent->setCaptured(opCapture);

	return (move);
}
