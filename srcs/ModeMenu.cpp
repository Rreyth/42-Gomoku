#include <ModeMenu.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

ModeMenu::ModeMenu(void)
{
	this->title = "MODE MENU";

	this->play = Button("Play", 40, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 100, WIN_H / 3 * 2, 200, 80,
						SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF);

	this->back = Button("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);

	std::vector<std::string>	playerTypes;
	playerTypes.push_back("Humain");
	playerTypes.push_back("Robot");

	this->playerLeft = Select(playerTypes, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 - 250, WIN_H / 2 - 40, 200, 80,
								SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF,
								SPRITE_ROUND_BUTTON_ON);

	this->playerRight = Select(playerTypes, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 + 50, WIN_H / 2 - 40, 200, 80,
								SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF,
								SPRITE_ROUND_BUTTON_ON);

	std::vector<std::string>	gamemodeTypes;
	gamemodeTypes.push_back("Normal");
	gamemodeTypes.push_back("Blitz");

	this->gamemode = Select(gamemodeTypes, 40, MID_CENTER, sf::Color::White,
								WIN_W - 250, WIN_H / 2 - 40, 200, 80,
								SPRITE_ROUND_BUTTON_ON, SPRITE_ROUND_BUTTON_OFF,
								SPRITE_ROUND_BUTTON_ON);
}

ModeMenu::~ModeMenu()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	ModeMenu::tick(display_state *displayState, float delta, Mouse *mouse, Game *game)
{
	this->play.tick(mouse);
	this->back.tick(mouse);
	this->playerLeft.tick(mouse);
	this->playerRight.tick(mouse);
	this->gamemode.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
	else if (this->play.getPressed())
	{
		game->setGame((player_type)this->playerLeft.getSelected(),
						(player_type)this->playerRight.getSelected(),
						(game_mode)this->gamemode.getSelected());
		*displayState = DISPLAY_GAME;
	}
}


void	ModeMenu::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	this->play.draw(window, text, textureManager);
	this->back.draw(window, text, textureManager);
	this->playerLeft.draw(window, text, textureManager);
	this->playerRight.draw(window, text, textureManager);
	this->gamemode.draw(window, text, textureManager);
}
