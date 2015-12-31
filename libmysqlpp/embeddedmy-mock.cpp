#include "embeddedmy-connection.h"
#include "embeddedmy-mock.h"
#include "embeddedmy-server.h"
#include <boost/filesystem/convenience.hpp>
#include <buffer.h>

namespace MySQL {
	namespace Embedded {

		Mock::Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss) :
			MockDatabase(name),
			dbname(stringbf("test_%d_%d", getpid(), ++DB::MockDatabase::mocked))
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
			return new Connection(stringbf("path=%s;database=%s", mockDbPath(), dbname));
		}

		void
		Mock::CreateNewDatabase() const
		{
			auto path = mockDbPath();
			embeddedServer = Server::getMock(path);
			Connection initialize(embeddedServer.get(), "mysql");
			initialize.execute("CREATE DATABASE " + dbname);
		}

		void
		Mock::DropDatabase() const
		{
			Connection initialize(embeddedServer.get(), "mysql");
			initialize.execute("DROP DATABASE " + dbname);
		}

		boost::filesystem::path
		Mock::mockDbPath()
		{
			return boost::filesystem::temp_directory_path() / stringbf("mysql-mock-%d", getpid());
		}
	}
}

