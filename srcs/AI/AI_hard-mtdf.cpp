#include <AI/AI.hpp>
#include <utils/Functions.hpp>

# define DEPTH 4

static Move	mtdf(
				std::unordered_map<int, Node> *memoryNode,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int depth,
				Move *firstGuess, Tracker *tracker);

static Move	alphaBeta(
				std::unordered_map<int, Node> *memoryNode,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int alpha, int beta,
				bool maximizingEval, int depth, Tracker *tracker);

static bool	makeMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				sf::Vector2i *pos, int nbMoves, int depth, Tracker *tracker);

static void	reverseMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				BboxManager *bboxManagerSave,
				int plCaptureSave, int opCaptureSave,
				int plNbMoveSave, int opNbMoveSave,
				sf::Vector2i *pos, int depth);


sf::Vector2i	getHardMove(
					std::unordered_map<int, Node> *memoryNode,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	Move	firstGuess;

	firstGuess.pos = sf::Vector2i(-1, -1);
	firstGuess.eval = 0;
	// TODO PUT REAL DEPTH
	printf("\nTEST\n");
	for (int depth = 1; depth <= DEPTH; depth++)
	{
		firstGuess = mtdf(
						memoryNode, grid, player, opponent,
						evaluator, depth, &firstGuess, tracker);
		printf("move %i %i %i\n",
				firstGuess.pos.x, firstGuess.pos.y, firstGuess.eval);
	}
	return (firstGuess.pos);
}


static Move	mtdf(
				std::unordered_map<int, Node> *memoryNode,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int depth,
				Move *firstGuess, Tracker *tracker)
{
	int			beta, hash;
	Node		node;
	Move		move;
	std::unordered_map<int, Node>::iterator	moveFind;
	BoardState	boardState;

	boardState = BoardState(grid->getBitBoard(player->interType),
							grid->getBitBoard(opponent->interType));
	hash = boardState.getHash();

	move = *firstGuess;

	// Create and insert root node (if not in memory)
	node.upper.pos = sf::Vector2i(-1,-1);
	node.upper.eval = 1000000001;
	node.lower.pos = sf::Vector2i(-1,-1);
	node.lower.eval = -1000000001;

	moveFind = memoryNode->find(hash);
	if (moveFind == memoryNode->end())
		memoryNode->insert(std::pair<int, Node>(hash, node));
	else // TODO : CHECK IF RESET IS USEFULL
		moveFind->second = node;

	while (node.lower.eval < node.upper.eval)
	{
		// Get root node in memory
		node = memoryNode->at(hash);

		if (move.eval == node.lower.eval)
			beta = move.eval + 1;
		else
			beta = move.eval;

		move = alphaBeta(
				memoryNode, grid, player, opponent,
				evaluator, beta - 1, beta, true, depth, tracker);

		// Get root node in memory
		node = memoryNode->at(hash);

		// Update node
		if (move.eval < beta)
			node.upper = move;
		else
			node.lower = move;

		(*memoryNode)[hash] = node;
	}

	return (move);
}


static Move	alphaBeta(
				std::unordered_map<int, Node> *memoryNode,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int alpha, int beta,
				bool maximizingEval, int depth, Tracker *tracker)
{
	int					plCaptureSave, opCaptureSave,
						plNbMoveSave, opNbMoveSave,
						hash, nbMoves,
						alphaTmp, betaTmp;
	bool				nodeInMemory;
	Node				node;
	Move				bestMove, tmp;
	std::vector<Move>	moves;
	BoardState			boardState;
	BitBoard			plBitBoard, opBitBoard;
	BboxManager			BboxManagerSave;
	std::unordered_map<int, Node>::iterator	nodeFind;

	// TODO : REMOVE
	std::clock_t	start, start2;
	int				diff;

	// Compute usefull variables
	plCaptureSave = player->nbCapture;
	opCaptureSave = opponent->nbCapture;
	plNbMoveSave = player->nbMove;
	opNbMoveSave = opponent->nbMove;
	nbMoves = plNbMoveSave + opNbMoveSave;
	BboxManagerSave = *grid->getBboxManager();
	plBitBoard = *grid->getBitBoard(player->interType);
	opBitBoard = *grid->getBitBoard(opponent->interType);
	boardState = BoardState(&plBitBoard, &opBitBoard);
	hash = boardState.getHash();

	nodeFind = memoryNode->find(hash);
	nodeInMemory = nodeFind != memoryNode->end();

	// If we already have this grid in memory
	if (nodeInMemory)
	{
		node = nodeFind->second;
		if (node.lower.eval >= beta)
			return (node.lower);

		if (node.upper.eval <= alpha)
			return (node.upper);

		alpha = max(alpha, node.lower.eval);
		beta = min(beta, node.upper.eval);
	}

	bestMove.pos = sf::Vector2i(-1, -1);
	// Compute grid evaluation at the end of recursion
	if (depth == 0)
	{
		start2 = std::clock();

		// Compute evaluation
		start = std::clock();
		bestMove.eval = evaluator->evaluateGrid(
							&plBitBoard, &opBitBoard, plCaptureSave, opCaptureSave);
		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->computeEvalTime += diff;
		tracker->nbComputeEval++;

		diff = ((double)(std::clock() - start2) / CLOCKS_PER_SEC) * 1000000;
		tracker->evaluationTime += diff;
		tracker->nbEvaluations++;
	}
	else if (maximizingEval)
	{
		bestMove.eval = -1000000001;
		alphaTmp = alpha;
		moves = grid->getInterestingMovesSorted(
						evaluator, player, opponent, true, tracker);
		if (moves.size() == 0)
		{
			bestMove.eval = 0;
			if (nbMoves == 0)
				bestMove.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
			return (bestMove);
		}

		for (int i = 0; i < moves.size() && bestMove.eval < beta; i++)
		{
			// Skip move if it's illegal
			if (!makeMove(grid, player, opponent,
							&moves[i].pos, nbMoves, depth, tracker))
				continue;

			// Get evaluation by recursion
			tmp = alphaBeta(
					memoryNode, grid, player, opponent,
					evaluator, alphaTmp, beta, false, depth - 1, tracker);

			// Restore grid and player
			reverseMove(
				grid, player, opponent, &plBitBoard, &opBitBoard,
				&BboxManagerSave, plCaptureSave, opCaptureSave,
				plNbMoveSave, opNbMoveSave, &moves[i].pos, depth);

			// Update alpha
			if (bestMove.eval < tmp.eval)
			{
				bestMove.eval = tmp.eval;
				bestMove.pos = moves[i].pos;
			}
			alphaTmp = max(alphaTmp, bestMove.eval);
		}
	}
	else // minimizing eval case
	{
		bestMove.eval = 1000000001;
		betaTmp = beta;
		moves = grid->getInterestingMovesSorted(
						evaluator, opponent, player, false, tracker);
		if (moves.size() == 0)
		{
			bestMove.eval = 0;
			if (nbMoves == 0)
				bestMove.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
			return (bestMove);
		}

		for (int i = 0; i < moves.size() && bestMove.eval > alpha; i++)
		{
			// Skip move if it's illegal
			if (!makeMove(grid, opponent, player,
							&moves[i].pos, nbMoves, depth, tracker))
				continue;

			// Get evaluation by recursion
			tmp = alphaBeta(
					memoryNode, grid, player, opponent,
					evaluator, alpha, betaTmp, true, depth - 1, tracker);

			// Restore grid and player
			reverseMove(
				grid, opponent, player, &opBitBoard, &plBitBoard,
				&BboxManagerSave, opCaptureSave, plCaptureSave,
				opNbMoveSave, plNbMoveSave, &moves[i].pos, depth);

			// Update beta
			if (bestMove.eval > tmp.eval)
			{
				bestMove.eval = tmp.eval;
				bestMove.pos = moves[i].pos;
			}
			betaTmp = min(betaTmp, bestMove.eval);
		}
	}

	if (bestMove.eval <= alpha)
	{
		if (!nodeInMemory)
		{
			node.lower.pos = sf::Vector2i(-1, -1);
			node.lower.eval = -1000000001;
		}
		node.upper = bestMove;
	}
	if (bestMove.eval > alpha && bestMove.eval < beta)
	{
		node.lower = bestMove;
		node.upper = bestMove;
	}
	if (bestMove.eval >= beta)
	{
		node.lower = bestMove;
		if (!nodeInMemory)
		{
			node.upper.pos = sf::Vector2i(-1, -1);
			node.upper.eval = 1000000001;
		}
	}

	// If node is already in memory, update it
	if (nodeInMemory)
		nodeFind->second = node;
	else // else insert it
		memoryNode->insert(std::pair<int, Node>(hash, node));

	return (bestMove);
}


static bool	makeMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				sf::Vector2i *pos, int nbMoves, int depth, Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	start = std::clock();
	if (!grid->putStoneAI(pos, nbMoves, player, opponent))
		return (false);

	if (depth > 1)
		grid->updateBboxManager(pos);

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->checkStoneNumber++;
	tracker->putStoneTime += diff;
	player->nbMove++;
	return (true);
}


static void	reverseMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				BboxManager *bboxManagerSave,
				int plCaptureSave, int opCaptureSave,
				int plNbMoveSave, int opNbMoveSave,
				sf::Vector2i *pos, int depth)
{
	// Reset grid
	if (player->nbCapture != plCaptureSave
		|| opponent->nbCapture != opCaptureSave)
	{
		grid->setBitBoard(plBitBoard, player->interType);
		grid->setBitBoard(opBitBoard, opponent->interType);
	}
	else
		grid->removeStone(pos, player->interType);

	if (depth > 1)
		grid->setBboxManager(bboxManagerSave);

	// Reset player
	player->nbMove = plNbMoveSave;
	player->nbCapture = plCaptureSave;
	opponent->nbMove = opNbMoveSave;
	opponent->nbCapture = opCaptureSave;
}
