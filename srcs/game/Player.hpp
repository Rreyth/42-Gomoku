#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <define.hpp>
# include <utils/TextureManager.hpp>
# include <string>

class Player
{
public:
	Player(void);
	~Player();

	void		setType(player_type type);
	player_type	getType(void);
	void 		setCaptured(int captured);
	void 		addCaptured(int captured);
	int			getCaptured(void);
	void		setTimer(game_mode mode);
	float		getTimer(void);
	void		setName(std::string name);
	std::string	getName(void);

	bool 		isPlaying(void);
	void 		setPlaying(bool playing);

	win_state 	getWinState(void);
	void 		setWinState(win_state winState);

	sprite_name	getStoneSprite(void);

	void		addMove(void);
	int			getMoves(void);

	void 		setPlayer(player_type type, game_mode mode, int pos, sprite_name stoneSprite);

	void 		tick(float delta, game_mode mode);

private:
	player_type	type;
	std::string	name;
	win_state	winState;
	sprite_name	stoneSprite;

	float		timer;
	bool		playing;
	int 		captured;
	int			moves;
};

#endif
