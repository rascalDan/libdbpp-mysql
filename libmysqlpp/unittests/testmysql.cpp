#include <my-mock.h>
#include <definedDirs.h>

class StandardMockDatabase : public MySQL::Mock {
	public:
		StandardMockDatabase() : MySQL::Mock("mysqlmock", {
				rootDir / "mysqlschema.sql" })
		{
		}
};

#define BOOST_TEST_MODULE TestMySQL

#include "test.impl.h"
BOOST_GLOBAL_FIXTURE( StandardMockDatabase );

BOOST_FIXTURE_TEST_SUITE( Core, DB::TestCore );

BOOST_AUTO_TEST_CASE( bulkload )
{
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));

	auto count = ro->select("SELECT COUNT(*) FROM bulktest");
	// Test empty
	ro->beginBulkUpload("bulktest", "");
	ro->endBulkUpload(NULL);
	assertScalarValueHelper(*count, 0);
	// Test sample file
	ro->beginBulkUpload("bulktest", "");
	std::ifstream in((rootDir / "bulk.sample").string());
	if (!in.good()) throw std::runtime_error("Couldn't open bulk.sample");
	char buf[BUFSIZ];
	for (std::streamsize r; (r = in.readsome(buf, sizeof(buf))) > 0; ) {
		ro->bulkUploadData(buf, r);
	}
	ro->endBulkUpload(NULL);
	assertScalarValueHelper(*count, 800);
}

BOOST_AUTO_TEST_CASE( bigIterate )
{
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));

	auto count = ro->select("SELECT * FROM bulktest");
	unsigned int rows = 0;
	while (count->fetch()) {
		rows += 1;
	}
	BOOST_REQUIRE_EQUAL(800, rows);
}

BOOST_AUTO_TEST_SUITE_END();

