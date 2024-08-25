#include <Game.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Game::Game(void)
{
	this->turn = "<NAME> TURN";

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
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Game::tick(display_state *displayState, float delta, Mouse *mouse)
{
	this->leave.tick(mouse);
	this->grid.tick(mouse);

	this->playerLeft.tick(delta, this->mode);
	this->playerRight.tick(delta, this->mode);

	if (this->leave.getPressed())
		*displayState = DISPLAY_MENU;
}


void	Game::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->turn, WIN_W / 2, 20, 60, sf::Color::White, TOP_CENTER);

	//draw board
	//draw stones
	grid.draw(window, text, textureManager);

	//draw players infos
	//draw ai infos

	this->drawLeftSide(window, text, textureManager);
	this->drawRightSide(window, text, textureManager);
	this->drawBottom(window, text, textureManager);

	this->leave.draw(window, text, textureManager);
}

void	Game::setGame(player_type playerLeft, player_type playerRight, game_mode mode)
{
	this->mode = mode;

	this->playerLeft.setType(playerLeft);
	this->playerLeft.setTimer(this->mode);

	this->playerRight.setType(playerRight);
	this->playerRight.setTimer(this->mode);
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
	drawText(window, text, "PLAYER 1", size.x / 2, gridY + (size.y * 0.05), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, "stone sprite", size.x / 2, gridY + (size.y * 0.4), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, "captured stones", size.x / 2, gridY + (size.y * 0.8), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIMER", size.x / 2, gridY + (size.y * 0.95), 30, sf::Color::White, MID_CENTER);
}

void	Game::drawRightSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	int gridY = this->grid.getY();

	this->rect.setPosition(WIN_W * 0.85 - 2, gridY);
	window->draw(this->rect);

	sf::Vector2f size = this->rect.getSize();
	drawText(window, text, "PLAYER 2", WIN_W - (size.x / 2), gridY + (size.y * 0.05), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, "stone sprite", WIN_W - (size.x / 2), gridY + (size.y * 0.4), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, "captured stones", WIN_W - (size.x / 2), gridY + (size.y * 0.8), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, "TIMER", WIN_W - (size.x / 2), gridY + (size.y * 0.95), 30, sf::Color::White, MID_CENTER);
}

void Game::drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	int gridX = this->grid.getX();

	this->rect.setSize(sf::Vector2f(this->grid.getW(), WIN_H * 0.1));
	this->rect.setPosition(gridX, WIN_H * 0.9 - 2);

	window->draw(this->rect);

	sf::Vector2f size = this->rect.getSize();
	sf::Vector2f pos = this->rect.getPosition();
	drawText(window, text, "AI PREDICT", pos.x + (size.x / 2), pos.y + 5, 20, sf::Color::White, MID_CENTER);
	drawText(window, text, "AI SPRITE", pos.x + 10, pos.y + (size.y / 2), 15, sf::Color::White, MID_LEFT);
	drawText(window, text, "PREDICTED POS", pos.x + (size.x / 2), pos.y + (size.y / 2), 15, sf::Color::White, MID_CENTER);



	this->rect.setSize(sf::Vector2f(WIN_W * 0.15, this->grid.getH()));

}
