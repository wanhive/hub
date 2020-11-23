/*
 * Network.cpp
 *
 * Linux network programming routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Network.h"
#include "NetworkAddressException.h"
#include "SystemException.h"
#include "common/Exception.h"
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/stat.h>

namespace wanhive {

int Network::serverSocket(const char *service, socketAddress &sa, bool blocking,
		int type, int family, int protocol) {
	int sfd;
	addrinfo *result, *rp;
	result = getAddrInfo(nullptr, service, family, type, AI_PASSIVE, protocol);

	/*
	 * getaddrinfo() returns a list of address structures.
	 * Try each address until we successfully bind(2).
	 * If socket(2) (or bind(2)) fails, we (close the socket and)
	 * try the next address.
	 */
	for (rp = result; rp != nullptr; rp = rp->ai_next) {
		int sockType =
				blocking ? rp->ai_socktype : rp->ai_socktype | SOCK_NONBLOCK;
		sfd = ::socket(rp->ai_family, sockType, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		int yes = 1;
		setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		//setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int));

		if (::bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			memcpy(&sa.address, rp->ai_addr, rp->ai_addrlen);
			sa.length = rp->ai_addrlen;
			break; /* Success */
		}
		close(sfd);
	}

	freeAddrInfo(result); //No longer needed
	if (rp == nullptr) {
		throw SystemException();
	}
	return sfd;
}

int Network::connectedSocket(const char *name, const char *service,
		socketAddress &sa, bool blocking, int type, int family, int flags,
		int protocol) {
	int sfd = -1;
	addrinfo *result, *rp;
	result = getAddrInfo(name, service, family, type, flags, protocol);

	/*
	 * getaddrinfo() returns a list of address structures.
	 * Try each address until we successfully connect(2).
	 * If socket(2) (or connect(2)) fails, we (close the socket and)
	 * try the next address.
	 */
	for (rp = result; rp != nullptr; rp = rp->ai_next) {
		//Linux-specific, saves a system call
		int sockType =
				blocking ? rp->ai_socktype : rp->ai_socktype | SOCK_NONBLOCK;
		sfd = ::socket(rp->ai_family, sockType, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		int ret = ::connect(sfd, rp->ai_addr, rp->ai_addrlen);
		if (ret == 0 || (!blocking && errno == EINPROGRESS)) {
			memcpy(&sa.address, rp->ai_addr, rp->ai_addrlen);
			sa.length = rp->ai_addrlen;
			break; /* Success */
		}
		close(sfd);
	}

	freeAddrInfo(result); //No longer needed
	if (rp == nullptr) {
		throw SystemException();
	}
	return sfd;
}

int Network::connectedSocket(const nameInfo &ni, socketAddress &sa,
		bool blocking, int type, int family, int flags, int protocol) {
	return connectedSocket(ni.host, ni.service, sa, blocking, type, family,
			flags, protocol);
}

int Network::socket(const char *name, const char *service, socketAddress &sa,
		int type, int family, int flags, int protocol) {
	int sfd;
	addrinfo *result, *rp;
	result = getAddrInfo(name, service, family, type, flags, protocol);
	for (rp = result; rp != nullptr; rp = rp->ai_next) {
		sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		//store the details of socket
		memcpy(&sa.address, rp->ai_addr, rp->ai_addrlen);
		sa.length = rp->ai_addrlen;
		break;
	}

	freeAddrInfo(result); //No longer needed
	if (rp == nullptr) {
		throw SystemException();
	}
	return sfd;
}

int Network::socket(const nameInfo &ni, socketAddress &sa, int type, int family,
		int flags, int protocol) {
	return socket(ni.host, ni.service, sa, type, family, flags, protocol);
}

void Network::bind(int sfd, socketAddress &sa) {
	if (::bind(sfd, (sockaddr*) &sa.address, sa.length) == -1) {
		throw SystemException();
	}
}

void Network::listen(int sfd, int backlog) {
	if (::listen(sfd, backlog) == -1) {
		throw SystemException();
	}
}

int Network::accept(int listenfd, socketAddress &sa, int flags) {
	sa.length = sizeof(sockaddr_storage);
	int sfd = ::accept4(listenfd, (sockaddr*) &sa.address, &sa.length, flags);
	if (sfd != -1) {
		return sfd;
	} else if (errno == EWOULDBLOCK || errno == EAGAIN) {
		return -1;
	} else {
		throw SystemException();
	}
}

int Network::connect(int sfd, socketAddress &sa) {
	int ret = ::connect(sfd, (sockaddr*) (&sa.address), sa.length);
	if (ret != -1) {
		return ret; //=0
	} else if (errno == EINPROGRESS) {
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

void Network::setBlocking(int sfd, bool block) {
	int flags = fcntl(sfd, F_GETFL);
	if (flags != -1) {
		if (block) {
			flags &= ~O_NONBLOCK;
		} else {
			flags |= O_NONBLOCK;
		}

		if (fcntl(sfd, F_SETFL, flags) == 0) {
			return;
		}
	}

	//If we have reached here, then there must have been an error
	throw SystemException();
}

bool Network::isBlocking(int sfd) {
	int val;
	if ((val = fcntl(sfd, F_GETFL, 0)) == -1) {
		throw SystemException();
	} else {
		return !(val & O_NONBLOCK);
	}
}

int Network::unixServerSocket(const char *path, socketAddress &sa,
		bool blocking, int type, int protocol) {
	if (!path) {
		throw Exception(EX_NULL);
	}

	int sockType = !blocking ? type | SOCK_NONBLOCK : type;
	int sfd = ::socket(AF_UNIX, sockType, protocol);
	if (sfd == -1) {
		throw SystemException();
	}

	sockaddr_un local;
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	strncpy(local.sun_path, path, sizeof(local.sun_path) - 1);

	struct stat filestat;
	if (!stat(local.sun_path, &filestat) && S_ISSOCK(filestat.st_mode)) {
		//Unlink only if the given file is a valid unix domain socket file
		unlink(local.sun_path);
	}

	if (::bind(sfd, (struct sockaddr*) &local, sizeof(local)) == -1) {
		close(sfd);
		throw SystemException();
	}

	memcpy(&sa.address, &local, sizeof(local));
	sa.length = sizeof(local);
	return sfd;
}

int Network::unixConnectedSocket(const char *path, socketAddress &sa,
		bool blocking, int type, int protocol) {
	if (!path) {
		throw Exception(EX_NULL);
	}
	int sockType = !blocking ? type | SOCK_NONBLOCK : type;
	int sfd = ::socket(AF_UNIX, sockType, protocol);
	if (sfd == -1) {
		throw SystemException();
	}

	sockaddr_un remote;
	memset(&remote, 0, sizeof(remote));
	remote.sun_family = AF_UNIX;
	strncpy(remote.sun_path, path, sizeof(remote.sun_path) - 1);

	int ret = ::connect(sfd, (struct sockaddr*) &remote, sizeof(remote));
	if (ret == 0 || (!blocking && errno == EINPROGRESS)) {
		memcpy(&sa.address, &remote, sizeof(remote));
		sa.length = sizeof(remote);
		return sfd;
	} else {
		close(sfd);
		throw SystemException();
	}
}

void Network::socketPair(int sv[2], bool blocking, int type) {
	type = blocking ? type : (type | SOCK_NONBLOCK);
	if (socketpair(AF_UNIX, type, 0, sv) == -1) {
		throw SystemException();
	}
}

void Network::getNameInfo(const socketAddress &sa, nameInfo &ni, int flags) {
	int s;
	s = getnameinfo((sockaddr*) &sa.address, sa.length, ni.host, NI_MAXHOST,
			ni.service, NI_MAXSERV, flags);
	if (s) {
		throw NetworkAddressException(s);
	}
}

void Network::getSockName(int sfd, socketAddress &sa) {
	sa.length = sizeof(sockaddr_storage);
	if (getsockname(sfd, (sockaddr*) &sa.address, &sa.length)) {
		throw SystemException();
	}
}

void Network::getPeerName(int sfd, socketAddress &sa) {
	sa.length = sizeof(sockaddr_storage);
	if (getpeername(sfd, (sockaddr*) &sa.address, &sa.length))
		throw SystemException();
}

size_t Network::sendStream(int sockfd, const unsigned char *buf,
		size_t length) {
	size_t toSend = length;
	size_t index = 0;
	ssize_t n = 0;
	while (toSend != 0) {
		if ((n = send(sockfd, buf + index, toSend, MSG_NOSIGNAL)) == -1) {
			throw SystemException();
		} else {
			toSend -= n;
			index += n;
		}
	}
	return (length - toSend);
}

size_t Network::receiveStream(int sockfd, unsigned char *buf, size_t length,
		bool strict) {
	size_t toRecv = length;
	size_t index = 0;
	ssize_t n = 0;
	while (toRecv != 0) {
		if ((n = recv(sockfd, buf + index, toRecv, 0)) == -1) {
			throw SystemException();
		} else if (n == 0) {
			//Peer shutdown
			if (strict && length) {
				throw Exception(EX_RESOURCE);
			} else {
				break;
			}
		} else {
			toRecv -= n;
			index += n;
		}
	}
	return (length - toRecv);
}

void Network::setReceiveTimeout(int sfd, int milliseconds) {
	timeval to;
	to.tv_sec = milliseconds / 1000;
	to.tv_usec = (milliseconds % 1000) * 1000;
	if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(timeval))) {
		throw SystemException();
	}
}

void Network::setSendTimeout(int sfd, int milliseconds) {
	timeval to;
	to.tv_sec = milliseconds / 1000;
	to.tv_usec = (milliseconds % 1000) * 1000;
	if (setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(timeval))) {
		throw SystemException();
	}
}

void Network::setSocketTimeout(int sfd, int recvTimeout, int sendTimeout) {
	if (recvTimeout >= 0) {
		setReceiveTimeout(sfd, recvTimeout);
	}
	if (sendTimeout >= 0) {
		setSendTimeout(sfd, sendTimeout);
	}
}

addrinfo* Network::getAddrInfo(const char *name, const char *service,
		int family, int type, int flags, int protocol) {
	addrinfo hints, *result;
	//Prepare the hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_flags = flags;
	hints.ai_protocol = protocol;

	int s = getaddrinfo(name, service, &hints, &result);
	if (s) {
		throw NetworkAddressException(s);
	}
	return result;
}

void Network::freeAddrInfo(addrinfo *res) noexcept {
	freeaddrinfo(res);
}

} /* namespace wanhive */
