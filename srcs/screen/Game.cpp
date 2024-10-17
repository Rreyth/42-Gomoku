#include <screen/Game.hpp>
#include <utils/Functions.hpp>


////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Game::Game(void)
{
	this->suggestion = false;
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
	bool			moveDone;
	int				nbMoves;
	sf::Vector2i	move;
	Player			*player, *opponent;

	this->leave.tick(mouse);
	this->grid.tick(displayState, mouse, &this->playerLeft, &this->playerRight,
						&this->evaluator);

	if (this->leave.getPressed())
		*displayState = DISPLAY_MENU;

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

	moveDone = false;
	nbMoves = this->playerLeft.getNbMove() + this->playerRight.getNbMove();
	if (this->playerLeft.isPlaying())
	{
		player = &this->playerLeft;
		opponent = &this->playerRight;
		move = this->playerLeft.getNextMove(
								&this->grid, &this->playerRight, mouse,
								&this->evaluator, &moveDone);
		this->playerLeft.tick(delta, this->mode);
	}
	else
	{
		player = &this->playerRight;
		opponent = &this->playerLeft;
		move = this->playerRight.getNextMove(
								&this->grid, &this->playerLeft, mouse,
								&this->evaluator, &moveDone);
		this->playerRight.tick(delta, this->mode);
	}

	// If no move is done, skip next part
	if (!moveDone)
		return ;

	if (this->grid.putStone(&move, nbMoves, player->getPlayerInfo(), opponent->getPlayerInfo()))
	{
		this->grid.addBoardToHistory();
		player->addMove(&move);
		if (this->grid.checkWinCondition(player->getPlayerInfo(), opponent->getPlayerInfo()))
		{
			this->grid.goToHistoryEnd();
			*displayState = DISPLAY_END;
			return ;
		}
		this->swapTurn();
	}
}

void	Game::swapTurn(void)
{
	if (this->playerLeft.isPlaying())
	{
		this->playerLeft.setPlaying(false);
		this->playerRight.setPlaying(true);
	}
	else
	{
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

	if (this->suggestion)
		this->drawBottom(window, text, textureManager);

	this->leave.draw(window, text, textureManager);
}

void	Game::setGame(player_type playerLeft, player_type playerRight,
						game_mode mode, game_rules rule,
						AI_difficulty aiLeft, AI_difficulty aiRight,
						starter startingPlayer, stone_sprite *sprite,
						bool suggestion)
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

	this->suggestion = suggestion && !solo && playerLeft == PLAYER;

	this->playerLeft.setPlayer(&this->grid, mode, this->playerRight.getPlayerInfo(),
								playerLeft, aiLeft, 1, leftStone, solo, this->suggestion);
	this->playerRight.setPlayer(&this->grid, mode, this->playerLeft.getPlayerInfo(),
								playerRight, aiRight, 2, rightStone, solo, this->suggestion);

	this->startingPlayer = startingPlayer;
	if (startingPlayer == PLAYER1)
		this->playerLeft.setPlaying(true);
	else if (startingPlayer == PLAYER2)
		this->playerRight.setPlaying(true);
	else
	{
		if (rand_int(1, 2) == 1)
			this->playerLeft.setPlaying(true);
		else
			this->playerRight.setPlaying(true);
	}
	this->grid.clearGrid(leftStone, rightStone, rule);
}


void	Game::replay(display_state *displayState)
{
	this->grid.reset();
	this->playerLeft.reset(&this->grid, this->mode, this->playerRight.getPlayerInfo());
	this->playerRight.reset(&this->grid, this->mode, this->playerLeft.getPlayerInfo());

	if (this->startingPlayer == PLAYER1)
		this->playerLeft.setPlaying(true);
	else if (this->startingPlayer == PLAYER2)
		this->playerRight.setPlaying(true);
	else
	{
		if (rand_int(1, 2) == 1)
			this->playerLeft.setPlaying(true);
		else
			this->playerRight.setPlaying(true);
	}
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
		std::string	difficulty[] = {"RANDOM", "BETTER_RANDOM", "EASY", "BETTER_EASY", "MEDIUM", "HARD"};
		AI *ai = this->playerLeft.getAI();
		str = difficulty[ai->getDifficulty()];
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.15), 30, sf::Color::White, MID_CENTER);

		str = "TIME OF PREDICTION";
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.25), 25, sf::Color::White, MID_CENTER);
		str = format_time(std::to_string(ai->getTimer())) + " us";
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.30), 30, sf::Color::White, MID_CENTER);
	}

	if (this->playerLeft.isPlaying())
		textureManager->drawTexture(window, SPRITE_ARROW, size.x / 2, WIN_H * 0.075, MID_CENTER);

	textureManager->drawTexture(window, this->playerLeft.getStoneSprite(), size.x / 2, gridY + (size.y * 0.375), MID_CENTER);

	drawText(window, text, "LAST MOVE", size.x / 2, gridY + (size.y * 0.45), 25, sf::Color::White, MID_CENTER);
	if (this->playerLeft.getNbMove() != 0)
	{
		std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		sf::Vector2i move = this->playerLeft.getLastMove();
		
		str = alpha[move.x] + std::string(", ") + std::to_string(move.y + 1);
		drawText(window, text, str, size.x / 2, gridY + (size.y * 0.50), 25, sf::Color::White, MID_CENTER);
	}

	str = std::to_string(this->playerLeft.getNbMove());

	drawText(window, text, "MOVES", size.x / 2, gridY + (size.y * 0.60), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, size.x / 2, gridY + (size.y * 0.65), 30, sf::Color::White, MID_CENTER);

	str = std::to_string(this->playerLeft.getNbCapture());
	drawText(window, text, "CAPTURED STONES", size.x / 2, gridY + (size.y * 0.75), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, size.x / 2, gridY + (size.y * 0.8), 30, sf::Color::White, MID_CENTER);

	str = format_time(std::to_string((int)this->playerLeft.getTimer())) + " s";
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
		std::string	difficulty[] = {"RANDOM", "BETTER_RANDOM", "EASY", "BETTER_EASY", "MEDIUM", "HARD"};
		AI *ai = this->playerRight.getAI();
		str = difficulty[ai->getDifficulty()];
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.15), 30, sf::Color::White, MID_CENTER);

		str = "TIME OF PREDICTION";
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.25), 25, sf::Color::White, MID_CENTER);
		str = format_time(std::to_string(ai->getTimer())) + " us";
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.30), 30, sf::Color::White, MID_CENTER);
	}

	if (this->playerRight.isPlaying())
		textureManager->drawTexture(window, SPRITE_ARROW, WIN_W - (size.x / 2), WIN_H * 0.075, MID_CENTER);

	textureManager->drawTexture(window, this->playerRight.getStoneSprite(), WIN_W - (size.x / 2), gridY + (size.y * 0.375), MID_CENTER);

	drawText(window, text, "LAST MOVE", WIN_W - (size.x / 2), gridY + (size.y * 0.45), 25, sf::Color::White, MID_CENTER);
	if (this->playerRight.getNbMove() != 0)
	{
		std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		sf::Vector2i move = this->playerRight.getLastMove();
		
		str = alpha[move.x] + std::string(", ") + std::to_string(move.y + 1);
		drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.50), 25, sf::Color::White, MID_CENTER);
	}


	str = std::to_string(this->playerRight.getNbMove());
	drawText(window, text, "MOVES", WIN_W - (size.x / 2), gridY + (size.y * 0.60), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.65), 30, sf::Color::White, MID_CENTER);

	str = std::to_string(this->playerRight.getNbCapture());
	drawText(window, text, "CAPTURED STONES", WIN_W - (size.x / 2), gridY + (size.y * 0.75), 25, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.80), 30, sf::Color::White, MID_CENTER);

	str = format_time(std::to_string((int)this->playerRight.getTimer())) + " s";
	drawText(window, text, "TIMER", WIN_W - (size.x / 2), gridY + (size.y * 0.90), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.95), 30, sf::Color::White, MID_CENTER);
}

void Game::drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	AI				*ai;
	sf::Vector2f	size, pos;
	sf::Vector2i	move;
	std::string		str, alpha;
	int				gridX, avg;

	gridX = this->grid.getX();

	this->rect.setSize(sf::Vector2f(this->grid.getW(), WIN_H * 0.1));
	this->rect.setPosition(gridX, WIN_H * 0.9 - 2);

	size = this->rect.getSize();
	pos = this->rect.getPosition();

	window->draw(this->rect);
	drawText(window, text, "AI SUGGESTION", pos.x + (size.x / 2), pos.y + 5,
				20, sf::Color::White, MID_CENTER);
	drawText(window, text, "MOVE", pos.x + (size.x / 4), pos.y + (size.y * 0.3),
				20, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIME", pos.x + (size.x / 2), pos.y + (size.y * 0.3),
				20, sf::Color::White, MID_CENTER);
	drawText(window, text, "AVG TIME/SUGGESTION", pos.x + (size.x / 4 * 3), pos.y + (size.y * 0.3),
				20, sf::Color::White, MID_CENTER);

	alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (this->playerLeft.isPlaying() && this->playerLeft.getSuggestDone())
	{
		ai = this->playerLeft.getAI();
		move = this->playerLeft.getSuggestedMove();

		str = alpha[move.x] + std::string(", ") + std::to_string(move.y + 1);
		drawText(window, text, str, pos.x + (size.x / 4), pos.y + (size.y * 0.6),
				20, sf::Color::White, MID_CENTER);

		str = format_time(std::to_string(ai->getTimer())) + " us";
		drawText(window, text, str, pos.x + (size.x / 2), pos.y + (size.y * 0.6),
				20, sf::Color::White, MID_CENTER);

		avg = ai->getTotalTimer() / (this->playerLeft.getNbMove() + 1);
		str = format_time(std::to_string(avg)) + " us";
		drawText(window, text, str, pos.x + (size.x / 4 * 3), pos.y + (size.y * 0.6),
				20, sf::Color::White, MID_CENTER);
	}
	else if (this->playerRight.getSuggestDone())
	{
		ai = this->playerRight.getAI();
		move = this->playerRight.getSuggestedMove();

		str = alpha[move.x] + std::string(", ") + std::to_string(move.y + 1);
		drawText(window, text, str, pos.x + (size.x / 4), pos.y + (size.y * 0.6),
				20, sf::Color::White, MID_CENTER);

		str = format_time(std::to_string(ai->getTimer())) + " us";
		drawText(window, text, str, pos.x + (size.x / 2), pos.y + (size.y * 0.6),
				20, sf::Color::White, MID_CENTER);

		avg = ai->getTotalTimer() / (this->playerRight.getNbMove() + 1);
		str = format_time(std::to_string(avg)) + " us";
		drawText(window, text, str, pos.x + (size.x / 4 * 3), pos.y + (size.y * 0.6),
				20, sf::Color::White, MID_CENTER);
	}

	this->rect.setSize(sf::Vector2f(WIN_W * 0.15, this->grid.getH()));
}
