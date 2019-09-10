#pragma once

#include <string>
#include <iostream>

namespace Engine {
	// If debug: print to console
	// If release: log to file
	class LogManager {
	public:
		static void Log(const std::string& message) {
#ifdef CFG_DEBUG
			std::cout << message << std::endl;
#endif
			LogToFile(message);
		}

		static void LogError(const std::string& message) {
			std::cout << "---------> Error - " << message << std::endl;
			LogToFile(message);
		}

	private:
		static void LogToFile(const std::string& message) {
			// TODO: Log to file
		}
	};
	
}
