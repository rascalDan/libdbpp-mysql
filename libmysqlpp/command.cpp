#include "command.h"
#include "connection.h"
#include <stdlib.h>
#include <string.h>

MySQL::Command::Command(const Connection * conn, const std::string & sql) :
	DB::Command(sql),
	c(conn),
	stmt(mysql_stmt_init(&conn->conn)),
	paramsNeedBinding(false)
{
	if (!stmt) {
		fprintf(stderr, "here1\n");
		throw Error(mysql_error(&conn->conn));
	}
	if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
		fprintf(stderr, "here2\n");
		throw Error(mysql_stmt_error(stmt));
	}
	binds.resize(mysql_stmt_param_count(stmt));
	if (binds.size()) {
		paramsNeedBinding = true;
		for (Binds::iterator i = binds.begin(); i != binds.end(); ++i) {
			memset(&*i, 0, sizeof(MYSQL_BIND));
			i->buffer_type = MYSQL_TYPE_NULL;
		}
	}
}

MySQL::Command::~Command()
{
	for (Binds::const_iterator i = binds.begin(); i != binds.end(); ++i) {
		free(i->buffer);
	}
	mysql_stmt_close(stmt);
}

void *
MySQL::Command::realloc(void * buffer, size_t size)
{
	void * newBuffer = ::realloc(buffer, size);
	if (buffer != newBuffer) {
		paramsNeedBinding = true;
	}
	return newBuffer;
}

void
MySQL::Command::bindParamI(unsigned int n, int v)
{
	binds[n].buffer_type = MYSQL_TYPE_LONG;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(int));
	*static_cast<int*>(binds[n].buffer) = v;
	binds[n].is_unsigned = 0;
}
void
MySQL::Command::bindParamI(unsigned int n, long int v)
{
	binds[n].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(long long int));
	*static_cast<long long int*>(binds[n].buffer) = v;
	binds[n].is_unsigned = 0;
}
void
MySQL::Command::bindParamI(unsigned int n, long long int v)
{
	binds[n].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(long long int));
	*static_cast<long long int*>(binds[n].buffer) = v;
	binds[n].is_unsigned = 0;
}
void
MySQL::Command::bindParamI(unsigned int n, unsigned int v)
{
	binds[n].buffer_type = MYSQL_TYPE_LONG;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(int));
	*static_cast<int*>(binds[n].buffer) = v;
	binds[n].is_unsigned = 1;
}
void
MySQL::Command::bindParamI(unsigned int n, long unsigned int v)
{
	binds[n].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(long long int));
	*static_cast<long long int*>(binds[n].buffer) = v;
	binds[n].is_unsigned = 1;
}
void
MySQL::Command::bindParamI(unsigned int n, long long unsigned int v)
{
	binds[n].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(long long int));
	*static_cast<long long int*>(binds[n].buffer) = v;
	binds[n].is_unsigned = 1;
}
void
MySQL::Command::bindParamF(unsigned int n, double v)
{
	binds[n].buffer_type = MYSQL_TYPE_DOUBLE;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(double));
	*static_cast<double*>(binds[n].buffer) = v;
}
void
MySQL::Command::bindParamF(unsigned int n, float v)
{
	binds[n].buffer_type = MYSQL_TYPE_FLOAT;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(float));
	*static_cast<float*>(binds[n].buffer) = v;
}
void
MySQL::Command::bindParamS(unsigned int n, const Glib::ustring & s)
{
	binds[n].buffer_type = MYSQL_TYPE_STRING;
	binds[n].buffer = realloc(binds[n].buffer, s.bytes());
	s.copy(static_cast<char*>(binds[n].buffer), s.bytes());
	binds[n].buffer_length = s.bytes();
	binds[n].is_unsigned = 0;
}
void
MySQL::Command::bindParamT(unsigned int n, const tm * v)
{
	binds[n].buffer_type = MYSQL_TYPE_DATETIME;
	binds[n].buffer = realloc(binds[n].buffer, sizeof(MYSQL_TIME));
	MYSQL_TIME & ts = *static_cast<MYSQL_TIME*>(binds[n].buffer);
	ts.year = v->tm_year + 1900;
	ts.month = v->tm_mon + 1;
	ts.day = v->tm_mday;
	ts.hour = v->tm_hour;
	ts.minute = v->tm_min;
	ts.second = v->tm_sec;
	ts.neg = 0;
}
void
MySQL::Command::bindParamT(unsigned int n, time_t v)
{
	struct tm t;
	gmtime_r(&v, &t);
	bindParamT(n, &t);
}
void
MySQL::Command::bindNull(unsigned int n)
{
	binds[n].buffer_type = MYSQL_TYPE_NULL;
	binds[n].buffer = NULL;
	free(binds[n].buffer);
}

void
MySQL::Command::bindParams()
{
	if (paramsNeedBinding) {
		if (mysql_stmt_bind_param(stmt, &binds.front())) {
			throw Error(mysql_stmt_error(stmt));
			paramsNeedBinding = false;
		}
	}
}

