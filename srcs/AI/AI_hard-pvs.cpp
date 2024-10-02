#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	alphaBetaMemory(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker);

static void	makeMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				sf::Vector2i *pos, bool playerTurn,
				int nbMoves, int depth, Tracker *tracker);

static void	reverseMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				BboxManager *bboxManagerSave,
				int plSaveNbCapture, int opSaveNbCapture,
				int plSaveNbMove, int opSaveNbMove,
				sf::Vector2i *pos, bool playerTurn, int depth);

static bool	computeKillerMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				bool playerTurn, Move *tmpMove, Tracker *tracker);

#define DEPTH 4

sf::Vector2i	getHardMove(
					std::unordered_map<int, std::vector<Move>> *memoryMoves,
					std::unordered_map<int, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	Move	bestMove;

	bestMove = alphaBetaMemory(memoryMoves, memoryEval, grid,
								player, opponent, evaluator,
								true, -1000000001, 1000000001,
								DEPTH, tracker);

	return (bestMove.pos);
}


static Move	alphaBetaMemory(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker)
{
	int					plSaveNbCapture, opSaveNbCapture,
						plSaveNbMove, opSaveNbMove, nbMoves, hash;
	Move				bestMove, tmpMove;
	inter_type			plType, opType;
	std::vector<Move>	moves;
	BitBoard			plBitBoard, opBitBoard;
	BboxManager			bboxManagerSave;
	BoardState			boardState;
	std::unordered_map<int, int>::const_iterator	evalFind;
	std::unordered_map<int, std::vector<Move>>::const_iterator	movesFind;

	// TODO : REMOVE
	std::clock_t	start, start2;
	int				diff;

	// Compute variables for evaluation and put stone
	plType = player->interType;
	opType = opponent->interType;
	plSaveNbCapture = player->nbCapture;
	opSaveNbCapture = opponent->nbCapture;
	plBitBoard = *grid->getBitBoard(plType);
	opBitBoard = *grid->getBitBoard(opType);
	boardState = BoardState(&plBitBoard, &opBitBoard);
	hash = boardState.getHash();

	bestMove.pos = sf::Vector2i(-1, -1);
	bestMove.eval = 0;
	tmpMove.pos = sf::Vector2i(-1, -1);
	tmpMove.eval = 0;

	if (computeKillerMove(
			grid, player, opponent, !maximizingEval,
			&tmpMove, tracker))
		return (tmpMove);

	// Stop recursion and return the grid evaluation
	if (depth <= 0)
	{
		start2 = std::clock();

		// Evalute move
		evalFind = memoryEval->find(hash);
		if (evalFind != memoryEval->end())
		{
			// Try to get evaluation in memory
			tmpMove.eval = evalFind->second;
			tracker->nbMemoryEval++;
		}
		else
		{
			// If not in memory, compute it and put it in memory
			start = std::clock();

			tmpMove.eval = evaluator->evaluateGrid(
									grid->getBitBoard(plType),
									grid->getBitBoard(opType),
									plSaveNbCapture, opSaveNbCapture);
			memoryEval->insert(std::pair<int, int>(hash, tmpMove.eval));

			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
			tracker->computeEvalTime += diff;
			tracker->nbComputeEval++;
		}

		diff = ((double)(std::clock() - start2) / CLOCKS_PER_SEC) * 1000000;
		tracker->evaluationTime += diff;
		tracker->nbEvaluations++;

		return (tmpMove);
	}

	// Get interesting moves
	movesFind = memoryMoves->find(hash);
	if (movesFind != memoryMoves->end())
	{
		moves = movesFind->second;
	}
	else
	{
		// If not in memory, compute it and put it in memory
		if (maximizingEval)
			moves = grid->getInterestingMovesSorted(
							evaluator, player, opponent, true, tracker);
		else
			moves = grid->getInterestingMovesSorted(
							evaluator, opponent, player, false, tracker);
		memoryMoves->insert(std::pair<int, std::vector<Move>>(hash, moves));
	}

	// Compute nb moves
	plSaveNbMove = player->nbMove;
	opSaveNbMove = opponent->nbMove;
	nbMoves = plSaveNbMove + opSaveNbMove;

	if (moves.size() == 0)
	{
		if (nbMoves == 0)
			bestMove.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		return (bestMove);
	}

	// Compute variables put stone and reset players
	bboxManagerSave = *grid->getBboxManager();

	// Compute first move
	bestMove.eval = -1000000001;

	for (int i = 0; i < moves.size(); i++)
	{
		// Simulate move
		makeMove(
			grid, player, opponent, &moves[i].pos,
			maximizingEval, nbMoves, depth, tracker);

		// Get evaluation for this move
		if (i == 0)
		{
			tmpMove = alphaBetaMemory(memoryMoves, memoryEval, grid,
										player, opponent, evaluator,
										!maximizingEval, -beta, -alpha,
										depth - 1, tracker);
			tmpMove.eval = -tmpMove.eval;
		}
		else
		{
			tmpMove = alphaBetaMemory(memoryMoves, memoryEval, grid,
										player, opponent, evaluator,
										!maximizingEval, -alpha - 1, -alpha,
										depth - 1, tracker);
			tmpMove.eval = -tmpMove.eval;

			if (alpha < tmpMove.eval && tmpMove.eval < beta)
			{
				tmpMove = alphaBetaMemory(memoryMoves, memoryEval, grid,
											player, opponent, evaluator,
											!maximizingEval, -beta, -alpha,
											depth - 1, tracker);
				tmpMove.eval = -tmpMove.eval;
			}
		}
		tmpMove.pos = moves[i].pos;

		reverseMove(
			grid, player, opponent, &plBitBoard, &opBitBoard,
			&bboxManagerSave, plSaveNbCapture, opSaveNbCapture,
			plSaveNbMove, opSaveNbMove, &moves[i].pos, maximizingEval, depth);

		// For maximizing
		// Update score
		if (tmpMove.eval > bestMove.eval)
			bestMove = tmpMove;

		// Update alpha
		if (tmpMove.eval > alpha)
			alpha = tmpMove.eval;

		// If we already have already better move, stop searching by pruning
		if (alpha >= beta)
			break;
	}

	return (bestMove);
}


static void	makeMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				sf::Vector2i *pos, bool playerTurn,
				int nbMoves, int depth, Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	start = std::clock();

	if (playerTurn)
	{
		grid->putStoneAI(pos, nbMoves, player, opponent);
		player->nbMove++;
	}
	else
	{
		grid->putStoneAI(pos, nbMoves, opponent, player);
		opponent->nbMove++;
	}
	if (depth > 1)
		grid->updateBboxManager(pos);

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->putStoneTime += diff;
	tracker->checkStoneNumber++;
}


static void	reverseMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				BboxManager *bboxManagerSave,
				int plSaveNbCapture, int opSaveNbCapture,
				int plSaveNbMove, int opSaveNbMove,
				sf::Vector2i *pos, bool playerTurn, int depth)
{
	// Reset grid
	if (player->nbCapture != plSaveNbCapture
		|| opponent->nbCapture != opSaveNbCapture)
	{
		grid->setBitBoard(plBitBoard, player->interType);
		grid->setBitBoard(opBitBoard, opponent->interType);
	}
	else
	{
		if (playerTurn)
			grid->removeStone(pos, player->interType);
		else
			grid->removeStone(pos, opponent->interType);
	}
	if (depth > 1)
		grid->setBboxManager(bboxManagerSave);

	// Reset player
	player->nbMove = plSaveNbMove;
	player->nbCapture = plSaveNbCapture;
	opponent->nbMove = opSaveNbMove;
	opponent->nbCapture = opSaveNbCapture;
}


static bool	computeKillerMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				bool playerTurn, Move *tmpMove, Tracker *tracker)
{
	bool	killerMove;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	start = std::clock();

	if (playerTurn)
	{
		killerMove = grid->checkWinCondition(player, opponent);
	}
	else
	{
		killerMove = grid->checkWinCondition(opponent, player);
	}

	if (player->winState != WIN_STATE_NONE)
	{
		player->winState = WIN_STATE_NONE;
		tmpMove->eval = CASE_WIN_POINT; // Nothing is better than a victory
	}
	else
	{
		opponent->winState = WIN_STATE_NONE;
		tmpMove->eval = CASE_LOOSE_POINT; // Nothing is worst than a defeat
	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->checkWinTime += diff;
	tracker->checkWinNumber++;

	return (killerMove);
}
