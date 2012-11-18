#ifndef MY_SELECTCOMMAND_H
#define MY_SELECTCOMMAND_H

#include "../libdbpp/selectcommand.h"
#include "../libdbpp/column.h"
#include "command.h"
#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <vector>
#include <map>

namespace MySQL {
	class Connection;
	class ColumnBase;
	class SelectCommand : public DB::SelectCommand, public Command {
		public:
			SelectCommand(const Connection *, const std::string & sql);

			bool fetch();
			void execute();
			const DB::Column& operator[](unsigned int) const;
			const DB::Column& operator[](const Glib::ustring&) const;
			unsigned int columnCount() const;
			unsigned int getOrdinal(const Glib::ustring&) const;

		private:
			bool executed;
			Binds fields;
			typedef boost::multi_index_container<boost::shared_ptr<ColumnBase>, boost::multi_index::indexed_by<
				boost::multi_index::ordered_unique<boost::multi_index::member<DB::Column, const unsigned int, &DB::Column::colNo>>,
				boost::multi_index::ordered_unique<boost::multi_index::member<DB::Column, const Glib::ustring, &DB::Column::name>>
								>> Columns;
			Columns columns;

			friend class ColumnBase;
	};
}

#endif


