#ifndef AI_HPP
# define AI_HPP

# include <define.hpp>
# include <game/Grid.hpp>
# include <AI/Evaluation.hpp>


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

	void			setAI(AI_difficulty difficulty);

	sf::Vector2i	getNextMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator);

private:
	double			timer;
	AI_difficulty	difficulty;

	sf::Vector2i	getRandomMove(Grid *grid, Player *player, Player *opponent);
	sf::Vector2i	getBetterRandom(Grid *grid, Player *player, Player *opponent);
	sf::Vector2i	getEasyMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator);
	sf::Vector2i	getMediumMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator);
};

#endif
