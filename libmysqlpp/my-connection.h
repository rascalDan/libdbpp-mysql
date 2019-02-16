#ifndef MY_CONNECTION_H
#define MY_CONNECTION_H

#include <connection.h>
#include "my-error.h"
#include <mysql.h>
#include <boost/shared_ptr.hpp>

namespace MySQL {
	class ConnectionError : public virtual Error, public virtual DB::ConnectionError {
		public:
			ConnectionError(MYSQL *);
	};

	class LoadContext;

	class Connection : public DB::Connection {
		public:
			Connection(const std::string & info);
			~Connection();

			void beginTxInt() override;
			void commitTxInt() override;
			void rollbackTxInt() override;
			void ping() const override;
			DB::BulkDeleteStyle bulkDeleteStyle() const override;
			DB::BulkUpdateStyle bulkUpdateStyle() const override;

			DB::SelectCommandPtr select(const std::string & sql, const DB::CommandOptionsCPtr &) override;
			DB::ModifyCommandPtr modify(const std::string & sql, const DB::CommandOptionsCPtr &) override;

			void	beginBulkUpload(const char *, const char *) override;
			void	endBulkUpload(const char *) override;
			size_t bulkUploadData(const char *, size_t) const override;

			int64_t insertId() override;

			mutable MYSQL conn;

		private:
			mutable boost::shared_ptr<LoadContext> ctx;
	};
}

#endif

