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
	sf::Vector2i	move(-1, -1);

	std::vector<sf::Vector2i>	legalMoves = grid->getLegalMoves(player, opponent);
	if (legalMoves.size() > 0)
	{
		int index = rand_int(0, legalMoves.size() - 1);
		move = legalMoves[index];
	}
	return (move);
}

sf::Vector2i	AI::getBetterRandom(Grid *grid, Player *player, Player *opponent)
{
	sf::Vector2i	move(-1, -1);
	std::vector<sf::Vector2i>	interestingMoves = grid->getInterestingMoves(player, opponent);

	int index = rand_int(0, interestingMoves.size() - 1);
	move = interestingMoves[index];

	return (move);
}

sf::Vector2i	AI::getEasyMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator)
{
	int							maxEval, tmpEval, plCapture, opCapture;
	sf::Vector2i				move(-1, -1);
	std::vector<sf::Vector2i>	interestingMoves = grid->getInterestingMoves(player, opponent);
	inter_type					plType = player->getInterType();
	inter_type					opType = opponent->getInterType();

	plCapture = player->getCaptured();
	opCapture = opponent->getCaptured();
	maxEval = -1;
	for (int i = 0; i < interestingMoves.size(); i++)
	{
		tmpEval = evaluator->evaluationPosition(grid, plType, opType, plCapture, opCapture, interestingMoves[i].x, interestingMoves[i].y);
		if (tmpEval > maxEval)
		{
			maxEval = tmpEval;
			move = interestingMoves[i];
		}
	}

	return (move);
}