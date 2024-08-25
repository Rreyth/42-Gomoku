#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <Button.hpp>

class Grid
{
public:
	Grid(void);
	~Grid();

	void			tick(Mouse *mouse); //add player
	void			draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);

private:
	int				w;
	int				h;
	int 			x;
	int 			y;

	sf::Vector2i	closestIntersection(int x, int y);
};

#endif
