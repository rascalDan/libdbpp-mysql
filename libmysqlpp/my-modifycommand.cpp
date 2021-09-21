#include "my-modifycommand.h"
#include "command.h"
#include "modifycommand.h"
#include "my-command.h"
#include "my-error.h"
#include <memory>
#include <mysql.h>

MySQL::ModifyCommand::ModifyCommand(const Connection * conn, const std::string & sql) :
	DB::Command(sql), DB::ModifyCommand(sql), MySQL::Command(conn, sql)
{
}

unsigned int
MySQL::ModifyCommand::execute(bool anc)
{
	bindParams();
	if (mysql_stmt_execute(stmt.get())) {
		throw Error(stmt.get());
	}
	auto rows = mysql_stmt_affected_rows(stmt.get());
	if (rows == 0 && !anc) {
		throw DB::NoRowsAffected();
	}
	return static_cast<unsigned int>(rows);
}
