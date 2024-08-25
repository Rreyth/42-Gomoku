#include <define.hpp>
#include <Functions.hpp>
#include <TextureManager.hpp>
#include <Mouse.hpp>
#include <Button.hpp>
#include <Menu.hpp>
#include <Settings.hpp>
#include <ModeMenu.hpp>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdexcept>

int	main(void)
{
	// Initialisation
	sf::RenderWindow	window(sf::VideoMode(WIN_W, WIN_H), "42 :)");
	sf::Clock			clock;
	TextureManager		textureManager;
	Mouse				mouse;
	sf::Font			font;
	sf::Text			text;
	display_state		displayState = DISPLAY_MENU;
	Menu				menu;
	Settings			settings;
	ModeMenu			modeMenu;

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
			settings.tick(&displayState, delta, &mouse);
			settings.draw(&window, &text, &textureManager);
			break;
		case DISPLAY_MODEMENU:
			modeMenu.tick(&displayState, delta, &mouse);
			modeMenu.draw(&window, &text, &textureManager);
			break;
		}

		window.display();

		if (displayState == DISPLAY_QUIT)
			window.close();
	}

	return 0;
}
