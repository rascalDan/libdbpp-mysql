#include "column.h"
#include "selectcommand.h"
#include "error.h"
#include <string.h>

MySQL::ColumnBase::ColumnBase(const char * name, unsigned int i) :
	DB::Column(name, i)
{
}

bool
MySQL::ColumnBase::isNull() const
{
	return is_null;
}

void
MySQL::ColumnBase::rebind(DB::Command *, unsigned int) const
{
	throw Error("Not supported");
}

MySQL::StringColumn::StringColumn(const char * name, unsigned int field, MYSQL_BIND * b, unsigned int len) :
	ColumnBase(name, field),
	value(new char[len])
{
	b->is_null = &is_null;
	b->buffer_type = MYSQL_TYPE_STRING;
	b->is_unsigned = 0;
	b->buffer = value;
	b->buffer_length = len;
	b->length = &length;
}
MySQL::StringColumn::~StringColumn()
{
	delete[] value;
}
void
MySQL::StringColumn::apply(DB::HandleField & h) const
{
	if (is_null) {
		h.null();
	}
	else {
		h.string(value, length);
	}
}
MySQL::NullColumn::NullColumn(const char * name, unsigned int field, MYSQL_BIND * b) :
	ColumnBase(name, field)
{
	b->is_null = &is_null;
	b->buffer_type = MYSQL_TYPE_NULL;
	b->buffer = NULL;
	b->buffer_length = 0;
}
void
MySQL::NullColumn::apply(DB::HandleField & h) const
{
	h.null();
}

namespace MySQL {
	template <class T, enum_field_types MT> Column<T, MT>::Column(const char * name, unsigned int field, MYSQL_BIND * b) :
		ColumnBase(name, field)
	{
		b->is_null = &is_null;
		b->buffer_type = MT;
		b->is_unsigned = 0;
		b->buffer = &value;
		b->buffer_length = sizeof(T);
	}

	template <> void Column<int64_t, MYSQL_TYPE_LONGLONG>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			h.integer(value);
		}
	}
	template <> void Column<double, MYSQL_TYPE_DOUBLE>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			h.floatingpoint(value);
		}
	}
	template <> void Column<MYSQL_TIME, MYSQL_TYPE_DATETIME>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			struct tm tm;
			memset(&tm, 0, sizeof(tm));
			tm.tm_year = value.year - 1900;
			tm.tm_mon = value.month - 1;
			tm.tm_mday = value.day;
			tm.tm_hour = value.hour;
			tm.tm_min = value.minute;
			tm.tm_sec = value.second;
			h.timestamp(tm);
		}
	}

	template class Column<int64_t, MYSQL_TYPE_LONGLONG>;
	template class Column<double, MYSQL_TYPE_DOUBLE>;
	template class Column<MYSQL_TIME, MYSQL_TYPE_DATETIME>;
}
