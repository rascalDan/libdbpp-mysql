#ifndef MY_COMMAND_H
#define MY_COMMAND_H

#include <command.h>
#include <mysql.h>
#include <vector>

namespace MySQL {
	class Connection;
	class Command : public virtual DB::Command {
	public:
		Command(const Connection *, const std::string & sql);
		virtual ~Command() override = 0;

		void bindParamI(unsigned int, int) override;
		void bindParamI(unsigned int, long int) override;
		void bindParamI(unsigned int, long long int) override;
		void bindParamI(unsigned int, unsigned int) override;
		void bindParamI(unsigned int, long unsigned int) override;
		void bindParamI(unsigned int, long long unsigned int) override;

		void bindParamB(unsigned int, bool) override;

		void bindParamF(unsigned int, double) override;
		void bindParamF(unsigned int, float) override;

		void bindParamS(unsigned int, const Glib::ustring &) override;
		void bindParamS(unsigned int, const std::string_view &) override;

		void bindParamT(unsigned int, const boost::posix_time::time_duration &) override;
		void bindParamT(unsigned int, const boost::posix_time::ptime &) override;

		void bindNull(unsigned int) override;

	protected:
		void bindParams();
		void * realloc(void * buffer, size_t size);

		const Connection * c;
		std::unique_ptr<MYSQL_STMT, decltype(&mysql_stmt_close)> stmt;
		using Binds = std::vector<MYSQL_BIND>;
		Binds binds;
		bool paramsNeedBinding;
	};
}

#endif
