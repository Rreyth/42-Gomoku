#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	miniMax(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker);

static Move	negaMax(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker);
static void	makeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn,
				Tracker *tracker);
static void	unmakeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn,
				BitBoard *saveBitboard, BboxManager *saveBboxManager,
				int plSaveNbCapture, int opSaveNbCapture);
static bool	checkKillerMove(
				Grid *grid, bool playerTurn, Move *tmpMove,
				PlayerInfo *player, PlayerInfo *opponent,
				Tracker *tracker);

sf::Vector2i	getMediumMove(
					std::unordered_map<int, std::vector<Move>> *memoryMoves,
					std::unordered_map<int, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	Move	bestMove;

	bestMove = negaMax(memoryMoves, memoryEval, grid,
							player, opponent, evaluator,
							true, -1000000001, 1000000001,
							4, tracker);

	return (bestMove.pos);
}


static Move	negaMax(
				std::unordered_map<int, std::vector<Move>> *memoryMoves,
				std::unordered_map<int, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth, Tracker *tracker)
{
	bool				killerMove;
	int					nbMove, plSaveNbCapture, opSaveNbCapture, hash;
	std::vector<Move>	moves;
	Move				bestMove, tmpMove;
	BitBoard			saveBitboard, *plBitboard, *opBitboard;
	BboxManager			saveBboxManager;
	BoardState			boardState;
	// std::unordered_map<int, int>::const_iterator	evalFind;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// plBitboard = grid->getBitBoard(player->interType);
	// opBitboard = grid->getBitBoard(opponent->interType);
	// Compute hash of the current grid
	// boardState = BoardState(plBitboard, opBitboard);
	// hash = boardState.getHash();

	if (depth <= 0)
	{
		// start = std::clock();
		// tmpMove.pos = sf::Vector2i(-1, -1);

		// // Check in memory if we have already compute the evaluation
		// evalFind = memoryEval->find(hash);

		// diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		// tracker->evaluationTime += diff;
		tracker->nbEvaluations++;

		// If the evaluate have already be compute, get it's value from saved one
		// if (evalFind != memoryEval->end())
		// {
		// 	tmpMove.eval = evalFind->second;
		// 	tracker->nbMemoryEval++;
		// }
		// // Else compute the evaluation and put it in memory
		// else
		// {
		start = std::clock();

		// If this is player turn, evaluate position from it's view point
		if (playerTurn)
			tmpMove.eval = evaluator->evaluateGrid(
										grid->getBitBoard(player->interType),
										grid->getBitBoard(opponent->interType),
										player->nbCapture, opponent->nbCapture);
		// If this is opponent turn, evaluate position from it's view point
		else
			tmpMove.eval = evaluator->evaluateGrid(
										grid->getBitBoard(opponent->interType),
										grid->getBitBoard(player->interType),
										opponent->nbCapture, player->nbCapture);

		// Put the result in memory to avoid to recompute the evaluation for this board
		// memoryEval->insert(std::pair<int, int>(hash, tmpMove.eval));

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->computeEvalTime += diff;
		tracker->nbComputeEval++;

		tracker->evaluationTime += diff;
		// }

		return (tmpMove);
	}

	// Compute variables
	nbMove = player->nbMove + opponent->nbMove;
	plSaveNbCapture = player->nbCapture;
	opSaveNbCapture = opponent->nbCapture;
	// If this is player turn, save only oppenent bitboard for capture case
	if (playerTurn)
		saveBitboard = *grid->getBitBoard(opponent->interType);
	else
		saveBitboard = *grid->getBitBoard(player->interType);
	if (depth > 1)
		saveBboxManager = *grid->getBboxManager();

	// Get possible moves
	if (playerTurn)
		moves = grid->getInterestingMovesSorted(evaluator, player, opponent, true, tracker);
	else
		moves = grid->getInterestingMovesSorted(evaluator, opponent, player, true, tracker);

	// If there is no move, 2 case possible
	if (moves.size() == 0)
	{
		tmpMove.eval = 0;
		// This is the first move, play at center
		if (nbMove == 0)
			tmpMove.pos = sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2);
		// This is end game, return an error
		else
			tmpMove.pos = sf::Vector2i(-1, -1);
		return (tmpMove);
	}

	bestMove.eval = -1000000001;
	bestMove.pos = sf::Vector2i(-1, -1);
	for (int i = 0; i < moves.size(); i++)
	{
		// Make the move
		makeMove(
			grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn,
			tracker);

		// Check if move is a killer move
		killerMove = checkKillerMove(
						grid, playerTurn, &tmpMove, player, opponent,
						tracker);

		// Check if this move isn't a killer one
		// A killer move is the end of the game, so don't contine recursion
		if (!killerMove)
		{
			// Call negaMax to next player reply score
			tmpMove = negaMax(
						memoryMoves, memoryEval,
						grid, player, opponent, evaluator, !playerTurn,
						-beta, -alpha, depth - 1, tracker);
			// Reverse evaluation because a good opponent move is
			// a bad move for player
			tmpMove.eval = -tmpMove.eval;
		}

		// Reverse the move
		unmakeMove(
			grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn,
			&saveBitboard, &saveBboxManager,
			plSaveNbCapture, opSaveNbCapture);

		// Keep the best move encounter so far
		if (tmpMove.eval > bestMove.eval)
		{
			bestMove.eval = tmpMove.eval;
			bestMove.pos = moves[i].pos;
		}

		// Update alpha
		if (tmpMove.eval > alpha)
			alpha = tmpMove.eval;

		// If the move is too good, opponent will avoir this move
		if (alpha >= beta)
			break;
	}

	return (bestMove);
}


static void	makeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn,
				Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	start = std::clock();

	if (playerTurn)
	{
		grid->putStoneAI(move, nbMove, player, opponent);
		if (depth > 1)
			grid->updateBboxManager(move);
		player->nbMove++;
	}
	else
	{
		grid->putStoneAI(move, nbMove, opponent, player);
		if (depth > 1)
			grid->updateBboxManager(move);
		opponent->nbMove++;
	}

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->putStoneTime += diff;
	tracker->checkStoneNumber++;
}


static void	unmakeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn,
				BitBoard *saveBitboard, BboxManager *saveBboxManager,
				int plSaveNbCapture, int opSaveNbCapture)
{
	if (playerTurn)
	{
		// Remove the stone of player
		grid->removeStone(move, player->interType);

		// If the move make a capture
		if (player->nbCapture != plSaveNbCapture)
		{
			// Reset opponent bitboard
			grid->setBitBoard(saveBitboard, opponent->interType);
			player->nbCapture = plSaveNbCapture;
		}

		if (depth > 1)
			grid->setBboxManager(saveBboxManager);
		player->nbMove--;
	}
	else
	{
		// Remove the stone of player
		grid->removeStone(move, opponent->interType);

		// If the move make a capture
		if (opponent->nbCapture != opSaveNbCapture)
		{
			// Reset player bitboard
			grid->setBitBoard(saveBitboard, player->interType);
			opponent->nbCapture = opSaveNbCapture;
		}

		if (depth > 1)
			grid->setBboxManager(saveBboxManager);
		opponent->nbMove--;
	}
}


static bool	checkKillerMove(
				Grid *grid, bool playerTurn, Move *tmpMove,
				PlayerInfo *player, PlayerInfo *opponent,
				Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// Use the evaluation of position to limit the number of check win
	if (tmpMove->eval < 10000000)
		return (false);

	if (playerTurn)
	{
		start = std::clock();

		// Check if player move is a killer move
		if (grid->checkWinCondition(player, opponent))
		{
			// Player win from player view point = good
			if (player->winState != WIN_STATE_NONE)
			{
				player->winState = WIN_STATE_NONE;
				tmpMove->eval = CASE_WIN_POINT;
			}
			// Opponent win from player view point = bad
			else
			{
				opponent->winState = WIN_STATE_NONE;
				tmpMove->eval = CASE_LOOSE_POINT;
			}

			return (true);
		}

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->checkWinTime += diff;
		tracker->checkWinNumber++;
	}
	else
	{
		start = std::clock();

		// Check if opponent move is a killer move
		if (grid->checkWinCondition(opponent, player))
		{
			// Player win from opponent view point = bad
			if (player->winState != WIN_STATE_NONE)
			{
				player->winState = WIN_STATE_NONE;
				tmpMove->eval = CASE_LOOSE_POINT;
			}
			// Opponent win from opponent view point = good
			else
			{
				opponent->winState = WIN_STATE_NONE;
				tmpMove->eval = CASE_WIN_POINT;
			}

			return (true);
		}

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->checkWinTime += diff;
		tracker->checkWinNumber++;
	}

	return (false);
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

	for (int i = 0; i < moves.size() && i < AI_MEDIUM_LIMIT; i++)
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
