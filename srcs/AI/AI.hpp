#ifndef AI_HPP
# define AI_HPP

# include <define.hpp>
# include <game/Grid.hpp>
# include <AI/Evaluation.hpp>
# include <functional>

// # include <unordered_map>

// class BoardMemoryid
// {
// public:
// 	int	plBitBoard[GRID_W_INTER],
// 		opBitBoard[GRID_W_INTER];

// 	BoardMemoryid	&operator=(const BoardMemoryid &boardMemoryid)
// 	{
// 		if (this == &boardMemoryid)
// 			return (*this);

// 		for (int i = 0; i < GRID_W_INTER; i++)
// 		{
// 			this->plBitBoard[i] = boardMemoryid.plBitBoard[i];
// 			this->opBitBoard[i] = boardMemoryid.opBitBoard[i];
// 		}

// 		return (*this);
// 	}

// 	bool	operator==(const BoardMemoryid &boardMemoryid) const
// 	{
// 		for (int i = 0; i < GRID_W_INTER; i++)
// 		{
// 			if (this->plBitBoard[i] != boardMemoryid.plBitBoard[i]
// 					|| this->opBitBoard[i] != boardMemoryid.opBitBoard[i])
// 				return (false);
// 		}

// 		return (true);
// 	}

// };

// // Custom specialization of std::hash can be injected in namespace std.
// template<>
// struct std::hash<BoardMemoryid>
// {
// 	std::size_t operator()(const BoardMemoryid& boardMemoryid) const noexcept
// 	{
// 		std::size_t	res;
// 		std::size_t	plTmp;
// 		std::size_t	opTmp;

// 		res = 0;
// 		for (int i = 0; i < GRID_W_INTER; i++)
// 		{
// 			plTmp = std::hash<int>{}(boardMemoryid.plBitBoard[i]);
// 			opTmp = std::hash<int>{}(boardMemoryid.opBitBoard[i]);
// 			res = plTmp + 0x9e3779b9 + (res<<6) + (res>>2);
// 			res = opTmp + 0x9e3779b9 + (res<<6) + (res>>2);
// 		}

// 		return (res);

// 	}
// };


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
	double									timer;
	AI_difficulty							difficulty;
	// std::unordered_map<BoardMemoryid, int>	memoryEvaluation;

	sf::Vector2i	getRandomMove(Grid *grid, Player *player, Player *opponent);
	sf::Vector2i	getBetterRandom(Grid *grid, Player *player, Player *opponent);
	sf::Vector2i	getEasyMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator);
	sf::Vector2i	getMediumMove(Grid *grid, Player *player, Player *opponent, Evaluation *evaluator, Tracker *tracker);
};

#endif
