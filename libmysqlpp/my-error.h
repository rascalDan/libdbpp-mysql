#ifndef MY_ERROR_H
#define MY_ERROR_H

#include <error.h>
#include <mysql.h>
#include <exception.h>
#include <visibility.h>

namespace MySQL {
	class DLL_PUBLIC Error : public AdHoc::Exception<DB::Error> {
		public:
			Error(MYSQL_STMT *);
			Error(MYSQL *);
			std::string message() const throw() override;

		private:
			std::string msg;
	};
}

#endif

