#include <Menu.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Menu::Menu(TextureManager *textureManager)
{
	this->title = "GOMOCOOL";

	this->play.setProperties("Play", 25, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 50, WIN_H / 2 - 80, 100, 40,
						SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF,
						textureManager);

	this->settings.setProperties("Settings", 25, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 50, WIN_H / 2 - 20, 100, 40,
						SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF,
						textureManager);

	this->quit.setProperties("QUIT", 25, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 50, WIN_H / 2 + 40, 100, 40,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF,
						textureManager);
}

Menu::~Menu()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	Menu::tick(display_state *displayState, float delta, Mouse *mouse)
{
	this->play.tick(mouse);
	this->settings.tick(mouse);
	this->quit.tick(mouse);

	if (this->quit.getPressed())
		*displayState = DISPLAY_QUIT;
}


void	Menu::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	this->play.draw(window, text, textureManager);
	this->settings.draw(window, text, textureManager);
	this->quit.draw(window, text, textureManager);
}
