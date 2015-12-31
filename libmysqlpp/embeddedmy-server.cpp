#include "embeddedmy-server.h"
#include "embeddedmy-connection.h"
#include <stdexcept>
#include <buffer.h>
#include <mysql.h>
#include <boost/filesystem/convenience.hpp>
#include <fstream>

namespace MySQL {
	namespace Embedded {
		ServerPtr
		Server::get(const boost::filesystem::path & path, const std::vector<std::string> & extraOpts)
		{
			if (instance) {
				if (path == instance->path) {
					return instance;
				}
				throw std::runtime_error(stringbf("Only one embedded server per process. [old=%s, new=%s]", instance->path, path));
			}
			instance = new Server(path, extraOpts);
			return instance;
		}

		ServerPtr
		Server::getMock(const boost::filesystem::path & path)
		{
			boost::filesystem::create_directories(path / "mysql");
			if (instance) {
				if (path == instance->path && dynamic_cast<MockServer *>(instance)) {
					return instance;
				}
				throw std::runtime_error(stringbf("Only one embedded server per process. [old=%s, new=%s]", instance->path, path));
			}
			auto i = new MockServer(path);
			i->initialize();
			return (instance = i);
		}

		Server::Server(const boost::filesystem::path & p, const std::vector<std::string> & extraOpts) :
			path(p)
		{
			const auto datadir = stringbf("--datadir=%s", path.string());
			std::vector<const char *> opts;
			opts.push_back(typeid(this).name());
			opts.push_back(datadir.c_str());
			for (auto & opt : extraOpts) {
				opts.push_back(opt.c_str());
			}
			opts.push_back(nullptr);
			static const char * groups[] = { NULL };
			mysql_library_init(opts.size() - 1, (char**)&opts.front(), (char**)groups);
		}

		Server::~Server()
		{
			mysql_library_end();
			instance = nullptr;
		}

		MockServer::MockServer(const boost::filesystem::path & p) :
			Server(p, { "--bootstrap" })
		{
		}

		void
		MockServer::initialize()
		{
			Connection initialize(this, "mysql");
			std::ifstream sql1("/usr/share/mysql/mysql_system_tables.sql");
			initialize.executeScript(sql1, path);
			std::ifstream sql2("/usr/share/mysql/mysql_system_tables_data.sql");
			initialize.executeScript(sql2, path);
		}

		MockServer::~MockServer()
		{
			boost::filesystem::remove_all(path);
		}

		Server * Server::instance = nullptr;
	}
}

