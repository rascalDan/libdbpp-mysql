#ifndef MY_SELECTCOMMAND_H
#define MY_SELECTCOMMAND_H

#include "my-command.h"
#include <selectcommand.h>
#include <string>

namespace MySQL {
	class Connection;
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
