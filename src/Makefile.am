WH_BASE_COMMONHEADERS = base/common/Atomic.h base/common/BaseException.h \
	base/common/CommandLine.h base/common/Exception.h base/common/Memory.h \
	base/common/audit.h base/common/defines.h base/common/pod.h
WH_BASE_DSHEADERS = base/ds/Array.h base/ds/BinaryHeap.h base/ds/Buffer.h \
	base/ds/CircularBuffer.h base/ds/CircularBufferVector.h base/ds/Encoding.h \
	base/ds/Khash.h base/ds/MemoryPool.h base/ds/MersenneTwister.h \
	base/ds/Serializer.h base/ds/State.h base/ds/StaticBuffer.h \
	base/ds/StaticCircularBuffer.h base/ds/Twiddler.h base/ds/functors.h
WH_BASE_TOPHEADERS = base/Condition.h base/Configuration.h base/Logger.h \
	base/Network.h base/NetworkAddressException.h base/Selector.h base/Signal.h \
	base/Storage.h base/System.h base/SystemException.h base/Task.h base/Thread.h \
	base/Timer.h
WH_BASE_SECURITYHEADERS = base/security/CryptoUtils.h base/security/CSPRNG.h \
	base/security/Rsa.h base/security/SecurityException.h base/security/Sha.h \
	base/security/Srp.h base/security/SSLContext.h

WH_BASEHEADERS = $(WH_BASE_COMMONHEADERS) $(WH_BASE_DSHEADERS) $(WH_BASE_TOPHEADERS) \
	$(WH_BASE_SECURITYHEADERS)
WH_BASESOURCES = base/common/CommandLine.cpp base/common/Exception.cpp \
	base/common/Memory.cpp base/ds/Encoding.cpp base/ds/MemoryPool.cpp \
	base/ds/MersenneTwister.cpp base/ds/Serializer.cpp base/ds/State.cpp \
	base/ds/Twiddler.cpp base/Condition.cpp base/Configuration.cpp base/Logger.cpp \
	base/Network.cpp base/NetworkAddressException.cpp base/Selector.cpp \
	base/Signal.cpp base/Storage.cpp base/System.cpp base/SystemException.cpp \
	base/Thread.cpp base/Timer.cpp \
	base/security/CryptoUtils.cpp base/security/CSPRNG.cpp base/security/Rsa.cpp \
	base/security/SecurityException.cpp base/security/Sha.cpp base/security/Srp.cpp \
	base/security/SSLContext.cpp

WH_REACTORHEADERS = reactor/Descriptor.h reactor/Handler.h reactor/Reactor.h \
	reactor/Watcher.h reactor/Watchers.h
WH_REACTORSOURCES = reactor/Descriptor.cpp reactor/Reactor.cpp reactor/Watcher.cpp \
	reactor/Watchers.cpp

WH_UTILHEADERS = util/Authenticator.h util/commands.h util/Endpoint.h util/Hash.h \
	util/Host.h util/Identity.h util/InstanceID.h util/Message.h util/MessageHeader.h \
	util/PKI.h util/Random.h util/TransactionKey.h
WH_UTILSOURCES = util/Authenticator.cpp util/Endpoint.cpp util/Hash.cpp util/Host.cpp \
	util/Identity.cpp util/InstanceID.cpp util/Message.cpp util/MessageHeader.cpp \
	util/PKI.cpp util/Random.cpp

WH_HUBHEADERS = hub/ClientHub.h hub/Clock.h hub/EventNotifier.h hub/Hub.h \
	hub/Inotifier.h hub/Protocol.h hub/SignalWatcher.h hub/Socket.h hub/Topic.h
WH_HUBSOURCES = hub/ClientHub.cpp hub/Clock.cpp hub/EventNotifier.cpp \
	hub/Hub.cpp hub/Inotifier.cpp hub/Protocol.cpp hub/SignalWatcher.cpp \
	hub/Socket.cpp hub/Topic.cpp

WH_SERVERHEADERS = server/auth/AuthenticationHub.h server/overlay/commands.h \
	server/overlay/DHT.h server/overlay/Finger.h server/overlay/Node.h \
	server/overlay/OverlayHub.h server/overlay/OverlayHubInfo.h \
	server/overlay/OverlayProtocol.h server/overlay/OverlayService.h \
	server/overlay/OverlayTool.h server/overlay/Topics.h
WH_SERVERSOURCES = server/auth/AuthenticationHub.cpp server/overlay/DHT.cpp \
	server/overlay/Finger.cpp server/overlay/Node.cpp server/overlay/OverlayHub.cpp \
	server/overlay/OverlayProtocol.cpp server/overlay/OverlayService.cpp \
	server/overlay/OverlayTool.cpp server/overlay/Topics.cpp

WH_TESTHEADERS = test/ds/BufferTest.h test/ds/HashTableTest.h test/flood/Agent.h \
	test/flood/NetworkTest.h test/multicast/MulticastConsumer.h
WH_TESTSOURCES = test/ds/BufferTest.cpp test/ds/HashTableTest.cpp test/flood/Agent.cpp \
	test/flood/NetworkTest.cpp test/multicast/MulticastConsumer.cpp

WH_APPHEADERS = app/AppManager.h app/ConfigTool.h app/version.h
WH_APPSOURCES = app/AppManager.cpp app/ConfigTool.cpp wanhive.cpp

WH_TOPHEADERS = wanhive-base.h wanhive-reactor.h wanhive-util.h wanhive-hub.h wanhive.h

ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)
AM_CXXFLAGS = -Wall

if WH_WITH_EXECUTABLES
bin_PROGRAMS = wanhive
wanhive_SOURCES = $(WH_SERVERHEADERS) $(WH_SERVERSOURCES) $(WH_TESTHEADERS) \
	$(WH_TESTSOURCES) $(WH_APPHEADERS) $(WH_APPSOURCES)
wahhive_CXXFLAGS = -Wall
wanhive_LDFLAGS = -lpq
wanhive_LDADD = libwanhive.la
endif

lib_LTLIBRARIES = libwanhive.la
libwanhive_la_CXXFLAGS = -Wall -DWH_CONF_SYSTEM_BASE="\"@sysconfdir@\""
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libwanhive_la_LDFLAGS = -version-info 1:0:0 -lpthread -lsqlite3 -lssl -lcrypto
libwanhive_la_SOURCES = $(WH_BASEHEADERS) $(WH_BASESOURCES) $(WH_REACTORHEADERS) \
	$(WH_REACTORSOURCES) $(WH_UTILHEADERS) $(WH_UTILSOURCES) $(WH_HUBHEADERS) \
	$(WH_HUBSOURCES) $(WH_TOPHEADERS)

# https://www.gnu.org/software/automake/manual/html_node/Uniform.html
topincludedir = $(pkgincludedir)
topinclude_HEADERS = $(WH_TOPHEADERS)

baseincludedir = $(pkgincludedir)/base
baseinclude_HEADERS= $(WH_BASE_TOPHEADERS)

basecommonincludedir = $(pkgincludedir)/base/common
basecommoninclude_HEADERS = $(WH_BASE_COMMONHEADERS)

basedsincludedir = $(pkgincludedir)/base/ds
basedsinclude_HEADERS = $(WH_BASE_DSHEADERS)

basesecurityincludedir = $(pkgincludedir)/base/security
basesecurityinclude_HEADERS = $(WH_BASE_SECURITYHEADERS)

reactorincludedir = $(pkgincludedir)/reactor
reactorinclude_HEADERS = $(WH_REACTORHEADERS)

utilincludedir = $(pkgincludedir)/util
utilinclude_HEADERS = $(WH_UTILHEADERS)

hubincludedir = $(pkgincludedir)/hub
hubinclude_HEADERS= $(WH_HUBHEADERS)

pkgconfig_DATA = wanhive.pc
