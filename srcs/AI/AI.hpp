#ifndef AI_HPP
# define AI_HPP

# include <define.hpp>
# include <game/Grid.hpp>
# include <game/BoardState.hpp>
# include <game/PlayerInfo.hpp>
# include <game/Node.hpp>
# include <AI/Evaluation.hpp>

# include <functional>
# include <unordered_map>
# include <thread>
# include <mutex>

typedef struct	s_ParallelRun
{
	int				timeLastMove;
	bool			needCompute;
	bool			computeDone;
	bool			running;
	sf::Vector2i	move;
}	ParallelRun;


typedef struct	s_threadParams
{
	ParallelRun		*parallelRun;
	std::mutex		*mutex;
	AI_difficulty	aiDifficulty;
	Grid			*grid;
	PlayerInfo		*player;
	PlayerInfo		*opponent;
}	ThreadParams;


void			aiThreadCore(ThreadParams *threadParams);
sf::Vector2i	getRandomMove(
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent);
sf::Vector2i	getBetterRandom(
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent);
sf::Vector2i	getEasyMove(
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator);
sf::Vector2i	getMediumMove(
					std::unordered_map<int, std::vector<Move>> *memoryMoves,
					std::unordered_map<int, int> *memoryEval,
					Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
					Evaluation *evaluator, Tracker *tracker);
sf::Vector2i	getHardMove(
					Node *rootNode, Evaluation *evaluator, Tracker *tracker);

class AI
{
public:
	AI(void);
	~AI();

	void			setDifficulty(AI_difficulty difficulty);
	AI_difficulty	getDifficulty(void);

	int				getTimer(void);

	void			setAI(
						Grid *grid, AI_difficulty difficulty,
						PlayerInfo *player, PlayerInfo *opponent);

	void			startTurn(void);
	sf::Vector2i	getNextMove(bool *moveDone);
	void			reset(Grid *grid, PlayerInfo *player, PlayerInfo *opponent);

private:
	int				timer;
	AI_difficulty	difficulty;
	std::thread		thread;
	std::mutex		mutex;
	ParallelRun		parallelRun;
	ThreadParams	threadParams;

	void	startThread(
				Grid *grid, PlayerInfo *player, PlayerInfo *opponent);
	void	destroyThread(void);
};

#endif
