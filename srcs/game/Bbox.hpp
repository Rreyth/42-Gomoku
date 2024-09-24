#ifndef BBOX_HPP
# define BBOX_HPP

class Bbox
{
public:
	int	Lx, Rx, Ly, Ry;

	Bbox(void);
	Bbox(const Bbox &obj);
	~Bbox();

	Bbox	&operator=(const Bbox &obj);
	bool	operator==(const Bbox &obj);

	void	update(int x, int y);
	void	merge(const Bbox *bbox);
	bool	isInside(int x, int y) const;
	bool	collideBbox(const Bbox *bbox) const;
	bool	containsBbox(const Bbox *bbox) const;

private:
};

#endif
