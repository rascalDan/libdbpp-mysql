#ifndef MY_COMMAND_H
#define MY_COMMAND_H

#include "../libdbpp/command.h"
#include <mysql.h>
#include <vector>

namespace MySQL {
	class Connection;
	class Command : public virtual DB::Command {
		public:
			Command(const Connection *, const std::string & sql);
			virtual ~Command() = 0;

			void bindParamI(unsigned int, int);
			void bindParamI(unsigned int, long int);
			void bindParamI(unsigned int, long long int);
			void bindParamI(unsigned int, unsigned int);
			void bindParamI(unsigned int, long unsigned int);
			void bindParamI(unsigned int, long long unsigned int);

			void bindParamF(unsigned int, double);
			void bindParamF(unsigned int, float);
			
			void bindParamS(unsigned int, const Glib::ustring&);
			
			void bindParamT(unsigned int, const boost::posix_time::time_duration &);
			void bindParamT(unsigned int, const boost::posix_time::ptime &);
			
			void bindNull(unsigned int);
		protected:
			void bindParams();
			void * realloc(void * buffer, size_t size);

			const Connection * c;
			MYSQL_STMT * stmt;
			typedef std::vector<MYSQL_BIND> Binds;
			Binds binds;
			bool paramsNeedBinding;
	};
}

#endif


