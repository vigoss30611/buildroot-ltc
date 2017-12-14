
This readme is explaining the valgrind error suppressions:

dl_open:
it seams that dl_open is having an on-purpose memory allocation with no free when cancelling a thread
http://stackoverflow.com/questions/9763025/memory-leaked-with-pthread-cancel-itself
and
http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=588997
suggest that it is a bug in valgrind to consider it a memory leak

MeosLeak:
leak when terminating Meos. Is it similar than the pthread_cancel leak?
The leak is 48 Bytes.

DebugCond and DebugValue8:
This is due to the fact that not all the registers were access for write but their value is printed nonetheless.

TConfLeak:
TConf has a leak in its initialisation process but it is not vital to fix as only used when running against the simulator.
