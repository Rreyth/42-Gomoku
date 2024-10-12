#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	mtdf(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				std::unordered_map<std::size_t, std::pair<Move, Move>> *memoryBounds,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int depth, Move *firstGuess);
static Move	alphaBetaWithMemory(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				std::unordered_map<std::size_t, std::pair<Move, Move>> *memoryBounds,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth);
static bool	makeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn);
static void	unmakeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn,
				BitBoard *saveBitboard, BboxManager *saveBboxManager,
				int plSaveNbCapture, int opSaveNbCapture);
static int	checkKillerMove(
				Grid *grid, bool playerTurn, int moveEvaluation,
				PlayerInfo *player, PlayerInfo *opponent);

sf::Vector2i	getMTDFMove(
					std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
					std::unordered_map<std::size_t, int> *memoryEval,
					std::unordered_map<std::size_t, std::pair<Move, Move>> *memoryBounds,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator)
{
	Move	bestMove;

	bestMove.eval = 0;
	bestMove.pos = sf::Vector2i(-1, -1);
	for (int d = 1; d <= AI_HARD_DEPTH; d++)
	{
		bestMove = mtdf(
					memoryMoves, memoryEval, memoryBounds,
					grid, player, opponent, evaluator,
					d, &bestMove);
	}

	return (bestMove.pos);
}


static Move	mtdf(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				std::unordered_map<std::size_t, std::pair<Move, Move>> *memoryBounds,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, int depth, Move *firstGuess)
{
	Move	bestMove;
	int		upperBound, lowerBound, beta;

	bestMove = *firstGuess;
	upperBound = 1000000001;
	lowerBound = -1000000001;
	while (lowerBound < upperBound)
	{
		if (bestMove.eval == lowerBound)
			beta = bestMove.eval + 1;
		else
			beta = bestMove.eval;

		bestMove = alphaBetaWithMemory(
					memoryMoves, memoryEval, memoryBounds,
					grid, player, opponent,
					evaluator, true, beta - 1, beta, depth);

		if (bestMove.eval < beta)
			upperBound = bestMove.eval;
		else
			lowerBound = bestMove.eval;
	}

	return (bestMove);
}


static Move	alphaBetaWithMemory(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				std::unordered_map<std::size_t, std::pair<Move, Move>> *memoryBounds,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth)
{
	bool				boundsInMemory;
	int					nbMove, plSaveNbCapture, opSaveNbCapture;
	std::size_t			hash;
	std::vector<Move>	moves;
	Move				bestMove, tmpMove, lowerBound, upperBound;
	BitBoard			saveBitboard, *plBitboard, *opBitboard;
	BboxManager			saveBboxManager;
	std::unordered_map<std::size_t, int>::const_iterator					evalFind;
	std::unordered_map<std::size_t, std::vector<Move>>::const_iterator		movesFind;
	std::unordered_map<std::size_t, std::pair<Move, Move>>::const_iterator	boundsFind;

	plBitboard = grid->getBitBoard(player->interType);
	opBitboard = grid->getBitBoard(opponent->interType);
	// Compute hash of the current grid
	hash = plBitboard->getHash(opBitboard);

	// Check if there is bounds in memory
	boundsFind = memoryBounds->find(hash);
	boundsInMemory = (boundsFind != memoryBounds->end());

	if (boundsInMemory)
	{
		lowerBound = boundsFind->second.first;
		upperBound = boundsFind->second.second;

		// If yes, check if we can avoid computation
		if (lowerBound.eval >= beta)
			return (lowerBound);
		if (upperBound.eval <= alpha)
			return (upperBound);

		// If we can avoid computation,
		alpha = max(alpha, lowerBound.eval);
		beta = min(beta, upperBound.eval);
	}

	if (depth <= 0)
	{
		tmpMove.pos = sf::Vector2i(-1, -1);

		// Check in memory if we have already compute the evaluation
		evalFind = memoryEval->find(hash);

		// If the evaluate have already be compute, get it's value from saved one
		if (evalFind != memoryEval->end())
			tmpMove.eval = evalFind->second;
		// Else compute the evaluation and put it in memory
		else
		{
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
		moves.insert(moves.begin(), movesFind->second.begin(), movesFind->second.end());
	// Else compute moves and put them in memory
	else
	{
		if (playerTurn)
			grid->getInterestingMovesSorted(&moves, evaluator, player, opponent, true);
		else
			grid->getInterestingMovesSorted(&moves, evaluator, opponent, player, true);

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

	// Init best move
	if (playerTurn)
		bestMove.eval = -1000000001;
	else
		bestMove.eval = 1000000001;
	bestMove.pos = sf::Vector2i(-1, -1);

	// Find best move
	for (int i = 0; i < moves.size(); i++)
	{
		// Make the move
		if (!makeMove(
				grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn))
			continue; // Move is invalid, go to the next one

		// Check if move is a killer move
		tmpMove.eval = checkKillerMove(
							grid, playerTurn, moves[i].eval, player, opponent);

		// Check if this move isn't a killer one. In this case, the eval is 0
		// A killer move is the end of the game, so don't contine recursion
		if (tmpMove.eval == 0)
		{
			// Call pvs to next player reply score
			tmpMove = alphaBetaWithMemory(
						memoryMoves, memoryEval, memoryBounds,
						grid, player, opponent, evaluator, !playerTurn,
						alpha, beta, depth - 1);
		}

		// Reverse the move
		unmakeMove(
			grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn,
			&saveBitboard, &saveBboxManager,
			plSaveNbCapture, opSaveNbCapture);

		// Keep the best move encounter so far
		if (playerTurn)
		{
			if (bestMove.eval < tmpMove.eval)
			{
				bestMove.eval = tmpMove.eval;
				bestMove.pos = moves[i].pos;
			}

			// Update alpha
			if (alpha < tmpMove.eval)
				alpha = tmpMove.eval;
		}
		else if (!playerTurn)
		{
			if (bestMove.eval > tmpMove.eval)
			{
				bestMove.eval = tmpMove.eval;
				bestMove.pos = moves[i].pos;
			}

			// Update beta
			if (beta > tmpMove.eval)
				beta = tmpMove.eval;
		}
	}

	// Update or create lower and upper bounds
	if (bestMove.eval <= alpha)
	{
		upperBound = bestMove;
		if (boundsInMemory)
			lowerBound = boundsFind->second.first;
		else
		{
			lowerBound.eval = -1000000001;
			lowerBound.pos = sf::Vector2i(-1, -1);
		}
	}
	else if (bestMove.eval > alpha && bestMove.eval < beta)
	{
		upperBound = bestMove;
		lowerBound = bestMove;
	}
	else
	{
		lowerBound = bestMove;
		if (boundsInMemory)
			upperBound = boundsFind->second.second;
		else
		{
			upperBound.eval = 1000000001;
			upperBound.pos = sf::Vector2i(-1, -1);
		}
	}

	// Save bounds in memory
	if (boundsInMemory)
		memoryBounds->erase(hash);
	memoryBounds->insert(std::pair<std::size_t, std::pair<Move, Move>>(hash,
																		std::pair<Move, Move>(lowerBound, upperBound)));

	return (bestMove);
}

static bool	makeMove(
				Grid *grid, sf::Vector2i *move, int nbMove, int depth,
				PlayerInfo *player, PlayerInfo *opponent, bool playerTurn)
{
	if (playerTurn)
	{
		if (!grid->putStoneAI(move, nbMove, player, opponent))
			return (false);
		player->nbMove++;
	}
	else
	{
		if (!grid->putStoneAI(move, nbMove, opponent, player))
			return (false);
		opponent->nbMove++;
	}
	if (depth > 1)
		grid->updateBboxManager(move);

	return (true);
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
		opponent->nbMove--;
	}
	if (depth > 1)
		grid->setBboxManager(saveBboxManager);
}


static int	checkKillerMove(
				Grid *grid, bool playerTurn, int moveEvaluation,
				PlayerInfo *player, PlayerInfo *opponent)
{
	int	score; // Remove it and return instant

	// Use the evaluation of position to limit the number of check win
	if (moveEvaluation < 10000000)
		return (0);

	score = 0;
	if (playerTurn)
	{
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
	}
	else
	{
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
	}

	return (score);
}
