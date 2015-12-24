#define BOOST_TEST_MODULE TestMySQL
#include <boost/test/unit_test.hpp>

#include <my-mock.h>
#include <definedDirs.h>

class StandardMockDatabase : public MySQL::Mock {
	public:
		StandardMockDatabase() : MySQL::Mock("mysqlmock", {
				rootDir / "mysqlschema.sql" })
		{
		}
};

#include "testMain.cpp"

