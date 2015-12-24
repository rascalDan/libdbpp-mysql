#define BOOST_TEST_MODULE TestEmbeddedMySQL
#include <boost/test/unit_test.hpp>

#include <embeddedmy-mock.h>
#include <definedDirs.h>

class StandardMockDatabase : public MySQL::Embedded::Mock {
	public:
		StandardMockDatabase() : MySQL::Embedded::Mock("mysqlmock", {
				rootDir / "mysqlschema.sql" })
		{
		}
};

#include "testMain.cpp"

