/*
 * ConfigTool.cpp
 *
 * Settings generator
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "ConfigTool.h"
#include "../base/Logger.h"
#include "../base/common/CommandLine.h"
#include "../base/common/Exception.h"
#include "../base/ds/Encoding.h"
#include "../util/Authenticator.h"
#include "../util/Host.h"
#include "../util/PKI.h"
#include <cinttypes>
#include <cstring>
#include <iostream>

namespace wanhive {

ConfigTool::ConfigTool() noexcept {

}

ConfigTool::~ConfigTool() {

}

void ConfigTool::execute() noexcept {
	std::cout << "Select an option\n" << "1. Generate keys\n"
			<< "2. Manage hosts\n" << "3. Generate verifier\n" << "::";

	unsigned int mode = 0;
	std::cin >> mode;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		switch (mode) {
		case 1:
			generateKeyPair();
			break;
		case 2:
			manageHosts();
			break;
		case 3:
			generateVerifier();
			break;
		default:
			std::cout << "Invalid option" << std::endl;
			break;
		}
	} catch (BaseException &e) {
		std::cout << "Request denied" << std::endl;
		WH_LOG_EXCEPTION(e);
	}
}

void ConfigTool::generateKeyPair() {
	char pkf[1024];
	char skf[1024];
	try {
		std::cout << "Enter pathname of the public key file: ";
		std::cin.ignore();
		std::cin.getline(pkf, sizeof(pkf));
		if (CommandLine::inputError()) {
			return;
		}
		std::cout << "Enter pathname of the secret key file: ";
		std::cin.getline(skf, sizeof(skf));
		if (CommandLine::inputError()) {
			return;
		}

		std::cout << "Generating " << PKI::KEY_LENGTH << " bit RSA keys"
				<< std::endl;
		PKI::generateKeyPair(skf, pkf);
	} catch (BaseException &e) {
		throw;
	}
}

void ConfigTool::manageHosts() {
	unsigned int mode = 0;
	char hf[1024];
	char sf[1024];
	std::cout << "Select operation\n"
			<< "1: Dump \"hosts\" file over sqlite3 database\n"
			<< "2: Dump \"hosts\" sqlite3 database over file\n"
			<< "3: Generate a sample \"hosts\" file\n:: ";
	std::cin >> mode;
	if (CommandLine::inputError()) {
		return;
	}

	std::cin.ignore();
	if (mode <= 3) {
		std::cout << "Enter pathname of the \"hosts\" file: ";
		std::cin.getline(hf, sizeof(hf));
		if (CommandLine::inputError()) {
			return;
		}
	}

	if (mode <= 2) {
		std::cout << "Enter pathname of the \"hosts\" database: ";
		std::cin.getline(sf, sizeof(sf));
		if (CommandLine::inputError()) {
			return;
		}
	}
	try {
		if (mode == 1) {
			Host h(sf);
			h.batchUpdate(hf);
		} else if (mode == 2) {
			Host h(sf, true);
			h.batchDump(hf);
		} else if (mode == 3) {
			createDummyHostsFile(hf);
		} else {
			std::cout << "Invalid option" << std::endl;
		}
	} catch (BaseException &e) {
		throw;
	}
}

void ConfigTool::generateVerifier() {
	char name[64];
	char password[64];
	unsigned int rounds;

	std::cout << "Enter identity: ";
	std::cin.ignore();
	std::cin.getline(name, sizeof(name));
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Enter password: ";
	std::cin.getline(password, sizeof(password));
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Enter password hashing rounds: ";
	std::cin >> rounds;
	if (CommandLine::inputError()) {
		return;
	}

	Authenticator *authenticator = nullptr;
	try {
		bool success = (authenticator = new (std::nothrow) Authenticator(true))
				&& authenticator->generateVerifier(name,
						(const unsigned char*) password, strlen(password),
						rounds);

		if (!success) {
			throw Exception(EX_INVALIDOPERATION);
		}

		char buffer[4096];
		const unsigned char *binary;
		unsigned int bytes;
		int offset = snprintf(buffer, sizeof(buffer),
				"{\n   \"id\": \"%s\",\n   \"salt\": \"", name);
		authenticator->getSalt(binary, bytes);
		offset += Encoding::base16Encode(buffer + offset, binary, bytes,
				sizeof(buffer) - offset);
		offset += snprintf(buffer + offset, sizeof(buffer),
				"\",\n   \"verifier\": \"");
		authenticator->getPasswordVerifier(binary, bytes);
		offset += Encoding::base16Encode(buffer + offset, binary, bytes,
				sizeof(buffer) - offset);
		offset += snprintf(buffer + offset, sizeof(buffer), "\"\n}");
		std::cerr << buffer << std::endl;
		delete authenticator;
	} catch (BaseException &e) {
		delete authenticator;
		throw;
	}
}

void ConfigTool::createDummyHostsFile(const char *path) {
	std::cout << "Generating dummy \"hosts\" file..." << std::endl;
	FILE *f = Storage::openStream(path, "wt", true);
	if (f) {
		const char *hostname = "127.0.0.1";
		for (uint64_t id = 0; id < 256; id++) {
			unsigned int port = (9001 + id);
			fprintf(f, "%" PRIu64 "\t%s\t%u%s", id, hostname, port,
					Storage::NEWLINE);
		}
		Storage::closeStream(f);
	} else {
		throw Exception(EX_INVALIDPARAM);
	}
}

} /* namespace wanhive */
