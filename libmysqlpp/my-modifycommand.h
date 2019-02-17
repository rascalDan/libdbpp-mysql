#ifndef MY_MODIFYCOMMAND_H
#define MY_MODIFYCOMMAND_H

#include <modifycommand.h>
#include "my-command.h"

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



