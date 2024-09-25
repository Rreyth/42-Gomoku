#include <game/Bbox.hpp>
#include <utils/Functions.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Bbox::Bbox( void )
{
	this->Lx = -1;
	this->Rx = -1;
	this->Ly = -1;
	this->Ry = -1;
}


Bbox::Bbox(const Bbox &obj)
{
	this->Lx = obj.Lx;
	this->Rx = obj.Rx;
	this->Ly = obj.Ly;
	this->Ry = obj.Ry;
}

//---- Destructor --------------------------------------------------------------

Bbox::~Bbox( void )
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Bbox	&Bbox::operator=(const Bbox &obj)
{
	if (this == &obj)
		return (*this);

	this->Lx = obj.Lx;
	this->Rx = obj.Rx;
	this->Ly = obj.Ly;
	this->Ry = obj.Ry;

	return (*this);
}


bool	Bbox::operator==(const Bbox &obj)
{
	return (this->Lx == obj.Lx && this->Rx == obj.Rx &&
				this->Ly == obj.Ly && this->Ry == obj.Ry);
}

//**** PUBLIC METHODS **********************************************************

void	Bbox::update(int x, int y)
{
	if (this->Lx == -1)
	{
		this->Lx = max(x - 1, 0);
		this->Ly = max(y - 1, 0);
		this->Rx = min(x + 1, GRID_W_INTER - 1);
		this->Ry = min(y + 1, GRID_W_INTER - 1);
		return ;
	}

	if (x <= this->Lx)
		this->Lx = max(x - 1, 0);

	else if (x >= this->Rx)
		this->Rx = min(x + 1, GRID_W_INTER - 1);

	if (y <= this->Ly)
		this->Ly = max(y - 1, 0);

	else if (y >= this->Ry)
		this->Ry = min(y + 1, GRID_NB_INTER - 1);
}


void	Bbox::merge(const Bbox *bbox)
{
	if (bbox->Lx <= this->Lx)
		this->Lx = bbox->Lx;

	if (bbox->Rx >= this->Rx)
		this->Rx = bbox->Rx;

	if (bbox->Ly <= this->Ly)
		this->Ly = bbox->Ly;

	if (bbox->Ry >= this->Ry)
		this->Ry = bbox->Ry;
}


bool	Bbox::isInside(int x, int y) const
{
	return (x >= this->Lx && x <= this->Rx && y >= this->Ly && y <= this->Ry);
}


bool	Bbox::collideBbox(const Bbox *bbox) const
{
	return (bbox->Rx >= this->Lx && bbox->Lx <= this->Rx
				&& bbox->Ry >= this->Ly && bbox->Ly <= this->Ry);
}


bool	Bbox::containsBbox(const Bbox *bbox) const
{
	return (this->Lx <= bbox->Lx && this->Rx >= bbox->Rx
				&& this->Ly <= bbox->Ly && this->Ry >= bbox->Ry);
}


//**** PRIVATE METHODS *********************************************************
