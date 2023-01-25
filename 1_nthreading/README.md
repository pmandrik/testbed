# Concurrency 

Concurrency - multiple independent activities in parallel, rather than sequentially, or one after the other.  

CONCURRENCY WITH MULTIPLE PROCESSES - pass messages to each other through all the normal interprocess communication channels (signals, sockets, files,
pipes):  
 -  complicated/slow, because operating systems typically provide a lot of protection between processes to avoid one process accidentally modifying data belonging to another process.   
 - it takes time to start a process, the operatingsystem must devote internal resources to managing the process  
 + higher-level communication mechanisms  
 + separate processes on distinct machines connected over a network  

CONCURRENCY WITH MULTIPLE THREADS - all threads in a process share the same address space, and most of the data can be accessed directly from all threads—global variables remain global, and pointers or references to objects or data can be passed around among threads.  
The C++ Standard doesn’t provide any intrinsic support for communication between processes, applications that use multiple processes will have to rely on platform-specific APIs to do so.  

parallelism - taking advantage of the available hardware to increase the performance  
concurrency - separation of concerns, or responsiveness  
  



 





