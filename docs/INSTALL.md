# Requirements

* Supported CPU architectures: X86, X86-64, ARM (32 and 64 bit)
* Operating systems: GNU/Linux (Linux kernel version 4.x or higher)
* C/C++ compilers: GCC version 6.1 or higher (7.x or higher recommended)
* Programming languages: C++ 14
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

Follow these steps to create a "bare minimum" functional setup.

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

**NOTE**: All of the above files get copied into the [datadir](https://www.gnu.org/software/automake/manual/html_node/Standard-Directory-Variables.html) during installation.

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
    * Does not support user/endpoint management.
    * Does not support authentication and access control.
    * Supports clustering.
    * Sufficient for testing.
    * Sufficient for building basic IoT applications over the local IP network.

## Standard setup

* Install this package.
* Install Postgresql database server 10 or above.
* Install **Wanhive IoT web console**.
* Enable **Network key**.
* Enable **Client authentication**.
* Enable **Domain controlled access**.

# Configuration

Wanhive hubs are configured using an external configuration file.

The program will attempt to read **wanhive.conf** from the locations listed below (in the given order) if the pathname of the configuration file is not provided via the command line:

1. Current working directory (`$PWD`)
2. Executable's directory
3. $HOME/.config/wanhive
4. [sysconfdir](https://www.gnu.org/software/automake/manual/html_node/Standard-Directory-Variables.html)

## Configuration file

The configuration file uses a restricted version of the [INI file](https://en.wikipedia.org/wiki/INI_file) format.

* Update the **HOSTS** and **BOOTSTRAP** sections of the configuration file correctly.
    * Do not skip this step.
* This package includes a sample configuration file (*wanhive.conf*).
* A sample configuration file (*wanhive.conf*) is copied into the **datadir** during installation.

## Hosts

Wanhive hubs use a special text file to resolve network addresses.

* Wanhive hub acting as the server uses this file to bind it's listening socket to the correct address.
* Wanhive hub acting as the client uses this file to find and connect to a server.

The required steps have been summarized below:

1. Create a text file containing addresses of all the *available* servers (overlay and authentication hubs).
2. Update the **HOSTS** section of the configuration file, as illustrated below.

```
[HOSTS]
.
.
hostsFile = <pathname-of-hosts-file>
```

### Hosts file format

The "hosts" file contains rows of tab-separated tuples:

```
[IDENTIFIER]	[HOST]	[SERVICE]
```

Each row maps an identifier to a network address. An example of the "hosts" file:

```
0	/home/user/wh0.uds	unix
1	127.0.0.1	9001
2	hub2.example.com	9002
3	testmachine.local 9003
256	localhost	7788

```

**NOTES**:

* Specify **unix** in the SERVICE column to use *unix domain socket* instead of TCP/IP socket.
* The HOST column is irrelevant for the creation of a listening TCP/IP socket.

### Option 1: Manually create a hosts file

You can manually create a small **hosts** file for *testing*. For example the following "hosts" file can be used to describe five hubs:

```
0	127.0.0.1	9000
1	127.0.0.1	9001
2	127.0.0.1	9002
3	127.0.0.1	9003
256	localhost	5555
```

1. Four overlay hubs having identifiers 0, 1, 2, and 3 running on 127.0.0.1 at ports 9000, 9001, 9002, and 9003 respectively.
2. An authentication hub having identifier 256, running at localhost:5555

The type of hub is discerned using the **Bootstrap** files described further below.

Please note that values in rows are **TAB-separated**.

### Option 2: Create a new file

```
wanhive -m
```

1. Select *UTILITIES(2)* from the list of available options.
2. Next, select *Manage hosts(2)* and then,
3. Select *Generate a sample "hosts" file(3)*.
4. Follow the instructions.
5. Edit the file using your favorite text editor.

### Option 3: Use the default template

* This package includes a sample **hosts** file.
* A sample **hosts** file is installed into the **datadir**.
* Edit the file using your favorite text editor.
* The sample file is sufficient for testing purposes.

## Bootstrap

1. Create text files:
    1. Containing a whitespace/newline-separated list of *stable* overlay hub identifiers.
        * Used by Wanhive clients (for bootstrapping).
        * Used by overlay hubs for joining a cluster (for bootstrapping).
        * A sample file named **nodes** has been included with this package.
        * A sample file named **nodes** is installed into the **datadir**.
        * The included file is sufficient for testing purposes.
    2. **OPTIONAL**: Containing a whitespace/newline-separated list of *stable* authentication hub identifiers.
        * Used by Wanhive clients to discover an authentication hub.
        * Not needed if client authentication is not desired (**Basic setup**).
        * A sample file named **auths** has been included with this package.
        * A sample file named **auths** is installed into the **datadir**.
        * The included file is sufficient for testing purposes.
2. Add the files to the section named **BOOTSTRAP** of the configuration file.

```
[BOOTSTRAP]
nodes = <pathname-of-nodes-file>
auths = <pathname-of-auths-file>
```

**NOTES**:

* All the identifiers are resolved to their physical addresses using the **Hosts** file/database.
* Identifier **0** is reserved for a special purpose (see **clustering**) and should not be used here.
* If **clustering** is enabled, identifiers of only a few (ideally, three or more) stable overlay hubs are sufficient for bootstrapping irrespective of the number of hubs in the cluster.

### Example (for testing)

Example of a "nodes" file (two stable overlay hubs).
Overlay hubs listed below will be contacted randomly during bootstrapping and the first live one will be used.

```
1
3
```

Example of an "auths" file (one single authentication hub).
Authentication hubs listed in this file will be contacted randomly and the first live one will be used.

```
256
```

## Network key (optional)

Network keys allow:

* Mutual authentication in a cluster of overlay hubs.
* Establishing trust with an *authentication hub* during distributed authentication of clients.


Follow these instructions to generate a new pair of keys:

```
wanhive -m
```

1. Select the option *UTILITIES(2)*.
2. Next, select *Generate keys(2)*.
3. Follow the instructions.
4. Install the keys to overlay and authentication hubs:
    1. Copy keys to all the host machines.
    2. Update the **KEYS** section of the configuration file(s).

```
[KEYS]
privateKey = <pathname-of-private-key-file>
publicKey = <pathname-of-public-key-file>
```

# Program invocation

## Overlay hub (Server)

Every Wanhive hub (server and client), including an overlay hub, has a *unique numerical identifier*.

Starting an overlay hub:

```
wanhive -to -n <identifier>
wanhive -to -n <identifier> -c <configfile>


wanhive -to -n 1
```

### Clustering (optional)

Overlay hubs can form a cluster in a p2p fashion. When clustering is enabled, publish-subscribe based multicasting gets automatically disabled.

1. Enable message forwarding in the configuration file(s).
2. Set the *OVERLAY/connectToOverlay* option to TRUE in the configuration file(s).
3. Create a **controller** which is a special overlay hub having **0** as its identifier:

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

```
wanhive -to -n 0 -c <configfile>


wanhive -to -n 0
```

Run few "well-known" overlay hubs as bootstrap nodes. If multiple bootstrap nodes are desired, then start them in succession to avoid network partitioning.

Restart all the other overlay hubs. Overlay hubs will automatically organize themselves into a structured overlay network. Overlay hubs can join and leave the network at any time.

**NOTES**:

* Availability of the *controller* and a few bootstrapping nodes is required for the formation, reorganization, and stabilization of a cluster. However, a stable cluster will survive and continue to function normally despite *controller* failure.
* Install **Network key** for mutual authentication.

### Client authentication (optional)

The following instruction is valid for a **Standard setup**.

Client authentication is disabled by default, i.e., the overlay hub accepts all well-formed registration requests from the clients.

To enable client authentication at an overlay hub:

1. Install **Network key**.
2. Set the *OVERLAY/authenticateClient* option to TRUE in the configuration file.

```
[OVERLAY]
.
.
authenticateClient = YES
```

### Domain controlled access (optional)

The following instructions are valid for a **Standard setup**.

*Domain controlled access* allows only those clients that belong to the same logical group (**domain**) to communicate with one another.

Set the *OVERLAY/netMask* option to **0xfffffffffffffc00** in the configuration file to activate *domain controlled access* at an overlay hub.

```
[OVERLAY]
.
.
netMask = 0xfffffffffffffc00
```

**NOTE**: client-to-client communication is further restricted based on the "client type" (not covered in this documentation).

## Authentication hub (Server)

The following instructions are valid for a **Standard setup**.

The authentication hub acts as a *trusted third party* during authentication and authorization of the clients.

1. Update *AUTH* section of the configuration file to adjust the database connection parameters.
2. Install **Network key** to establish trust.

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

A large Wanhive network can have multiple authentication hubs to provide redundancy (see **bootstrapping**).

## Multicast consumer (client)

The following instructions are valid for a **Basic setup** (for testing purposes only).

1. Run an overlay hub with clustering and client authentication disabled.
    * Publish-subscribe is automatically disabled on clustering.
2. Update the configuration file for the client as follows:
    * Disable *HUB/listen* (comment out).

```
[HUB]
#listen = YES

```

Start the multicast consumer:

```
wanhive -tm -n <identifier>
wanhive -tm -n <identifier> -c <configfile>


wanhive -tm -n 65536
```

**NOTE**: Clients should not use the identifiers in the range [0-65535].

Following "additional" instructions are applicable for a **Standard setup** (for testing purposes only).

1. Start an authentication hub.
2. Disable clustering at the overlay hub.
3. Add a new client (a new thing of type "Processor") and create a password for it inside **Wanhive IoT web console**.
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

Wanhive hubs (server/client) support SSL/TLS.

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

1. Enable SSL by setting the *SSL/enable* option to TRUE in the configuration file.
2. Install the root certificate by updating the *SSL/trust* option in the configuration file.

```
[SSL]
enable = TRUE
trust = <pathname-of-root-certificate-file>
```

**NOTE**: CA certificates are loaded from the default locations if not provided by the configuration file.
