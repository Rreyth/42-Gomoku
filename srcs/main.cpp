#include <define.hpp>
#include <utils/Functions.hpp>
#include <utils/TextureManager.hpp>
#include <utils/Mouse.hpp>
#include <ui/Button.hpp>
#include <screen/Menu.hpp>
#include <screen/Settings.hpp>
#include <screen/ModeMenu.hpp>
#include <screen/Game.hpp>
#include <screen/End.hpp>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdexcept>

int	main(void)
{
	// Initialisation
	sf::RenderWindow	window(sf::VideoMode(WIN_W, WIN_H), "Gomocool", sf::Style::Titlebar | sf::Style::Close);
	sf::View			view(sf::FloatRect(0, 0, WIN_W, WIN_H));
	sf::Clock			clock;
	TextureManager		textureManager;
	Mouse				mouse;
	sf::Font			font[3];
	sf::Text			text;
	display_state		displayState = DISPLAY_MENU;
	Menu				menu;
	Settings			settings;
	ModeMenu			modeMenu;
	Game				game;
	End					end;
	stone_sprite		sprite = DEFAULT;

	try
	{
		textureManager.loadTextures();
		if (!font[0].loadFromFile(FONT_PATH_SQUADA))
			throw std::invalid_argument("");
		if (!font[1].loadFromFile(FONT_PATH_ROBOTO))
			throw std::invalid_argument("");
		if (!font[2].loadFromFile(FONT_PATH_SANKOFA))
			throw std::invalid_argument("");
	}
	catch (const std::exception &e)
	{
		window.close();
		return (1);
	}

	window.setFramerateLimit(MAX_FPS);
	window.setView(view);
	text.setFont(font[0]);

	// Main loop
	while (window.isOpen())
	{
		// Event part
		sf::Event event;
		mouse.updatePosition(&window);
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			else if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.key.code == 0)
					mouse.updateMbutton(MBUT_LEFT, true);
				else if (event.key.code == 2)
					mouse.updateMbutton(MBUT_MIDDLE, true);
				else if (event.key.code == 1)
					mouse.updateMbutton(MBUT_RIGHT, true);
			}

			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.key.code == 0)
					mouse.updateMbutton(MBUT_LEFT, false);
				else if (event.key.code == 2)
					mouse.updateMbutton(MBUT_MIDDLE, false);
				else if (event.key.code == 1)
					mouse.updateMbutton(MBUT_RIGHT, false);
			}
		}

		// Tick part
		float delta = clock.restart().asSeconds();
		float fps = 1.0f / delta;
		std::string title(std::to_string(fps));
		window.setTitle(title);

		// Draw part
		window.clear();

		switch (displayState)
		{
		case DISPLAY_MENU:
			menu.draw(&window, &text, &textureManager);
			menu.tick(&displayState, delta, &mouse);
			break;
		case DISPLAY_SETTINGS:
			settings.draw(&window, &text, &textureManager);
			settings.tick(&modeMenu, &displayState, delta, &mouse, &window, &view, &text, font, &sprite);
			break;
		case DISPLAY_MODEMENU:
			modeMenu.draw(&window, &text, &textureManager);
			modeMenu.tick(&displayState, delta, &mouse, &game, &sprite);
			break;
		case DISPLAY_GAME:
			game.draw(&window, &text, &textureManager);
			game.tick(&displayState, delta, &mouse);
			break;
		case DISPLAY_END:
			end.draw(&window, &text, &textureManager,
						game.getLeftPlayer(), game.getRightPlayer(),
						game.getGrid());
			end.tick(&displayState, delta, &mouse, &game);
			break;
		}

		window.display();

		if (displayState == DISPLAY_QUIT)
			window.close();
	}

	return 0;
}
