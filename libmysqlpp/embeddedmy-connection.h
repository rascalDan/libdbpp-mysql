#ifndef EMBEDDEDMY_CONNECTION_H
#define EMBEDDEDMY_CONNECTION_H

#include "my-connection.h"
#include "my-error.h"
#include <mysql.h>

namespace MySQL {
	namespace Embedded {
		class Connection : public ::MySQL::Connection {
			public:
				Connection(const std::string &);

			private:
		};
	}
}

#endif

