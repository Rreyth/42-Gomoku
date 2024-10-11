#include <AI/AI.hpp>
#include <utils/Functions.hpp>

// TODO: REMOVE
typedef struct s_lastMove
{
	struct s_lastMove	*lastMove;
	sf::Vector2i		move;
	Grid				grid;
}	LastMove;


static Move	pvs(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth, Tracker *tracker, LastMove *lastMove);
static bool	makeMove(
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
	LastMove	move;

	move.move = sf::Vector2i(-1, -1);
	move.lastMove = NULL;
	move.grid = *grid;

	bestMove = pvs(
				memoryMoves, memoryEval,
				grid, player, opponent, evaluator, true,
				-1000000001, 1000000001, AI_HARD_DEPTH,
				tracker, &move);

	return (bestMove.pos);
}


static Move	pvs(
				std::unordered_map<std::size_t, std::vector<Move>> *memoryMoves,
				std::unordered_map<std::size_t, int> *memoryEval,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
				int depth, Tracker *tracker, LastMove *lastMove)
{
	int					nbMove, plSaveNbCapture, opSaveNbCapture;
	std::size_t			hash;
	std::vector<Move>	moves;
	std::vector<Move>	movesTEST;
	Move				bestMove, tmpMove;
	BitBoard			saveBitboard, *plBitboard, *opBitboard;
	BboxManager			saveBboxManager;
	std::unordered_map<std::size_t, int>::const_iterator				evalFind;
	std::unordered_map<std::size_t, std::vector<Move>>::const_iterator	movesFind;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;
	std::size_t HASHTEST = 3616546876818078414;


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

	if (hash == HASHTEST)
	{
		printf("\ndepth %i - grid\n", depth);
		printf("Player : nbMove %i, nbCapture %i\n", player->nbMove, player->nbCapture);
		printf("Opponent : nbMove %i, nbCapture %i\n", opponent->nbMove, opponent->nbCapture);
		grid->print();

		LastMove	*tmpLastMove = lastMove;
		printf("GRID ORIGIN\n");
		tmpLastMove->grid.print();
		printf("Moves :\n");
		while(tmpLastMove->lastMove)
		{
			printf(" |%2i,%2i|\n", tmpLastMove->move.x, tmpLastMove->move.y);
			tmpLastMove = tmpLastMove->lastMove;
		}
		printf("\n");
	}

	// If moves is in memory, get moves from it
	if (movesFind != memoryMoves->end())
	{
		moves.insert(moves.begin(), movesFind->second.begin(), movesFind->second.end());

		if (playerTurn)
			grid->getInterestingMovesSorted(&movesTEST, evaluator, player, opponent, true, tracker);
		else
			grid->getInterestingMovesSorted(&movesTEST, evaluator, opponent, player, true, tracker);

		// if (moves.size() != movesTEST.size())
		// 	printf(" hash %lu\n", hash);

		if (hash == HASHTEST)
		{

			printf(" equality compute - memory\n");
			printf(" hash %lu\n", hash);
			printf(" size %lu != %lu\n",
					moves.size(), movesTEST.size());

			for (int i = 0; i < max(moves.size(), movesTEST.size()); i++)
			{
				if (i < moves.size())
					printf("%2i,%2i-%6i", moves[i].pos.x, moves[i].pos.y, moves[i].eval);
				else
					printf("  ,  -      ");
				printf(" | ");
				if (i < movesTEST.size())
					printf("%2i,%2i-%6i", movesTEST[i].pos.x, movesTEST[i].pos.y, movesTEST[i].eval);
				printf("\n");
			}
		}
	}
	// Else compute moves and put them in memory
	else
	{
		if (playerTurn)
			grid->getInterestingMovesSorted(&moves, evaluator, player, opponent, true, tracker);
		else
			grid->getInterestingMovesSorted(&moves, evaluator, opponent, player, true, tracker);

		if (hash == HASHTEST)
		{
			printf("Compute list : %lu\n", moves.size());
			for (int i = 0; i < moves.size(); i++)
				printf("%2i,%2i-%6i\n", moves[i].pos.x, moves[i].pos.y, moves[i].eval);
		}

		memoryMoves->insert(std::pair<std::size_t, std::vector<Move>>(hash, moves));

		if (hash == HASHTEST)
		{
			movesFind = memoryMoves->find(hash);

			if (movesFind == memoryMoves->end())
			{
				printf("No moves in memory ?!\n");
			}
			else
			{
				movesTEST = movesFind->second;
				printf("Memory list :\n");
				for (int i = 0; i < movesTEST.size(); i++)
					printf(" [%i, %i] => %i\n", movesTEST[i].pos.x, movesTEST[i].pos.y, movesTEST[i].eval);
			}
		}
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
		if (!makeMove(
				grid, &moves[i].pos, nbMove, depth, player, opponent, playerTurn,
				tracker))
			continue; // Move is invalid, go to the next one

		// Check if move is a killer move
		tmpMove.eval = checkKillerMove(
							grid, playerTurn, moves[i].eval, player, opponent,
							tracker);

		// Check if this move isn't a killer one. In this case, the eval is 0
		// A killer move is the end of the game, so don't contine recursion
		if (tmpMove.eval == 0)
		{
			// TODO: REMOVE
			LastMove	newLastMove;
			newLastMove.move = moves[i].pos;
			newLastMove.lastMove = lastMove;
			newLastMove.grid = lastMove->grid;

			// For first child, do a classic recursion
			if (i == 0)
			{
				// Call pvs to next player reply score
				tmpMove = pvs(
							memoryMoves, memoryEval,
							grid, player, opponent, evaluator, !playerTurn,
							-beta, -alpha, depth - 1, tracker, &newLastMove);
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
							-alpha - 1, -alpha, depth - 1, tracker, &newLastMove);
				tmpMove.eval = -tmpMove.eval;

				// If the 0-window search fail
				if (alpha < tmpMove.eval && tmpMove.eval < beta)
				{
					// Do a classic one
					tmpMove = pvs(
								memoryMoves, memoryEval,
								grid, player, opponent, evaluator, !playerTurn,
								-beta, -alpha, depth - 1, tracker, &newLastMove);
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


static bool	makeMove(
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
		if (!grid->putStoneAI(move, nbMove, player, opponent))
		{
			// printf("Nope %i %i\n", move->x, move->y);
			return (false);
		}
		player->nbMove++;
	}
	else
	{
		if (!grid->putStoneAI(move, nbMove, opponent, player))
		{
			// printf("Nope %i %i\n", move->x, move->y);
			return (false);
		}
		opponent->nbMove++;
	}
	if (depth > 1)
		grid->updateBboxManager(move);

	diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
	tracker->putStoneTime += diff;
	tracker->checkStoneNumber++;
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
