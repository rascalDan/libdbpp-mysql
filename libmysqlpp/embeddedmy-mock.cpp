#include "embeddedmy-connection.h"
#include "embeddedmy-mock.h"
#include <boost/filesystem/convenience.hpp>
#include <buffer.h>

namespace MySQL {
	namespace Embedded {

		Mock::Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss) :
			MockDatabase(name),
			testDbPath(boost::filesystem::path("/tmp") / stringbf("embeddedmysql-%d-%d", getpid(), ++DB::MockDatabase::mocked))
		{
			CreateNewDatabase();
			PlaySchemaScripts(ss);
		}

		Mock::~Mock()
		{
			DropDatabase();
		}

		DB::Connection *
		Mock::openConnection() const
		{
			return new Connection(testDbPath.string());
		}

		void
		Mock::CreateNewDatabase() const
		{
			boost::filesystem::create_directories(testDbPath);
			const auto datadir = stringbf("--datadir=%s", testDbPath.string());
			static const char * opts[] = {
				typeid(this).name(),
				datadir.c_str(),
				"--bootstrap",
				NULL
			};
			static const char * groups[] = { NULL };
			mysql_library_init(3, (char**)opts, (char**)groups);
			sleep(20);
		}

		void
		Mock::DropDatabase() const
		{
			mysql_library_end();
			boost::filesystem::remove_all(testDbPath);
		}
	}
}

