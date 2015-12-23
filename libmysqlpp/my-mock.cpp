#include "my-connection.h"
#include "my-mock.h"
#include <buffer.h>

namespace MySQL {

Mock::Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss) :
	MockServerDatabase("options=p2testmysql", name, "mysql")
{
	CreateNewDatabase();
	PlaySchemaScripts(ss);
}

DB::Connection *
Mock::openConnection() const
{
	return new Connection(stringbf("options=p2testmysql;database=%s", testDbName));
}

Mock::~Mock()
{
	DropDatabase();
}

}

