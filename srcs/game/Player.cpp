#include <game/Player.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Player::Player(void)
{
	this->type = PLAYER;
	this->playing = false;
	this->suggestion = false;
	this->suggestDone = false;
	this->timer = 0;
	this->stoneSprite = SPRITE_STONE_BLUE;
	this->suggestedMove = sf::Vector2i(-1, -1);
	this->lastMove = sf::Vector2i(-1, -1);
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
	if (this->playing)
	{
		this->ai.startTurn();
	}
	this->suggestDone = false;
}


void		Player::setWinState(win_state winState)
{
	this->info.winState = winState;
}


win_state	Player::getWinState(void)
{
	return (this->info.winState);
}


sprite_name	Player::getStoneSprite()
{
	return (this->stoneSprite);
}


void		Player::addMove(sf::Vector2i *move)
{
	this->info.nbMove++;
	this->lastMove = *move;
}


int			Player::getNbMove(void)
{
	return (this->info.nbMove);
}


int			Player::getNbCapture(void)
{
	return (this->info.nbCapture);
}


AI	*Player::getAI(void)
{
	return (&this->ai);
}


PlayerInfo		*Player::getPlayerInfo(void)
{
	return (&this->info);
}


bool			Player::getSuggestDone(void)
{
	return (this->suggestDone);
}


sf::Vector2i	Player::getSuggestedMove(void)
{
	return (this->suggestedMove);
}

sf::Vector2i	Player::getLastMove(void)
{
	return (this->lastMove);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

sf::Vector2i	Player::getNextMove(
							Grid *grid, Player *opponent, Mouse *mouse,
							Evaluation *evaluator, bool *moveDone)
{
	sf::Vector2i	move, tmp;
	int				nbMoves;
	
	move = sf::Vector2i(-1, -1);
	nbMoves = this->getNbMove() + opponent->getNbMove();
	if (this->type == PLAYER)
	{
		if (mouse->isPressed(MBUT_LEFT))
		{
			int	mx = mouse->getX();
			int	my = mouse->getY();
			mx -= grid->getX();
			my -= grid->getY();
			int px = (mx + GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
			int py = (my + GRID_SQUARE_HALF_SIZE) / GRID_SQUARE_SIZE;
			if (grid->checkLegalMove(
						px, py, nbMoves,
						this->info.interType,
						opponent->getPlayerInfo()->interType))
			{
				move.x = px;
				move.y = py;
				*moveDone = true;
			}
		}
		if (this->suggestion && !this->suggestDone)
		{
			tmp = this->ai.getNextMove(&this->suggestDone);
			if (this->suggestDone)
				this->suggestedMove = tmp;
		}
	}
	else
	{
		grid->disablePreview();
		move = this->ai.getNextMove(moveDone);
	}
	return (move);
}


void		Player::setPlayer(
						Grid *grid, game_mode mode, PlayerInfo *opponent,
						player_type type, AI_difficulty difficulty, int pos,
						sprite_name stoneSprite, bool solo, bool suggestion)
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
		this->ai.setAI(grid, difficulty, &this->info, opponent);

	if (suggestion)
		this->ai.setAI(grid, HARD, &this->info, opponent);

	if (pos == 1)
		this->info.interType = INTER_LEFT;
	else
		this->info.interType = INTER_RIGHT;

	this->stoneSprite = stoneSprite;
	this->playing = false;
	this->suggestion = suggestion;
	this->suggestDone = false;
	this->info.reset();
}


void		Player::tick(float delta, game_mode mode)
{
	if (mode == BLITZ)
	{
		this->timer -= delta;
		if (this->timer < 0)
			this->timer = 0;
	}
	else
		this->timer += delta;
}


void		Player::reset(
						Grid *grid, game_mode mode, PlayerInfo *opponent)
{
	this->setTimer(mode);
	this->playing = false;
	this->suggestDone = false;
	if (type == AI_PLAYER || this->suggestion)
		this->ai.reset(grid, &this->info, opponent);
	this->info.reset();
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////
