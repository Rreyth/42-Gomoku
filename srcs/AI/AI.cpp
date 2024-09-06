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

static int	mediumMiniMax(std::unordered_map<std::string, int> *memory,
				Grid *grid, Player *player, Player *opponent,
				Evaluation *evaluator, bool playerTurn, int depth, int *nbEval)
{
	int							maxEval, tmpEval, plCapture, opCapture,
								nbMoves, plMoves, opMoves;
	std::string					boardId;
	std::vector<sf::Vector2i>	moves;
	inter_type					plType, opType;
	Grid						gridCopy;

	// Compute variables for evaluation and put stone
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();

	if (depth <= 0)
	{
		boardId = grid->getCurrentBoardState();
		// Check in memory if we already compute the evaluation of this grid
		try
		{
			// If yes, just get it from memory
			tmpEval = memory->at(boardId);
		}
		catch (std::exception e)
		{
			// If not, compute it and store it in memory
			(*nbEval)++;
			tmpEval = evaluator->evaluateGrid(
								grid, plType, opType, plCapture, opCapture);
			memory->insert(std::pair<std::string, int>(boardId, tmpEval));
		}
		return (tmpEval);
	}

	// Get interesting moves
	if (playerTurn)
		moves = grid->getInterestingMoves(player, opponent);
	else
		moves = grid->getInterestingMoves(opponent, player);
	if (moves.size() == 0)
	{
		printf("NO MOVE FOUND\n");
		return (0);
	}

	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;

	std::string	xaxis = "ABCDEFGHIKLMNOPQRST";
	// Find move
	maxEval = -1000000000;
	for (int i = 0; i < moves.size(); i++)
	{
		gridCopy = *grid;
		player->setMoves(plMoves);
		player->setCaptured(plCapture);
		opponent->setMoves(opMoves);
		opponent->setCaptured(opCapture);

		if (playerTurn)
		{
			if (!gridCopy.putStone(&moves[i], nbMoves, player, opponent))
			{
				printf("  - CAN'T PUT STONE at %c %i (depth %i)\n",
							xaxis[moves[i].x], moves[i].y + 1, depth);
				continue;
			}
		}
		else
		{
			if (!gridCopy.putStone(&moves[i], nbMoves, opponent, player))
			{
				printf("  - CAN'T PUT STONE at %c %i (depth %i)\n",
							xaxis[moves[i].x], moves[i].y + 1, depth);
				continue;
			}
		}

		// Check victory / defeat
		if (gridCopy.checkWinCondition(player, opponent, moves[i]))
		{
			if ((player->getWinState() != WIN_STATE_NONE))
			{
				if (playerTurn)
				{
					// printf("player win !\n");
					player->setWinState(WIN_STATE_NONE);
					return (1000000000);
				}

				// printf("opponent loose !\n");
				// printf("move  %c %i (depth %i)\n",
				// 				xaxis[moves[i].x], moves[i].y + 1, depth);
				// std::cout << gridCopy.getCurrentBoardState() << std::endl;

				return (-1000000000);
			}

			if (opponent->getWinState() != WIN_STATE_NONE)
			{
				if (playerTurn)
				{
					// printf("player loose !\n");
					return (-1000000000);
				}

				// printf("opponent win !\n");
				opponent->setWinState(WIN_STATE_NONE);
				return (1000000000);
			}

			// printf("EUHHHHH\n");
			return (0);
		}

		tmpEval = -mediumMiniMax(memory, &gridCopy,
						player, opponent, evaluator, !playerTurn, depth - 1, nbEval);
		if (tmpEval > maxEval)
			maxEval = tmpEval;
	}

	return (maxEval);
}


sf::Vector2i	AI::getMediumMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator)
{
	int							maxEval, tmpEval, plCapture, opCapture,
								nbMoves, plMoves, opMoves, nbEval;
	bool						playerTurn;
	std::vector<sf::Vector2i>	moves;
	sf::Vector2i				move;
	inter_type					plType, opType;
	Grid						gridCopy;

	// Compute variables for evaluation and put stone
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	plMoves = player->getMoves();
	opMoves = opponent->getMoves();
	nbMoves = plMoves + opMoves;
	playerTurn = true;

	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->getMoves() + opponent->getMoves() == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// std::string	xaxis = "ABCDEFGHIKLMNOPQRST";
	// printf("\n\nIA choosen process\n");

	int depth = 2;

	// Find move to play
	nbEval = 0;
	move = sf::Vector2i(-1, -1);
	maxEval = -1000000000;
	for (int i = 0; i < moves.size(); i++)
	{
		// printf("put stone at %c%i\n",
		// 			xaxis[moves[i].x], moves[i].y);
		gridCopy = *grid;
		player->setMoves(plMoves);
		player->setCaptured(plCapture);
		opponent->setMoves(opMoves);
		opponent->setCaptured(opCapture);
		if (!gridCopy.putStone(&moves[i],
								nbMoves, player, opponent))
		{
			printf("pb ?\n");
			continue;
		}
		tmpEval = mediumMiniMax(&this->memory, &gridCopy,
					player, opponent, evaluator, !playerTurn, depth, &nbEval);
		if (depth % 2 == 1)
			tmpEval = -tmpEval;
		// printf(" - stone at %c%i for %i points\n",
		// 			xaxis[moves[i].x], moves[i].y, tmpEval);
		if (tmpEval > maxEval)
		{
			maxEval = tmpEval;
			move = moves[i];
		}
	}

	// printf("CHOOSE: stone at %c%i for %i points\n",
	// 		xaxis[move.x], move.y + 1, maxEval);

	printf("Number of evaluation %i", nbEval);

	return (move);
}
