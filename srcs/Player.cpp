#include <Player.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Player::Player(void)
{
	this->type = PLAYER;
	this->captured = 0;
}

Player::~Player()
{
}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

player_type	Player::getType(void)
{
	return (this->type);
}

void		Player::setType(player_type type)
{
	this->type = type;
}

int			Player::getCaptured(void)
{
	return (this->captured);
}

void		Player::setCaptured(int captured)
{
	this->captured = captured;
}

void		Player::setTimer(game_mode mode)
{
	if (mode == BLITZ)
	{
		this->timer = 180;
	}
	else
	{
		this->timer = 0;
	}
}

float		Player::getTimer(void)
{
	return (this->timer);
}

void		Player::setName(std::string name)
{
	this->name = name;
}

std::string	Player::getName(void)
{
	return (this->name);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void		Player::setPlayer(player_type type, game_mode mode, int pos)
{
	this->setType(type);
	this->setTimer(mode);
	if (pos != 0 && type == PLAYER)
		this->setName("PLAYER " + std::to_string(pos));
	else if (pos != 0 && type == AI)
		this->setName("AI " + std::to_string(pos));
	else if (type == PLAYER)
		this->setName("PLAYER");
	else
		this->setName("AI");
}

void		Player::tick(float delta, game_mode mode, std::string turn)
{
	if (turn != this->name)
		return ;
	if (mode == BLITZ)
	{
		this->timer -= delta;
		if (this->timer < 0)
		{
			this->timer = 0;
		}
	}
	else
	{
		this->timer += delta;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
