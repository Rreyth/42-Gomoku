#include <define.hpp>
#include <Functions.hpp>
#include <TextureManager.hpp>
#include <Mouse.hpp>
#include <Button.hpp>

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

	Button but("Test", 20, MID_CENTER, sf::Color(10, 10, 100),
				500, 100, 100, 40, sf::Color(100, 100, 100), sf::Color(200, 200, 200),
				2, sf::Color::Black);
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

		but.tick(&mouse);
		if (but.getPressed())
			std::cout << "General Kenobi\n";

		// Draw part
		window.clear(sf::Color(150, 150, 150));

		textureManager.drawTexture(&window, SPRITE_GRID, WIN_W / 2, WIN_H / 2, MID_CENTER);
		drawText(&window, &text, "Hello there", 100, 100, 20, sf::Color(255, 0, 10), MID_CENTER);
		but.draw(&window, &text);

		window.display();
	}

	return 0;
}
