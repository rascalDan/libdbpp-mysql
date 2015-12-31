#include "embeddedmy-connection.h"
#include "my-opts.h"
#include <nvpParse.h>

NAMEDFACTORY("embeddedmysql", MySQL::Embedded::Connection, DB::ConnectionFactory);

namespace MySQL {
	namespace Embedded {
		class Opts {
			public:
				boost::filesystem::path path;
				MySQL::OptString database;
				MySQL::OptString options;
				std::vector<std::string> serverOptions;
		};

		using namespace AdHoc;
		NvpTarget(Opts) OptsTargetMap {
			NvpValue(Opts, path),
			NvpValue(Opts, database),
			NvpValue(Opts, options),
			NvpValue(Opts, serverOptions),
		};

		Connection::Connection(const std::string & str)
		{
			std::stringstream i(str);
			Opts o;
			NvpParse::parse(i, OptsTargetMap, o);
			mysql_init(&conn);
			if (o.options) {
				mysql_options(&conn, MYSQL_READ_DEFAULT_GROUP, ~o.options);
			}
			mysql_options(&conn, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
			server = Server::get(o.path, o.serverOptions);
			if (mysql_real_connect(&conn, NULL, NULL, NULL, ~o.database,
						0, NULL, CLIENT_LOCAL_FILES | CLIENT_MULTI_STATEMENTS) == NULL) {
				throw MySQL::ConnectionError(&conn);
			}
			if (mysql_set_character_set(&conn, "utf8")) {
				throw MySQL::ConnectionError(&conn);
			}
		}

		Connection::Connection(Server *, const std::string & db)
		{
			mysql_init(&conn);
			mysql_options(&conn, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
			if (mysql_real_connect(&conn, NULL, NULL, NULL, db.c_str(),
						0, NULL, CLIENT_LOCAL_FILES | CLIENT_MULTI_STATEMENTS) == NULL) {
				throw MySQL::ConnectionError(&conn);
			}
			if (mysql_set_character_set(&conn, "utf8")) {
				throw MySQL::ConnectionError(&conn);
			}
		}

		void Connection::beginBulkUpload(const char * t, const char * e)
		{
			DB::Connection::beginBulkUpload(t, e);
		}

		void Connection::endBulkUpload(const char * msg)
		{
			DB::Connection::endBulkUpload(msg);
		}

		size_t Connection::bulkUploadData(const char * b, size_t s) const
		{
			return DB::Connection::bulkUploadData(b, s);
		}

	}
}
