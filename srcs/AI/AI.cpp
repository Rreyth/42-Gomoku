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
	return (move);
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


static long	mediumMiniMax(
				Grid *grid, Player *player, Player *opponent,
				Evaluation *evaluator, int x, int y, int depth)
{
	long						maxEval, tmpEval;
	int							plCapture, opCapture;
	std::vector<sf::Vector2i>	interestingMoves;
	inter_type					plType, opType;
	Grid						gridCopy;

	// Compute variables for evaluation and put stone
	plType = player->getInterType();
	opType = opponent->getInterType();
	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();


	// End of recursion, return only the evaluation of this move
	if (depth <= 0)
		return (evaluator->evaluationPosition(
							grid, plType, opType, plCapture, opCapture, x, y));
	// Copy grid
	gridCopy = *grid;

	// Simulate put stone
	if (!gridCopy.putStone(
					sf::Vector2i(x, y),
					player->getMoves() + opponent->getMoves(), plType, opType))
		return (-1);

	// Get interesting moves
	interestingMoves = gridCopy.getInterestingMoves(opponent, player);
	if (interestingMoves.size() == 0)
		return (-1);

	// Find best move for player
	// Find moves
	// std::string	xaxis = "ABCDEFGHIKLMNOPQRST";
	depth--;
	maxEval = mediumMiniMax(
						&gridCopy, opponent, player, evaluator,
						interestingMoves[0].x, interestingMoves[0].y, depth);
	for (int i = 1; i < interestingMoves.size(); i++)
	{
		tmpEval = mediumMiniMax(
						&gridCopy, opponent, player, evaluator,
						interestingMoves[i].x, interestingMoves[i].y, depth);
		// printf("   - stone at %c%i for %i points\n",
		// 			xaxis[interestingMoves[i].x], interestingMoves[i].y, tmpEval);
		if (tmpEval > maxEval)
			maxEval = tmpEval;
	}
	// Find best move for opponent
	return (maxEval);
}


sf::Vector2i	AI::getMediumMove(
						Grid *grid, Player *player, Player *opponent,
						Evaluation *evaluator)
{
	long						maxEval, tmpEval;
	std::vector<sf::Vector2i>	interestingMoves;
	sf::Vector2i				move;

	// Get interesting move
	interestingMoves = grid->getInterestingMoves(player, opponent);
	if (interestingMoves.size() == 0)
	{
		if (player->getMoves() + opponent->getMoves() == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	std::string	xaxis = "ABCDEFGHIKLMNOPQRST";

	// printf("\n\nIA choosen process\n");
	// Find move to play
	move = sf::Vector2i(-1, -1);
	maxEval = -10000000000;
	for (int i = 0; i < interestingMoves.size(); i++)
	{
		tmpEval = mediumMiniMax(
					grid, opponent, player, evaluator,
					interestingMoves[i].x, interestingMoves[i].y, 0);
		// printf(" - stone at %c%i for %li points\n",
		// 			xaxis[interestingMoves[i].x], interestingMoves[i].y, tmpEval);
		if (tmpEval > maxEval)
		{
			maxEval = tmpEval;
			move = interestingMoves[i];
		}
	}

	// printf("CHOOSE: stone at %c%i for %li points\n",
	// 		xaxis[move.x], move.y + 1, maxEval);

	return (move);
}
