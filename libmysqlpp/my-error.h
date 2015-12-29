#ifndef MY_ERROR_H
#define MY_ERROR_H

#include <error.h>
#include <mysql.h>
#include <exception.h>

namespace MySQL {
	class Error : public AdHoc::Exception<DB::Error> {
		public:
			Error(MYSQL_STMT *);
			Error(MYSQL *);
			std::string message() const throw() override;

		private:
			std::string msg;
	};
}

#endif

