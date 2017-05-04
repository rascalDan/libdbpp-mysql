#ifndef MY_CONNECTION_H
#define MY_CONNECTION_H

#include <connection.h>
#include <visibility.h>
#include "my-error.h"
#include <mysql.h>
#include <boost/shared_ptr.hpp>

namespace MySQL {
	class DLL_PUBLIC ConnectionError : public virtual Error, public virtual DB::ConnectionError {
		public:
			ConnectionError(MYSQL *);
	};

	class LoadContext;

	class DLL_PUBLIC Connection : public DB::Connection {
		public:
			Connection(const std::string & info);
			~Connection();

			void beginTxInt() override;
			void commitTxInt() override;
			void rollbackTxInt() override;
			void ping() const override;
			DB::BulkDeleteStyle bulkDeleteStyle() const override;
			DB::BulkUpdateStyle bulkUpdateStyle() const override;

			DB::SelectCommand * newSelectCommand(const std::string & sql, const DB::CommandOptions *) override;
			DB::ModifyCommand * newModifyCommand(const std::string & sql, const DB::CommandOptions *) override;

			void	beginBulkUpload(const char *, const char *) override;
			void	endBulkUpload(const char *) override;
			size_t bulkUploadData(const char *, size_t) const override;

			int64_t insertId() override;
			void execute(const std::string &, const DB::CommandOptions *) override;

			mutable MYSQL conn;

		protected:
			Connection();

		private:
			my_bool my_true;

			mutable boost::shared_ptr<LoadContext> ctx;
	};
}

#endif

