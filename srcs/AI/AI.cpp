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

sf::Vector2i	AI::getNextMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator)
{
	sf::Vector2i	move(-1, -1);
	std::clock_t	start, diff;

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
		move = this->getMediumMove(grid, player, opponent, evaluator);
	//TODO: Implement other difficulties

	// Compute time taken to choose ai
	diff = std::clock() - start;
	this->timer = ((double)diff / CLOCKS_PER_SEC) * 1000000;

	printf(" in %i us\n", (int)this->timer);
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
				int depth, int *nbEval)
{
	int							maxEval, tmpEval, plCapture, opCapture,
								nbMoves, plMoves, opMoves;
	std::string					boardState;
	std::vector<sf::Vector2i>	moves;
	inter_type					plType, opType;
	sf::Vector2i				bboxUL, bboxDR;

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
			(*nbEval)++;
			tmpEval = evaluator->evaluateGrid(
								grid, plType, opType, plCapture, opCapture);
			memory->insert(std::pair<std::string, int>(boardState, tmpEval));
		}
		return (tmpEval);
	}

	// Get interesting moves
	if (maximizingEval)
		moves = grid->getInterestingMoves(player, opponent);
	else
		moves = grid->getInterestingMoves(opponent, player);
	if (moves.size() == 0)
		return (0);

	// Compute variables put stone and reset players
	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;
	grid->saveBbox(&bboxUL, &bboxDR);

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
			grid->removeStone(&moves[i - 1]);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);
		}

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

		// Check victory by capture
		if (player->getCaptured() >= WIN_CAPTURE)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->removeStone(&moves[i]);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);

			return (1000000000); // Nothing is better than a victory
		}
		else if (opponent->getCaptured() >= WIN_CAPTURE)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->removeStone(&moves[i]);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState);

			// Reset players
			player->setMoves(plMoves);
			player->setCaptured(plCapture);
			opponent->setMoves(opMoves);
			opponent->setCaptured(opCapture);

			return (-1000000000); // Nothing is worst than a defeat
		}

		// Get evaluation for this move
		tmpEval = mediumMiniMax(memory, grid,
						player, opponent, evaluator,
						!maximizingEval, alpha, beta,
						depth - 1, nbEval);

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
			grid->removeStone(&moves[i]);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState);

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
	grid->removeStone(&moves[moves.size() - 1]);
	if (player->getCaptured() != plCapture
		|| opponent->getCaptured() != opCapture)
		grid->resetGridByBoardState(boardState);

	// Reset players
	player->setMoves(plMoves);
	player->setCaptured(plCapture);
	opponent->setMoves(opMoves);
	opponent->setCaptured(opCapture);

	return (maxEval);
}


sf::Vector2i	AI::getMediumMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator)
{
	int							maxEval, tmpEval, plCapture, opCapture,
								nbMoves, plMoves, opMoves, nbEval;
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
	nbEval = 0;
	move = sf::Vector2i(-1, -1);
	maxEval = -1000000001;
	for (int i = 0; i < moves.size(); i++)
	{
		if (i > 0)
		{
			// Reset grid
			grid->loadBbox(&bboxUL, &bboxDR);
			grid->removeStone(&moves[i - 1]);
			if (player->getCaptured() != plCapture
				|| opponent->getCaptured() != opCapture)
				grid->resetGridByBoardState(boardState);

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
					DEPTH, &nbEval);

		// Keep the higher scored move
		if (tmpEval > maxEval)
		{
			maxEval = tmpEval;
			move = moves[i];
		}
	}

	// Reset grid
	grid->loadBbox(&bboxUL, &bboxDR);
	grid->removeStone(&moves[moves.size() - 1]);
	if (player->getCaptured() != plCapture
		|| opponent->getCaptured() != opCapture)
		grid->resetGridByBoardState(boardState);

	// Reset players
	player->setMoves(plMoves);
	player->setCaptured(plCapture);
	opponent->setMoves(opMoves);
	opponent->setCaptured(opCapture);

	printf("Number of evaluation %i", nbEval);
	return (move);
}
