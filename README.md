# Proxy Server &amp; client (for integration tests)

The files downloaded by users will be stored in the server memory.

## Getting started

### Server commands

Server users can issue commands:

|   **command**   |   **description**   |
| --- | --- |
| **connect to <ip:port>** | Proxy creates 2 connections, 1 - with the client, 2 - with the required server at the sent address like "0.0.0.0:80" |
| **disconnect** | Proxy terminates the connection with the client server and waits for other commands from it |
| **get list of cached files** | Proxy sends the client a list of paths to the files it has |
| **get file** **\<****path****\>** | Proxy sends the client the desired file saved in itself |
| **reconnect to** **\<****ip:port****\>** | Proxy breaks the connection with the current server and creates a new one before the desired one. |

After establishing connections, the proxy forwards the client's requests to the server and writes to itself in memory (or in files) the client's requests and server responses (not exceeding the maximum specified amount of **maxdata**) and, if the client's request is repeated, it sends the response from itself, not from the server. If the answers do not fit, replaces the oldest one with the new one. Reads and writes are buffered. Handle connection closings and errors correctly. In case of an error, a specific error with a description is sent back to the user.

When you reset the only session that worked with a cache entry, it is allowed to both reset the page resume and destroy the cache entry, as well as the background continuation of the resume.


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
