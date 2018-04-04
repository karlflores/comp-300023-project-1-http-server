# Multithreaded HTTP1.0 Server

USAGE:
```
./myserver [port_number] [path_to_web_root]
```

* Implemented pthreads to handle accept of client connection
  - Need to check if the threads handling connections concurrently or are they waiting until the connection is closed
* HTTP GET request passes script testing
  - might need to segment the send_response function -- pass in a struct that contains all the information to write out to the client

* Multithreaded test results 
  - Running on one thread to transfer 2x8MB jpgs: Time taken - 6.986s (2.4MB/s) : 4.567s (4.7MB/s)
  - Running on two threads to transfer 2x8MB jpgs: Time taken - 5.90s (608kB/s) : 1.740s (5.2MB/s) : 1.677s (5.7MB/s) : 1.540s (5.46MB/s) : 2.244s (3.84MB/s)
