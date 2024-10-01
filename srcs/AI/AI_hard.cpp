#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	alphaBetaMemory(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker);

static bool	makeMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				sf::Vector2i *pos, int nbMoves, int depth, Tracker *tracker);
static void	reverseMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				BitBoard *plBitBoard, BitBoard *opBitBoard,
				BboxManager *bboxManagerSave,
				int plSaveNbCapture, int opSaveNbCapture,
				int plSaveNbMove, int opSaveNbMove,
				sf::Vector2i *pos, int depth);

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
						plSaveNbMove, opSaveNbMove, nbMove;
	Move				bestMove, current;
	bool				killerMove;
	std::vector<Move>	moves;
	BitBoard			plSaveBitboard, opSaveBitboard;
	BboxManager			bboxManagerSave;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;


	// If case of end of recursion, evaluate grid
	if (depth <= 0)
	{
		start = std::clock();

		current.pos = sf::Vector2i(-1, -1);
		current.eval = evaluator->evaluateGrid(
									grid->getBitBoard(player->interType),
									grid->getBitBoard(opponent->interType),
									player->nbCapture, opponent->nbCapture);

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->computeEvalTime += diff;
		tracker->nbComputeEval++;

		tracker->evaluationTime += diff;
		tracker->nbEvaluations++;

		return (bestMove);
	}

	// TODO: IMPLEMENT KILLER MOVES
	// if (maximizingEval)
	// 	killerMove = grid->checkWinCondition(opponent, player);
	// else
	// 	killerMove = grid->checkWinCondition(player, opponent);

	// // If a there is a win, return here
	// if (killerMove)
	// {
	// 	current.pos = sf::Vector2i(-1, -1);
	// 	if (player->winState != WIN_STATE_NONE)
	// 	{
	// 		current.eval = CASE_WIN_POINT;
	// 		player->winState = WIN_STATE_NONE;
	// 	}
	// 	else
	// 	{
	// 		current.eval = CASE_LOOSE_POINT;
	// 		opponent->winState = WIN_STATE_NONE;
	// 	}
	// 	return (bestMove);
	// }

	// Compute variables
	plSaveNbMove = player->nbMove;
	opSaveNbMove = opponent->nbMove;
	nbMove = plSaveNbCapture + opSaveNbCapture;
	plSaveNbCapture = player->nbCapture;
	opSaveNbCapture = opponent->nbCapture;
	plSaveBitboard = *grid->getBitBoard(player->interType);
	opSaveBitboard = *grid->getBitBoard(opponent->interType);
	bboxManagerSave = *grid->getBboxManager();

	// Get moves
	if (maximizingEval)
		moves = grid->getInterestingMovesSorted(
						evaluator, player, opponent, true, tracker);
	else
		moves = grid->getInterestingMovesSorted(
						evaluator, opponent, player, false, tracker);

	current.eval = 0;
	current.pos = sf::Vector2i(-1, -1);
	if (moves.size() == 0)
	{
		if (nbMove == 0)
			current.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		return (current);
	}

	for (int i = 0; i < moves.size(); i++)
	{
		// Make move
		if (!makeMove(
				grid, player, opponent, &moves[i].pos,
				nbMove, depth, tracker))
			continue;

		// Compute evaluation for this position
		current = alphaBetaMemory(
					memoryMoves, memoryEval, grid,
					player, opponent, evaluator, !maximizingEval,
					-alpha - 1, -alpha, depth - 1, tracker);
		current.eval = -current.eval;

		if (current.eval > alpha && current.eval < beta)
		{
			current = alphaBetaMemory(
						memoryMoves, memoryEval, grid,
						player, opponent, evaluator, !maximizingEval,
						-beta, -alpha, depth - 1, tracker);
			current.eval = -current.eval;
		}
		current.pos = moves[i].pos;

		// Unmake move
		reverseMove(
			grid, player, opponent,
			&plSaveBitboard, &opSaveBitboard, &bboxManagerSave,
			plSaveNbCapture, opSaveNbCapture,
			plSaveNbMove, opSaveNbMove, &moves[i].pos, depth);

		// Update best eval and alpha
		if (current.eval >= bestMove.eval)
		{
			bestMove = current;

			if (bestMove.eval >= alpha)
				alpha = bestMove.eval;

			if (bestMove.eval >= beta)
				break;
		}
	}

	return (bestMove);
}


static bool	makeMove(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				sf::Vector2i *pos, int nbMove, int depth, Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	start = std::clock();
	if (!grid->putStoneAI(pos, nbMove, player, opponent))
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
				int plSaveNbCapture, int opSaveNbCapture,
				int plSaveNbMove, int opSaveNbMove,
				sf::Vector2i *pos, int depth)
{
	// Reset grid
	if (player->nbCapture != plSaveNbCapture
		|| opponent->nbCapture != opSaveNbCapture)
	{
		grid->setBitBoard(plBitBoard, player->interType);
		grid->setBitBoard(opBitBoard, opponent->interType);
	}
	else
		grid->removeStone(pos, player->interType);

	if (depth > 1)
		grid->setBboxManager(bboxManagerSave);

	// Reset player
	player->nbMove = plSaveNbMove;
	player->nbCapture = plSaveNbCapture;
	opponent->nbMove = opSaveNbMove;
	opponent->nbCapture = opSaveNbCapture;
}
