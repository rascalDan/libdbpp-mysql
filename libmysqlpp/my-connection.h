#ifndef MY_CONNECTION_H
#define MY_CONNECTION_H

#include "my-error.h"
#include <connection.h>
#include <memory>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <mysql.h>
#pragma GCC diagnostic pop
#include "command_fwd.h"
#include <cstddef>
#include <cstdint>
#include <string>

namespace MySQL {
	class ConnectionError : public virtual Error, public virtual DB::ConnectionError {
	public:
		explicit ConnectionError(MYSQL *);
	};

	class LoadContext;

	class Connection : public DB::Connection {
	public:
		explicit Connection(const std::string & info);
		~Connection() override;

		void beginTxInt() override;
		void commitTxInt() override;
		void rollbackTxInt() override;
		void ping() const override;
		DB::BulkDeleteStyle bulkDeleteStyle() const override;
		DB::BulkUpdateStyle bulkUpdateStyle() const override;

		DB::SelectCommandPtr select(const std::string & sql, const DB::CommandOptionsCPtr &) override;
		DB::ModifyCommandPtr modify(const std::string & sql, const DB::CommandOptionsCPtr &) override;

		void beginBulkUpload(const char *, const char *) override;
		void endBulkUpload(const char *) override;
		size_t bulkUploadData(const char *, size_t) const override;

		int64_t insertId() override;

		mutable MYSQL conn;

	private:
		mutable std::unique_ptr<LoadContext> ctx;
	};
}

#endif
