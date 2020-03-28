#ifndef MOCKMYSQLDATASOURCE_H
#define MOCKMYSQLDATASOURCE_H

#include <mockDatabase.h>
#include <filesystem>
#include <visibility.h>
#include <c++11Helpers.h>

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

