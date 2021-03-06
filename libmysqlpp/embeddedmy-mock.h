#ifndef MOCKMYSQLDATASOURCE_H
#define MOCKMYSQLDATASOURCE_H

#include <mockDatabase.h>
#include <boost/filesystem/path.hpp>
#include <visibility.h>
#include <boost/shared_ptr.hpp>
#include "embeddedmy-server.h"

namespace MySQL {
	namespace Embedded {
		class DLL_PUBLIC Mock : public DB::MockDatabase {
			public:
				Mock(const std::string & name, const std::vector<boost::filesystem::path> & ss);
				~Mock();

			protected:
				void DropDatabase() const override;
				void CreateNewDatabase() const override;

				DB::Connection * openConnection() const override;

			private:
				static boost::filesystem::path mockDbPath();

				std::string dbname;
				mutable ServerPtr embeddedServer;
		};

	}
}

#endif

