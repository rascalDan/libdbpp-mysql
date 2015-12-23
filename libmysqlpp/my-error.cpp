#include "my-error.h"
#include <string.h>

MySQL::Error::Error() :
	msg(NULL)
{
}

MySQL::Error::Error(const MySQL::Error & e) :
	msg(e.msg ? strdup(e.msg) : NULL)
{
}

MySQL::Error::Error(const char * e) :
	msg(e ? strdup(e) : NULL)
{
}

MySQL::Error::~Error() throw()
{
	free(msg);
}

const char *
MySQL::Error::what() const throw()
{
	return msg ? msg : "No message";
}

