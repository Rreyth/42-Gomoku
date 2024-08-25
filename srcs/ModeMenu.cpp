#include <ModeMenu.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

ModeMenu::ModeMenu(void)
{
	this->title = "MODE MENU";

	this->play.setProperties("Play", 40, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 100, WIN_H / 3 * 2, 200, 80,
						SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF);

	this->back.setProperties("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);
}

ModeMenu::~ModeMenu()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	ModeMenu::tick(display_state *displayState, float delta, Mouse *mouse)
{
	this->play.tick(mouse);
	this->back.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
	else if (this->play.getPressed())
		*displayState = DISPLAY_GAME;
}


void	ModeMenu::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	this->play.draw(window, text, textureManager);
	this->back.draw(window, text, textureManager);
}
