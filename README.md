# Multithreaded HTTP1.0 Server

USAGE:
```
./myserver [port_number] [path_to_web_root]
```

* Implemented pthreads to handle accept of client connection
  - Need to check if the threads handling connections concurrently or are they waiting until the connection is closed
* HTTP GET request passes script testing
  - might need to segment the send_response function -- pass in a struct that contains all the information to write out to the client
* Chose to use the sendfile system call instead of making my own send function as it was available to use, but also because it is more efficient as it sends the file directly from system space rather than using a read call to read into a buffer in user space, then sending that buffer back out into system space with the write call.
* Did not need to use any mutex locks with concurrency as at no point in time during the program would two threads be competing against each other by modifying global variables. The only place that would warrant the use of a mutex lock would be when accessing a file. But, since we are only reading from a file, using the open system call with the O_RDONLY flag (read-only) it will return a different file descriptor when called multiple times on the same file. Therefore did not need to use any locks.
