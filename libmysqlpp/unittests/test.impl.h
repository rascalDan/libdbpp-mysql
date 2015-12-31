#include <boost/test/unit_test.hpp>
#include <modifycommand.h>
#include <selectcommand.h>
#include <connection.h>
#include <mockDatabase.h>
#include <column.h>
#include <testCore.h>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <definedDirs.h>

BOOST_FIXTURE_TEST_SUITE( Core, DB::TestCore );

BOOST_AUTO_TEST_CASE( transactions )
{
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));

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
	auto rw = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));

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
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));

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
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));

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

BOOST_AUTO_TEST_CASE( insertId )
{
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));
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
	auto ro = DB::ConnectionPtr(DB::MockDatabase::openConnectionTo("mysqlmock"));
	BOOST_REQUIRE_THROW(ro->execute("nonsense"), DB::Error);
	BOOST_REQUIRE_THROW(DB::ConnectionFactory::createNew("mysql", "server=nohost"), DB::ConnectionError);
}

BOOST_AUTO_TEST_SUITE_END();

