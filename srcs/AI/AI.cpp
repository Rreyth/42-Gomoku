#include <AI/AI.hpp>
#include <utils/Functions.hpp>

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
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
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

sf::Vector2i	AI::getRandomMove(Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
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


sf::Vector2i	AI::getBetterRandom(Grid *grid, PlayerInfo *player, PlayerInfo *opponent)
{
	std::vector<sf::Vector2i>	moves;

	// Get interesting move
	moves = grid->getInterestingMoves(player, opponent);
	if (moves.size() == 0)
	{
		if (player->nbMove + opponent->nbMove == 0)
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
						Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
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
		if (player->nbMove + opponent->nbMove == 0)
			return(sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		else
			return (sf::Vector2i(-1, -1));
	}

	// Compute variables for evaluation
	plCapture = player->nbCapture;
	opCapture = opponent->nbCapture;
	plBitBoard = grid->getBitBoard(player->interType);
	opBitBoard = grid->getBitBoard(opponent->interType);

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