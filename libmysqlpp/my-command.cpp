#include "my-command.h"
#include "my-connection.h"
#include <cstdlib>
#include <cstring>
#include <boost/numeric/conversion/cast.hpp>

MySQL::Command::Command(const Connection * conn, const std::string & sql) :
	DB::Command(sql),
	c(conn),
	stmt(mysql_stmt_init(&conn->conn)),
	paramsNeedBinding(false)
{
	if (!stmt) {
		throw Error(&conn->conn);
	}
	if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
		throw Error(stmt);
	}
	binds.resize(mysql_stmt_param_count(stmt));
	if (binds.size()) {
		paramsNeedBinding = true;
		for (auto & b : binds) {
			memset(&b, 0, sizeof(MYSQL_BIND));
			b.buffer_type = MYSQL_TYPE_NULL;
		}
	}
}

MySQL::Command::~Command()
{
	for (auto & b : binds) {
		// NOLINTNEXTLINE(hicpp-no-malloc)
		free(b.buffer);
	}
	mysql_stmt_close(stmt);
}

void *
MySQL::Command::realloc(void * buffer, size_t size)
{
	// NOLINTNEXTLINE(hicpp-no-malloc)
	void * newBuffer = ::realloc(buffer, size);
	if (buffer != newBuffer) {
		paramsNeedBinding = true;
	}
	return newBuffer;
}

template<typename B, enum_field_types b, typename P>
void
bindNumber(MYSQL_BIND & bind, const P & v)
{
	bind.buffer_type = b;
	// NOLINTNEXTLINE(hicpp-no-malloc)
	bind.buffer = realloc(bind.buffer, sizeof(B));
	*static_cast<B *>(bind.buffer) = boost::numeric_cast<B>(v);
	bind.is_unsigned = std::is_unsigned<P>::value;
}

void
MySQL::Command::bindParamI(unsigned int n, int v)
{
	bindNumber<int, MYSQL_TYPE_LONG>(binds[n], v);
}
void
MySQL::Command::bindParamI(unsigned int n, long int v)
{
	bindNumber<long long int, MYSQL_TYPE_LONGLONG>(binds[n], v);
}
void
MySQL::Command::bindParamI(unsigned int n, long long int v)
{
	bindNumber<long long int, MYSQL_TYPE_LONGLONG>(binds[n], v);
}
void
MySQL::Command::bindParamI(unsigned int n, unsigned int v)
{
	bindNumber<int, MYSQL_TYPE_LONG>(binds[n], v);
}
void
MySQL::Command::bindParamI(unsigned int n, long unsigned int v)
{
	bindNumber<long long int, MYSQL_TYPE_LONGLONG>(binds[n], v);
}
void
MySQL::Command::bindParamI(unsigned int n, long long unsigned int v)
{
	bindNumber<long long int, MYSQL_TYPE_LONGLONG>(binds[n], v);
}
void
MySQL::Command::bindParamB(unsigned int n, bool v)
{
	bindNumber<char, MYSQL_TYPE_TINY>(binds[n], (v ? 1 : 0));
}
void
MySQL::Command::bindParamF(unsigned int n, double v)
{
	bindNumber<double, MYSQL_TYPE_DOUBLE>(binds[n], v);
}
void
MySQL::Command::bindParamF(unsigned int n, float v)
{
	bindNumber<float, MYSQL_TYPE_FLOAT>(binds[n], v);
}
void
MySQL::Command::bindParamS(unsigned int n, const Glib::ustring & s)
{
	bindParamS(n, std::string_view { s.data(), s.bytes() });
}

void
MySQL::Command::bindParamS(unsigned int n, const std::string_view & s)
{
	binds[n].buffer_type = MYSQL_TYPE_STRING;
	// NOLINTNEXTLINE(hicpp-no-malloc)
	binds[n].buffer = realloc(binds[n].buffer, s.length());
	s.copy(static_cast<char*>(binds[n].buffer), s.length(), 0);
	binds[n].buffer_length = s.length();
	binds[n].is_unsigned = 0;
}
void
MySQL::Command::bindParamT(unsigned int n, const boost::posix_time::ptime & v)
{
	binds[n].buffer_type = MYSQL_TYPE_DATETIME;
	// NOLINTNEXTLINE(hicpp-no-malloc)
	binds[n].buffer = realloc(binds[n].buffer, sizeof(MYSQL_TIME));
	MYSQL_TIME & ts = *static_cast<MYSQL_TIME*>(binds[n].buffer);
	ts.year = v.date().year();
	ts.month = v.date().month();
	ts.day = v.date().day();
	ts.hour = v.time_of_day().hours();
	ts.minute = v.time_of_day().minutes();
	ts.second = v.time_of_day().seconds();
	ts.second_part = v.time_of_day().total_microseconds() % 1000000;
	ts.neg = 0;
}
void
MySQL::Command::bindParamT(unsigned int n, const boost::posix_time::time_duration & v)
{
	binds[n].buffer_type = MYSQL_TYPE_TIME;
	// NOLINTNEXTLINE(hicpp-no-malloc)
	binds[n].buffer = realloc(binds[n].buffer, sizeof(MYSQL_TIME));
	MYSQL_TIME & ts = *static_cast<MYSQL_TIME*>(binds[n].buffer);
	ts.year = 0;
	ts.month = 0;
	ts.day = 0;
	ts.hour = v.hours();
	ts.minute = v.minutes();
	ts.second = v.seconds();
	ts.second_part = v.total_microseconds() % 1000000;
	ts.neg = (v.is_negative() ? 1 : 0);
}
void
MySQL::Command::bindNull(unsigned int n)
{
	binds[n].buffer_type = MYSQL_TYPE_NULL;
	binds[n].buffer = nullptr;
	// NOLINTNEXTLINE(hicpp-no-malloc)
	free(binds[n].buffer);
}

void
MySQL::Command::bindParams()
{
	if (paramsNeedBinding) {
		if (mysql_stmt_bind_param(stmt, &binds.front())) {
			throw Error(stmt);
			paramsNeedBinding = false;
		}
	}
}


