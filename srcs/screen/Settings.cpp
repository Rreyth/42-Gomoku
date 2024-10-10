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

	std::vector<std::string>	res;
	res.push_back("1600x900");
	res.push_back("1920x1080");
	res.push_back("2560x1440");

	this->resolution = Select(res, 30, MID_CENTER, sf::Color::White,
						WIN_W / 4 - 95, WIN_H * 0.45, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF, SPRITE_SQUARE_BUTTON_ON);
	this->currentResolutionId = 0;

	res.clear();
	res.push_back("Default");
	res.push_back("Amogus");
	res.push_back("Coins");

	this->stones = Select(res, 30, MID_CENTER, sf::Color::White,
						WIN_W / 4 * 2 - 95, WIN_H * 0.45, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF, SPRITE_SQUARE_BUTTON_ON);
	this->currentStoneId = 0;

	res.clear();
	res.push_back("Squada One");
	res.push_back("Roboto");
	res.push_back("Sankofa");

	this->fonts = Select(res, 30, MID_CENTER, sf::Color::White,
						WIN_W / 4 * 2 - 95, WIN_H * 0.65, 190, 48,
						SPRITE_SQUARE_BUTTON_ON, SPRITE_SQUARE_BUTTON_OFF, SPRITE_SQUARE_BUTTON_ON);
	this->currentFontId = 0;

	this->fullscreen = ToggleButton("", 0, TOP_LEFT, sf::Color::White,
									WIN_W / 4 + 70, WIN_H * 0.6625, 24, 24,
									SPRITE_SMALL_ROUND_BUTTON_ON, SPRITE_SMALL_ROUND_BUTTON_OFF,
									SPRITE_SMALL_ROUND_BUTTON_ON);

	this->suggestion = ToggleButton("", 0, TOP_LEFT, sf::Color::White,
									WIN_W / 4 * 3 + 70, WIN_H * 0.54, 24, 24,
									SPRITE_SMALL_ROUND_BUTTON_ON, SPRITE_SMALL_ROUND_BUTTON_OFF,
									SPRITE_SMALL_ROUND_BUTTON_ON);

}

Settings::~Settings()
{
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////
void	Settings::tick(Game *game, display_state *displayState, float delta, Mouse *mouse, sf::RenderWindow *window, sf::View *view, sf::Text *text, sf::Font *font, stone_sprite *sprite)
{
	this->back.tick(mouse);
	this->resolution.tick(mouse);
	this->fullscreen.tick(mouse);
	this->suggestion.tick(mouse);
	this->stones.tick(mouse);
	this->fonts.tick(mouse);

	if (this->back.getPressed())
		*displayState = DISPLAY_MENU;

	this->updateWindow(window, view);
	this->updateFont(text, font);
	this->updateStone(sprite);
	if (this->suggestion.getPressed())
		this->updateSuggestion(game);
}


void	Settings::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	drawText(window, text, this->title, WIN_W / 2, 20, 192, sf::Color::White, TOP_CENTER);
	this->back.draw(window, text, textureManager);

	drawText(window, text, "Resolution", WIN_W / 4, WIN_H * 0.4, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "Stones", WIN_W / 4 * 2, WIN_H * 0.4, 50, sf::Color::White, MID_CENTER);
	drawText(window, text, "Font", WIN_W / 4 * 2, WIN_H * 0.6, 50, sf::Color::White, MID_CENTER);

	drawText(window, text, "Fullscreen", WIN_W * 0.245, WIN_H * 0.67, 35, sf::Color::White, MID_CENTER);
	this->fullscreen.draw(window, text, textureManager);

	drawText(window, text, "AI suggestion", WIN_W * 0.73, WIN_H * 0.5525, 35, sf::Color::White, MID_CENTER);
	this->suggestion.draw(window, text, textureManager);

	this->fonts.draw(window, text, textureManager);
	this->stones.draw(window, text, textureManager);
	this->resolution.draw(window, text, textureManager);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

void	Settings::updateWindow(sf::RenderWindow *window, sf::View *view)
{
	int	resId = this->resolution.getSelected();
	if (resId != this->currentResolutionId)
	{
		switch (resId)
		{
		case 0:
			view->zoom(1);
			window->setSize(sf::Vector2u(1600, 900));
			break;
		case 1:
			view->zoom(1920 / 1600);
			window->setSize(sf::Vector2u(1920, 1080));
			break;
		case 2:
			view->zoom(2560 / 1600);
			window->setSize(sf::Vector2u(2560, 1440));
			break;
		}
		window->setFramerateLimit(MAX_FPS);
		window->setView(*view);
		this->currentResolutionId = resId;
	}

	if (this->fullscreen.getPressed())
	{
		int	w, h;

		if (this->currentResolutionId == 0)
		{
			w = 1600;
			h = 900;
		}
		else if (this->currentResolutionId == 1){
			w = 1920;
			h = 1080;
		}
		else {
			w = 2560;
			h = 1440;
		}

		if (this->fullscreen.getToggle())
			window->create(sf::VideoMode(w, h), "Gomocool", sf::Style::None);
		else
			window->create(sf::VideoMode(w, h), "Gomocool", sf::Style::Titlebar | sf::Style::Close);
		window->setPosition(sf::Vector2i(0, 0));
		window->setFramerateLimit(MAX_FPS);
		window->setView(*view);
	}
}

void	Settings::updateFont(sf::Text *text, sf::Font *font)
{
	int	fontId = this->fonts.getSelected();

	if (fontId != this->currentFontId)
	{
		text->setFont(font[fontId]);
		this->currentFontId = fontId;
	}
}

void	Settings::updateStone(stone_sprite *sprite)
{
	int	stoneId = this->stones.getSelected();

	if (stoneId != this->currentStoneId)
	{
		*sprite = (stone_sprite)stoneId;
		this->currentStoneId = stoneId;
	}
}

void	Settings::updateSuggestion(Game *game)
{
	game->setSuggestion(this->suggestion.getToggle());
}