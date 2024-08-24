#ifndef LOADEXCEPTION_HPP
# define LOADEXCEPTION_HPP

# include <string>
# include <stdexcept>

class LoadException : public std::invalid_argument
{
public:
	const char	*what( void ) const throw();
};

#endif
