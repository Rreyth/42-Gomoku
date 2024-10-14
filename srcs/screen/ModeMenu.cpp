#include <screen/ModeMenu.hpp>
#include <utils/Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

ModeMenu::ModeMenu(void)
{
	this->title = "MODE MENU";

	this->suggestion = false;

	this->play = Button("Play", 40, MID_CENTER, sf::Color::White,
						WIN_W * 0.795, WIN_H * 0.9, 300, 60,
						SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF);

	this->back = Button("BACK TO MENU", 25, MID_CENTER, sf::Color::White,
						25, WIN_H * 0.92, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF);

	std::vector<std::string>	vec;
	vec.push_back("Player");
	vec.push_back("AI");

	this->playerLeft = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 - 350, WIN_H / 3, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	this->playerRight = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 + 50, WIN_H / 3, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	vec.clear();
	vec.push_back("Normal");
	vec.push_back("Blitz");

	this->gamemode = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W - 350, WIN_H / 3, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	vec.clear();
	vec.push_back("Standard");
	vec.push_back("Pro");

	this->gamerules = Select(vec, 40, MID_CENTER, sf::Color::White,
								50, WIN_H / 3, 300, 60,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON, SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_SQUARE_ROUNDED_BUTTON_ON);

	vec.clear();
	vec.push_back("RANDOM");
	vec.push_back("BETTER_RANDOM");
	vec.push_back("EASY");
	vec.push_back("MEDIUM");
	vec.push_back("HARD");
	vec.push_back("MTDF");

	this->aiLeft = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W * 0.3 - 150, WIN_H * 0.565, 300, 45,
								SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON, SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON);

	this->aiRight = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W * 0.7 - 150, WIN_H * 0.565, 300, 45,
								SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON, SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON);

	vec.clear();
	vec.push_back("RANDOM");
	vec.push_back("PLAYER 1");
	vec.push_back("PLAYER 2");

	this->starting = Select(vec, 40, MID_CENTER, sf::Color::White,
								WIN_W / 2 - 150, WIN_H * 0.73, 300, 45,
								SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON, SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_OFF,
								SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON);

	// TODO: REMOVE
	// this->playerLeft.setSelected(1);
	// this->playerRight.setSelected(1);
	// this->aiLeft.setSelected(2);
	// this->aiRight.setSelected(5);
	this->starting.setSelected(1);
}

ModeMenu::~ModeMenu()
{
}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

void	ModeMenu::setSuggestion(bool suggestion)
{
	this->suggestion = suggestion;
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
	this->aiLeft.tick(mouse);
	this->aiRight.tick(mouse);
	this->starting.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;
	else if (this->play.getPressed())
	{
		game->setGame((player_type)this->playerLeft.getSelected(),
						(player_type)this->playerRight.getSelected(),
						(game_mode)this->gamemode.getSelected(),
						(game_rules)this->gamerules.getSelected(),
						(AI_difficulty)this->aiLeft.getSelected(),
						(AI_difficulty)this->aiRight.getSelected(),
						(starter)this->starting.getSelected(),
						sprite, this->suggestion);
		*displayState = DISPLAY_GAME;
	}
}


void	ModeMenu::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 0, 150, sf::Color::White, TOP_CENTER);
	this->play.draw(window, text, textureManager);
	this->back.draw(window, text, textureManager);

	drawText(window, text, "Who starts", WIN_W / 2, WIN_H * 0.7, 30, sf::Color::White, MID_CENTER);
	this->starting.draw(window, text, textureManager);

	if (this->playerLeft.getSelected() == AI_PLAYER)
	{
		drawText(window, text, "AI difficulty", WIN_W * 0.3, WIN_H * 0.535, 30, sf::Color::White, MID_CENTER);
		this->aiLeft.draw(window, text, textureManager);
	}

	if (this->playerRight.getSelected() == AI_PLAYER)
	{
		drawText(window, text, "AI difficulty", WIN_W * 0.7, WIN_H * 0.535, 30, sf::Color::White, MID_CENTER);
		this->aiRight.draw(window, text, textureManager);
	}

	drawText(window, text, "Player 1", WIN_W * 0.375, WIN_H / 3 - 60, 50, sf::Color::White, MID_CENTER);
	this->playerLeft.draw(window, text, textureManager);
	drawText(window, text, "Player 2", WIN_W * 0.625, WIN_H / 3 - 60, 50, sf::Color::White, MID_CENTER);
	this->playerRight.draw(window, text, textureManager);
	drawText(window, text, "Game mode", WIN_W * 0.875, WIN_H / 3 - 60, 50, sf::Color::White, MID_CENTER);
	this->gamemode.draw(window, text, textureManager);
	drawText(window, text, "Game rules", WIN_W * 0.125, WIN_H / 3 - 60, 50, sf::Color::White, MID_CENTER);
	this->gamerules.draw(window, text, textureManager);
}
