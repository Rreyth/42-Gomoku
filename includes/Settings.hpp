#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <define.hpp>
# include <Button.hpp>

class Settings
{
public:
	Settings(void);
	~Settings();

	void	tick(display_state *displayState, float delta, Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	std::string	title;
	Button		back;
};

#endif
