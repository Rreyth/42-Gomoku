#ifndef LOADEXCEPTION_HPP
# define LOADEXCEPTION_HPP

# include <string>
# include <stdexcept>

class LoadException : public std::exception
{
public:
	LoadException(std::string error);
	~LoadException();

	const char	*what( void ) const throw();

private:
	std::string error;
};

#endif
