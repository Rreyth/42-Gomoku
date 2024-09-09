#ifndef AI_HPP
# define AI_HPP

# include <define.hpp>
# include <game/Grid.hpp>
# include <AI/Evaluation.hpp>

# include <unordered_map>

class Player;

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

	sf::Vector2i	getNextMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator);

private:
	double			timer;
	AI_difficulty	difficulty;
	std::unordered_map<std::string, int>						memoryEvaluation;
	std::unordered_map<std::string, std::vector<sf::Vector2i>>	memoryMoves;

	sf::Vector2i	getRandomMove(Grid *grid, Player *player, Player *opponent);
	sf::Vector2i	getBetterRandom(Grid *grid, Player *player, Player *opponent);
	sf::Vector2i	getEasyMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator);
	sf::Vector2i	getMediumMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator, Tracker *tracker);
};

#endif
