#include "modifycommand.h"
#include "error.h"
#include <stdlib.h>
#include "connection.h"

MySQL::ModifyCommand::ModifyCommand(const Connection * conn, const std::string & sql) :
	DB::Command(sql),
	DB::ModifyCommand(sql),
	MySQL::Command(conn, sql)
{
}

MySQL::ModifyCommand::~ModifyCommand()
{
}

unsigned int
MySQL::ModifyCommand::execute(bool anc)
{
	bindParams();
	if (mysql_stmt_execute(stmt)) {
		throw Error(mysql_stmt_error(stmt));
	}
	int rows = mysql_stmt_affected_rows(stmt);
	if (rows == 0 && !anc) {
		throw Error("No rows affected");
	}
	return rows;
}
