#ifndef MENU_HPP
# define MENU_HPP

# include <define.hpp>
# include <Button.hpp>

class Menu
{
public:
	Menu(void);
	~Menu();

	void	tick(display_state *displayState, float delta, Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text);

private:
	std::string	title;
	Button		play, settings, quit;
};

#endif
