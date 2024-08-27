#include <screen/Settings.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Settings::Settings(void)
{
	this->title = "SETTINGS";

	this->back = Button("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
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
	this->back.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
}


void	Settings::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	this->back.draw(window, text, textureManager);
}
