#ifndef MOCKMYSQLDATASOURCE_H
#define MOCKMYSQLDATASOURCE_H

#include "connection_fwd.h"
#include <c++11Helpers.h>
#include <filesystem>
#include <mockDatabase.h>
#include <string>
#include <vector>
#include <visibility.h>

namespace MySQL {

	class DLL_PUBLIC Mock : public DB::MockServerDatabase {
	public:
		Mock(const std::string & master, const std::string & name, const std::vector<std::filesystem::path> & ss);
		~Mock() override;

		SPECIAL_MEMBERS_MOVE_RO(Mock);

	private:
		[[nodiscard]] DB::ConnectionPtr openConnection() const override;
	};

}

#endif
