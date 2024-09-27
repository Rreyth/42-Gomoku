#ifndef PLAYERINFO_HPP
# define PLAYERINFO_HPP

# include <define.hpp>

class PlayerInfo
{
public:
	win_state	winState;
	inter_type	interType;
	int 		nbCapture, nbMove;

	PlayerInfo(void);
	~PlayerInfo();

	PlayerInfo	&operator=(const PlayerInfo &playerInfo);

	void			reset(void);
};

#endif
