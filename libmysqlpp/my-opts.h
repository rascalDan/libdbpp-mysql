#ifndef MYSQL_OPTS_H
#define MYSQL_OPTS_H

#include <boost/optional.hpp>
#include <string>
#include <visibility.h>

namespace MySQL {
	typedef boost::optional<std::string> OptString;

	DLL_PUBLIC const char * operator~(const OptString & os);
}

namespace std {
	template <typename T>
	DLL_PUBLIC std::istream & operator>>(std::istream & s, boost::optional<T> & o);
	template <typename T>
	DLL_PUBLIC std::istream & operator>>(std::istream & s, std::vector<T> & o);
}

#endif

