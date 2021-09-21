#include "my-error.h"
#include <mysql.h>

MySQL::Error::Error(MYSQL * m) : msg(mysql_error(m)) { }

MySQL::Error::Error(MYSQL_STMT * m) : msg(mysql_stmt_error(m)) { }

std::string
MySQL::Error::message() const noexcept
{
	return msg;
}
