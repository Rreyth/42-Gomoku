#include <screen/Game.hpp>
#include <utils/Functions.hpp>


////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Game::Game(void)
{
	this->leave = Button("LEAVE", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);

	this->rect.setPosition(2, this->grid.getY());
	this->rect.setSize(sf::Vector2f(WIN_W * 0.15, this->grid.getH()));
	this->rect.setOutlineThickness(2);
	this->rect.setOutlineColor(sf::Color::White);
	this->rect.setFillColor(sf::Color(0, 0, 0, 0));
}

Game::~Game()
{
}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

Player	*Game::getLeftPlayer(void)
{
	return (&this->playerLeft);
}

Player	*Game::getRightPlayer(void)
{
	return (&this->playerRight);
}

Grid	*Game::getGrid(void)
{
	return (&this->grid);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Game::tick(display_state *displayState, float delta, Mouse *mouse)
{
	// TODO : REMOVE
	static std::vector<int>				lCaptures;
	static std::vector<int>				rCaptures;
	static std::vector<sf::Vector2i>	lastMoves;
	static std::vector<std::string>		lastBoardStates;

	// TODO : REMOVE
	if (mouse->isPressed(MBUT_MIDDLE) && lastMoves.size() > 0)
	{
		int	lastId = lastMoves.size() - 1;
		printf("Remove last move !\n");
		this->grid.removeStone(&lastMoves[lastId]);
		if (lCaptures[lastId] != this->playerLeft.getCaptured()
			|| rCaptures[lastId] != this->playerRight.getCaptured())
			this->grid.resetGridByBoardState(lastBoardStates[lastId]);

		this->playerLeft.setCaptured(lCaptures[lastId]);
		this->playerRight.setCaptured(rCaptures[lastId]);

		lastMoves.pop_back();
		lCaptures.pop_back();
		rCaptures.pop_back();
		lastBoardStates.pop_back();

		if (this->playerLeft.isPlaying())
		{
			this->playerLeft.setPlaying(false);
			this->playerRight.setPlaying(true);
			this->playerRight.setMoves(this->playerRight.getMoves() - 1);
		}
		else
		{
			this->playerRight.setPlaying(false);
			this->playerLeft.setPlaying(true);
			this->playerLeft.setMoves(this->playerLeft.getMoves() - 1);
		}
	}

	this->leave.tick(mouse);
	this->grid.tick(displayState, mouse, &this->playerLeft, &this->playerRight,
						&this->evaluator);

	sf::Vector2i	move;
	int 			nbMoves = this->playerLeft.getMoves() + this->playerRight.getMoves();
	Player			*me = &this->playerLeft;
	Player			*opponent = &this->playerRight;
	if (this->playerLeft.isPlaying())
	{
		move = this->playerLeft.getNextMove(&this->grid, &this->playerRight, mouse, &this->evaluator);
		this->playerLeft.tick(delta, this->mode);
	}
	else
	{
		me = &this->playerRight;
		opponent = &this->playerLeft;
		move = this->playerRight.getNextMove(&this->grid, &this->playerLeft, mouse, &this->evaluator);
		this->playerRight.tick(delta, this->mode);
	}

	int	lCapture = this->playerLeft.getCaptured();
	int	rCapture = this->playerRight.getCaptured();
	std::string lastBoardState = this->grid.getCurrentBoardState();
	if (this->grid.putStone(&move, nbMoves, me, opponent))
	{
		// TODO : REMOVE
		lastMoves.push_back(move);
		lCaptures.push_back(lCapture);
		rCaptures.push_back(rCapture);
		lastBoardStates.push_back(lastBoardState);

		this->swapTurn(&move);
		this->grid.addBoardState();
		if (this->grid.checkWinCondition(me, opponent, move))
		{
			this->grid.goToLastMove();
			*displayState = DISPLAY_END;
			return ;
		}
	}

	if (this->mode == BLITZ)
	{
		if (this->playerLeft.getTimer() <= 0)
		{
			this->playerRight.setWinState(WIN_STATE_TIME);
			*displayState = DISPLAY_END;
		}
		else if (this->playerRight.getTimer() <= 0)
		{
			this->playerLeft.setWinState(WIN_STATE_TIME);
			*displayState = DISPLAY_END;
		}
	}

	if (this->leave.getPressed())
		*displayState = DISPLAY_MENU;
}

void	Game::swapTurn(sf::Vector2i *move)
{
	if (this->playerLeft.isPlaying())
	{
		this->playerLeft.addMove();
		this->playerLeft.setPlaying(false);
		this->playerRight.setPlaying(true);
	}
	else
	{
		this->playerRight.addMove();
		this->playerRight.setPlaying(false);
		this->playerLeft.setPlaying(true);
	}
}

void	Game::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	std::string name;
	if (this->playerLeft.isPlaying())
		name = this->playerLeft.getName();
	else
		name = this->playerRight.getName();
	drawText(window, text, name + " TURN", WIN_W / 2, 20, 60, sf::Color::White, TOP_CENTER);

	grid.draw(window, text, textureManager);

	this->drawLeftSide(window, text, textureManager);
	this->drawRightSide(window, text, textureManager);
	this->drawBottom(window, text, textureManager);

	this->leave.draw(window, text, textureManager);
}

void	Game::setGame(player_type playerLeft, player_type playerRight,
						game_mode mode, game_rules rule,
						AI_difficulty aiLeft, AI_difficulty aiRight,
						stone_sprite *sprite)
{
	this->mode = mode;

	sprite_name	leftStone = SPRITE_STONE_BLUE;
	sprite_name	rightStone = SPRITE_STONE_RED;
	if (*sprite == AMOGUS)
	{
		leftStone = SPRITE_AMOGUS_BLUE;
		rightStone = SPRITE_AMOGUS_RED;
	}
	else if (*sprite == COINS)
	{
		leftStone = SPRITE_COIN_BLUE;
		rightStone = SPRITE_COIN_RED;
	}

	bool solo = true;
	if ((playerLeft == PLAYER && playerRight == PLAYER)||
		(playerLeft == AI_PLAYER && playerRight == AI_PLAYER))
		solo = false;
	this->playerLeft.setPlayer(playerLeft, mode, 1, leftStone, solo, aiLeft);
	this->playerRight.setPlayer(playerRight, mode, 2, rightStone, solo, aiRight);

	// TODO: RE SET RANDOM
	this->playerRight.setPlaying(true);
	// if (rand_int(1, 2) == 1)
	// 	this->playerLeft.setPlaying(true);
	// else
	// 	this->playerRight.setPlaying(true);

	this->grid.clearGrid(leftStone, rightStone, rule);
}


void	Game::replay(display_state *displayState)
{
	this->grid.reset();
	this->playerLeft.reset(this->mode);
	this->playerRight.reset(this->mode);
	if (rand_int(1, 2) == 1)
		this->playerLeft.setPlaying(true);
	else
		this->playerRight.setPlaying(true);
	*displayState = DISPLAY_GAME;
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

void	Game::drawLeftSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	int gridY = this->grid.getY();

	this->rect.setPosition(2, gridY);
	window->draw(this->rect);

	sf::Vector2f size = this->rect.getSize();
	std::string str = this->playerLeft.getName();
	drawText(window, text, str, size.x / 2, gridY + (size.y * 0.05), 30, sf::Color::White, MID_CENTER);

	if (this->playerLeft.getType() == AI_PLAYER)
	{
		std::string	difficulty[] = {"RANDOM", "BETTER_RANDOM", "EASY", "MEDIUM", "HARD"};
		AI *ai = this->playerLeft.getAI();
		str = difficulty[ai->getDifficulty()];
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.15), 30, sf::Color::White, MID_CENTER);

		str = "TIME OF PREDICTION";
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.25), 25, sf::Color::White, MID_CENTER);
		str = std::to_string((int)ai->getTimer()) + " us";
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.30), 30, sf::Color::White, MID_CENTER);
	}

	if (this->playerLeft.isPlaying())
		textureManager->drawTexture(window, SPRITE_ARROW, size.x / 2, WIN_H * 0.075, MID_CENTER);

	textureManager->drawTexture(window, this->playerLeft.getStoneSprite(), size.x / 2, gridY + (size.y * 0.4), MID_CENTER);

	str = std::to_string(this->playerLeft.getMoves());

	drawText(window, text, "MOVES", size.x / 2, gridY + (size.y * 0.60), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, size.x / 2, gridY + (size.y * 0.65), 30, sf::Color::White, MID_CENTER);

	str = std::to_string(this->playerLeft.getCaptured());
	drawText(window, text, "CAPTURED STONES", size.x / 2, gridY + (size.y * 0.75), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, size.x / 2, gridY + (size.y * 0.8), 30, sf::Color::White, MID_CENTER);

	str = std::to_string((int)this->playerLeft.getTimer()) + " s";
	drawText(window, text, "TIMER", size.x / 2, gridY + (size.y * 0.90), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, size.x / 2, gridY + (size.y * 0.95), 30, sf::Color::White, MID_CENTER);
}

void	Game::drawRightSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	int gridY = this->grid.getY();

	this->rect.setPosition(WIN_W * 0.85 - 2, gridY);
	window->draw(this->rect);

	sf::Vector2f size = this->rect.getSize();
	std::string str = this->playerRight.getName();
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.05), 30, sf::Color::White, MID_CENTER);

	if (this->playerRight.getType() == AI_PLAYER)
	{
		std::string	difficulty[] = {"RANDOM", "BETTER_RANDOM", "EASY", "MEDIUM", "HARD"};
		AI *ai = this->playerRight.getAI();
		str = difficulty[ai->getDifficulty()];
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.15), 30, sf::Color::White, MID_CENTER);

		str = "TIME OF PREDICTION";
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.25), 25, sf::Color::White, MID_CENTER);
		str = std::to_string((int)ai->getTimer()) + " us";
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.30), 30, sf::Color::White, MID_CENTER);
	}

	if (this->playerRight.isPlaying())
		textureManager->drawTexture(window, SPRITE_ARROW, WIN_W - (size.x / 2), WIN_H * 0.075, MID_CENTER);

	textureManager->drawTexture(window, this->playerRight.getStoneSprite(), WIN_W - (size.x / 2), gridY + (size.y * 0.4), MID_CENTER);

	str = std::to_string(this->playerRight.getMoves());
	drawText(window, text, "MOVES", WIN_W - (size.x / 2), gridY + (size.y * 0.60), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.65), 30, sf::Color::White, MID_CENTER);

	str = std::to_string(this->playerRight.getCaptured());
	drawText(window, text, "CAPTURED STONES", WIN_W - (size.x / 2), gridY + (size.y * 0.75), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.80), 30, sf::Color::White, MID_CENTER);

	str = std::to_string((int)this->playerRight.getTimer()) + " s";
	drawText(window, text, "TIMER", WIN_W - (size.x / 2), gridY + (size.y * 0.90), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.95), 30, sf::Color::White, MID_CENTER);
}

void Game::drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager) //TODO: not for actual ai player
{
	// if (this->playerLeft.getType() != AI_PLAYER && this->playerRight.getType() != AI_PLAYER) //not a player but just a predict

	int gridX = this->grid.getX();

	this->rect.setSize(sf::Vector2f(this->grid.getW(), WIN_H * 0.1));
	this->rect.setPosition(gridX, WIN_H * 0.9 - 2);

	window->draw(this->rect);

	sf::Vector2f size = this->rect.getSize();
	sf::Vector2f pos = this->rect.getPosition();
	drawText(window, text, "AI PREDICT", pos.x + (size.x / 2), pos.y + 5, 20, sf::Color::White, MID_CENTER);
	drawText(window, text, "AI SPRITE", pos.x + 10, pos.y + (size.y / 2), 15, sf::Color::White, MID_LEFT);
	// drawText(window, text, "PREDICTED POS", pos.x + (size.x / 2), pos.y + (size.y / 2), 15, sf::Color::White, MID_CENTER);

	long	leftEval = this->evaluator.evaluateGrid(&this->grid, INTER_LEFT, INTER_RIGHT,
												this->playerLeft.getCaptured(), this->playerRight.getCaptured());
	long	rightEval = this->evaluator.evaluateGrid(&this->grid, INTER_RIGHT, INTER_LEFT,
												this->playerRight.getCaptured(), this->playerLeft.getCaptured());
	std::string tkt = std::to_string(leftEval) + " | " + std::to_string(rightEval);
	drawText(window, text, tkt, pos.x + (size.x / 2), pos.y + (size.y / 2), 15, sf::Color::White, MID_CENTER);

	this->rect.setSize(sf::Vector2f(WIN_W * 0.15, this->grid.getH()));

}
