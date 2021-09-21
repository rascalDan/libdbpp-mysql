#include "my-mock.h"
#include "connection_fwd.h"
#include "mockDatabase.h"
#include "my-connection.h"
#include <compileTimeFormatter.h>
#include <factory.h>
#include <memory>
#include <string>
#include <vector>

NAMEDFACTORY("mysql", MySQL::Mock, DB::MockDatabaseFactory)

namespace MySQL {

	Mock::Mock(const std::string & master, const std::string & name, const std::vector<std::filesystem::path> & ss) :
		MockServerDatabase(master, name, "mysql")
	{
		CreateNewDatabase();
		PlaySchemaScripts(ss);
	}

	AdHocFormatter(MockConnStr, "options=libdbpp;database=%?");
	DB::ConnectionPtr
	Mock::openConnection() const
	{
		return std::make_shared<Connection>(MockConnStr::get(testDbName));
	}

	Mock::~Mock()
	{
		DropDatabase();
	}

}
