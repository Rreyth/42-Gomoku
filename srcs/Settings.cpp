#include <Settings.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Settings::Settings(void)
{
	this->title = "SETTINGS";

	// this->play.setProperties("Play", 40, MID_CENTER, sf::Color::White,
	// 					WIN_W / 2 - 100, WIN_H / 2 - 40, 200, 80,
	// 					SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF);

	// this->settings.setProperties("", 25, MID_CENTER, sf::Color::White,
	// 					WIN_W - 70, WIN_H - 70, 50, 50,
	// 					SPRITE_GEAR_BUTTON_ON, SPRITE_GEAR_BUTTON_OFF);

	this->back.setProperties("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);
}

Settings::~Settings()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	Settings::tick(display_state *displayState, float delta, Mouse *mouse)
{
	// this->play.tick(mouse);
	// this->settings.tick(mouse);
	this->back.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
}


void	Settings::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	// this->play.draw(window, text, textureManager);
	// this->settings.draw(window, text, textureManager);
	this->back.draw(window, text, textureManager);
}
