#include <stdexcept>
#include <iostream>
#include <common/logger.hpp>

#include "config.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <config path>" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		config config(argv[1]);

		server server(config.port(), config.password(), config.clients());
		while (true)
			server.process();
	} catch (const std::exception& e) {
		common::logger::get().error() << e.what() << std::endl;
	}

	return EXIT_FAILURE;
}