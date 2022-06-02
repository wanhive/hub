# Requirements

* Supported CPU architectures: X86, X86-64, ARM (32 and 64 bit)
* Operating systems: GNU/Linux (Linux kernel version 5.x or higher)
* C/C++ compilers: GCC version 8 or higher (11 or higher recommended)
* Programming languages: C++17
* Shared libraries
    * pthread
    * sqlite3
    * ssl
    * crypto

### Optional components

* Shared libraries
    * pq (PostgreSQL driver required by the authentication hub)
* PostgreSQL server version 10 or above (required by authentication hub and Wanhive IoT web console)
* Wanhive IoT web console (web application for the management of users and IoT endpoints)

# Build and install

* Extract the archive.

```
tar -xvzf <archive-name>
```

* Change the current directory (`cd`) to the extracted directory.
* Configure the package for your system.

```
autoreconf -ifv                     (update the build system)
```

```
./configure --help                  (view configuration options)
./configure                         (default configuration)
./configure --without-executables   (build library and headers only)
```

* Compile the package.

```
make
```

* Install the programs.

```
make install
```

* Uninstall the programs.

```
make uninstall
```

* Remove the program binaries and object files from the source directory.

```
make clean
```

* Remove the temporary files generated during configuration.

```
make distclean
```

# Getting started

Verify the installation:

```
wanhive -h
wanhive --help
```

Read the output carefully.

## Minimal setup

Follow these steps to create a "bare minimum" functional setup for testing.

* Create **$HOME/.config/wanhive** directory.
    
```
mkdir -pv $HOME/.config/wanhive
```
    
* Copy following files from this source package to the directory created in the previous step:
    * data/wanhive.conf
    * data/hosts
    * data/nodes
    * data/auths

```
cp data/{wanhive.conf,hosts,nodes,auths} $HOME/.config/wanhive/
```

**NOTE**: All of the above files get copied into the [datadir](https://www.gnu.org/software/automake/manual/html_node/Standard-Directory-Variables.html) during [installation](#build-and-install).

```
wanhive -to -n 1
wanhive -m
```

Press **Ctrl+C** to exit the program.

**NOTES:**

* Clients should not use the identifiers in the range [0-65535].
* Overlay hubs use the identifiers in the range [0-1023].
* Clients cannot connect with the overlay hub attached to the identifier 0.

# Setup options

## Basic setup

* Install this package.
    * Sufficient for [testing](#getting-started).
    * Sufficient for building small IoT applications over a local IP network.
    * Supports clustering.
    * Does not support user and endpoint management.
    * Does not support authentication.
    * Does not support access control.
    * Does not support device type identification.

## Standard setup

* Install this package.
* Install Postgresql database server 10 or above.
* Install the [Wanhive IoT web console](https://github.com/wanhive/webconsole).
* Install [Network key](#network-key-optional).
* Enable [Client authentication](#client-authentication-optional).
* Enable [Domain controlled access](#domain-controlled-access-optional).

# Configuration

Wanhive hub is configured using an external configuration file.

If the command line doesn't provide the configuration file's pathname, then the program will attempt to read **wanhive.conf** from the locations listed below (in the given order):

1. Current working directory (`$PWD`)
2. Executable's directory
3. $HOME/.config/wanhive
4. [sysconfdir](https://www.gnu.org/software/automake/manual/html_node/Standard-Directory-Variables.html)

## Configuration file

The configuration file uses a restricted version of the [INI file](https://en.wikipedia.org/wiki/INI_file) format.

* Update the **HOSTS** and **BOOTSTRAP** sections of the configuration file correctly.
    * Do not skip this step.
* This package includes a sample configuration file (*wanhive.conf*).
* After installation, you can find a copy of the sample configuration file inside the [datadir](https://www.gnu.org/software/automake/manual/html_node/Standard-Directory-Variables.html).

## Hosts

Wanhive hubs use a special text file to learn the network addresses.

* A server uses this file to bind it's listening socket to the correct address.
* A client uses this file to find and connect to a server.

The required steps have been summarized below:

1. Create a text file containing addresses of all the *available* hosts (overlay and authentication hubs).
2. Update the **HOSTS** section of the configuration file as illustrated below.

```
[HOSTS]
.
.
hostsFile = <pathname-of-hosts-file>
```

### The hosts file

A **hosts file** contains rows of *tab-separated* tuples:

```
IDENTITY	HOST	SERVICE		[TYPE]
```

Each row maps an **identifier** to a **network address** and a **type (optional)**.

For example:

```
0	/home/user/wh0.uds	unix
1	127.0.0.1	9001	1
2	hub2.example.com	9002
3	testmachine.local	9003
256	localhost	5555	2

```

**NOTES**:

* **IDENTITY** and **TYPE** should be non-negative integers.
* The **TYPE** column is optional.
* The following **TYPE** codes have special meanings ([see bootstrapping](#bootstrap)):
    * An overlay hub (**1**)
    * An authentication hub (**2**)
* Server applications bind their listening TCP/IP socket to the wildcard address.
* Specify **unix** under the **SERVICE** column to create a *Unix domain socket*.

#### Option 1: Create manually

You can use a text editor to create or edit a **hosts file**. For example, the following **hosts file** describes five hubs:

```
0	127.0.0.1	9000
1	127.0.0.1	9001
2	127.0.0.1	9002
3	127.0.0.1	9003
256	example.com	5555
```

1. Four hubs with identities *0, 1, 2, and 3* running on *127.0.0.1* at ports *9000, 9001, 9002, and 9003* in that order.
2. A hub with identity 256 and listening at *example.com:5555*.

**NOTES**:

* Columns are **TAB-separated**.

#### Option 2: Generate a dummy file

```
wanhive -m
```

1. Select *UTILITIES(2)* from the list of available options.
2. Next, select *Manage hosts(2)* and then,
3. Select *Generate a sample "hosts" file(3)*.
4. Follow further instructions.
5. Edit the automatically generated file in your favorite text editor.

#### Option 3: Use the default template

* This package includes a sample **hosts file** for testing purposes.
* After installation, you can find a copy of the sample **hosts file** at the [datadir](https://www.gnu.org/software/automake/manual/html_node/Standard-Directory-Variables.html).
* Edit the file in a text editor.

### The hosts database

A **hosts database file** consumes less memory than the clear text **hosts file**. On the downside, the persistent database file is not human-readable.

#### Using a hosts database

Convert a clear text **hosts file** to SQLite3 **database**:

```
wanhive -m
```

1. Select *UTILITIES(2)* from the list of available options.
2. Next, select *Manage hosts(2)* and then
3. Select *Dump the "hosts" file into an SQLite3 database(1)*
4. Follow further instructions.
5. Add the database file's pathname to the configuration file.

```
[HOSTS]
.
.
hostsDb = <pathname-of-hosts-database>
```

## Bootstrap

A Wanhive hub must go through a bootstrap process to join the network. The joining hub needs to know a list of *stable* host identifiers:

1. A list of *stable overlay hub* identifiers.
2. A list of *stable authentication hub* identifiers (optional, for authentication).

A Wanhive hub obtains such a list from either

1. the [hosts file](#hosts), or
2. the **bootstrap files** (default).

**NOTES**:

* The [hosts file](#hosts) converts the host identifiers to network addresses.
* Do not use the identifier **0** for bootstrapping because it is reserved for [special purpose](#clustering-optional).

### Bootstrap files

The **bootstrap files** store a list of *stable* host identifiers:

1. A text file containing a whitespace-separated list of *stable overlay hub* identifiers.
     - This package includes a sample file named **nodes**.
     - The sample file gets copied into the **datadir** during installation.
2. A text file containing a whitespace-separated list of *stable authentication hub* identifiers.
     - This package includes a sample file named **auths**.
     - The sample file gets copied into the **datadir** during installation.

Create both the text files and update the configuration file as shown below:

```
[BOOTSTRAP]
nodes = <pathname-of-nodes-file>
auths = <pathname-of-auths-file>
```

#### Bootstrap files example

The contents of a "nodes" file may look like the following. This example lists four overlay hub identifiers (1, 2, 35 and 8).

```
1 2 35

8
```

The contents of an "auths" file may look like the following. This example lists a single authentication hub identifier (256).

```
256
```

## Network key (optional)

Network keys are useful for:

* Mutual authentication in an [overlay network](#clustering-optional).
* Distributed [authentication](#client-authentication-optional) of the clients.


Generating a network key pair:

```
wanhive -m
```

1. Select *UTILITIES(2)*.
2. Next, select *Generate keys(1)*.
3. Follow the further instructions.
4. Configure the servers (overlay and authentication hubs):
    1. Copy the key pair to all the host machines.
    2. Update the configuration file(s) as illustrated below.

```
[KEYS]
privateKey = <pathname-of-private-key-file>
publicKey = <pathname-of-public-key-file>
```

# Program invocation

## Overlay hub (Server)

Every overlay hub must have a *unique numerical identity*.

Starting an overlay hub:

```
wanhive -to -n <identifier>
wanhive -to -n <identifier> -c <configfile>


wanhive -to -n 1
```

### Clustering (optional)

> Overlay hubs can form a p2p cluster. A cluster doesn't support the publish-subscribe-based multicasting.

Update the configuration file(s) to enable clustering:

```
[HUB]
.
answerRatio = 0.15
forwardRatio = 0.70
.
.

[OVERLAY]
.
connectToOverlay = YES
```

Start the **controller** that is an overlay hub having the identity **0** (zero):

```
wanhive -to -n 0 -c <configfile>


wanhive -to -n 0
```

Configure and run a few (ideally three or more) "well-known" overlay hubs as the [bootstrap nodes](#bootstrap). Start multiple bootstrap nodes in succession to prevent network partitioning.

Start additional overlay hubs in any order. The overlay hubs will automatically organize themselves into a structured overlay network. The overlay hubs can join and leave the overlay network at any time.

**NOTES**:

* Formation, reorganization, and stabilization of a cluster require the **controller** and a few bootstrap nodes.
* A stable cluster will continue to function normally after temporary **controller** failure.
* Install the [network key](#network-key-optional) for mutual authentication.

### Client authentication (optional)

The following instructions are valid for a **Standard setup**.

Client authentication is disabled by default, i.e., an overlay hub accepts all the well-formed registration requests from it's clients.

To enable client authentication at an overlay hub:

1. Install the [network key](#network-key-optional).
2. Set the following property in the configuration file.

```
[OVERLAY]
.
.
authenticateClient = YES
```

### Domain controlled access (optional)

The following instructions are valid for a **Standard setup**.

*Domain controlled access* allows only those clients that belong to the same logical group (**domain**) to communicate with one another.

Set the value of *OVERLAY/netMask* property to **0xfffffffffffffc00** in an overlay hub's configuration file to activate the *domain controlled access*.

Set the following property in an overlay hub's configuration file to turn on the *domain controlled access*:

```
[OVERLAY]
.
.
netMask = 0xfffffffffffffc00
```

**NOTE**: A client's type further restricts communication with the other clients (not covered in this documentation).

## Authentication hub (Server)

The following instructions are valid for a **Standard setup**.

An authentication hub acts as a *trusted third party* during the authentication and authorization of the clients.

1. Install the [network key](#network-key-optional) to establish trust.
2. Update the configuration file to adjust the database connection parameters.

```
[AUTH]
.
.
connInfo= user=<user-name> dbname=<database-name>
```

Start the authentication hub:

```
wanhive -ta -n <identifier>
wanhive -ta -n <identifier> -c <configfile>


wanhive -ta -n 256
```

Wanhive network can have multiple authentication hubs for [redundancy](#bootstrap).

## Multicast consumer (client)

The following instructions are valid for a **Basic setup** (for testing purposes only).

1. Start an overlay hub with [clustering](#clustering-optional) and [client authentication](#client-authentication-optional) disabled.
2. Disable the following property (comment-out or set to FALSE) in the client's configuration file:

```
[HUB]
#listen = YES

```

Start the multicast consumer (client):

```
wanhive -tm -n <identifier>
wanhive -tm -n <identifier> -c <configfile>


wanhive -tm -n 65536
```

**NOTE**: Clients should not use identifiers in the range [0-65535].

Following "additional" instructions are applicable for a **Standard setup** (for testing purposes only).

1. Start an authentication hub.
2. Disable clustering at the overlay hub.
3. Register a new thing (and create a password for it) in the **Wanhive IoT web console**.
4. Memorize newly created thing's *identifier* and *password*.
5. Set the client's password in its configuration file.

```
[CLIENT]
password = <password>
passwordHashRounds = <rounds>
```

Start the multicast consumer:

```
wanhive -tm -n <identifier>
wanhive -tm -n <identifier> -c <configfile>
```

# Data security and privacy (optional)

Wanhive hubs use TLS/SSL for communication security.

## Enabling SSL (server)

1. Create a root CA if required [HOWTO](HOWTO-crypto.md).
2. Generate key and certificate for each server (overlay and authentication hubs).
3. Enable SSL by setting the *SSL/enable* option to TRUE in the configuration file.
4. Install the root certificate by updating the *SSL/trust* option in the configuration file.
5. Install server key and certificate by updating the relevant options in the **SSL** section of the configuration file.
6. Make the root certificate available for the client applications.

```
[SSL]
enable = TRUE
trust = <pathname-of-root-certificate-file>
certificate = <pathname-of-server-certificate-file>
key = <pathname-of-server-key-file>
```

## Enabling SSL (client)

1. Set the *SSL/enable* option to TRUE in the configuration file.
2. Update the *SSL/trust* option in the configuration file to install the root certificate.

```
[SSL]
enable = TRUE
trust = <pathname-of-root-certificate-file>
```

**NOTE**: CA certificates are loaded from the default locations if not provided by the configuration file.
