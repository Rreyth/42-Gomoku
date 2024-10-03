#include <game/Node.hpp>
#include <game/BoardState.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Node::Node(void)
{
	this->hash = 0;
	this->evaluation = 0;
	this->killerMoveScore = 0;

	this->playerTurn = true;
	this->childrenCompute = false;
	this->evaluationCompute = false;
	this->moveCompute = false;
	this->bboxManagerUpdate = false;
	this->killerMoveCompute = false;
	this->killerMoveResult = false;

	this->lastMove.pos = sf::Vector2i(-1, -1);
	this->lastMove.eval = 0;
}


Node::Node(
		Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
		bool playerTurn, Move *moveFrom)
{
	// Copy data from parameter
	this->grid = *grid;
	this->player = *player;
	this->opponent = *opponent;
	this->playerTurn = playerTurn;
	this->lastMove = *moveFrom;
	this->killerMoveScore = 0;

	// Compute data
	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);

	BoardState boardState(this->plBitboard, this->opBitboard);
	this->hash = boardState.getHash();

	// Set compute variable to false
	this->evaluation = 0;
	this->childrenCompute = false;
	this->evaluationCompute = false;
	this->moveCompute = false;
	this->bboxManagerUpdate = false;
	this->killerMoveCompute = false;
	this->killerMoveResult = false;
}


Node::Node(const Node &obj)
{
	this->hash = obj.hash;
	this->evaluation = obj.evaluation;
	this->killerMoveScore = obj.killerMoveScore;

	this->playerTurn = obj.playerTurn;
	this->childrenCompute = obj.childrenCompute;
	this->evaluationCompute = obj.evaluationCompute;
	this->moveCompute = obj.moveCompute;
	this->bboxManagerUpdate = obj.bboxManagerUpdate;
	this->killerMoveCompute = obj.killerMoveCompute;
	this->killerMoveResult = obj.killerMoveResult;

	this->lastMove = obj.lastMove;
	this->player = obj.player;
	this->opponent = obj.opponent;
	this->grid = obj.grid;

	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);

	for (int i = 0; i < obj.children.size(); i++)
		this->children.push_back(obj.children[i]);
}


Node::~Node()
{

}


////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

Move	*Node::getMove(void)
{
	return (&this->lastMove);
}


int	Node::getNbMove(void)
{
	return (this->player.nbMove, this->opponent.nbMove);
}


int	Node::getKillerScore(void)
{
	return (this->killerMoveScore);
}

////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////

Node	&Node::operator=(const Node &obj)
{
	if (this == &obj)
		return (*this);

	this->hash = obj.hash;
	this->evaluation = obj.evaluation;
	this->killerMoveScore = obj.killerMoveScore;

	this->playerTurn = obj.playerTurn;
	this->childrenCompute = obj.childrenCompute;
	this->evaluationCompute = obj.evaluationCompute;
	this->moveCompute = obj.moveCompute;
	this->bboxManagerUpdate = obj.bboxManagerUpdate;
	this->killerMoveCompute = obj.killerMoveCompute;
	this->killerMoveResult = obj.killerMoveResult;

	this->lastMove = obj.lastMove;
	this->player = obj.player;
	this->opponent = obj.opponent;
	this->grid = obj.grid;

	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);

	this->children.clear();
	for (int i = 0; i < obj.children.size(); i++)
		this->children.push_back(obj.children[i]);

	return (*this);
}


////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

// --- Update node methods -----------------------------------------------------

Node	*Node::getActualNode(Grid *grid)
{
	Node		*tmpNode;
	BitBoard	*plBitboard, *opBitboard;

	// If the node haven't compute children, stop the search here
	if (!this->childrenCompute)
		return (NULL);

	plBitboard = grid->getBitBoard(this->player.interType);
	opBitboard = grid->getBitBoard(this->opponent.interType);

	// Check every children to find the one can match
	for (int i = 0; i < this->children.size(); i++)
	{
		tmpNode = &this->children[i];

		// If a node match, return it
		if (*tmpNode->plBitboard == *plBitboard
			&& *tmpNode->opBitboard == *opBitboard)
			return (tmpNode);
	}

	return (NULL);
}


Node	*Node::getNextNode(sf::Vector2i *move)
{
	Node		*tmpNode;
	BitBoard	*plBitboard, *opBitboard;

	// If the node haven't compute children, stop the search here
	if (!this->childrenCompute)
		return (NULL);

	// Check every children to find the one can match
	for (int i = 0; i < this->children.size(); i++)
	{
		tmpNode = &this->children[i];

		// If a node match, return it
		if (tmpNode->lastMove.pos == *move)
			return (tmpNode);
	}

	return (NULL);
}

// --- Recursion methods -------------------------------------------------------

int	Node::getEvaluation(Evaluation *evaluator, Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	tracker->nbEvaluations++;

	// If the evaluation is already compute, give it
	if (this->evaluationCompute)
		return (this->evaluation);

	// Else compute it
	start = std::clock();

	this->evaluation = evaluator->evaluateGrid(
									this->plBitboard, this->opBitboard,
									this->player.nbCapture,
									this->opponent.nbCapture);
	this->evaluationCompute = true;

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->evaluationTime += diff;
	tracker->computeEvalTime += diff;
	tracker->nbComputeEval++;

	return (this->evaluation);
}


std::vector<Node>	*Node::getChildren(Evaluation *evaluator, Tracker *tracker)
{
	Node				newNode;
	Move				tmp;
	std::vector<Move>	moves;

	// If children are already compute, return them
	if (this->childrenCompute)
	{
		return (&this->children);
	}

	// Else compute them
	if (this->playerTurn)
		moves = this->grid.getInterestingMovesSorted(
							evaluator, &this->player, &this->opponent,
							true, tracker);
	else
		moves = this->grid.getInterestingMovesSorted(
							evaluator, &this->opponent, &this->player,
							true, tracker);

	// Case of first move of the game
	if (moves.size() == 0 && this->player.nbMove + this->opponent.nbMove == 0)
	{
		tmp.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		tmp.eval = 0;
		moves.push_back(tmp);
	}

	for (int i = 0; i < moves.size(); i++)
	{
		newNode = Node(
					&this->grid, &this->player, &this->opponent,
					!this->playerTurn, &moves[i]);
		this->children.push_back(newNode);
	}
	this->childrenCompute = true;

	return (&this->children);
}


void	Node::applyMove(int depth, Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	if (!this->moveCompute)
	{
		start = std::clock();

		if (this->playerTurn)
		{
			this->grid.putStoneAI(
						&this->lastMove.pos,
						this->player.nbMove + this->opponent.nbMove,
						&this->player,
						&this->opponent);
			this->player.nbMove++;
		}
		else
		{
			this->grid.putStoneAI(
						&this->lastMove.pos,
						this->player.nbMove + this->opponent.nbMove,
						&this->opponent,
						&this->player);
			this->opponent.nbMove++;
		}

		this->moveCompute = true;

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->checkStoneNumber++;
		tracker->putStoneTime += diff;
	}

	if (!this->bboxManagerUpdate)
	{
		if (depth > 1)
		{
			start = std::clock();

			this->grid.updateBboxManager(&this->lastMove.pos);
			this->bboxManagerUpdate = true;

			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
			tracker->putStoneTime += diff;
		}
	}
}


bool	Node::isKillerMove(void)
{
	if (this->killerMoveCompute)
	{
		return (this->killerMoveResult);
	}

	this->killerMoveResult = false;
	if (this->playerTurn)
	{
		if (this->lastMove.eval >= 10000000)
			this->killerMoveResult = this->grid.checkWinCondition(
													&this->player,
													&this->opponent);
	}
	else
	{
		if (this->lastMove.eval <= -1000000)
			this->killerMoveResult = this->grid.checkWinCondition(
													&this->opponent,
													&this->player);
	}

	if (this->player.winState != WIN_STATE_NONE)
	{
		this->player.winState = WIN_STATE_NONE;
		this->killerMoveScore = CASE_WIN_POINT;
		printf("WIIIIIIIIIIIIIIIIIIN\n");
	}
	else if (this->opponent.winState != WIN_STATE_NONE)
	{
		this->opponent.winState = WIN_STATE_NONE;
		this->killerMoveScore = CASE_LOOSE_POINT;
		printf("OH NOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
	}

	this->killerMoveCompute = true;
	return (this->killerMoveResult);
}



// TODO REMOVE
void	Node::printGrid(void)
{
	for (int i = 0; i < GRID_W_INTER; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (this->plBitboard->bbH[i] & (1 << j))
				printf("P ");
			else if (this->opBitboard->bbH[i] & (1 << j))
				printf("O ");
			else
				printf(". ");
		}
		printf("\n");
	}
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
