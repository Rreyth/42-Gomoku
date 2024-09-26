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
	this->memory.clear();
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
	tracker.sortSizeTotal = 0;
	tracker.sortSizeMin = GRID_NB_INTER + 1;
	tracker.sortSizeMax = -1;

	tracker.checkStoneNumber = 0;
	tracker.putStoneTime = 0;

	tracker.checkWinNumber = 0;
	tracker.checkWinTime = 0;

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
		// printf("  memory\n");
		// printf("   - nb evaluation : %'lu\n", this->memory.size());
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
	std::vector<sf::Vector2i>	moves;

	// Get all legal moves
	moves = grid->getLegalMoves(player, opponent);
	if (moves.size() == 0)
		return (sf::Vector2i(-1, -1));

	// Choose move to play
	int index = rand_int(0, moves.size() - 1);
	return (moves[index]);
}


sf::Vector2i	AI::getBetterRandom(Grid *grid, Player *player, Player *opponent)
{
	std::vector<sf::Vector2i>	moves;

	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->getMoves() + opponent->getMoves() == 0)
			return(sf::Vector2i(rand_int(0, GRID_W_INTER - 1),
								rand_int(0, GRID_W_INTER - 1)));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Find move to play
	int index = rand_int(0, moves.size() - 1);
	return (moves[index]);
}


sf::Vector2i	AI::getEasyMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator)
{
	int							bestEval, tmpEval, plCapture, opCapture;
	sf::Vector2i				move;
	std::vector<sf::Vector2i>	interestingMoves;
	BitBoard					*plBitBoard, *opBitBoard;

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
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	plBitBoard = grid->getBitBoard(player->getInterType());
	opBitBoard = grid->getBitBoard(opponent->getInterType());

	// Find move to play
	move = sf::Vector2i(-1, -1);
	bestEval = -1;
	for (int i = 0; i < interestingMoves.size(); i++)
	{
		tmpEval = evaluator->evaluationPosition(
								plBitBoard, opBitBoard, plCapture, opCapture,
								interestingMoves[i].x, interestingMoves[i].y);
		if (tmpEval > bestEval)
		{
			bestEval = tmpEval;
			move = interestingMoves[i];
		}
	}

	return (move);
}


int	AI::mediumMiniMax(
				Grid *grid, Player *player, Player *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker)
{
	int					bestEval, tmpEval,
						plCapture, opCapture,
						plMoves, opMoves, nbMoves;
	bool				killerMove;
	inter_type			plType, opType;
	std::vector<Move>	moves;
	BitBoard			plBitBoard, opBitBoard;
	BboxManager			bboxManagerSave;
	BoardState			boardState;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// Compute variables for evaluation and put stone
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();

	// Stop recursion and return the grid evaluation
	if (depth <= 0)
	{
		tracker->nbEvaluations++;
		start = std::clock();

		tmpEval = evaluator->evaluateGrid(
								grid->getBitBoard(plType),
								grid->getBitBoard(opType),
								plCapture, opCapture);

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->evaluationTime += diff;

		return (tmpEval);
	}

	// Get interesting moves
	if (maximizingEval)
		moves = grid->getInterestingMovesSorted(
						evaluator, player, opponent, true, tracker);
	else
		moves = grid->getInterestingMovesSorted(
						evaluator, opponent, player, false, tracker);

	if (moves.size() == 0)
		return (0);

	// Compute variables put stone and reset players
	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;
	plBitBoard = *grid->getBitBoard(plType);
	opBitBoard = *grid->getBitBoard(opType);
	bboxManagerSave = *grid->getBboxManager();

	// Find move
	if (maximizingEval)
		bestEval = -1000000001;
	else
		bestEval = 1000000001;

	for (int i = 0; i < moves.size() && i < AI_MEDIUM_LIMIT; i++)
	{
		tracker->nbEvaluations++;
		start = std::clock();

		// Simulate move
		if (maximizingEval)
		{
			if (!grid->putStoneAI(&moves[i].pos, nbMoves, player, opponent))
				continue;
			if (depth > 1)
				grid->updateBboxManager(&moves[i].pos);
			player->addMove();
		}
		else
		{
			if (!grid->putStoneAI(&moves[i].pos, nbMoves, opponent, player))
				continue;
			if (depth > 1)
				grid->updateBboxManager(&moves[i].pos);
			opponent->addMove();
		}

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->putStoneTime += diff;
		tracker->checkStoneNumber++;

		// If move have an high score, check win case
		if (moves[i].eval >= 100000)
		{
			start = std::clock();

			if (maximizingEval)
				killerMove = grid->checkWinCondition(player, opponent);
			else
				killerMove = grid->checkWinCondition(opponent, player);

			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
			tracker->checkWinTime += diff;
			tracker->checkWinNumber++;
		}
		else
			killerMove = false;

		if (killerMove)
		{
			if (player->getWinState() != WIN_STATE_NONE)
			{
				player->setWinState(WIN_STATE_NONE);
				tmpEval = CASE_WIN_POINT; // Nothing is better than a victory
			}
			else
			{
				opponent->setWinState(WIN_STATE_NONE);
				tmpEval = CASE_LOOSE_POINT; // Nothing is worst than a defeat
			}
		}
		else
		{
			// Get evaluation for this move
			tmpEval = this->mediumMiniMax(grid,
								player, opponent, evaluator,
								!maximizingEval, alpha, beta,
								depth - 1, tracker);
		}

		// Reset grid
		if (player->getCaptured() != plCapture
			|| opponent->getCaptured() != opCapture)
		{
			grid->setBitBoard(&plBitBoard, plType);
			grid->setBitBoard(&opBitBoard, opType);
		}
		else
		{
			if (maximizingEval)
				grid->removeStone(&moves[i].pos, plType);
			else
				grid->removeStone(&moves[i].pos, opType);
		}
		if (depth > 1)
			grid->setBboxManager(&bboxManagerSave);

		// Reset player
		player->setMoves(plMoves);
		player->setCaptured(plCapture);
		opponent->setMoves(opMoves);
		opponent->setCaptured(opCapture);

		// For maximizing
		if (maximizingEval)
		{
			// Update score
			if (tmpEval > bestEval)
				bestEval = tmpEval;

			// Update alpha
			if (tmpEval > alpha)
				alpha = tmpEval;
		}
		// For minimizing
		else
		{
			// Update score
			if (tmpEval < bestEval)
				bestEval = tmpEval;

			// Update beta
			if (tmpEval < beta)
				beta = tmpEval;
		}

		// If we already have already better move, stop searching by pruning
		if (beta <= alpha)
			break;
	}
	
	return (bestEval);
}


sf::Vector2i	AI::getMediumMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator, Tracker *tracker)
{
	int					bestEval, tmpEval,
						plCapture, opCapture,
						plMoves, opMoves, nbMoves;
	inter_type			plType, opType;
	std::vector<Move>	moves;
	sf::Vector2i		bestMove;
	BitBoard			plBitBoard, opBitBoard;
	BboxManager			bboxManagerSave;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// Compute variables
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;
	plBitBoard = *grid->getBitBoard(plType);
	opBitBoard = *grid->getBitBoard(opType);
	bboxManagerSave = *grid->getBboxManager();

	// Get interesting moves
	moves = grid->getInterestingMovesSorted(
					evaluator, player, opponent, true, tracker);
	if (moves.size() == 0)
	{
		if (nbMoves == 0)
			return (sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		return (sf::Vector2i(-1, -1));
	}

	// Find move
	bestEval = -1000000001;
	bestMove = sf::Vector2i(-1, -1);
	for (int i = 0; i < moves.size() && i < AI_MEDIUM_LIMIT; i++)
	{
		// Simulate move
		if (!grid->putStone(&moves[i].pos, nbMoves, player, opponent))
			continue;
		player->addMove();

		// Check win condition
		if (grid->checkWinCondition(player, opponent))
		{
			if (player->getWinState() != WIN_STATE_NONE)
			{
				// Reset grid
				if (player->getCaptured() != plCapture
					|| opponent->getCaptured() != opCapture)
				{
					grid->setBitBoard(&plBitBoard, plType);
					grid->setBitBoard(&opBitBoard, opType);
				}
				else
					grid->removeStone(&moves[i].pos, plType);

				// Reset player
				player->setMoves(plMoves);
				player->setCaptured(plCapture);
				opponent->setMoves(opMoves);
				opponent->setCaptured(opCapture);
				player->setWinState(WIN_STATE_NONE);

				return (moves[i].pos);
			}

			opponent->setWinState(WIN_STATE_NONE);
			tmpEval = CASE_LOOSE_POINT;
		}
		else
		{
			// Get evaluation for this move
			tmpEval = this->mediumMiniMax(grid,
								player, opponent, evaluator,
								false, -1000000001, 1000000001,
								AI_MEDIUM_DEPTH - 1, tracker);
		}

		// Update score
		if (tmpEval > bestEval)
		{
			bestEval = tmpEval;
			bestMove = moves[i].pos;
		}

		// Reset grid
		if (player->getCaptured() != plCapture
			|| opponent->getCaptured() != opCapture)
		{
			grid->setBitBoard(&plBitBoard, plType);
			grid->setBitBoard(&opBitBoard, opType);
		}
		else
			grid->removeStone(&moves[i].pos, plType);
		grid->setBboxManager(&bboxManagerSave);

		// Reset player
		player->setMoves(plMoves);
		player->setCaptured(plCapture);
		opponent->setMoves(opMoves);
		opponent->setCaptured(opCapture);

		if (bestEval >= CASE_WIN_POINT)
			return(bestMove);
	}

	return (bestMove);
}
