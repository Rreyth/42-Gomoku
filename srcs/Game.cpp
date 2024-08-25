#include <Game.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Game::Game(void)
{
	this->leave.setProperties("LEAVE", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);
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

	if (this->leave.getPressed())
		*displayState = DISPLAY_MENU;
}


void	Game::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	// drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);

	//draw board
	//draw stones
	grid.draw(window, text, textureManager);

	//draw players infos
	//draw ai infos

	//draw left side (p1)
	//draw right side (p2)

	//draw buttons
	this->leave.draw(window, text, textureManager);
}
