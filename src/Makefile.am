# --- SOURCE FILES DECLARATIONS START ---

## src/base/common
WH_BASE_COMMONHEADERS = base/common/Activity.h base/common/Atomic.h \
	base/common/BaseException.h base/common/CommandLine.h \
	base/common/Duplex.h base/common/Exception.h base/common/Logger.h \
	base/common/Memory.h base/common/NonCopyable.h base/common/Sink.h \
	base/common/Source.h base/common/Task.h base/common/defines.h \
	base/common/reflect.h
WH_BASE_COMMONSOURCES = base/common/CommandLine.cpp base/common/Exception.cpp \
	 base/common/Logger.cpp

## src/base/db
WH_BASE_DBHEADERS = base/db/DataStore.h base/db/Postgres.h base/db/SQLite.h
WH_BASE_DBSOURCES = base/db/DataStore.cpp base/db/Postgres.cpp base/db/SQLite.cpp

## src/base/ds
WH_BASE_DSHEADERS = base/ds/BinaryHeap.h base/ds/Buffer.h base/ds/BufferVector.h \
	base/ds/CircularBuffer.h base/ds/CircularBufferVector.h base/ds/Counter.h \
	base/ds/Encoding.h base/ds/Feature.h base/ds/Gradient.h base/ds/Handle.h \
	base/ds/Mean.h base/ds/Khash.h base/ds/MemoryPool.h base/ds/MersenneTwister.h \
	base/ds/Pooled.h base/ds/ReadyList.h base/ds/Serializer.h base/ds/Spatial.h \
	base/ds/State.h base/ds/StaticBuffer.h base/ds/StaticCircularBuffer.h \
	base/ds/Tokens.h base/ds/Twiddler.h base/ds/UID.h base/ds/functors.h
WH_BASE_DSSOURCES = base/ds/Counter.cpp base/ds/Encoding.cpp base/ds/Gradient.cpp \
	base/ds/MemoryPool.cpp base/ds/MersenneTwister.cpp base/ds/Serializer.cpp \
	base/ds/State.cpp base/ds/Tokens.cpp base/ds/Twiddler.cpp base/ds/UID.cpp

## src/base/ipc
WH_BASE_IPCHEADERS = base/ipc/DNS.h base/ipc/NetworkAddressException.h \
	base/ipc/inet.h
WH_BASE_IPCSOURCES = base/ipc/DNS.cpp base/ipc/NetworkAddressException.cpp

## src/base/security
WH_BASE_SECURITYHEADERS = base/security/CryptoUtils.h base/security/CSPRNG.h \
	base/security/KeyPair.h base/security/Rsa.h base/security/Sha.h \
	base/security/Srp.h base/security/SSLContext.h
WH_BASE_SECURITYSOURCES = base/security/CryptoUtils.cpp base/security/CSPRNG.cpp \
	base/security/KeyPair.cpp base/security/Rsa.cpp base/security/Sha.cpp \
	base/security/Srp.cpp base/security/SSLContext.cpp

## src/base/unix
WH_BASEUNIXHEADERS = base/unix/Config.h base/unix/Directory.h base/unix/Environment.h \
	base/unix/Fcntl.h base/unix/File.h base/unix/FileStream.h base/unix/FileSystem.h \
	base/unix/FStat.h base/unix/Group.h base/unix/Mounts.h base/unix/OS.h \
	base/unix/PGroup.h base/unix/Process.h base/unix/PSession.h \
	base/unix/PSignal.h base/unix/PThread.h base/unix/SignalAction.h \
	base/unix/SignalSet.h base/unix/SystemException.h base/unix/Time.h \
	base/unix/TSignal.h base/unix/User.h base/unix/WorkingDirectory.h
WH_BASEUNIXSOURCES = base/unix/Config.cpp base/unix/Directory.cpp \
	base/unix/Environment.cpp base/unix/Fcntl.cpp base/unix/File.cpp \
	base/unix/FileStream.cpp base/unix/FileSystem.cpp base/unix/FStat.cpp \
	base/unix/Group.cpp base/unix/Mounts.cpp base/unix/OS.cpp \
	base/unix/PGroup.cpp base/unix/Process.cpp base/unix/PSession.cpp \
	base/unix/PSignal.cpp base/unix/PThread.cpp base/unix/SignalAction.cpp \
	base/unix/SignalSet.cpp base/unix/SystemException.cpp \
	base/unix/Time.cpp base/unix/TSignal.cpp base/unix/User.cpp \
	base/unix/WorkingDirectory.cpp

## src/base
WH_VERHEADERS = base/version.h
WH_VERSOURCES = base/version.cpp
WH_BASE_TOPHEADERS = base/Network.h base/Options.h base/Selector.h \
	base/Signal.h base/Storage.h base/System.h base/Thread.h base/Timer.h \
	base/TurnGate.h
WH_BASE_TOPSOURCES = base/Network.cpp base/Options.cpp base/Selector.cpp \
	base/Signal.cpp base/Storage.cpp base/System.cpp base/Thread.cpp \
	base/Timer.cpp base/TurnGate.cpp

## src/base collection
WH_BASEHEADERS = $(WH_BASE_COMMONHEADERS) $(WH_BASE_DBHEADERS) \
	$(WH_BASE_DSHEADERS) $(WH_BASE_IPCHEADERS) $(WH_BASE_SECURITYHEADERS) \
	$(WH_BASEUNIXHEADERS) $(WH_BASE_TOPHEADERS)

WH_BASESOURCES =  $(WH_BASE_COMMONSOURCES) $(WH_BASE_DBSOURCES) \
	$(WH_BASE_DSSOURCES) $(WH_BASE_IPCSOURCES) $(WH_BASE_SECURITYSOURCES) \
	$(WH_BASEUNIXSOURCES) $(WH_BASE_TOPSOURCES)

## src/reactor collection
WH_REACTORHEADERS = reactor/Descriptor.h reactor/Handler.h reactor/Reactor.h \
	reactor/Watcher.h
WH_REACTORSOURCES = reactor/Descriptor.cpp reactor/Reactor.cpp reactor/Watcher.cpp

## src/util collection
WH_UTILHEADERS = util/Endpoint.h util/FlowControl.h util/Frame.h util/Hash.h \
	util/Hosts.h util/InstanceID.h util/Message.h util/MessageAddress.h \
	util/MessageContext.h util/MessageControl.h util/MessageHeader.h \
	util/PKI.h util/Packet.h util/Random.h util/Verifier.h util/commands.h
WH_UTILSOURCES = util/Endpoint.cpp util/FlowControl.cpp util/Frame.cpp \
	util/Hash.cpp util/Hosts.cpp util/InstanceID.cpp util/Message.cpp \
	util/MessageAddress.cpp util/MessageContext.cpp util/MessageControl.cpp \
	util/MessageHeader.cpp util/PKI.cpp util/Packet.cpp util/Random.cpp \
	util/Verifier.cpp

## src/hub collection
WH_HUBHEADERS = hub/Agent.h hub/Alarm.h hub/Event.h hub/Hub.h \
	hub/HubInfo.h hub/Identity.h hub/Inotifier.h hub/Interrupt.h \
	hub/Job.h hub/Logic.h hub/Protocol.h hub/Socket.h hub/Stream.h \
	hub/Topic.h hub/Watchers.h
WH_HUBSOURCES = hub/Agent.cpp hub/Alarm.cpp hub/Event.cpp hub/Hub.cpp \
	hub/HubInfo.cpp hub/Identity.cpp hub/Inotifier.cpp hub/Interrupt.cpp \
	hub/Job.cpp hub/Logic.cpp hub/Protocol.cpp hub/Socket.cpp hub/Stream.cpp \
	hub/Topic.cpp hub/Watchers.cpp

## src/server collection
WH_SERVERHEADERS = server/auth/AuthenticationHub.h server/auth/Things.h \
	server/core/DHT.h server/core/Finger.h server/core/Node.h \
	server/core/OverlayHub.h server/core/OverlayHubInfo.h \
	server/core/OverlayProtocol.h server/core/OverlayService.h \
	server/core/OverlayTool.h server/core/Topics.h server/core/commands.h
WH_SERVERSOURCES = server/auth/AuthenticationHub.cpp server/auth/Things.cpp \
	server/core/DHT.cpp server/core/Finger.cpp server/core/Node.cpp \
	server/core/OverlayHub.cpp server/core/OverlayHubInfo.cpp \
	server/core/OverlayProtocol.cpp server/core/OverlayService.cpp \
	server/core/OverlayTool.cpp server/core/Topics.cpp

## src/test collection
WH_TESTHEADERS = test/ds/BufferTest.h test/ds/HashTableTest.h \
	test/flood/TestClient.h test/flood/NetworkTest.h \
	test/multicast/MulticastConsumer.h
WH_TESTSOURCES = test/ds/BufferTest.cpp test/ds/HashTableTest.cpp \
	test/flood/TestClient.cpp test/flood/NetworkTest.cpp \
	test/multicast/MulticastConsumer.cpp

## src/app collection
WH_APPHEADERS = app/ConfigTool.h app/Manager.h
WH_APPSOURCES = app/ConfigTool.cpp app/Manager.cpp wanhive.cpp

## src/ collection
WH_TOPHEADERS = wanhive-base.h wanhive-reactor.h wanhive-util.h wanhive-hub.h wanhive.h

# --- SOURCE FILES DECLARATIONS END ---

AM_CXXFLAGS = -Wall -Werror

if WH_WITH_EXECUTABLES
bin_PROGRAMS = wanhive
wanhive_SOURCES = $(WH_SERVERHEADERS) $(WH_SERVERSOURCES) $(WH_TESTHEADERS) \
	$(WH_TESTSOURCES) $(WH_VERHEADERS) $(WH_APPHEADERS) $(WH_APPSOURCES)
wanhive_CXXFLAGS = -Wall
wanhive_LDADD = libwanhive.la
endif

lib_LTLIBRARIES = libwanhive.la
libwanhive_la_CXXFLAGS = $(AM_CXXFLAGS) -DWH_CONF_SYSTEM_BASE='"@sysconfdir@"'
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libwanhive_la_LDFLAGS = -version-info @WH_LIB_VERSION@
libwanhive_la_SOURCES = $(WH_BASEHEADERS) $(WH_BASESOURCES) $(WH_REACTORHEADERS) \
	$(WH_REACTORSOURCES) $(WH_UTILHEADERS) $(WH_UTILSOURCES) $(WH_HUBHEADERS) \
	$(WH_HUBSOURCES) $(WH_TOPHEADERS) $(WH_VERHEADERS) $(WH_VERSOURCES)

# --- Library headers ---
# https://www.gnu.org/software/automake/manual/html_node/Uniform.html
# https://www.gnu.org/software/automake/manual/html_node/Headers.html
nobase_pkginclude_HEADERS = $(WH_TOPHEADERS) $(WH_BASE_TOPHEADERS) \
	$(WH_BASE_COMMONHEADERS) $(WH_BASE_DBHEADERS) $(WH_BASE_DSHEADERS) \
	$(WH_BASE_IPCHEADERS) $(WH_BASE_SECURITYHEADERS) $(WH_BASEUNIXHEADERS) \
	$(WH_REACTORHEADERS) $(WH_UTILHEADERS) $(WH_HUBHEADERS)

# --- pkg-config metadata file ---
pkgconfig_DATA = wanhive.pc
