#include <define.hpp>
#include <Functions.hpp>
#include <TextureManager.hpp>
#include <Mouse.hpp>

#include <SFML/Graphics.hpp>
#include <iostream>

int	main(void)
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "42 :)");

	window.setFramerateLimit(1000);

	sf::Clock clock;

	TextureManager	textureManager;
	Mouse			mouse;

	sf::Font font;
	if (!font.loadFromFile("data/fonts/Roboto.ttf"))
	{
		// erreur...
	}

	sf::Text text;
	text.setFont(font);

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// Event part
		sf::Event event;
		mouse.updatePosition();
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
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
		window.clear(sf::Color(150, 150, 150));

		// draw everything here...
		// window.draw(...);
		textureManager.drawTexture(&window, SPRITE_GRID, WIN_W / 2, WIN_H / 2, MID_CENTER);

		drawText(&window, text, "Hello there", 100, 100, 20, sf::Color(255, 0, 10), MID_CENTER);

		// end the current frame
		window.display();
	}

	return 0;
}
