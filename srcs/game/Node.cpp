#include <game/Node.hpp>
#include <game/BoardState.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Node::Node(void)
{
	// Set default attribute
	this->evaluationCompute = false;
	this->killerMoveCompute = false;
	this->bboxManagerUpdate = false;
	this->childrenCompute = false;
	this->moveApplied = false;
	this->hashCompute = false;
	this->hash = 0;
	this->evaluation = 0;
	this->killerMoveResult = 0;

	// Compute attribute
	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);
}


Node::Node(
		PlayerInfo *player, PlayerInfo *opponent,
		Grid *grid, Move *move)
{
	// Copy variable
	this->player = *player;
	this->opponent = *opponent;
	this->grid = *grid;
	this->move = *move;

	// Set default attribute
	this->evaluationCompute = false;
	this->killerMoveCompute = false;
	this->bboxManagerUpdate = false;
	this->childrenCompute = false;
	this->moveApplied = false;
	this->hashCompute = false;
	this->hash = 0;
	this->evaluation = 0;
	this->killerMoveResult = 0;

	// Compute attribute
	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);
}


Node::Node(const Node &obj)
{
	// Copy variable
	this->player = obj.player;
	this->opponent = obj.opponent;
	this->grid = obj.grid;
	this->move = obj.move;
	this->evaluationCompute = obj.evaluationCompute;
	this->killerMoveCompute = obj.killerMoveCompute;
	this->bboxManagerUpdate = obj.bboxManagerUpdate;
	this->childrenCompute = obj.childrenCompute;
	this->moveApplied = obj.moveApplied;
	this->hashCompute = obj.hashCompute;
	this->hash = obj.hash;
	this->evaluation = obj.evaluation;
	this->killerMoveResult = obj.killerMoveResult;
	for (int i = 0; i < obj.children.size(); i++)
	{
		this->children.push_back(obj.children[i]);
	}

	// Compute attribute
	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);
}


Node::~Node()
{

}


////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

sf::Vector2i	*Node::getMove(void)
{
	return (&this->move.pos);
}

////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////

Node	&Node::operator=(const Node &obj)
{
	if (this == &obj)
		return (*this);

		// Copy variable
	this->player = obj.player;
	this->opponent = obj.opponent;
	this->grid = obj.grid;
	this->move = obj.move;
	this->evaluationCompute = obj.evaluationCompute;
	this->killerMoveCompute = obj.killerMoveCompute;
	this->bboxManagerUpdate = obj.bboxManagerUpdate;
	this->childrenCompute = obj.childrenCompute;
	this->moveApplied = obj.moveApplied;
	this->hashCompute = obj.hashCompute;
	this->hash = obj.hash;
	this->evaluation = obj.evaluation;
	this->killerMoveResult = obj.killerMoveResult;
	this->children.clear();
	for (int i = 0; i < obj.children.size(); i++)
	{
		this->children.push_back(obj.children[i]);
	}

	// Compute attribute
	this->plBitboard = this->grid.getBitBoard(this->player.interType);
	this->opBitboard = this->grid.getBitBoard(this->opponent.interType);

	return (*this);
}


////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

int	Node::getKillerMove(
			Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// If killer move is already compute, return it
	if (this->killerMoveCompute)
		return (this->killerMoveResult);

	this->killerMoveCompute = true;

	// If the evaluation of the move is too low, skip check killer move
	// to avoid useless computation
	if (this->move.eval < 10000000)
		return (this->killerMoveResult);

	start = std::clock();

	bool	killerMove = this->grid.checkWinCondition(&this->opponent, &this->player);

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->checkWinTime += diff;
	tracker->checkWinNumber++;

	// If there is no killer move, return result
	if (!killerMove)
		return (this->killerMoveResult);

	// If player win from opponent view point
	if (this->player.winState != WIN_STATE_NONE)
	{
		this->player.winState = WIN_STATE_NONE;
		// Nothing is better than a victory
		this->killerMoveResult = CASE_LOOSE_POINT;
	}
	// If opponent win from opponent view point
	else
	{
		this->opponent.winState = WIN_STATE_NONE;
		// Nothing is better than a victory
		this->killerMoveResult = CASE_WIN_POINT;
	}

	return (this->killerMoveResult);
}


int	Node::getEvaluation(
			std::unordered_map<int, int> *memoryEval,
			Evaluation *evaluator, Tracker *tracker)
{
	std::unordered_map<int, int>::const_iterator	evalFind;
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	tracker->nbEvaluations++;

	// If evaluation is already compute, return it
	if (this->evaluationCompute)
		return (this->evaluation);

	// Else compute it
	this->evaluationCompute = true;

	start = std::clock();

	evalFind = memoryEval->find(this->hash);

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->evaluationTime += diff;

	// If the evaluation has already be compute, just get it from memory
	if (evalFind != memoryEval->end())
	{
		tracker->nbMemoryEval++;

		this->evaluation = evalFind->second;
		return (this->evaluation);
	}

	// Else compute it
	start = std::clock();
	this->evaluation = evaluator->evaluateGrid(
									this->plBitboard, this->opBitboard,
									this->player.nbCapture, this->opponent.nbCapture);

	// And save it in memory
	memoryEval->insert(std::pair<int, int>(this->hash, this->evaluation));

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->computeEvalTime += diff;
	tracker->evaluationTime += diff;
	tracker->nbComputeEval++;

	return (this->evaluation);
}


void	Node::appliedMove(
				int depth, Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// If move isn't already be apply, apply it
	if (!this->moveApplied)
	{
		start = std::clock();

		// Move is the move done by the opponent to arrived on this grid state.
		this->grid.putStoneAI(
						&this->move.pos,
						this->player.nbMove + this->opponent.nbMove,
						&this->opponent, &this->player);
		this->moveApplied = true;
		this->opponent.nbMove++;

		this->computeHash();

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->putStoneTime += diff;
		tracker->checkStoneNumber++;
	}

	// If bboxManager isn't update and depth is higher than 1, update it
	if (!this->bboxManagerUpdate && depth > 1)
	{
		start = std::clock();

		this->grid.updateBboxManager(&this->move.pos);
		this->bboxManagerUpdate = true;

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->putStoneTime += diff;
	}
}


void	Node::computeHash(void)
{
	// If hash is already compute, skip it
	if (this->hashCompute)
		return ;

	this->hashCompute = true;
	BoardState boardState(this->plBitboard, this->opBitboard);
	this->hash = boardState.getHash();
}


std::vector<Node>	*Node::getChildren(
							std::unordered_map<int, std::vector<Move>> *memoryMoves,
							Evaluation *evaluator, Tracker *tracker)
{
	std::vector<Move>											moves;
	std::unordered_map<int, std::vector<Move>>::const_iterator	movesFind;

	// If children is already compute, just return them
	if (this->childrenCompute)
		return (&this->children);

	// Else compute them
	this->childrenCompute = true;

	movesFind = memoryMoves->find(this->hash);

	// If moves are already be computed, just get them from memory
	if (movesFind != memoryMoves->end())
	{
		moves = movesFind->second;
	}
	else
	{
		// Else compute them
		moves = this->grid.getInterestingMovesSorted(
							evaluator, &this->player, &this->opponent, true, tracker);
		// And add them into memory
		memoryMoves->insert(std::pair<int, std::vector<Move>>(this->hash, moves));
	}

	// If there is no children interesting
	if (moves.size() == 0)
	{
		// If this isn't the first move -> end game
		if (this->player.nbMove + this->opponent.nbMove > 0)
			return (&this->children);

		// Else, add a move on grid center
		Move	tmp;
		tmp.eval = 0;
		tmp.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		moves.push_back(tmp);
	}

	for (int i = 0; i < moves.size() && i < AI_HARD_LIMIT; i++)
	{
		Node	tmp(&this->opponent, &this->player, &this->grid, &moves[i]);
		this->children.push_back(tmp);
	}

	return (&this->children);
}


Node	*Node::getRootNodeFromGrid(
				Grid *grid)
{
	BitBoard	*plBitboard, *opBitboard;
	Node		*child;

	plBitboard = grid->getBitBoard(this->player.interType);
	opBitboard = grid->getBitBoard(this->opponent.interType);

	for (int i = 0; i < this->children.size(); i++)
	{
		child = &this->children[i];

		if (*plBitboard == *child->opBitboard
			&& *opBitboard == *child->plBitboard)
			return (child);
	}

	return (NULL);
}


Node	*Node::getRootNodeFromMove(
				sf::Vector2i *move)
{
	Node		*child;

	for (int i = 0; i < this->children.size(); i++)
	{
		child = &this->children[i];

		if (*move == child->move.pos)
			return (child);
	}

	return (NULL);
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
