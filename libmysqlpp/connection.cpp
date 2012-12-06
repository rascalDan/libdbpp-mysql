#include "connection.h"
#include "error.h"
#include "selectcommand.h"
#include "modifycommand.h"
#include "reflection.h"
#include <ucontext.h>

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

namespace MySQL {
	class LoadContext {
		public:
			LoadContext(MYSQL * c, const char * table, const char * extra) :
				loadBuf(NULL),
				loadBufLen(0),
				bufOff(0),
				conn(c)
			{
				static char buf[BUFSIZ];
				int len = snprintf(buf, BUFSIZ, "LOAD DATA LOCAL INFILE 'any' INTO TABLE %s %s", table, extra);
				mysql_send_query(conn, buf, len);
			}

			static int local_infile_init(void ** ptr, const char *, void * ctx) {
				*ptr = ctx;
				return 0;
			}

			static int local_infile_read(void * obj, char * buf, unsigned int bufSize) {
				LoadContext * ctx = static_cast<LoadContext *>(obj);
				if (ctx->loadBufLen - ctx->bufOff == 0) {
					swapcontext(&ctx->jmpMySQL, &ctx->jmpP2);
					if (ctx->loadBufLen - ctx->bufOff <= 0) {
						// Nothing to do or error
						return ctx->bufOff;
					}
				}
				unsigned int copy = std::min(ctx->loadBufLen - ctx->bufOff, bufSize);
				memcpy(buf, ctx->loadBuf + ctx->bufOff, copy);
				ctx->bufOff += copy;
				return copy;
			}

			static void local_infile_end(void *) {
			}

			static int local_infile_error(void *, char*, unsigned int) {
				return 0;
			}

			static void loadLocalData(LoadContext * ctx)
			{
				ctx->loadReturn = mysql_read_query_result(ctx->conn);
			}

			char stack[16384];
			ucontext_t jmpP2;
			ucontext_t jmpMySQL;
			const char * loadBuf;
			unsigned int loadBufLen;
			int bufOff;
			MYSQL * conn;
			int loadReturn;
	};
}

void
MySQL::Connection::beginBulkUpload(const char * table, const char * extra) const
{
	ctx = boost::shared_ptr<LoadContext>(new MySQL::LoadContext(&conn, table, extra));
	getcontext(&ctx->jmpMySQL);
	ctx->jmpMySQL.uc_stack.ss_sp = ctx->stack;
	ctx->jmpMySQL.uc_stack.ss_size = sizeof(ctx->stack);
	ctx->jmpMySQL.uc_link = &ctx->jmpP2;
	makecontext(&ctx->jmpMySQL, (void (*)())&LoadContext::loadLocalData, 1, ctx.get());

	mysql_set_local_infile_handler(&conn, LoadContext::local_infile_init, LoadContext::local_infile_read,
			LoadContext::local_infile_end, LoadContext::local_infile_error, ctx.get());

	// begin the load, context swaps back when buffer is empty
	swapcontext(&ctx->jmpP2, &ctx->jmpMySQL);
}

void
MySQL::Connection::endBulkUpload(const char * msg) const
{
	ctx->loadBuf = NULL;
	ctx->loadBufLen = 0;
	ctx->bufOff = msg ? -1 : 0;
	// switch context with empty buffer fires finished
	swapcontext(&ctx->jmpP2, &ctx->jmpMySQL);
	// Check result
	if (!msg) {
		if (ctx->loadReturn) {
			ctx.reset();
			throw Error(mysql_error(&conn));
		}
	}
	ctx.reset();
}

size_t
MySQL::Connection::bulkUploadData(const char * data, size_t len) const
{
	ctx->loadBuf = data;
	ctx->loadBufLen = len;
	ctx->bufOff = 0;
	// switch context to load the buffered data
	swapcontext(&ctx->jmpP2, &ctx->jmpMySQL);
	return len;
}

