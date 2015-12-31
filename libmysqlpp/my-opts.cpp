#include "my-opts.h"

namespace MySQL {
	const char *
	operator~(const OptString & os)
	{
		if (os) {
			return os->c_str();
		}
		return NULL;
	}
}

namespace std {
	template <typename T>
	std::istream &
	operator>>(std::istream & s, std::vector<T> & o)
	{
		o.push_back(T());
		return (s >> o.back());
	}

	template <typename T>
	std::istream &
	operator>>(std::istream & s, boost::optional<T> & o)
	{
		o = T();
		return (s >> *o);
	}

	template std::istream & operator>>(std::istream & s, boost::optional<std::string> & o);
	template std::istream & operator>>(std::istream & s, std::vector<std::string> & o);
}

