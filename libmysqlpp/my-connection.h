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

			void finish() const override;
			int beginTx() const override;
			int commitTx() const override;
			int rollbackTx() const override;
			bool inTx() const override;
			void ping() const override;
			DB::BulkDeleteStyle bulkDeleteStyle() const override;
			DB::BulkUpdateStyle bulkUpdateStyle() const override;

			DB::SelectCommand * newSelectCommand(const std::string & sql) override;
			DB::ModifyCommand * newModifyCommand(const std::string & sql) override;

			void	beginBulkUpload(const char *, const char *) override;
			void	endBulkUpload(const char *) override;
			size_t bulkUploadData(const char *, size_t) const override;

			int64_t insertId() const override;

			mutable MYSQL conn;

		private:
			my_bool my_true;

			mutable unsigned int txDepth;
			mutable bool rolledback;

			mutable boost::shared_ptr<LoadContext> ctx;
	};
}

#endif

