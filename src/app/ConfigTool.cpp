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
#include "../base/common/CommandLine.h"
#include "../base/common/Exception.h"
#include "../base/common/Logger.h"
#include "../base/ds/Encoding.h"
#include "../util/Authenticator.h"
#include "../util/Hosts.h"
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

	int mode;
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
			std::cerr << "Invalid option" << std::endl;
			break;
		}
	} catch (const BaseException &e) {
		std::cerr << "Request denied" << std::endl;
		WH_LOG_EXCEPTION(e);
	}
}

void ConfigTool::generateKeyPair() {
	char pkf[1024] = { '\0' };
	char skf[1024] = { '\0' };
	try {
		std::cout << "Pathname of the public key file: ";
		std::cin.ignore();
		std::cin.getline(pkf, sizeof(pkf));
		if (CommandLine::inputError()) {
			return;
		}
		std::cout << "Pathname of the secret key file: ";
		std::cin.getline(skf, sizeof(skf));
		if (CommandLine::inputError()) {
			return;
		}

		std::cout << "Generating " << PKI::KEY_LENGTH << " bit RSA keys"
				<< std::endl;
		PKI::generateKeyPair(skf, pkf);
	} catch (const BaseException &e) {
		throw;
	}
}

void ConfigTool::manageHosts() {
	char hf[1024] = { '\0' };
	char sf[1024] = { '\0' };
	unsigned int mode; //unsigned

	std::cout << "Select operation\n"
			<< "1: Dump the \"hosts\" file into an SQLite3 database\n"
			<< "2: Dump the SQLite3 \"hosts\" database into a file\n"
			<< "3: Generate a sample \"hosts\" file\n:: ";

	std::cin >> mode;
	if (CommandLine::inputError()) {
		return;
	}

	std::cin.ignore();
	if (mode <= 3) {
		std::cout << "Pathname of the \"hosts\" file: ";
		std::cin.getline(hf, sizeof(hf));
		if (CommandLine::inputError()) {
			return;
		}
	}

	if (mode <= 2) {
		std::cout << "Pathname of the \"hosts\" database: ";
		std::cin.getline(sf, sizeof(sf));
		if (CommandLine::inputError()) {
			return;
		}
	}
	try {
		if (mode == 1) {
			Hosts hosts(sf);
			hosts.batchUpdate(hf);
		} else if (mode == 2) {
			Hosts hosts(sf, true);
			hosts.batchDump(hf);
		} else if (mode == 3) {
			createDummyHostsFile(hf);
		} else {
			std::cout << "Invalid option" << std::endl;
		}
	} catch (const BaseException &e) {
		throw;
	}
}

void ConfigTool::generateVerifier() {
	char name[64] = { '\0' };
	char secret[64] = { '\0' };
	unsigned int rounds { 0 };

	std::cout << "Identity: ";
	std::cin.ignore();
	std::cin.getline(name, sizeof(name));
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Password: ";
	std::cin.getline(secret, sizeof(secret));
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Password hashing rounds: ";
	std::cin >> rounds;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		Data password { (const unsigned char*) secret, strlen(secret) };
		Authenticator auth(true);
		if (!auth.generateVerifier(name, password, rounds)) {
			throw Exception(EX_OPERATION);
		}

		char buffer[4096];
		Data data;

		std::cout << "{\n";
		std::cout << " \"id\": \"" << name << "\",\n";
		auth.getSalt(data);
		buffer[0] = '\0';
		Encoding::base16Encode(buffer, data.base, data.length, sizeof(buffer));
		std::cout << " \"salt\": \"" << buffer << "\",\n";
		auth.getPasswordVerifier(data);
		buffer[0] = '\0';
		Encoding::base16Encode(buffer, data.base, data.length, sizeof(buffer));
		std::cout << " \"verifier\": \"" << buffer << "\"\n";
		std::cout << "}" << std::endl;
	} catch (const BaseException &e) {
		throw;
	}
}

void ConfigTool::createDummyHostsFile(const char *path) {
	std::cout << "Generating a sample \"hosts\" file..." << std::endl;
	Hosts::createDummy(path);
}

} /* namespace wanhive */
