#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

using namespace std;

extern queue<char> data_queue; // 用于线程间通信的队列

void emgThread(emgCollector &emgcollector);
void socketThread(Socket &socketclient);

#endif
