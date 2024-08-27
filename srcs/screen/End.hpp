#ifndef END_HPP
# define END_HPP

# include <define.hpp>
# include <ui/Button.hpp>

class End
{
public:
	End(void);
	~End();

	void	tick(display_state *displayState, float delta, Mouse *mouse);
	void	draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	std::string	title;
	Button		back;
};

#endif
