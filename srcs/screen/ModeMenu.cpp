#include <screen/ModeMenu.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

ModeMenu::ModeMenu(void)
{
	this->title = "MODE MENU";

	this->play = Button("Play", 40, MID_CENTER, sf::Color::White,
						WIN_W / 2 - 150, WIN_H / 3 * 2, 300, 60,
						SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF);

	this->back = Button("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						20, WIN_H - 60, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);

	std::vector<std::string>	vec;
	vec.push_back("Player");
	vec.push_back("AI");

	this->playerLeft = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 - 350, WIN_H / 2 - 40, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	this->playerRight = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 + 50, WIN_H / 2 - 40, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	vec.clear();
	vec.push_back("Normal");
	vec.push_back("Blitz");

	this->gamemode = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W - 350, WIN_H / 2 - 40, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	vec.clear();
	vec.push_back("Standard");
	vec.push_back("Pro");

	this->gamerules = Select(vec, 40, MID_CENTER, sf::Color::White,
								50, WIN_H / 2 - 40, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);
}

ModeMenu::~ModeMenu()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	ModeMenu::tick(display_state *displayState, float delta, Mouse *mouse, Game *game, stone_sprite *sprite)
{
	this->play.tick(mouse);
	this->back.tick(mouse);
	this->playerLeft.tick(mouse);
	this->playerRight.tick(mouse);
	this->gamemode.tick(mouse);
	this->gamerules.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
	else if (this->play.getPressed())
	{
		game->setGame((player_type)this->playerLeft.getSelected(),
						(player_type)this->playerRight.getSelected(),
						(game_mode)this->gamemode.getSelected(),
						(game_rules)this->gamerules.getSelected(),
						sprite);
		*displayState = DISPLAY_GAME;
	}
}


void	ModeMenu::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	drawText(window, text, "Player 1", WIN_W * 0.375, WIN_H / 2 - 100, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "Player 2", WIN_W * 0.625, WIN_H / 2 - 100, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "Game mode", WIN_W * 0.875, WIN_H / 2 - 100, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "Game rules", WIN_W * 0.125, WIN_H / 2 - 100, 50, sf::Color::White, MID_CENTER);
	this->play.draw(window, text, textureManager);
	this->back.draw(window, text, textureManager);
	this->playerLeft.draw(window, text, textureManager);
	this->playerRight.draw(window, text, textureManager);
	this->gamemode.draw(window, text, textureManager);
	this->gamerules.draw(window, text, textureManager);
}
