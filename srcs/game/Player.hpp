#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <define.hpp>
# include <utils/TextureManager.hpp>
# include <game/Grid.hpp>
# include <game/PlayerInfo.hpp>
# include <AI/AI.hpp>
# include <string>

class Player
{
public:
	Player(void);
	~Player();

	void			setType(player_type type);
	player_type		getType(void);

	void			setTimer(game_mode mode);
	float			getTimer(void);
	
	void			setName(std::string name);
	std::string		getName(void);

	bool 			isPlaying(void);
	void 			setPlaying(bool playing);

	void			setWinState(win_state winState);
	win_state		getWinState(void);

	void			addMove(sf::Vector2i *move);
	int				getNbMove(void);

	sprite_name		getStoneSprite(void);
	int				getNbCapture(void);
	AI				*getAI(void);
	PlayerInfo		*getPlayerInfo(void);
	bool			getSuggestDone(void);
	sf::Vector2i	getSuggestedMove(void);
	sf::Vector2i	getLastMove(void);

	sf::Vector2i	getNextMove(
						Grid *grid, Player *opponent, Mouse *mouse,
						Evaluation *evaluator, bool *moveDone);

	void 			setPlayer(
						Grid *grid, game_mode mode, PlayerInfo *opponent,
						player_type type, AI_difficulty difficulty, int pos,
						sprite_name stoneSprite, bool solo, bool suggestion);

	void 			tick(float delta, game_mode mode);
	void			reset(
						Grid *grid, game_mode mode, PlayerInfo *opponent);

private:
	player_type		type;
	std::string		name;
	sprite_name		stoneSprite;
	AI				ai;
	PlayerInfo		info;
	sf::Vector2i	suggestedMove, lastMove;

	float			timer;
	bool			playing, suggestion, suggestDone;
};

#endif
