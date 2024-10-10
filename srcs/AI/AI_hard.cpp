#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	pvs(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
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
static int	checkKillerMove(
				Grid *grid, bool playerTurn, int moveEvaluation,
				PlayerInfo *player, PlayerInfo *opponent,
				Tracker *tracker);

sf::Vector2i	getHardMove(
					std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
					std::unordered_map<std::size_t, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	Move	bestMove;

	bestMove = pvs(
				memoryMoves, memoryEval,
				grid, player, opponent, evaluator, true,
				-1000000001, 1000000001, AI_HARD_DEPTH,
				tracker);

	return (bestMove.pos);
}


static Move	pvs(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth, Tracker *tracker)
{
	int					nbMove, plSaveNbCapture, opSaveNbCapture, hash;
	std::vector<Move>	moves;
	Move				bestMove, tmpMove;
	BitBoard			saveBitboard, *plBitboard, *opBitboard;
	BboxManager			saveBboxManager;
	std::unordered_map<std::size_t, int>::const_iterator				evalFind;
	std::unordered_map<std::size_t, std::vector<Move>>::const_iterator	movesFind;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	plBitboard = grid->getBitBoard(player->interType);
	opBitboard = grid->getBitBoard(opponent->interType);
	// Compute hash of the current grid
	hash = plBitboard->getHash(opBitboard);

	if (depth <= 0)
	{
		start = std::clock();
		tmpMove.pos = sf::Vector2i(-1, -1);

		// Check in memory if we have already compute the evaluation
		evalFind = memoryEval->find(hash);

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->evaluationTime += diff;
		tracker->nbEvaluations++;

		// If the evaluate have already be compute, get it's value from saved one
		if (evalFind != memoryEval->end())
		{
			tmpMove.eval = evalFind->second;
			tracker->nbMemoryEval++;
		}
		// Else compute the evaluation and put it in memory
		else
		{
			start = std::clock();

			// If this is player turn, evaluate position from it's view point
			if (playerTurn)
				tmpMove.eval = evaluator->evaluateGrid(
											plBitboard,
											opBitboard,
											player->nbCapture, opponent->nbCapture);
			// If this is opponent turn, evaluate position from it's view point
			else
				tmpMove.eval = evaluator->evaluateGrid(
											opBitboard,
											plBitboard,
											opponent->nbCapture, player->nbCapture);

			// Put the result in memory to avoid to recompute the evaluation for this board
			memoryEval->insert(std::pair<std::size_t, int>(hash, tmpMove.eval));

			diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
			tracker->computeEvalTime += diff;
			tracker->nbComputeEval++;

			tracker->evaluationTime += diff;
		}

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
	movesFind = memoryMoves->find(hash);

	// If moves is in memory, get moves from it
	if (movesFind != memoryMoves->end())
	{
		moves = movesFind->second;
	}
	// Else compute moves and put them in memory
	else
	{
		if (playerTurn)
			moves = grid->getInterestingMovesSorted(evaluator, player, opponent, true, tracker);
		else
			moves = grid->getInterestingMovesSorted(evaluator, opponent, player, true, tracker);

		memoryMoves->insert(std::pair<std::size_t, std::vector<Move>>(hash, moves));
	}

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
	for (int i = 0; i < moves.size() && i < AI_HARD_LIMIT; i++)
	{
		// Make the move
		makeMove(
			grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn,
			tracker);

		// Check if move is a killer move
		tmpMove.eval = checkKillerMove(
							grid, playerTurn, moves[i].eval, player, opponent,
							tracker);

		// Check if this move isn't a killer one. In this case, the eval is 0
		// A killer move is the end of the game, so don't contine recursion
		if (tmpMove.eval == 0)
		{
			// For first child, do a classic recursion
			if (i == 0)
			{
				// Call pvs to next player reply score
				tmpMove = pvs(
							memoryMoves, memoryEval,
							grid, player, opponent, evaluator, !playerTurn,
							-beta, -alpha, depth - 1, tracker);
				// Reverse evaluation because a good opponent move is
				// a bad move for player
				tmpMove.eval = -tmpMove.eval;
			}
			else
			{
				// Do a first 0-window search
				tmpMove = pvs(
							memoryMoves, memoryEval,
							grid, player, opponent, evaluator, !playerTurn,
							-alpha - 1, -alpha, depth - 1, tracker);
				tmpMove.eval = -tmpMove.eval;

				// If the 0-window search fail
				if (alpha < tmpMove.eval && tmpMove.eval < beta)
				{
					// Do a classic one
					tmpMove = pvs(
								memoryMoves, memoryEval,
								grid, player, opponent, evaluator, !playerTurn,
								-beta, -alpha, depth - 1, tracker);
					tmpMove.eval = -tmpMove.eval;
				}
			}
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


static int	checkKillerMove(
				Grid *grid, bool playerTurn, int moveEvaluation,
				PlayerInfo *player, PlayerInfo *opponent,
				Tracker *tracker)
{
	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	int				score; // Remove it and return instant

	// Use the evaluation of position to limit the number of check win
	if (moveEvaluation < 10000000)
		return (0);

	score = 0;
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
				score = CASE_WIN_POINT;
			}
			// Opponent win from player view point = bad
			else
			{
				opponent->winState = WIN_STATE_NONE;
				score = CASE_LOOSE_POINT;
			}
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
				score = CASE_LOOSE_POINT;
			}
			// Opponent win from opponent view point = good
			else
			{
				opponent->winState = WIN_STATE_NONE;
				score = CASE_WIN_POINT;
			}
		}

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->checkWinTime += diff;
		tracker->checkWinNumber++;
	}

	return (score);
}
