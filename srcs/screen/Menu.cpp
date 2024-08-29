#include <screen/Menu.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Menu::Menu(void)
{
	this->title = "GOMOCOOL";

	this->play = Button("Play", 40, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 150, WIN_H / 2 - 40, 300, 60,
						SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF);

	this->settings = Button("", 25, MID_CENTER, sf::Color::White,
						WIN_W - 70, WIN_H - 70, 50, 50,
						SPRITE_GEAR_BUTTON_ON, SPRITE_GEAR_BUTTON_OFF);

	this->quit = Button("QUIT", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);
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
	else if (this->settings.getPressed())
		*displayState = DISPLAY_SETTINGS;
	else if (this->play.getPressed())
		*displayState = DISPLAY_MODEMENU;
}


void	Menu::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	this->play.draw(window, text, textureManager);
	this->settings.draw(window, text, textureManager);
	this->quit.draw(window, text, textureManager);
}
