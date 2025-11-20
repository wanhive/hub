/*
 * Manager.cpp
 *
 * Real stuff happens here
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Manager.h"
#include "ConfigTool.h"
#include "../base/version.h"
#include "../server/auth/AuthenticationHub.h"
#include "../server/overlay/OverlayHub.h"
#include "../server/overlay/OverlayTool.h"
#include "../test/ds/BufferTest.h"
#include "../test/ds/HashTableTest.h"
#include "../test/flood/NetworkTest.h"
#include "../test/multicast/MulticastConsumer.h"
#include <iostream>
#include <getopt.h>

namespace {

constexpr unsigned long long INVALID_UID = static_cast<unsigned long long>(-1);

}  // namespace

namespace wanhive {

Manager::Context Manager::ctx;

Manager::Manager() noexcept {

}

Manager::~Manager() {

}

void Manager::execute(int argc, char *argv[]) noexcept {
	if (parseOptions(argc, argv) == 0) {
		processOptions();
	}
}

int Manager::parseOptions(int argc, char *argv[]) noexcept {
	clear();
	//-----------------------------------------------------------------
	ctx.program = strrchr(argv[0], Storage::PATH_SEPARATOR);
	ctx.program = ctx.program ? (ctx.program + 1) : argv[0];
	//-----------------------------------------------------------------
	auto shortOptions = "c:hmn:St:v";
	const struct option longOptions[] = { { "config", 1, nullptr, 'c' }, {
			"help", 0, nullptr, 'h' }, { "menu", 0, nullptr, 'm' }, { "name", 1,
			nullptr, 'n' }, { "syslog", 0, nullptr, 'S' }, { "type", 1, nullptr,
			't' }, { "version", 0, nullptr, 'v' }, { nullptr, 0, nullptr, 0 } };
	//-----------------------------------------------------------------
	while (true) {
		auto nextOption = getopt_long(argc, argv, shortOptions, longOptions,
				nullptr);
		switch (nextOption) {
		case 'c':
			ctx.config = optarg;
			break;
		case 'h':
			printHelp(stdout);
			return 1;
		case 'm':
			ctx.menu = true;
			break;
		case 'n':
			sscanf(optarg, "%llu", &ctx.uid);
			break;
		case 'S':
			Logger::getDefault().setTarget(WH_LOG_SYS);
			break;
		case 't':
			sscanf(optarg, "%c", &ctx.type);
			break;
		case 'v':
			printVersion(stdout);
			return 1;
		case -1:	//We are done
			return 0;
		case '?': //Unknown option
			WH_LOG_ERROR("Invalid option");
			printUsage(stderr);
			return -1;
		default:
			WH_LOG_ERROR("?? read character code 0%o ??", nextOption);
			printUsage(stderr);
			return -1;
		}
	}
}

void Manager::processOptions() noexcept {
	int option = 1; //Default option
	if (ctx.menu) {
		std::cout << "Select an option\n" << "1. WANHIVE HUB\n"
				<< "2. UTILITIES\n" << "3. PROTOCOL TEST\n"
				<< "4. NETWORK TEST\n" << "5. COMPONENTS TEST\n" << "6. ABOUT\n"
				<< "::";
		std::cin >> option;
		if (CommandLine::inputError()) {
			return;
		}
	}

	switch (option) {
	case 1:
		executeHub();
		break;
	case 2:
		runSettingsManager();
		break;
	case 3:
		runCommandTest();
		break;
	case 4:
		runNetworkTest();
		break;
	case 5:
		runComponentsTest();
		break;
	case 6:
		printHelp(stdout);
		break;
	default:
		std::cerr << "Invalid option" << std::endl;
		break;
	}
}

void Manager::executeHub() noexcept {
	ctx.hub = nullptr;
	int mode = 0;
	if (ctx.type == 'o') {
		mode = 1;
	} else if (ctx.type == 'a') {
		mode = 2;
	} else if (ctx.type == 'm') {
		mode = 3;
	} else if (ctx.type == '\0') {
		std::cout << "Select an option\n" << "1: Overlay server (-to)\n"
				<< "2: Authentication server (-ta)\n"
				<< "3: Multicast consumer for testing (-tm)\n" << ":: ";
		std::cin >> mode;
		if (CommandLine::inputError()) {
			return;
		}
	} else {
		std::cerr << "Invalid option" << std::endl;
		return;
	}

	if (ctx.uid == INVALID_UID) {
		std::cout << "Hub's identity: ";
		std::cin >> ctx.uid;
		if (CommandLine::inputError()) {
			return;
		}
	}

	try {
		if (mode == 1) {
			ctx.hub = new OverlayHub(ctx.uid, ctx.config);
		} else if (mode == 2) {
			ctx.hub = new AuthenticationHub(ctx.uid, ctx.config);
		} else if (mode == 3) {
			unsigned int topic;
			std::cout << "Topic [" << Topic::MIN_ID << "-" << Topic::MAX_ID
					<< "]: ";
			std::cin >> topic;
			if (CommandLine::inputError()) {
				return;
			}

			if (topic > Topic::MAX_ID) {
				std::cerr << "Invalid topic" << std::endl;
				return;
			}

			ctx.hub = new MulticastConsumer(ctx.uid, topic, ctx.config);
		} else {
			std::cerr << "Invalid option" << std::endl;
			return;
		}

		/*
		 * No race condition because signals have been blocked before
		 * calling Hub::execute()
		 */
		installSignals();
		if (ctx.hub->execute(nullptr)) {
			WH_LOG_INFO("Hub terminated normally.");
		} else {
			WH_LOG_ERROR("Hub terminated due to error.");
		}
		restoreSignals();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	} catch (...) {
		WH_LOG_EXCEPTION_U();
	}
	delete ctx.hub;
	ctx.hub = nullptr;
}

void Manager::runSettingsManager() noexcept {
	ConfigTool cft;
	cft.execute();
}

void Manager::runCommandTest() noexcept {
	OverlayTool n(ctx.config, 2000);
	n.run();
}

void Manager::runNetworkTest() noexcept {
	NetworkTest::test(ctx.config);
}

void Manager::runComponentsTest() noexcept {
	//Maintaining the scopes to free up the memory immediately
	{
		std::cout << "\n-----RING BUFFER TEST BEGIN-----\n";
		BufferTest bt;
		bt.test();
		std::cout << "\n-----RING BUFFER TEST END-----\n";
	}

	{
		std::cout << "\n-----HASH TABLE TEST BEGIN-----\n";
		HashTableTest t;
		t.execute();
		std::cout << "\n-----HASH TABLE TEST END-----\n";
	}

	{
		std::cout << "\n-----ENCODING TEST BEGIN-----\n";
		Encoding::test();
		std::cout << "\n-----ENCODING TEST END-----\n";
	}

	{
		std::cout << "\n-----SERIALIZER TEST BEGIN-----\n";
		Serializer::test();
		std::cout << "\n-----SERIALIZER TEST END-----\n";
	}

	{
		std::cout << "\n-----SRP VECTOR TEST BEGIN-----\n";
		Timer t;
		Srp::test();
		std::cout << "Time elapsed: " << t.elapsed() << " seconds" << std::endl;
		std::cout << "\n-----SRP VECTOR TEST END-----\n";
	}

	{
		std::cout << "\n-----SRP STRESS TEST BEGIN-----\n";
		std::cout << "Group size: 3072" << std::endl;
		std::cout << "Hash function: SHA 512" << std::endl;
		std::cout << "Iterations: 300" << std::endl;
		Timer t;
		Srp::test("alice", (const unsigned char*) "password123",
				strlen("password123"), 300, SRP_3072, WH_SHA512);
		std::cout << "Time elapsed: " << t.elapsed() << " seconds" << std::endl;
		std::cout << "\n-----SRP STRESS TEST END-----\n";
	}
}

void Manager::installSignals() {
	//Block all signals
	Signal::blockAll();
	//Suppress SIGPIPE
	Signal::ignore(SIGPIPE);
	//Install a dummy handler for SIGUSR1
	Signal::handle(SIGUSR1);
	//Following signals will initiate graceful shutdown
	Signal::handle(SIGINT, shutdown);
	Signal::handle(SIGTERM, shutdown);
	Signal::handle(SIGQUIT, shutdown);
	//SIGTSTP and SIGHUP not handled
	//Rest of the signals not handled
}

void Manager::restoreSignals() {
	//Unblock all signals
	Signal::unblockAll();
	//Restore SIGPIPE
	Signal::reset(SIGPIPE);
	//Restore SIGUSR1
	Signal::reset(SIGUSR1);
	//Restore the following signals
	Signal::reset(SIGINT);
	Signal::reset(SIGTERM);
	Signal::reset(SIGQUIT);
	//SIGTSTP and SIGHUP not handled
	//Rest of the signals not handled
}

void Manager::shutdown(int signum) noexcept {
	ctx.hub->cancel();
}

void Manager::printHelp(FILE *stream) noexcept {
	printVersion(stream);
	printUsage(stream);
	printContact(stream);
}

void Manager::printVersion(FILE *stream) noexcept {
	fprintf(stream, "\n%s %s version %s\nCopyright (C) %s %s.\n",
	WH_PRODUCT_NAME, WH_RELEASE_NAME, WH_RELEASE_VERSION,
	WH_RELEASE_YEAR, WH_RELEASE_AUTHOR);
	fprintf(stream, "LICENSE %s\n\n", WH_LICENSE_TEXT);
}

void Manager::printUsage(FILE *stream) noexcept {
	fprintf(stream, "Usage: %s [OPTIONS]\n", ctx.program);
	fprintf(stream, "OPTIONS\n");
	fprintf(stream, "-c --config   <path>      \tConfiguration file's path.\n");
	fprintf(stream, "-h --help                 \tDisplay usage information.\n");
	fprintf(stream, "-m --menu                 \tShow the application menu.\n");
	fprintf(stream, "-n --name     <identity>  \tSet hub's identity.\n");
	fprintf(stream, "-S --syslog               \tUse syslog.\n");
	fprintf(stream, "-t --type     <type>      \tSet hub's type.\n");
	fprintf(stream, "-v --version              \tPrint the version number.\n");

	fprintf(stream, "\n%s requires an external configuration file.\n"
			"If none is supplied via the command line then the program will\n"
			"try to read '%s' from the 'current working directory',\n"
			"the 'executable directory', %s, or\n"
			"%s in that order.\n\n", WH_PRODUCT_NAME, Identity::CONF_FILE_NAME,
			Identity::CONF_PATH, Identity::CONF_SYSTEM_PATH);
}

void Manager::printContact(FILE *stream) noexcept {
	fprintf(stream, "\nurl: %s   email: %s\n\n", WH_RELEASE_URL,
	WH_RELEASE_EMAIL);
}

void Manager::clear() noexcept {
	ctx.program = nullptr;
	ctx.menu = false;
	ctx.uid = INVALID_UID;
	ctx.type = '\0';
	ctx.config = nullptr;
	ctx.hub = nullptr;
}

} /* namespace wanhive */
