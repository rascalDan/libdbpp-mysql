#include "connection.h"
#include "error.h"
#include "selectcommand.h"
#include "modifycommand.h"
#include "reflection.h"

class Opts {
	public:
		Opts() { port = 3306; }
		std::string server;
		std::string user;
		std::string password;
		std::string database;
		unsigned int port;
		std::string unix_socket;

		static Reflector<Opts>::Vars vars;
};

Reflector<Opts>::Vars Opts::vars = {
	Map(Opts, server),
	Map(Opts, user),
	Map(Opts, password),
	Map(Opts, database),
	Map(Opts, unix_socket),
	Map(Opts, port),
};


MySQL::Connection::Connection(const std::string & str) :
	txDepth(0),
	rolledback(false)
{
	Opts o(Reflector<Opts>::NameValueNew(str));
	mysql_init(&conn);
	if (mysql_real_connect(&conn, o.server.c_str(), o.user.c_str(), o.password.c_str(), o.database.c_str(),
				o.port, o.unix_socket.c_str(), CLIENT_LOCAL_FILES | CLIENT_MULTI_STATEMENTS) == NULL) {
		throw ConnectionError();
	}
	if (mysql_set_character_set(&conn, "utf8")) {
		throw ConnectionError();
	}
}

MySQL::Connection::~Connection()
{
	mysql_close(&conn);
}

void
MySQL::Connection::finish() const
{
	if (txDepth != 0) {
		rollbackTx();
		throw Error("Transaction still open");
	}
}

int
MySQL::Connection::beginTx() const
{
	if (txDepth == 0) {
		if (mysql_autocommit(&conn, 0)) {
			throw Error(mysql_error(&conn));
		}
		rolledback = false;
	}
	return ++txDepth;
}

int
MySQL::Connection::commitTx() const
{
	if (rolledback) {
		return rollbackTx();
	}
	if (--txDepth == 0) {
		if (mysql_commit(&conn)) {
			throw Error(mysql_error(&conn));
		}
	}
	return txDepth;
}

int
MySQL::Connection::rollbackTx() const
{
	if (--txDepth == 0) {
		if (mysql_rollback(&conn)) {
			throw Error(mysql_error(&conn));
		}
	}
	else {
		rolledback = true;
	}
	return txDepth;
}

bool
MySQL::Connection::inTx() const
{
	return txDepth;
}

DB::BulkDeleteStyle
MySQL::Connection::bulkDeleteStyle() const
{
	return DB::BulkDeleteUsingUsingAlias;
}

DB::BulkUpdateStyle
MySQL::Connection::bulkUpdateStyle() const
{
	return DB::BulkUpdateUsingJoin;
}

void
MySQL::Connection::ping() const
{
	if (mysql_ping(&conn)) {
		throw Error(mysql_error(&conn));
	}
}


DB::SelectCommand *
MySQL::Connection::newSelectCommand(const std::string & sql) const
{
	return new SelectCommand(this, sql);
}

DB::ModifyCommand *
MySQL::Connection::newModifyCommand(const std::string & sql) const
{
	return new ModifyCommand(this, sql);
}

void
MySQL::Connection::beginBulkUpload(const char * table, const char * extra) const
{
	(void)table;
	(void)extra;
}

void
MySQL::Connection::endBulkUpload(const char * msg) const
{
	(void)msg;
}

size_t
MySQL::Connection::bulkUploadData(const char * data, size_t len) const
{
	(void)data;
	return len;
}

