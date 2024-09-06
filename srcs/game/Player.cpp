#include <game/Player.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Player::Player(void)
{
	this->type = PLAYER;
	this->captured = 0;
	this->playing = false;
	this->winState = WIN_STATE_NONE;
	this->timer = 0;
	this->stoneSprite = SPRITE_STONE_BLUE;
	this->moves = 0;
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

void		Player::addCaptured(int captured)
{
	this->captured += captured;
}

void		Player::setTimer(game_mode mode)
{
	if (mode == BLITZ)
		this->timer = 180;
	else
		this->timer = 0;
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

bool		Player::isPlaying(void)
{
	return (this->playing);
}

void		Player::setPlaying(bool playing)
{
	this->playing = playing;
}

win_state		Player::getWinState(void)
{
	return (this->winState);
}

void		Player::setWinState(win_state winState)
{
	this->winState = winState;
}

sprite_name	Player::getStoneSprite()
{
	return (this->stoneSprite);
}

void		Player::setMoves(int moves)
{
	this->moves = moves;
}


void		Player::addMove(void)
{
	this->moves++;
}

int			Player::getMoves(void)
{
	return (this->moves);
}

void		Player::setInterType(inter_type interType)
{
	this->interType = interType;
}

inter_type	Player::getInterType(void)
{
	return (this->interType);
}

AI			*Player::getAI(void)
{
	return (&this->ai);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

sf::Vector2i	Player::getNextMove(Grid *grid, Player *opponent, Mouse *mouse, Evaluation *evaluator)
{
	sf::Vector2i	move(-1, -1);

	int				nbMoves = this->moves + opponent->getMoves();


	if (this->type == PLAYER)
	{
		if (mouse->isPressed(MBUT_LEFT))
		{
			int	mx = mouse->getX();
			int	my = mouse->getY();
			mx -= grid->getX();
			my -= grid->getY();
			int px = (mx - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
			int py = (my - GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
			if (grid->checkLegalMove(px, py, nbMoves, this->interType, opponent->getInterType()))
			{
				move.x = px;
				move.y = py;
			}
		}
	}
	else
	{
		grid->disablePreview();
		move = this->ai.getNextMove(grid, this, opponent, evaluator);
	}
	return (move);
}


void		Player::setPlayer(player_type type, game_mode mode, int pos, sprite_name stoneSprite, bool solo, AI_difficulty difficulty)
{
	this->setType(type);
	this->setTimer(mode);
	if (!solo && type == PLAYER)
		this->setName("PLAYER " + std::to_string(pos));
	else if (!solo && type == AI_PLAYER)
		this->setName("AI " + std::to_string(pos));
	else if (type == PLAYER)
		this->setName("PLAYER");
	else
		this->setName("AI");

	if (type == AI_PLAYER)
		this->ai.setAI(difficulty);

	if (pos == 1)
		this->interType = INTER_LEFT;
	else
		this->interType = INTER_RIGHT;

	this->winState = WIN_STATE_NONE;
	this->stoneSprite = stoneSprite;
	this->playing = false;
	this->captured = 0;
	this->moves = 0;
}

void		Player::tick(float delta, game_mode mode)
{
	double time_add = delta;
	if (this->type == AI_PLAYER) //TODO: REMOVE THIS WHEN AI BECOME THREAD
	{
		time_add += this->ai.getTimer() / 1000000;
	}
	if (mode == BLITZ)
	{
		this->timer -= time_add;
		if (this->timer < 0)
			this->timer = 0;
	}
	else
		this->timer += time_add;
}


void		Player::reset(game_mode mode)
{
	this->setTimer(mode);
	this->winState = WIN_STATE_NONE;
	this->playing = false;
	this->captured = 0;
	this->moves = 0;
	this->ai.reset();
}



////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
