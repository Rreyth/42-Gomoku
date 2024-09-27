#ifndef AI_HPP
# define AI_HPP

# include <define.hpp>
# include <game/Grid.hpp>
# include <game/BoardState.hpp>
# include <AI/Evaluation.hpp>
# include <game/PlayerInfo.hpp>

# include <functional>
# include <unordered_map>


// typedef struct s_ParallelRun
// {
// 	bool			needCompute;
// 	bool			computeDone;
// 	bool			running;
// 	sf::Vector2i	move;
// }	ParallelRun;


class AI
{
public:
	AI(void);
	~AI();

	AI_difficulty	getDifficulty(void);
	void			setDifficulty(AI_difficulty difficulty);
	double			getTimer(void);
	void			setTimer(double timer);
	void			reset(void);

	void			setAI(AI_difficulty difficulty);

	sf::Vector2i	getNextMove(Grid *grid, PlayerInfo *player, PlayerInfo *opponent, Evaluation *evaluator);

private:
	double							timer;
	AI_difficulty					difficulty;
	std::unordered_map<int, std::vector<Move>>	memory;

	sf::Vector2i	getRandomMove(Grid *grid, PlayerInfo *player, PlayerInfo *opponent);
	sf::Vector2i	getBetterRandom(Grid *grid, PlayerInfo *player, PlayerInfo *opponent);
	sf::Vector2i	getEasyMove(Grid *grid, PlayerInfo *player, PlayerInfo *opponent, Evaluation *evaluator);
	int				mediumMiniMax(
						Grid *grid, PlayerInfo *player, PlayerInfo *opponent,
						Evaluation *evaluator, bool maximizingEval, int alpha, int beta,
						int depth, Tracker *tracker);
	sf::Vector2i	getMediumMove(Grid *grid, PlayerInfo *player, PlayerInfo *opponent, Evaluation *evaluator, Tracker *tracker);
};

#endif
