#include "my-connection.h"
#include "my-mock.h"
#include <compileTimeFormatter.h>

NAMEDFACTORY("mysql", MySQL::Mock, DB::MockDatabaseFactory);

namespace MySQL {

Mock::Mock(const std::string & master, const std::string & name, const std::vector<boost::filesystem::path> & ss) :
	MockServerDatabase(master, name, "mysql")
{
	CreateNewDatabase();
	PlaySchemaScripts(ss);
}

Mock::Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss) :
	Mock("options=p2testmysql", name, ss)
{
}

AdHocFormatter(MockConnStr, "options=p2testmysql;database=%?");
DB::Connection *
Mock::openConnection() const
{
	return new Connection(MockConnStr::get(testDbName));
}

Mock::~Mock()
{
	DropDatabase();
}

}

