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

	void			addMove(void);
	int				getNbMove(void);

	sprite_name		getStoneSprite(void);
	int				getNbCapture(void);
	AI				*getAI(void);
	PlayerInfo		*getPlayerInfo(void);

	sf::Vector2i	getNextMove(
						Grid *grid, Player *opponent, Mouse *mouse,
						Evaluation *evaluator, bool *moveDone);

	void 			setPlayer(player_type type, game_mode mode, int pos, sprite_name stoneSprite, bool solo, AI_difficulty difficulty);

	void 			tick(float delta, game_mode mode);
	void			reset(game_mode mode);

private:
	player_type	type;
	std::string	name;
	sprite_name	stoneSprite;
	AI			ai;
	PlayerInfo	info;

	float		timer;
	bool		playing;
};

#endif
