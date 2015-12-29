#ifndef MY_SELECTCOMMAND_H
#define MY_SELECTCOMMAND_H

#include <selectcommand.h>
#include "my-command.h"

namespace MySQL {
	class Connection;
	class ColumnBase;
	class SelectCommand : public DB::SelectCommand, public Command {
		public:
			SelectCommand(const Connection *, const std::string & sql);

			bool fetch() override;
			void execute() override;

		private:
			bool prepared;
			bool executed;
			Binds fields;
	};
}

#endif


