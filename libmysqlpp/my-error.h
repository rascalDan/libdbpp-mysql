#ifndef MY_ERROR_H
#define MY_ERROR_H

#include <error.h>
#include <exception.h>
#include <mysql.h>

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
