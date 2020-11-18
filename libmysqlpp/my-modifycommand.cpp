#include "my-modifycommand.h"
#include "connection.h"
#include "my-error.h"
#include <cstdlib>

MySQL::ModifyCommand::ModifyCommand(const Connection * conn, const std::string & sql) :
	DB::Command(sql), DB::ModifyCommand(sql), MySQL::Command(conn, sql)
{
}

unsigned int
MySQL::ModifyCommand::execute(bool anc)
{
	bindParams();
	if (mysql_stmt_execute(stmt)) {
		throw Error(stmt);
	}
	int rows = mysql_stmt_affected_rows(stmt);
	if (rows == 0 && !anc) {
		throw DB::NoRowsAffected();
	}
	return rows;
}
