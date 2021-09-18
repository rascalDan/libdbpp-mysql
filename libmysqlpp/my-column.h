#ifndef MY_COLUMN_H
#define MY_COLUMN_H

#include <column.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <mysql.h>
#pragma GCC diagnostic pop

namespace MySQL {
	class SelectCommand;
	class ColumnBase : public DB::Column {
	public:
		ColumnBase(const char * name, unsigned int field);

		[[nodiscard]] bool isNull() const override;

	protected:
		bool is_null;
		long unsigned int length;
		friend class SelectCommand;
	};

	class StringColumn : public ColumnBase {
	public:
		StringColumn(const char * name, unsigned int field, MYSQL_BIND * b, unsigned int len);

		void apply(DB::HandleField &) const override;

		std::vector<char> value;
	};

	class NullColumn : public ColumnBase {
	public:
		NullColumn(const char * name, unsigned int field, MYSQL_BIND * b);

		void apply(DB::HandleField &) const override;
	};

	template<class T, enum_field_types MT> class Column : public ColumnBase {
	public:
		Column(const char * name, unsigned int field, MYSQL_BIND * b);

		void apply(DB::HandleField & h) const override;

		T value;
	};
}

#endif
