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
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Game::tick(display_state *displayState, float delta, Mouse *mouse)
{
	this->leave.tick(mouse);
	this->grid.tick(displayState, mouse, &this->playerLeft, &this->playerRight);

	this->playerLeft.tick(delta, this->mode);
	this->playerRight.tick(delta, this->mode);

	if (this->leave.getPressed())
		*displayState = DISPLAY_MENU;
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

void	Game::setGame(player_type playerLeft, player_type playerRight, game_mode mode)
{
	this->mode = mode;

	if ((playerLeft == PLAYER && playerRight == PLAYER)||
		(playerLeft == AI && playerRight == AI))
	{
		this->playerLeft.setPlayer(playerLeft, mode, 1);
		this->playerRight.setPlayer(playerRight, mode, 2);
	}
	else
	{
		this->playerLeft.setPlayer(playerLeft, mode, 0);
		this->playerRight.setPlayer(playerRight, mode, 0);
	}
	if (rand_int(1, 2) == 1)
		this->playerLeft.setPlaying(true);
	else
		this->playerRight.setPlaying(true);

	this->grid.clearGrid();
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

	drawText(window, text, "stone sprite", size.x / 2, gridY + (size.y * 0.4), 30, sf::Color::White, MID_CENTER);

	str = std::to_string(this->playerLeft.getCaptured());
	drawText(window, text, "captured stones", size.x / 2, gridY + (size.y * 0.75), 25, sf::Color::White, MID_CENTER);
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

	drawText(window, text, "stone sprite", WIN_W - (size.x / 2), gridY + (size.y * 0.4), 30, sf::Color::White, MID_CENTER);

	str = std::to_string(this->playerRight.getCaptured());
	drawText(window, text, "captured stones", WIN_W - (size.x / 2), gridY + (size.y * 0.75), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.80), 30, sf::Color::White, MID_CENTER);

	str = std::to_string((int)this->playerRight.getTimer()) + " s";
	drawText(window, text, "TIMER", WIN_W - (size.x / 2), gridY + (size.y * 0.90), 30, sf::Color::White, MID_CENTER);
	drawText(window, text, str, WIN_W - (size.x / 2), gridY + (size.y * 0.95), 30, sf::Color::White, MID_CENTER);
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
