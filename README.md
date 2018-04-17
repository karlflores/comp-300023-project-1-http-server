# Multithreaded HTTP1.0 Server

USAGE:
```
./myserver [port_number] [path_to_web_root]
```

* Implemented pthreads to handle accept of client connection
  - Need to check if the threads handling connections concurrently or are they waiting until the connection is closed
* HTTP GET request passes script testing
  - might need to segment the send_response function -- pass in a struct that contains all the information to write out to the client
