#include "my-column.h"
#include "column.h"
#include <boost/date_time/gregorian_calendar.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time.hpp>
#include <cstdint>
#include <mysql.h>

MySQL::ColumnBase::ColumnBase(const char * name, unsigned int i) : DB::Column(name, i), is_null(false), length(0) { }

bool
MySQL::ColumnBase::isNull() const
{
	return is_null;
}

MySQL::StringColumn::StringColumn(const char * name, unsigned int field, MYSQL_BIND * b, unsigned int len) :
	ColumnBase(name, field), value(len)
{
	b->is_null = &is_null;
	b->buffer_type = MYSQL_TYPE_STRING;
	b->is_unsigned = false;
	b->buffer = value.data();
	b->buffer_length = len;
	b->length = &length;
}

void
MySQL::StringColumn::apply(DB::HandleField & h) const
{
	if (is_null) {
		h.null();
	}
	else {
		h.string({value.data(), length});
	}
}

MySQL::NullColumn::NullColumn(const char * name, unsigned int field, MYSQL_BIND * b) : ColumnBase(name, field)
{
	b->is_null = &is_null;
	b->buffer_type = MYSQL_TYPE_NULL;
	b->buffer = nullptr;
	b->buffer_length = 0;
}

void
MySQL::NullColumn::apply(DB::HandleField & h) const
{
	h.null();
}

namespace MySQL {
	template<class T, enum_field_types MT>
	Column<T, MT>::Column(const char * name, unsigned int field, MYSQL_BIND * b) : ColumnBase(name, field)
	{
		b->is_null = &is_null;
		b->buffer_type = MT;
		b->is_unsigned = false;
		b->buffer = &value;
		b->buffer_length = sizeof(T);
	}

	template<>
	void
	Column<int64_t, MYSQL_TYPE_LONGLONG>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			h.integer(value);
		}
	}
	template<>
	void
	Column<double, MYSQL_TYPE_DOUBLE>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			h.floatingpoint(value);
		}
	}
	template<>
	void
	Column<MYSQL_TIME, MYSQL_TYPE_DATETIME>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			h.timestamp(boost::posix_time::ptime(
					boost::gregorian::date(static_cast<short unsigned int>(value.year),
							static_cast<short unsigned int>(value.month), static_cast<short unsigned int>(value.day)),
					boost::posix_time::time_duration(value.hour, value.minute, value.second)
							+ boost::posix_time::microseconds(value.second_part)));
		}
	}
	template<>
	void
	Column<MYSQL_TIME, MYSQL_TYPE_TIME>::apply(DB::HandleField & h) const
	{
		if (is_null) {
			h.null();
		}
		else {
			h.interval(boost::posix_time::time_duration(value.hour, value.minute, value.second)
					+ boost::posix_time::microseconds(value.second_part));
		}
	}

	template class Column<int64_t, MYSQL_TYPE_LONGLONG>;
	template class Column<double, MYSQL_TYPE_DOUBLE>;
	template class Column<MYSQL_TIME, MYSQL_TYPE_DATETIME>;
	template class Column<MYSQL_TIME, MYSQL_TYPE_TIME>;
}
