/*
 * wanhive-base.h
 *
 * Basic functions headers
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WANHIVE_BASE_H_
#define WANHIVE_BASE_H_

/*
 * Common
 */
#include "base/common/CommandLine.h"
#include "base/common/Logger.h"
#include "base/common/Sink.h"
#include "base/common/Source.h"

/*
 * Data structures
 */
#include "base/ds/BinaryHeap.h"
#include "base/ds/Buffer.h"
#include "base/ds/CircularBuffer.h"
#include "base/ds/Encoding.h"
#include "base/ds/Feature.h"
#include "base/ds/Gradient.h"
#include "base/ds/Handle.h"
#include "base/ds/Khash.h"
#include "base/ds/MersenneTwister.h"
#include "base/ds/Pooled.h"
#include "base/ds/ReadyList.h"
#include "base/ds/Serializer.h"
#include "base/ds/State.h"
#include "base/ds/StaticBuffer.h"
#include "base/ds/StaticCircularBuffer.h"
#include "base/ds/Tokens.h"
#include "base/ds/UID.h"

/*
 * IPC library
 */
#include "base/ipc/DNS.h"
#include "base/ipc/NetworkAddressException.h"

/*
 * System utilities
 */
#include "base/Configuration.h"
#include "base/Network.h"
#include "base/Selector.h"
#include "base/Signal.h"
#include "base/Storage.h"
#include "base/System.h"
#include "base/Thread.h"
#include "base/Timer.h"
#include "base/TurnGate.h"

/*
 * Cryptography
 */
#include "base/security/CryptoUtils.h"
#include "base/security/CSPRNG.h"
#include "base/security/Rsa.h"
#include "base/security/SecurityException.h"
#include "base/security/Sha.h"
#include "base/security/Srp.h"
#include "base/security/SSLContext.h"

/*
 * System library
 */
#include "base/unix/Config.h"
#include "base/unix/Directory.h"
#include "base/unix/Environment.h"
#include "base/unix/Fcntl.h"
#include "base/unix/File.h"
#include "base/unix/FileStream.h"
#include "base/unix/FileSystem.h"
#include "base/unix/FStat.h"
#include "base/unix/Group.h"
#include "base/unix/Mounts.h"
#include "base/unix/OS.h"
#include "base/unix/PGroup.h"
#include "base/unix/Process.h"
#include "base/unix/PSession.h"
#include "base/unix/PSignal.h"
#include "base/unix/SignalAction.h"
#include "base/unix/SystemException.h"
#include "base/unix/Time.h"
#include "base/unix/TSignal.h"
#include "base/unix/User.h"
#include "base/unix/WorkingDirectory.h"

#endif /* WANHIVE_BASE_H_ */
