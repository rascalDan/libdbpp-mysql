#ifndef MOCKMYSQLDATASOURCE_H
#define MOCKMYSQLDATASOURCE_H

#include <mockDatabase.h>
#include <boost/filesystem/path.hpp>
#include <visibility.h>

namespace MySQL {

class DLL_PUBLIC Mock : public DB::MockServerDatabase {
	public:
		Mock(const std::string & master, const std::string & name, const std::vector<boost::filesystem::path> & ss);
		Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss);
		~Mock();

	private:
		DB::Connection * openConnection() const override;
};

}

#endif

