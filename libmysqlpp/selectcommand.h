#ifndef MY_SELECTCOMMAND_H
#define MY_SELECTCOMMAND_H

#include "../libdbpp/selectcommand.h"
#include "command.h"

namespace MySQL {
	class Connection;
	class ColumnBase;
	class SelectCommand : public DB::SelectCommand, public Command {
		public:
			SelectCommand(const Connection *, const std::string & sql);

			bool fetch();
			void execute();

		private:
			bool executed;
			Binds fields;
	};
}

#endif


