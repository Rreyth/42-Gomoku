#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static Move	pvs(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
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

sf::Vector2i	getHardMove(
					std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
					std::unordered_map<std::size_t, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator)
{
	Move	bestMove;

	bestMove = pvs(
				memoryMoves, memoryEval,
				grid, player, opponent, evaluator, true,
				-1000000001, 1000000001, AI_HARD_DEPTH);

	return (bestMove.pos);
}


static Move	pvs(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth)
{
	int					nbMove, plSaveNbCapture, opSaveNbCapture;
	std::size_t			hash;
	std::vector<Move>	moves;
	Move				bestMove, tmpMove;
	BitBoard			saveBitboard, *plBitboard, *opBitboard;
	BboxManager			saveBboxManager;
	std::unordered_map<std::size_t, int>::const_iterator				evalFind;
	std::unordered_map<std::size_t, std::vector<Move>>::const_iterator	movesFind;

	plBitboard = grid->getBitBoard(player->interType);
	opBitboard = grid->getBitBoard(opponent->interType);
	// Compute hash of the current grid
	hash = plBitboard->getHash(opBitboard);

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

	bestMove.eval = -2000000001;
	bestMove.pos = sf::Vector2i(-1, -1);
	for (int i = 0; i < moves.size() && i < AI_HARD_LIMIT; i++)
	{
		// Make the move
		if (!makeMove(
				grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn))
			continue; // Move is invalid, go to the next one

		// Check if this move isn't a killer one.
		// In this case, the eval is less than 1000000000
		// A killer move is the end of the game, so don't contine recursion
		if (moves[i].eval < CASE_WIN_POINT)
		{
			// For first child, do a classic recursion
			if (i == 0)
			{
				// Call pvs to next player reply score
				tmpMove = pvs(
							memoryMoves, memoryEval,
							grid, player, opponent, evaluator, !playerTurn,
							-beta, -alpha, depth - 1);
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
							-alpha - 1, -alpha, depth - 1);
				tmpMove.eval = -tmpMove.eval;

				// If the 0-window search fail
				if (alpha < tmpMove.eval && tmpMove.eval < beta)
				{
					// Do a classic one
					tmpMove = pvs(
								memoryMoves, memoryEval,
								grid, player, opponent, evaluator, !playerTurn,
								-beta, -alpha, depth - 1);
					tmpMove.eval = -tmpMove.eval;
				}
			}
		}
		else
			tmpMove.eval = moves[i].eval;

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

		// If the move is too good, opponent will take this move
		if (alpha >= beta)
			break;
	}

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
