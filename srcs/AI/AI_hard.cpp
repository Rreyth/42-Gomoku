#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	miniMax(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker);
static Move	mtdf(std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int f,
				int depth, Tracker *tracker);

sf::Vector2i	getHardMove(
					std::unordered_map<int, std::vector<Move>> *memoryMoves,
					std::unordered_map<int, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	Move	bestMove;

	bestMove.eval = 0;
	for (int depth = 1; depth <= AI_HARD_DEPTH; depth++)
	{
		bestMove = mtdf(
					memoryMoves, memoryEval, grid, player, opponent,
					evaluator, bestMove.eval, depth, tracker);
	}

	return (bestMove.pos);
}

static Move	mtdf(std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int f,
				int depth, Tracker *tracker)
{
	int		beta, upperBound, lowerBound;
	Move	bestMove;

	bestMove.eval = f;

	upperBound = 1000000001;
	lowerBound = -1000000001;

	while (lowerBound < upperBound)
	{
		beta = max(bestMove.eval, lowerBound + 1);
		bestMove = miniMax(memoryMoves, memoryEval, grid,
									player, opponent, evaluator,
									true, beta - 1, beta,
									depth, tracker);
		if (bestMove.eval < beta)
			upperBound = bestMove.eval;
		else
			lowerBound = bestMove.eval;
	}

	return (bestMove);
}

static Move	miniMax(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker)
{
	int					plCapture, opCapture,
						plMoves, opMoves, nbMoves, hash;
	bool				killerMove;
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
	plCapture = player->nbCapture;
	opCapture = opponent->nbCapture;
	plBitBoard = *grid->getBitBoard(plType);
	opBitBoard = *grid->getBitBoard(opType);
	boardState = BoardState(&plBitBoard, &opBitBoard);
	hash = boardState.getHash();

	bestMove.pos = sf::Vector2i(-1, -1);
	bestMove.eval = 0;
	tmpMove.pos = sf::Vector2i(-1, -1);
	tmpMove.eval = 0;

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
									plCapture, opCapture);
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
	plMoves = player->nbMove;
	opMoves = opponent->nbMove;
	nbMoves = plMoves + opMoves;

	if (moves.size() == 0)
	{
		if (nbMoves == 0)
			bestMove.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		return (bestMove);
	}

	// Compute variables put stone and reset players
	bboxManagerSave = *grid->getBboxManager();

	// Find move
	if (maximizingEval)
		bestMove.eval = -1000000001;
	else
		bestMove.eval = 1000000001;

	for (int i = 0; i < moves.size() && i < AI_HARD_LIMIT; i++)
	{
		start = std::clock();
		tmpMove.pos = moves[i].pos;
		tmpMove.eval = 0;

		// Simulate move
		if (maximizingEval)
		{
			if (!grid->putStoneAI(&tmpMove.pos, nbMoves, player, opponent))
				continue;
			if (depth > 1)
				grid->updateBboxManager(&tmpMove.pos);
			player->nbMove++;
		}
		else
		{
			if (!grid->putStoneAI(&tmpMove.pos, nbMoves, opponent, player))
				continue;
			if (depth > 1)
				grid->updateBboxManager(&tmpMove.pos);
			opponent->nbMove++;
		}

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->putStoneTime += diff;
		tracker->checkStoneNumber++;

		// If move have an high score, check win case
		if ((maximizingEval && moves[i].eval >= 10000000)
			|| (!maximizingEval && moves[i].eval <= -10000000))
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
			if (player->winState != WIN_STATE_NONE)
			{
				player->winState = WIN_STATE_NONE;
				tmpMove.eval = CASE_WIN_POINT; // Nothing is better than a victory
			}
			else
			{
				opponent->winState = WIN_STATE_NONE;
				tmpMove.eval = CASE_LOOSE_POINT; // Nothing is worst than a defeat
			}
		}
		else
		{
			// Get evaluation for this move
			tmpMove.eval = miniMax(memoryMoves, memoryEval, grid,
									player, opponent, evaluator,
									!maximizingEval, alpha, beta,
									depth - 1, tracker).eval;
		}

		// Reset grid
		if (player->nbCapture != plCapture
			|| opponent->nbCapture != opCapture)
		{
			grid->setBitBoard(&plBitBoard, plType);
			grid->setBitBoard(&opBitBoard, opType);
		}
		else
		{
			if (maximizingEval)
				grid->removeStone(&tmpMove.pos, plType);
			else
				grid->removeStone(&tmpMove.pos, opType);
		}
		if (depth > 1)
			grid->setBboxManager(&bboxManagerSave);

		// Reset player
		player->nbMove = plMoves;
		player->nbCapture = plCapture;
		opponent->nbMove = opMoves;
		opponent->nbCapture = opCapture;

		// For maximizing
		if (maximizingEval)
		{
			// Update score
			if (tmpMove.eval > bestMove.eval)
				bestMove = tmpMove;

			// Update alpha
			if (tmpMove.eval > alpha)
				alpha = tmpMove.eval;
		}
		// For minimizing
		else
		{
			// Update score
			if (tmpMove.eval < bestMove.eval)
				bestMove = tmpMove;

			// Update beta
			if (tmpMove.eval < beta)
				beta = tmpMove.eval;
		}

		// If we already have already better move, stop searching by pruning
		if (beta <= alpha)
			break;
	}

	return (bestMove);
}
