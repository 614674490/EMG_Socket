/*
 * @Author: Ken Kaneki
 * @Date: 2021-10-07 19:38:16
 * @LastEditTime: 2021-10-10 16:35:51
 * @Description: README
 */
#include <ads1115.h>
#include <head.h>
#include <socket.h>
#include <thread.h>

using namespace std;

queue<char> data_queue;
mutex mtx_syn;                //互斥信号量
condition_variable data_cond; //条件变量
bool con_flag = false;
//Thread1
void emgThread(emgCollector &emgcollector)
{
    while (!(emgcollector.Init_EmgHardWork()))
        usleep(MS_DELAY * 2); //必须要有延时;;
    while (1)
    {
        if (emgcollector.ReadOneEmgDate())
        {
            if (con_flag)
            {
                lock_guard<mutex> locker(mtx_syn); //使用lock_guard来锁定信号量
                for (int p = *emgcollector.readBuf; p < (*emgcollector.readBuf) + READBUFFLEN; p++)
                    data_queue.push(p);
                data_cond.notify_one(); // 通过条件变量通知其它等待的线程
            }

            usleep(MS_DELAY * 2); //必须要有延时
        }
        else
        {
            while (!(emgcollector.Init_EmgHardWork()))
                usleep(MS_DELAY * 2); //必须要有延时;
        }
    }
}

//Thread2
void socketThread(Socket &socketclient)
{
    do
    {
        con_flag = socketclient.connectServer();
        if (!con_flag)
            cout << "Reconnect Server(75s)" << endl;
    } while (!con_flag);

    while (1)
    {
        if (con_flag)
        {
            unique_lock<mutex> locker(mtx_syn);
            data_cond.wait(locker, []
                           { return !data_queue.empty(); });
            for (int i = 1; i <= READBUFFLEN; i++)
            {
                socketclient.sendBuf[i] = data_queue.front();
                data_queue.pop();
            }

            locker.unlock();
            try
            {
                if (socketclient.send_data(socketclient.sendBuf) != SENDBUFFLEN)
                    throw runtime_error("send failed\n");
            }
            catch (const runtime_error &e)
            {
                con_flag = false;
                cout << e.what() << endl;
                cout << "Reconnect Server(75s)" << endl;
                con_flag = socketclient.connectServer();
            }
        }
        else
        {
            cout << "Reconnect Server" << endl;
            con_flag = socketclient.connectServer();
        }
    }
}
