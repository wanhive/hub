# --- SOURCE FILES DECLARATIONS START ---

## src/base/common
WH_BASE_COMMONHEADERS = base/common/Atomic.h base/common/BaseException.h \
	base/common/CommandLine.h base/common/Exception.h base/common/Logger.h \
	base/common/Memory.h base/common/Sink.h base/common/Source.h base/common/Task.h \
	base/common/audit.h base/common/defines.h base/common/pod.h
WH_BASE_COMMONSOURCES = base/common/CommandLine.cpp base/common/Exception.cpp \
	 base/common/Logger.cpp base/common/Memory.cpp

## src/base/ds
WH_BASE_DSHEADERS = base/ds/Array.h base/ds/BinaryHeap.h base/ds/Buffer.h \
	base/ds/BufferVector.h base/ds/CircularBuffer.h base/ds/CircularBufferVector.h \
	base/ds/Counter.h base/ds/Encoding.h base/ds/Khash.h base/ds/MemoryPool.h \
	base/ds/MersenneTwister.h base/ds/Serializer.h base/ds/State.h \
	base/ds/StaticBuffer.h base/ds/StaticCircularBuffer.h base/ds/Twiddler.h \
	base/ds/UID.h base/ds/functors.h
WH_BASE_DSSOURCES = base/ds/Counter.cpp base/ds/Encoding.cpp base/ds/MemoryPool.cpp \
	base/ds/MersenneTwister.cpp base/ds/Serializer.cpp base/ds/State.cpp \
	base/ds/Twiddler.cpp base/ds/UID.cpp

## src/base/ipc
WH_BASE_IPCHEADERS = base/ipc/DNS.h base/ipc/NetworkAddressException.h \
	base/ipc/inet.h
WH_BASE_IPCSOURCES = base/ipc/DNS.cpp base/ipc/NetworkAddressException.cpp

## src/base/security
WH_BASE_SECURITYHEADERS = base/security/CryptoUtils.h base/security/CSPRNG.h \
	base/security/Rsa.h base/security/SecurityException.h base/security/Sha.h \
	base/security/Srp.h base/security/SSLContext.h
WH_BASE_SECURITYSOURCES = base/security/CryptoUtils.cpp base/security/CSPRNG.cpp \
	base/security/Rsa.cpp base/security/SecurityException.cpp base/security/Sha.cpp \
	base/security/Srp.cpp base/security/SSLContext.cpp

## src/base/unix
WH_BASEUNIXHEADERS = base/unix/Config.h base/unix/Directory.h base/unix/Environment.h \
	base/unix/Fcntl.h base/unix/File.h base/unix/FileStream.h base/unix/FileSystem.h \
	base/unix/FStat.h base/unix/Group.h base/unix/OS.h base/unix/PGroup.h \
	base/unix/Process.h base/unix/PSession.h base/unix/PSignal.h base/unix/PThread.h \
	base/unix/SignalAction.h base/unix/SignalSet.h base/unix/SystemException.h \
	base/unix/Time.h base/unix/TSignal.h base/unix/User.h \
	base/unix/WorkingDirectory.h
WH_BASEUNIXSOURCES = base/unix/Config.cpp base/unix/Directory.cpp \
	base/unix/Environment.cpp base/unix/Fcntl.cpp base/unix/File.cpp \
	base/unix/FileStream.cpp base/unix/FileSystem.cpp base/unix/FStat.cpp \
	base/unix/Group.cpp base/unix/OS.cpp base/unix/PGroup.cpp base/unix/Process.cpp \
	base/unix/PSession.cpp base/unix/PSignal.cpp base/unix/PThread.cpp \
	base/unix/SignalAction.cpp base/unix/SignalSet.cpp base/unix/SystemException.cpp \
	base/unix/Time.cpp base/unix/TSignal.cpp base/unix/User.cpp \
	base/unix/WorkingDirectory.cpp

## src/base
WH_BASE_TOPHEADERS = base/Condition.h base/Configuration.h base/Network.h \
	base/Selector.h base/Signal.h base/Storage.h base/System.h base/Thread.h \
	base/Timer.h
WH_BASE_TOPSOURCES = base/Condition.cpp base/Configuration.cpp base/Network.cpp \
	base/Selector.cpp base/Signal.cpp base/Storage.cpp base/System.cpp \
	base/Thread.cpp base/Timer.cpp

## src/base collection
WH_BASEHEADERS = $(WH_BASE_COMMONHEADERS) $(WH_BASE_DSHEADERS) $(WH_BASE_IPCHEADERS) \
	$(WH_BASE_SECURITYHEADERS) $(WH_BASEUNIXHEADERS) $(WH_BASE_TOPHEADERS)

WH_BASESOURCES =  $(WH_BASE_COMMONSOURCES) $(WH_BASE_DSSOURCES) $(WH_BASE_IPCSOURCES) \
	$(WH_BASE_SECURITYSOURCES) $(WH_BASEUNIXSOURCES) $(WH_BASE_TOPSOURCES)

## src/reactor collection
WH_REACTORHEADERS = reactor/Descriptor.h reactor/Handle.h reactor/Handler.h \
	reactor/Reactor.h reactor/Watcher.h reactor/Watchers.h
WH_REACTORSOURCES = reactor/Descriptor.cpp reactor/Reactor.cpp \
	reactor/Watcher.cpp reactor/Watchers.cpp

## src/util collection
WH_UTILHEADERS = util/Authenticator.h util/Endpoint.h util/FlowControl.h \
	util/Frame.h util/Hash.h util/Hosts.h util/InstanceID.h util/Message.h \
	util/MessageAddress.h util/MessageContext.h util/MessageControl.h \
	util/MessageHeader.h util/PKI.h util/Packet.h util/Random.h util/commands.h
WH_UTILSOURCES = util/Authenticator.cpp util/Endpoint.cpp util/FlowControl.cpp \
	util/Frame.cpp util/Hash.cpp util/Hosts.cpp util/InstanceID.cpp \
	util/Message.cpp util/MessageAddress.cpp util/MessageContext.cpp \
	util/MessageControl.cpp util/MessageHeader.cpp util/PKI.cpp \
	util/Packet.cpp util/Random.cpp

## src/hub collection
WH_HUBHEADERS = hub/Alarm.h hub/ClientHub.h hub/Event.h hub/Hub.h \
	hub/Identity.h hub/Inotifier.h hub/Interrupt.h hub/Logic.h \
	hub/Protocol.h hub/Socket.h hub/Topic.h
WH_HUBSOURCES = hub/Alarm.cpp hub/ClientHub.cpp hub/Event.cpp hub/Hub.cpp \
	hub/Identity.cpp hub/Inotifier.cpp hub/Interrupt.cpp hub/Logic.cpp \
	hub/Protocol.cpp hub/Socket.cpp hub/Topic.cpp

## src/server collection
WH_SERVERHEADERS = server/auth/AuthenticationHub.h server/overlay/commands.h \
	server/overlay/DHT.h server/overlay/Finger.h server/overlay/Node.h \
	server/overlay/OverlayHub.h server/overlay/OverlayHubInfo.h \
	server/overlay/OverlayProtocol.h server/overlay/OverlayService.h \
	server/overlay/OverlayTool.h server/overlay/Topics.h
WH_SERVERSOURCES = server/auth/AuthenticationHub.cpp server/overlay/DHT.cpp \
	server/overlay/Finger.cpp server/overlay/Node.cpp server/overlay/OverlayHub.cpp \
	server/overlay/OverlayProtocol.cpp server/overlay/OverlayService.cpp \
	server/overlay/OverlayTool.cpp server/overlay/Topics.cpp

## src/test collection
WH_TESTHEADERS = test/ds/BufferTest.h test/ds/HashTableTest.h \
	test/flood/TestClient.h test/flood/NetworkTest.h \
	test/multicast/MulticastConsumer.h
WH_TESTSOURCES = test/ds/BufferTest.cpp test/ds/HashTableTest.cpp \
	test/flood/TestClient.cpp test/flood/NetworkTest.cpp \
	test/multicast/MulticastConsumer.cpp

## src/app collection
WH_APPHEADERS = app/AppManager.h app/ConfigTool.h app/version.h
WH_APPSOURCES = app/AppManager.cpp app/ConfigTool.cpp wanhive.cpp

## src/ collection
WH_TOPHEADERS = wanhive-base.h wanhive-reactor.h wanhive-util.h wanhive-hub.h wanhive.h

# --- SOURCE FILES DECLARATIONS END ---

# ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)
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
libwanhive_la_CXXFLAGS = -Wall -DWH_CONF_SYSTEM_BASE='"@sysconfdir@"'
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libwanhive_la_LDFLAGS = -version-info @WH_LIB_VERSION@ -lpthread -lsqlite3 -lssl -lcrypto
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

baseipcincludedir = $(pkgincludedir)/base/ipc
baseipcinclude_HEADERS = $(WH_BASE_IPCHEADERS)

basesecurityincludedir = $(pkgincludedir)/base/security
basesecurityinclude_HEADERS = $(WH_BASE_SECURITYHEADERS)

baseunixincludedir = $(pkgincludedir)/base/unix
baseunixinclude_HEADERS = $(WH_BASEUNIXHEADERS)

reactorincludedir = $(pkgincludedir)/reactor
reactorinclude_HEADERS = $(WH_REACTORHEADERS)

utilincludedir = $(pkgincludedir)/util
utilinclude_HEADERS = $(WH_UTILHEADERS)

hubincludedir = $(pkgincludedir)/hub
hubinclude_HEADERS= $(WH_HUBHEADERS)

pkgconfig_DATA = wanhive.pc
