#include <embeddedmy-mock.h>
#include <definedDirs.h>

class StandardMockDatabase : public MySQL::Embedded::Mock {
	public:
		StandardMockDatabase() : MySQL::Embedded::Mock("mysqlmock", {
				rootDir / "mysqlschema.sql" })
		{
		}
};

#define BOOST_TEST_MODULE TestEmbeddedMySQL

#include "test.impl.h"
BOOST_GLOBAL_FIXTURE( StandardMockDatabase );

BOOST_FIXTURE_TEST_SUITE( Core, DB::TestCore );

BOOST_AUTO_TEST_CASE( bulkload )
{
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));
	BOOST_REQUIRE_THROW(ro->beginBulkUpload("any", nullptr), DB::BulkUploadNotSupported);
}

BOOST_AUTO_TEST_SUITE_END();

