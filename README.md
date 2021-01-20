# Proxy Server &amp; client (for integration tests)

The files downloaded by users will be stored in the server memory.

## Getting started

### Server commands

Server users can issue commands:

|   **command**   |   **description**   |
| --- | --- |
| **connect to <ip:port>** |     |
| **disconnect** |     |
| **get list of cached files** |     |
| **get file <path>** |     |
| **reconnect to <ip:port>** |     |


### Server configuration (and how to run .exe file)

|   Command   |   Description   |
| --- | --- |
| \-d \-\-dir=\<path\> | Path to the directory where files will be stored \(future\) |
| \-i \-\-ip=\<IP\> | IP address of server listener |
| \-p \-\-port=\<uint\> | Port of server listener |
| \-m \-\-maxdata=\<uint\> | The amount of data that the server can store per client |
| \-w \-\-workers=\<uint\> | Maximum number of available cache tables |
| \-v \-\-verbose | Flag that indicates that debug messages is printed to stdout \(stderr\), if not set server prints only errors |
| \-h \-\-help | Print help string |
