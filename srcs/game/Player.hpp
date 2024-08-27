#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <define.hpp>
# include <string>

class Player
{
public:
	Player(void);
	~Player();

	void		setType(player_type type);
	player_type	getType(void);
	void 		setCaptured(int captured);
	int			getCaptured(void);
	void		setTimer(game_mode mode);
	float		getTimer(void);
	void		setName(std::string name);
	std::string	getName(void);

	bool 		isPlaying(void);
	void 		setPlaying(bool playing);

	bool 		isWinner(void);
	void 		setWinner(bool winner);

	void 		setPlayer(player_type type, game_mode mode, int pos);

	void 		tick(float delta, game_mode mode);

private:
	std::string	name;
	player_type	type;
	int 		captured;
	float		timer;
	bool		playing;
	bool		winner;
};

#endif
