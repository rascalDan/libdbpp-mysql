#define BOOST_TEST_MODULE TestMySQL
#include <boost/test/unit_test.hpp>

#include <my-mock.h>
#include <my-error.h>
#include <definedDirs.h>
#include <modifycommand.h>
#include <selectcommand.h>
#include <column.h>
#include <connection.h>
#include <testCore.h>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

class StandardMockDatabase : public DB::PluginMock<MySQL::Mock> {
	public:
		StandardMockDatabase() : DB::PluginMock<MySQL::Mock>("mysqlmock", {
				rootDir / "mysqlschema.sql" }, "options=libdbpp")
		{
		}
};

BOOST_GLOBAL_FIXTURE( StandardMockDatabase );

BOOST_FIXTURE_TEST_SUITE( Core, DB::TestCore );

BOOST_AUTO_TEST_CASE( transactions )
{
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");

	BOOST_REQUIRE_EQUAL(false, ro->inTx());
	ro->beginTx();
	BOOST_REQUIRE_EQUAL(true, ro->inTx());
	ro->rollbackTx();
	BOOST_REQUIRE_EQUAL(false, ro->inTx());

	ro->beginTx();
	BOOST_REQUIRE_EQUAL(true, ro->inTx());
	ro->commitTx();
	BOOST_REQUIRE_EQUAL(false, ro->inTx());
}

BOOST_AUTO_TEST_CASE( bindAndSend )
{
	auto rw = DB::MockDatabase::openConnectionTo("mysqlmock");

	auto mod = rw->modify("INSERT INTO test VALUES(?, ?, ?, ?, ?, ?)");
	mod->bindParamI(0, testInt);
	mod->bindParamF(1, testDouble);
	mod->bindParamS(2, testString);
	mod->bindParamB(3, testBool);
	mod->bindParamT(4, testDateTime);
	mod->bindParamT(5, testInterval);
	mod->execute();
}

BOOST_AUTO_TEST_CASE( bindAndSelect )
{
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");

	auto select = ro->select("SELECT * FROM test WHERE id = ?");
	select->bindParamI(0, testInt);
	select->execute();
	int rows = 0;
	while (select->fetch()) {
		assertColumnValueHelper(*select, 0, testInt);
		assertColumnValueHelper(*select, 1, testDouble);
		assertColumnValueHelper(*select, 2, testString);
		assertColumnValueHelper(*select, 3, testBool);
		assertColumnValueHelper(*select, 4, testDateTime);
		assertColumnValueHelper(*select, 5, testInterval);
		rows += 1;
	}
	BOOST_REQUIRE_EQUAL(1, rows);
}

BOOST_AUTO_TEST_CASE( bindAndSelectOther )
{
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");

	auto select = ro->select("SELECT * FROM test WHERE id != ?");
	select->bindParamI(0, testInt);
	select->execute();
	int rows = 0;
	while (select->fetch()) {
		assertColumnValueHelper(*select, 0, 4);
		assertColumnValueHelper(*select, 1, 123.45);
		assertColumnValueHelper(*select, 2, std::string("some text"));
		assertColumnValueHelper(*select, 3, true);
		assertColumnValueHelper(*select, 4, boost::posix_time::ptime_from_tm({ 3, 6, 23, 27, 3, 115, 0, 0, 0, 0, 0}));
		assertColumnValueHelper(*select, 5, boost::posix_time::time_duration(38, 13, 12));
		rows += 1;
	}
	BOOST_REQUIRE_EQUAL(1, rows);
}

BOOST_AUTO_TEST_CASE( bulkload )
{
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");

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
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");

	auto count = ro->select("SELECT * FROM bulktest");
	unsigned int rows = 0;
	while (count->fetch()) {
		rows += 1;
	}
	BOOST_REQUIRE_EQUAL(800, rows);
}

BOOST_AUTO_TEST_CASE( insertId )
{
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");
	auto ins = ro->modify("INSERT INTO inserts(num) VALUES(?)");
	ins->bindParamI(0, 4);
	ins->execute();
	BOOST_REQUIRE_EQUAL(1, ro->insertId());
	ins->bindParamI(0, 40);
	ins->execute();
	BOOST_REQUIRE_EQUAL(2, ro->insertId());
	ins->bindParamI(0, -4);
	ins->execute();
	BOOST_REQUIRE_EQUAL(3, ro->insertId());
}

BOOST_AUTO_TEST_CASE( errors )
{
	auto ro = DB::MockDatabase::openConnectionTo("mysqlmock");
	BOOST_REQUIRE_THROW(ro->execute("nonsense"), DB::Error);
	BOOST_REQUIRE_THROW(DB::ConnectionFactory::createNew("mysql", "server=nohost"), DB::ConnectionError);
}

BOOST_AUTO_TEST_SUITE_END();

