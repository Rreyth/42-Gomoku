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
	sf::RenderWindow	window(sf::VideoMode(WIN_W, WIN_H), "42 :)", sf::Style::Titlebar | sf::Style::Close);
	sf::View			view(sf::FloatRect(0, 0, WIN_W, WIN_H));
	sf::Clock			clock;
	TextureManager		textureManager;
	Mouse				mouse;
	sf::Font			font;
	sf::Text			text;
	display_state		displayState = DISPLAY_MENU;
	Menu				menu;
	Settings			settings;
	ModeMenu			modeMenu;
	Game				game;
	End					end;

	try
	{
		textureManager.loadTextures();
		if (!font.loadFromFile(FONT_PATH))
			throw std::invalid_argument("");
	}
	catch (const std::exception &e)
	{
		window.close();
		return (1);
	}

	window.setFramerateLimit(1000);
	window.setVerticalSyncEnabled(false);
	window.setView(view);
	text.setFont(font);

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
			menu.tick(&displayState, delta, &mouse);
			menu.draw(&window, &text, &textureManager);
			break;
		case DISPLAY_SETTINGS:
			settings.tick(&displayState, delta, &mouse, &window, &view);
			settings.draw(&window, &text, &textureManager);
			break;
		case DISPLAY_MODEMENU:
			modeMenu.tick(&displayState, delta, &mouse, &game);
			modeMenu.draw(&window, &text, &textureManager);
			break;
		case DISPLAY_GAME:
			game.tick(&displayState, delta, &mouse);
			game.draw(&window, &text, &textureManager);
			break;
		case DISPLAY_END:
			end.tick(&displayState, delta, &mouse);
			end.draw(&window, &text, &textureManager,
						game.getLeftPlayer(), game.getRightPlayer(),
						game.getGrid());
			break;
		}

		window.display();

		if (displayState == DISPLAY_QUIT)
			window.close();
	}

	return 0;
}
