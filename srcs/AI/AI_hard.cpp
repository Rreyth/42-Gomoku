#include <AI/AI.hpp>
#include <utils/Functions.hpp>

// static Move	negaMax(
// 				std::unordered_map<int, std::vector<Move>> *memoryMoves,
// 				std::unordered_map<int, int> *memoryEval,
// 				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
// 				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
// 				int depth, Tracker *tracker);


sf::Vector2i	getHardMove(
					std::unordered_map<int, std::vector<Move>> *memoryMoves,
					std::unordered_map<int, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker)
{
	Move	bestMove;

	// bestMove = negaMax(memoryMoves, memoryEval, grid,
	// 							player, opponent, evaluator,
	// 							true, -1000000001, 1000000001,
	// 							AI_MEDIUM_DEPTH, tracker);

	return (bestMove.pos);
}


// static Move	negaMax(
// 				std::unordered_map<int, std::vector<Move>> *memoryMoves,
// 				std::unordered_map<int, int> *memoryEval,
// 				Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
// 				Evaluation *evaluator, bool playerTurn, int alpha, int beta,
// 				int depth, Tracker *tracker)
// {
// 	return (Move());
// }
