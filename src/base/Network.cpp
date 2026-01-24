/*
 * Network.cpp
 *
 * Stream (TCP/IP) socket routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Network.h"
#include "common/Exception.h"
#include "ipc/DNS.h"
#include "unix/Fcntl.h"
#include "unix/SystemException.h"
#include <cerrno>
#include <cstdio>
#include <sys/un.h>
#include <sys/stat.h>

namespace wanhive {

int Network::server(const char *service, SocketAddress &sa, bool blocking) {
	SocketTraits traits = { AF_UNSPEC, SOCK_STREAM, 0, AI_PASSIVE };
	DNS dns(nullptr, service, &traits);

	const addrinfo *rp; //The iterator
	//Try each address until we successfully bind(2).
	while ((rp = dns.next())) {
		auto sockType =
				blocking ? rp->ai_socktype : rp->ai_socktype | SOCK_NONBLOCK;
		auto sfd = ::socket(rp->ai_family, sockType, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		int yes = 1;
		::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

		if (::bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			::memcpy(&sa.address, rp->ai_addr, rp->ai_addrlen);
			sa.length = rp->ai_addrlen;
			return sfd; /* Success */
		}

		close(sfd); /* Close and continue */
	}

	//Something went wrong
	throw SystemException();
}

int Network::connect(const char *name, const char *service, SocketAddress &sa,
		bool blocking) {
	SocketTraits traits = { AF_UNSPEC, SOCK_STREAM, 0, 0 };
	DNS dns(name, service, &traits);

	const addrinfo *rp; //The iterator
	//Try each address until we successfully connect(2).
	while ((rp = dns.next())) {
		//Linux-specific, saves a system call
		auto sockType =
				blocking ? rp->ai_socktype : rp->ai_socktype | SOCK_NONBLOCK;
		auto sfd = ::socket(rp->ai_family, sockType, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		auto ret = ::connect(sfd, rp->ai_addr, rp->ai_addrlen);
		if (ret == 0 || (!blocking && errno == EINPROGRESS)) {
			::memcpy(&sa.address, rp->ai_addr, rp->ai_addrlen);
			sa.length = rp->ai_addrlen;
			return sfd; /* Success */
		}

		close(sfd); /* Close and continue */
	}

	//Something went wrong
	throw SystemException();
}

int Network::connect(const NameInfo &ni, SocketAddress &sa, bool blocking) {
	return connect(ni.host, ni.service, sa, blocking);
}

void Network::listen(int listener, int backlog) {
	if (::listen(listener, backlog) == -1) {
		throw SystemException();
	}
}

int Network::accept(int listener, SocketAddress &sa, bool blocking) {
	sa.length = sizeof(sockaddr_storage);
	auto flags = blocking ? 0 : SOCK_NONBLOCK;
	auto sfd = ::accept4(listener, (sockaddr*) &sa.address, &sa.length, flags);
	if (sfd != -1) {
		return sfd;
	} else if (errno == EWOULDBLOCK || errno == EAGAIN) {
		return -1;
	} else {
		throw SystemException();
	}
}

int Network::shutdown(int sfd, int how) noexcept {
	return ::shutdown(sfd, how);
}

int Network::close(int sfd) noexcept {
	return ::close(sfd);
}

void Network::setBlocking(int sfd, bool blocking) {
	auto flags = Fcntl::getStatusFlag(sfd);
	if (blocking) {
		flags &= ~O_NONBLOCK;
	} else {
		flags |= O_NONBLOCK;
	}

	Fcntl::setStatusFlag(sfd, flags);
}

bool Network::isBlocking(int sfd) {
	auto val = Fcntl::getStatusFlag(sfd);
	return !(val & O_NONBLOCK);
}

int Network::unixServer(const char *path, SocketAddress &sa, bool blocking) {
	if (!path) {
		throw Exception(EX_NULL);
	}

	auto type = blocking ? SOCK_STREAM : (SOCK_STREAM | SOCK_NONBLOCK);
	auto sfd = ::socket(AF_UNIX, type, 0);
	if (sfd == -1) {
		throw SystemException();
	}

	sockaddr_un local;
	::memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	::strncpy(local.sun_path, path, sizeof(local.sun_path) - 1);

	struct stat filestat;
	if (::stat(local.sun_path, &filestat) == 0 && S_ISSOCK(filestat.st_mode)) {
		//Unlink only if the given file is a valid unix domain socket file
		::unlink(local.sun_path);
	}

	if (::bind(sfd, (struct sockaddr*) &local, sizeof(local)) == -1) {
		close(sfd);
		throw SystemException();
	}

	::memcpy(&sa.address, &local, sizeof(local));
	sa.length = sizeof(local);
	return sfd;
}

int Network::unixConnect(const char *path, SocketAddress &sa, bool blocking) {
	if (!path) {
		throw Exception(EX_NULL);
	}
	auto type = blocking ? SOCK_STREAM : (SOCK_STREAM | SOCK_NONBLOCK);
	auto sfd = ::socket(AF_UNIX, type, 0);
	if (sfd == -1) {
		throw SystemException();
	}

	sockaddr_un remote;
	::memset(&remote, 0, sizeof(remote));
	remote.sun_family = AF_UNIX;
	::strncpy(remote.sun_path, path, sizeof(remote.sun_path) - 1);

	auto ret = ::connect(sfd, (struct sockaddr*) &remote, sizeof(remote));
	if (ret == 0 || (!blocking && errno == EINPROGRESS)) {
		::memcpy(&sa.address, &remote, sizeof(remote));
		sa.length = sizeof(remote);
		return sfd;
	} else {
		close(sfd);
		throw SystemException();
	}
}

void Network::socketPair(int (&sv)[2], bool blocking) {
	auto type = blocking ? SOCK_STREAM : (SOCK_STREAM | SOCK_NONBLOCK);
	if (::socketpair(AF_UNIX, type, 0, sv) == -1) {
		throw SystemException();
	}
}

size_t Network::read(int sfd, unsigned char *buf, size_t bytes, bool strict) {
	auto toRecv = bytes;
	size_t index = 0;
	ssize_t n = 0;
	while (toRecv != 0) {
		if ((n = ::recv(sfd, buf + index, toRecv, 0)) == -1) {
			throw SystemException();
		} else if (n == 0) {
			//Peer shutdown
			if (strict && bytes) {
				throw Exception(EX_RESOURCE);
			} else {
				break;
			}
		} else {
			toRecv -= n;
			index += n;
		}
	}
	return (bytes - toRecv);
}

size_t Network::write(int sfd, const unsigned char *buf, size_t bytes) {
	auto toSend = bytes;
	size_t index = 0;
	ssize_t n = 0;
	while (toSend != 0) {
		if ((n = ::send(sfd, buf + index, toSend, MSG_NOSIGNAL)) == -1) {
			throw SystemException();
		} else {
			toSend -= n;
			index += n;
		}
	}
	return (bytes - toSend);
}

void Network::setReceiveTimeout(int sfd, unsigned int timeout) {
	timeval to;
	to.tv_sec = timeout / 1000;
	to.tv_usec = (timeout % 1000) * 1000;
	if (::setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to))) {
		throw SystemException();
	}
}

void Network::setSendTimeout(int sfd, unsigned int timeout) {
	timeval to;
	to.tv_sec = timeout / 1000;
	to.tv_usec = (timeout % 1000) * 1000;
	if (::setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to))) {
		throw SystemException();
	}
}

void Network::setTimeout(int sfd, int input, int output) {
	if (input >= 0) {
		setReceiveTimeout(sfd, input);
	}
	if (output >= 0) {
		setSendTimeout(sfd, output);
	}
}

} /* namespace wanhive */
