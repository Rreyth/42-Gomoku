#ifndef PLAYERINFO_HPP
# define PLAYERINFO_HPP

# include <define.hpp>
# include <utils/TextureManager.hpp>

class PlayerInfo
{
public:
	win_state	winState;
	inter_type	interType;
	sprite_name	stoneSprite;
	int 		nbCapture, nbMove;

	PlayerInfo(void);
	~PlayerInfo();

	PlayerInfo	&operator=(const PlayerInfo &playerInfo);

	void			reset(void);
};

#endif
