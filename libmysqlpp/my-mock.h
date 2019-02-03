#ifndef MOCKMYSQLDATASOURCE_H
#define MOCKMYSQLDATASOURCE_H

#include <mockDatabase.h>
#include <filesystem>
#include <visibility.h>

namespace MySQL {

class DLL_PUBLIC Mock : public DB::MockServerDatabase {
	public:
		Mock(const std::string & master, const std::string & name, const std::vector<std::filesystem::path> & ss);
		~Mock();

	private:
		DB::ConnectionPtr openConnection() const override;
};

}

#endif

