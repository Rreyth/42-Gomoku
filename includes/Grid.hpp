#ifndef GRID_HPP
# define GRID_HPP

# include <define.hpp>
# include <Button.hpp>
# include <Player.hpp>

class Grid
{
public:
	Grid(void);
	~Grid();

	void			tick(Mouse *mouse, Player *leftPlayer, Player *rightPlayer, std::string *turn);
	void			draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	int 			getW(void);
	int 			getH(void);
	int				getX(void);
	int				getY(void);

private:
	int				w;
	int				h;
	int 			x;
	int 			y;

	sf::Vector2i	closestIntersection(int x, int y);
};

#endif
