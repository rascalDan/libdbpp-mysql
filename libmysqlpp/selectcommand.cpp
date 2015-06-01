#include "selectcommand.h"
#include "connection.h"
#include "column.h"
#include "error.h"
#include <string.h>

MySQL::SelectCommand::SelectCommand(const Connection * conn, const std::string & sql) :
	DB::Command(sql),
	DB::SelectCommand(sql),
	MySQL::Command(conn, sql),
	prepared(false),
	executed(false)
{
}

void
MySQL::SelectCommand::execute()
{
	if (!prepared) {
		bindParams();
		fields.resize(mysql_stmt_field_count(stmt));
		for (Binds::iterator i = fields.begin(); i != fields.end(); ++i) {
			memset(&*i, 0, sizeof(MYSQL_BIND));
		}
		MYSQL_RES * prepare_meta_result = mysql_stmt_result_metadata(stmt);
		MYSQL_FIELD * fieldDefs = mysql_fetch_fields(prepare_meta_result);
		for (unsigned int i = 0; i < fields.size(); i += 1) {
			switch (fieldDefs[i].type) {
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_INT24:
				case MYSQL_TYPE_LONGLONG:
				case MYSQL_TYPE_YEAR:
					columns.insert(boost::shared_ptr<ColumnBase>(new Column<int64_t, MYSQL_TYPE_LONGLONG>(fieldDefs[i].name, i, &fields[i])));
					break;
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_NEWDECIMAL:
				case MYSQL_TYPE_FLOAT:
				case MYSQL_TYPE_DOUBLE:
					columns.insert(boost::shared_ptr<ColumnBase>(new Column<double, MYSQL_TYPE_DOUBLE>(fieldDefs[i].name, i, &fields[i])));
					break;
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_DATE:
				case MYSQL_TYPE_DATETIME:
					columns.insert(boost::shared_ptr<ColumnBase>(new Column<MYSQL_TIME, MYSQL_TYPE_DATETIME>(fieldDefs[i].name, i, &fields[i])));
					break;
				case MYSQL_TYPE_TIME:
					columns.insert(boost::shared_ptr<ColumnBase>(new Column<MYSQL_TIME, MYSQL_TYPE_TIME>(fieldDefs[i].name, i, &fields[i])));
					break;
				case MYSQL_TYPE_STRING:
				case MYSQL_TYPE_VAR_STRING:
					columns.insert(boost::shared_ptr<ColumnBase>(new StringColumn(fieldDefs[i].name, i, &fields[i], fieldDefs[i].length)));
					break;
				case MYSQL_TYPE_NULL:
					columns.insert(boost::shared_ptr<ColumnBase>(new NullColumn(fieldDefs[i].name, i, &fields[i])));
					break;
				case MYSQL_TYPE_BIT:
				case MYSQL_TYPE_BLOB:
				case MYSQL_TYPE_SET:
				case MYSQL_TYPE_ENUM:
				case MYSQL_TYPE_GEOMETRY:
				default:
					mysql_free_result(prepare_meta_result);
					throw Error("Unexpected type");
			}
		}
		mysql_free_result(prepare_meta_result);
		if (mysql_stmt_bind_result(stmt, &fields.front())) {
			throw Error(mysql_stmt_error(stmt));
		}
		prepared = true;
	}
	if (!executed) {
		if (mysql_stmt_execute(stmt)) {
			throw Error(mysql_stmt_error(stmt));
		}
		if (mysql_stmt_store_result(stmt)) {
			throw Error(mysql_stmt_error(stmt));
		}
		executed = true;
	}
}

bool
MySQL::SelectCommand::fetch()
{
	execute();
	switch (mysql_stmt_fetch(stmt)) {
		case 0:
			return true;
		case MYSQL_NO_DATA:
			executed = false;
			return false;
		default:
			throw Error(mysql_stmt_error(stmt));
	}
}

