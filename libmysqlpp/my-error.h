#ifndef MY_ERROR_H
#define MY_ERROR_H

#include "error.h" // IWYU pragma: export
#include <exception.h>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <mysql.h>
#pragma GCC diagnostic pop
// IWYU pragma: no_forward_declare DB::Error

namespace MySQL {
	class Error : public AdHoc::Exception<DB::Error> {
	public:
		explicit Error(MYSQL_STMT *);
		explicit Error(MYSQL *);
		std::string message() const noexcept override;

	private:
		std::string msg;
	};
}

#endif
