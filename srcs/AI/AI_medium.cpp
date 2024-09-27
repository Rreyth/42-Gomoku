#include <AI/AI.hpp>
#include <utils/Functions.hpp>

static int	miniMax(
				std::unordered_map<int, std::vector<Move>> *memory,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker);


sf::Vector2i	getMediumMove(
					std::unordered_map<int, std::vector<Move>> *memory,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	int					bestEval, tmpEval,
						plCapture, opCapture,
						plMoves, opMoves, nbMoves;
	inter_type			plType, opType;
	std::vector<Move>	moves;
	sf::Vector2i		bestMove;
	BitBoard			plBitBoard, opBitBoard;
	BboxManager			bboxManagerSave;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// Compute variables
	plType = player->interType;
	opType = opponent->interType;
	plCapture = player->nbCapture;
	opCapture = opponent->nbCapture;
	plMoves = player->nbMove;
	opMoves = opponent->nbMove;
	nbMoves = plMoves + opMoves;
	plBitBoard = *grid->getBitBoard(plType);
	opBitBoard = *grid->getBitBoard(opType);
	bboxManagerSave = *grid->getBboxManager();

	// Get interesting moves
	moves = grid->getInterestingMovesSorted(
					evaluator, player, opponent, true, tracker);
	if (moves.size() == 0)
	{
		if (nbMoves == 0)
			return (sf::Vector2i(GRID_W_INTER / 2, GRID_W_INTER / 2));
		return (sf::Vector2i(-1, -1));
	}

	// Find move
	bestEval = -1000000001;
	bestMove = sf::Vector2i(-1, -1);
	for (int i = 0; i < moves.size() && i < AI_MEDIUM_LIMIT; i++)
	{
		// Simulate move
		if (!grid->putStone(&moves[i].pos, nbMoves, player, opponent))
			continue;
		player->nbMove++;

		// Check win condition
		if (grid->checkWinCondition(player, opponent))
		{
			if (player->winState != WIN_STATE_NONE)
			{
				// Reset grid
				if (player->nbCapture != plCapture
					|| opponent->nbCapture != opCapture)
				{
					grid->setBitBoard(&plBitBoard, plType);
					grid->setBitBoard(&opBitBoard, opType);
				}
				else
					grid->removeStone(&moves[i].pos, plType);

				// Reset player
				player->nbMove = plMoves;
				player->nbCapture = plCapture;
				opponent->nbMove = opMoves;
				opponent->nbCapture = opCapture;
				player->winState = WIN_STATE_NONE;

				return (moves[i].pos);
			}

			opponent->winState = WIN_STATE_NONE;
			tmpEval = CASE_LOOSE_POINT;
		}
		else
		{
			// Get evaluation for this move
			tmpEval = miniMax(memory, grid,
								player, opponent, evaluator,
								false, -1000000001, 1000000001,
								AI_MEDIUM_DEPTH - 1, tracker);
		}

		// Update score
		if (tmpEval > bestEval)
		{
			bestEval = tmpEval;
			bestMove = moves[i].pos;
		}

		// Reset grid
		if (player->nbCapture != plCapture
			|| opponent->nbCapture != opCapture)
		{
			grid->setBitBoard(&plBitBoard, plType);
			grid->setBitBoard(&opBitBoard, opType);
		}
		else
			grid->removeStone(&moves[i].pos, plType);
		grid->setBboxManager(&bboxManagerSave);

		// Reset player
		player->nbMove = plMoves;
		player->nbCapture = plCapture;
		opponent->nbMove = opMoves;
		opponent->nbCapture = opCapture;

		if (bestEval >= CASE_WIN_POINT)
			return(bestMove);
	}

	return (bestMove);
}


static int	miniMax(
				std::unordered_map<int, std::vector<Move>> *memory,
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
				Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
				int depth, Tracker *tracker)
{
	int					bestEval, tmpEval,
						plCapture, opCapture,
						plMoves, opMoves, nbMoves;
	bool				killerMove;
	inter_type			plType, opType;
	std::vector<Move>	moves;
	BitBoard			plBitBoard, opBitBoard;
	BboxManager			bboxManagerSave;
	BoardState			boardState;

	// TODO : REMOVE
	std::clock_t	start;
	int				diff;

	// Compute variables for evaluation and put stone
	plType = player->interType;
	opType = opponent->interType;
	plCapture = player->nbCapture;
	opCapture = opponent->nbCapture;

	// Stop recursion and return the grid evaluation
	if (depth <= 0)
	{
		tracker->nbEvaluations++;
		start = std::clock();

		tmpEval = evaluator->evaluateGrid(
								grid->getBitBoard(plType),
								grid->getBitBoard(opType),
								plCapture, opCapture);

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->evaluationTime += diff;

		return (tmpEval);
	}

	plBitBoard = *grid->getBitBoard(plType);
	opBitBoard = *grid->getBitBoard(opType);
	boardState = BoardState(&plBitBoard, &opBitBoard);
	int	hash = boardState.getHash();

	try
	{
		moves = memory->at(hash);
	}
	catch (std::exception &e)
	{
		// Get interesting moves
		if (maximizingEval)
			moves = grid->getInterestingMovesSorted(
							evaluator, player, opponent, true, tracker);
		else
			moves = grid->getInterestingMovesSorted(
							evaluator, opponent, player, false, tracker);
		memory->insert(std::pair<int, std::vector<Move>>(hash, moves));
	}

	if (moves.size() == 0)
		return (0);

	// Compute variables put stone and reset players
	plMoves = player->nbMove;
	opMoves = opponent->nbMove;
	nbMoves = plMoves + opMoves;
	bboxManagerSave = *grid->getBboxManager();

	// Find move
	if (maximizingEval)
		bestEval = -1000000001;
	else
		bestEval = 1000000001;

	for (int i = 0; i < moves.size() && i < AI_MEDIUM_LIMIT; i++)
	{
		tracker->nbEvaluations++;
		start = std::clock();

		// Simulate move
		if (maximizingEval)
		{
			if (!grid->putStoneAI(&moves[i].pos, nbMoves, player, opponent))
				continue;
			if (depth > 1)
				grid->updateBboxManager(&moves[i].pos);
			player->nbMove++;
		}
		else
		{
			if (!grid->putStoneAI(&moves[i].pos, nbMoves, opponent, player))
				continue;
			if (depth > 1)
				grid->updateBboxManager(&moves[i].pos);
			opponent->nbMove++;
		}

		diff = ((double)(std::clock() - start) / CLOCKS_PER_SEC) * 1000000;
		tracker->putStoneTime += diff;
		tracker->checkStoneNumber++;

		// If move have an high score, check win case
		if (moves[i].eval >= 10000000)
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
				tmpEval = CASE_WIN_POINT; // Nothing is better than a victory
			}
			else
			{
				opponent->winState = WIN_STATE_NONE;
				tmpEval = CASE_LOOSE_POINT; // Nothing is worst than a defeat
			}
		}
		else
		{
			// Get evaluation for this move
			tmpEval = miniMax(memory, grid,
								player, opponent, evaluator,
								!maximizingEval, alpha, beta,
								depth - 1, tracker);
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
				grid->removeStone(&moves[i].pos, plType);
			else
				grid->removeStone(&moves[i].pos, opType);
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
			if (tmpEval > bestEval)
				bestEval = tmpEval;

			// Update alpha
			if (tmpEval > alpha)
				alpha = tmpEval;
		}
		// For minimizing
		else
		{
			// Update score
			if (tmpEval < bestEval)
				bestEval = tmpEval;

			// Update beta
			if (tmpEval < beta)
				beta = tmpEval;
		}

		// If we already have already better move, stop searching by pruning
		if (beta <= alpha)
			break;
	}
	
	return (bestEval);
}