#include "my-connection.h"
#include "my-error.h"
#include "my-selectcommand.h"
#include "my-modifycommand.h"
#include <nvpParse.h>
#include <runtimeContext.h>
#include <ucontext.h>
#include <boost/optional.hpp>

NAMEDFACTORY("mysql", MySQL::Connection, DB::ConnectionFactory);

MySQL::ConnectionError::ConnectionError(MYSQL * m) :
	MySQL::Error(m)
{
}

class Opts {
	public:
		Opts() { port = 3306; }
		typedef boost::optional<std::string> OptString;
		OptString server;
		OptString user;
		OptString password;
		OptString database;
		unsigned int port;
		OptString unix_socket;
		OptString options;
};

const char *
operator~(const Opts::OptString & os)
{
	if (os) {
		return os->c_str();
	}
	return NULL;
}

namespace std {
	template <typename T>
	std::istream &
	operator>>(std::istream & s, boost::optional<T> & o)
	{
		o = T();
		return (s >> *o);
	}
}

using namespace AdHoc;
NvpTarget(Opts) OptsTargetMap {
	NvpValue(Opts, server),
	NvpValue(Opts, user),
	NvpValue(Opts, password),
	NvpValue(Opts, database),
	NvpValue(Opts, unix_socket),
	NvpValue(Opts, port),
	NvpValue(Opts, options),
};


MySQL::Connection::Connection(const std::string & str)
{
	std::stringstream i(str);
	Opts o;
	NvpParse::parse(i, OptsTargetMap, o);
	mysql_init(&conn);
	if (o.options) {
		mysql_options(&conn, MYSQL_READ_DEFAULT_GROUP, ~o.options);
	}
	if (mysql_real_connect(&conn, ~o.server, ~o.user, ~o.password, ~o.database,
				o.port, ~o.unix_socket, CLIENT_LOCAL_FILES | CLIENT_MULTI_STATEMENTS) == NULL) {
		throw ConnectionError(&conn);
	}
	if (mysql_set_character_set(&conn, "utf8")) {
		throw ConnectionError(&conn);
	}
}

MySQL::Connection::~Connection()
{
	mysql_close(&conn);
}

void
MySQL::Connection::beginTxInt()
{
	if (mysql_autocommit(&conn, 0)) {
		throw Error(&conn);
	}
}

void
MySQL::Connection::commitTxInt()
{
	if (mysql_commit(&conn)) {
		throw Error(&conn);
	}
}

void
MySQL::Connection::rollbackTxInt()
{
	if (mysql_rollback(&conn)) {
		throw Error(&conn);
	}
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
		throw Error(&conn);
	}
}


DB::SelectCommand *
MySQL::Connection::newSelectCommand(const std::string & sql)
{
	return new SelectCommand(this, sql);
}

DB::ModifyCommand *
MySQL::Connection::newModifyCommand(const std::string & sql)
{
	return new ModifyCommand(this, sql);
}

namespace MySQL {
	class LoadContext : public AdHoc::System::RuntimeContext {
		public:
			LoadContext(MYSQL * c) :
				loadBuf(NULL),
				loadBufLen(0),
				bufOff(0),
				conn(c)
			{
			}

			static int local_infile_init(void ** ptr, const char *, void * ctx) {
				*ptr = ctx;
				return 0;
			}

			static int local_infile_read(void * obj, char * buf, unsigned int bufSize) {
				LoadContext * ctx = static_cast<LoadContext *>(obj);
				if (ctx->loadBufLen - ctx->bufOff == 0) {
					ctx->swapContext();
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

			void callback() override
			{
				loadReturn = mysql_read_query_result(conn);
			}

			const char * loadBuf;
			unsigned int loadBufLen;
			int bufOff;
			MYSQL * conn;
			int loadReturn;
	};
}

void
MySQL::Connection::beginBulkUpload(const char * table, const char * extra)
{
	static char buf[BUFSIZ];
	int len = snprintf(buf, BUFSIZ, "LOAD DATA LOCAL INFILE 'any' INTO TABLE %s %s", table, extra);
	mysql_send_query(&conn, buf, len);

	ctx = boost::shared_ptr<LoadContext>(new MySQL::LoadContext(&conn));

	mysql_set_local_infile_handler(&conn, LoadContext::local_infile_init, LoadContext::local_infile_read,
			LoadContext::local_infile_end, LoadContext::local_infile_error, ctx.get());

	ctx->swapContext();
}

void
MySQL::Connection::endBulkUpload(const char * msg)
{
	ctx->loadBuf = NULL;
	ctx->loadBufLen = 0;
	ctx->bufOff = msg ? -1 : 0;
	// switch context with empty buffer fires finished
	ctx->swapContext();
	// Check result
	if (!msg) {
		if (ctx->loadReturn) {
			ctx.reset();
			throw Error(&conn);
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
	ctx->swapContext();
	return len;
}

int64_t
MySQL::Connection::insertId()
{
	return mysql_insert_id(&conn);
}

