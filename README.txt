Dynamic Scheduler: Implements a dynamic schedule to calculate numerical integration by
Iteration, Chunk or Thread. The granularity determines the size each thread will calculate.
A mutex is used to both protect the total and scheduler from accessing the data at the same
time.

Static Scheduler: Implements a static schedule to calculate numerical integration by iteration
or thread. The static schedule evenly distributes the work across the threads. A mutex is used
to protect the total from multiple threads accessing the data at once.