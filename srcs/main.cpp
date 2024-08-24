#include <SFML/Graphics.hpp>
#include <iostream>

#include <define.hpp>
#include <Functions.hpp>
#include <TextureManager.hpp>

int	main(void)
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "42 :)");

	window.setFramerateLimit(1000);

	sf::Clock clock;

	TextureManager	textureManager;

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
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		float delta = clock.restart().asSeconds();
		float fps = 1.0f / delta;
		std::string title(std::to_string(fps));
		window.setTitle(title);

		// clear the window with black color
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
