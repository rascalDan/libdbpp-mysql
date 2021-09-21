#ifndef MY_MODIFYCOMMAND_H
#define MY_MODIFYCOMMAND_H

#include "my-command.h"
#include <modifycommand.h>
#include <string>

namespace MySQL {
	class Connection;
	class ModifyCommand : public DB::ModifyCommand, public Command {
	public:
		ModifyCommand(const Connection *, const std::string & sql);

		unsigned int execute(bool) override;

	private:
		void prepare() const;
	};
}

#endif
