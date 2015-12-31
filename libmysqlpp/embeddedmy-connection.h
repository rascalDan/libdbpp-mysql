#ifndef EMBEDDEDMY_CONNECTION_H
#define EMBEDDEDMY_CONNECTION_H

#include "my-connection.h"
#include "embeddedmy-server.h"
#include "my-error.h"
#include <mysql.h>

namespace MySQL {
	namespace Embedded {
		class Connection : public ::MySQL::Connection {
			public:
				Connection(const std::string &);
				Connection(Server *, const std::string &);

				void	beginBulkUpload(const char *, const char *) override;
				void	endBulkUpload(const char *) override;
				size_t bulkUploadData(const char *, size_t) const override;

			private:
				ServerPtr server;
		};
	}
}

#endif

