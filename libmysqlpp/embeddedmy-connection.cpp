#include "embeddedmy-connection.h"

NAMEDFACTORY("embeddedmysql", MySQL::Embedded::Connection, DB::ConnectionFactory);

namespace MySQL {
	namespace Embedded {
		Connection::Connection(const std::string &)
		{
			mysql_init(&conn);
			mysql_options(&conn, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
			if (mysql_real_connect(&conn, NULL, NULL, NULL, "database1", 0, NULL, 0)) {
				throw MySQL::Error(&conn);
			}
		}
	}
}
