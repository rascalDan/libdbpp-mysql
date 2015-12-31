#ifndef MYSQL_EMBEDDED_SERVER_H
#define MYSQL_EMBEDDED_SERVER_H

#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <intrusivePtrBase.h>
#include <boost/intrusive_ptr.hpp>

namespace MySQL {
	namespace Embedded {
		class Server;

		typedef boost::intrusive_ptr<Server> ServerPtr;

		class Server : public IntrusivePtrBase {
			public:
				virtual ~Server();

				static ServerPtr get(const boost::filesystem::path &, const std::vector<std::string> &);
				static ServerPtr getMock(const boost::filesystem::path &);

			protected:
				Server(const boost::filesystem::path &, const std::vector<std::string> &);

				const boost::filesystem::path path;

			private:
				static Server * instance;
		};

		class MockServer : public Server {
			public:
				MockServer(const boost::filesystem::path &);

				void initialize();

			public:
				~MockServer();
		};
	}
}

#endif

