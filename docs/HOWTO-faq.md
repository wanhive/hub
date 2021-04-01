### Contact us.

- Create an [issue](https://github.com/wanhive/hub/issues) on the github.
- Send us an email [info@wanhive.com].

### Where is the protocol specification.

Use the following link to access the most recent document.

[Wanhive communication protocol](wanhive-protocol-v1.0.0.pdf)

### How to run the protocol tests.

```
wanhive -m
```

Select **PROTOCOL TEST(3)** from the list of available options.

Table of commands:

```
| SLN     | COMMAND   | NAME                            | REMARKS
| :------ | --------: | :------------------------------ | :--------------------------
| 1       | **0**     | NULL_CMD                        | 
|         | 0         | NULL                            | Always fails
|         | 1         | IDENTIFY                        | Authentication hub only
|         | 2         | AUTHENTICATE                    | Authentication hub only
|         | 3         | AUTHORIZE                       | Authentication hub only
|         | 127       | DESCRIBE                        | Privileged access
|         |           |                                 | 
| 2       | **1**     | BASIC_CMD                       | 
|         | 0         | REGISTER                        | 
|         | 1         | GETKEY                          | New connection only
|         | 2         | FINDROOT                        | 
|         | 3         | BOOTSTRAP                       | 
|         |           |                                 | 
| 3       | **2**     | MULTICAST_CMD                   | "Registered" client only
|         | 0         | PUBLISH                         | 
|         | 1         | SUBSCRIBE                       | 
|         | 2         | UNSUBSCRIBE                     | 
|         |           |                                 | 
| 4       | **3**     | NODE_CMD                        | Cluster only
|         | 0         | GET_PREDECESSOR                 | 
|         | 1         | SET_PREDECESSOR                 | 
|         | 2         | GET_SUCCESSOR                   | 
|         | 3         | SET_SUCCESSOR                   | 
|         | 4         | GET_FINGER                      | 
|         | 5         | SET_FINGER                      | 
|         | 6         | GET_NEIGHBOURS                  | 
|         | 7         | NOTIFY                          | 
|         |           |                                 | 
| 5       | **4**     | OVERLAY_CMD                     | Cluster only
|         | 0         | FIND_SUCCESSOR                  | 
|         | 1         | PING                            | 
|         | 2         | MAP                             | 
|         |           |                                 | 
| 6       | 5         | SELECT_HOST                     | 
| 7       | 6         | LOAD_NETWORK_KEY                | 
| 8       | 7         | FORGET_NETWORK_KEY              | 
| 9       | 8         | CONNECT                         | 
| 10      | 9         | DISCONNECT                      | 

```

*Cluster only* commands must be initiated via the controller.

The commands that require *Privileged access* are meant for the internal use and cannot be executed by a client. The following steps allow bypassing this restriction:

1. If clustering is disabled:
    1. Register at the overlay hub with an identifier in the range [1-1023] to simulate a privileged hub.
    2. If the registration request is denied, then load the network key (#7), reconnect(#9) and register(#2).
2. If clustering is enabled:
    1. Disable "domain controlled access" at the controller.
    2. Register(#2) at the controller as a client.
    3. If the registration request is denied, then load the network key (#7), reconnect(#9) and register(#2).
    4. Select a host (#6) other than the controller.
    5. Retry.

### What are the hub identifiers.

Each Wanhive hub is assigned a unique identifier.

* **Hub identifiers** can take up numerical values in the range [0-9223372036854775807].
* A hub acting as the client should not use an identifier in the range [0-65535].
* The default identifier range for the overlay hubs is [0-1023].
* The cluster **Controller** uses the identifier **0**. See [Clustering](INSTALL.md).

### How to modify the identifier range for overlay hubs.

The identifier range [0-1023] for overlay hubs may look limiting. You can modify this range while building from the source.

The default identifier range is hardcoded in the following manner (not exactly):

```
#define MAXKEYLENGTH 16U   //The architectural limit
#define WH_DHT_KEYLEN 10U  //The Current value

#define KEYLENGTH MINOF(MAXKEYLENGTH, WH_DHT_KEYLEN)

unsigned long CONTROLLER = 0
unsigned long MIN_ID = 1
unsigned long MAX_ID = (1UL << KEYLENGTH) - 1  //(2^KEYLENGTH) - 1
```

For example, setting the value of the macro **WH_DHT_KEYLEN** to 12 will increase the identifier range to [0-4095].

```
./configure CXXFLAGS="-DWH_DHT_KEYLEN=12 -O2 -g"
```

### How to calculate the capacity reservation ratios.

The *capacity reservation ratios* determine the allocation of a hub's computational resources for *answering* and *forwarding* the messages. Use the following formulas to calculate their values:

```
2*answerRatio + forwardRatio <= 1

answerRatio = (1 / (2 + (0.5 * KEYLENGTH)))
```
