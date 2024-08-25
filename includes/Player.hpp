#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <define.hpp>

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


	void 		tick(float delta, game_mode mode);

private:
	// std::string	name;
	player_type	type;
	int 		captured;
	float		timer;
};

#endif
