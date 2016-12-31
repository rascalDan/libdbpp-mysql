#include "my-connection.h"
#include "my-mock.h"
#include <compileTimeFormatter.h>

namespace MySQL {

Mock::Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss) :
	MockServerDatabase("options=p2testmysql", name, "mysql")
{
	CreateNewDatabase();
	PlaySchemaScripts(ss);
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

