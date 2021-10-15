#include <head.h>
#include <socket.h>
#include <ads1115.h>
#include <thread.h>
#include <getopt.h>

using namespace std;

int main(int argc, char **argv)
{
    int ch;
    char *ip = IP;
    char port = PORT;
    int opt, lopt, loidx;
    const char *optstring = "a:b:";
    const struct option long_options[] =
        {
            {"ip", required_argument, &lopt, 1},
            {"port", required_argument, &lopt, 2},
        };

    while ((opt = getopt_long(argc, argv, optstring, long_options, &loidx)) != -1)
    {
        if (opt == 0)
            opt = lopt;
        switch (opt)
        {
        case 1:
            ip = optarg;
            break;
        case 2:
            port = atoi(optarg);
            break;
        default:
            printf("error opt %c");
            return -1;
        }
    }
    printf("ip: %s\n", ip);
    printf("port: %d\n", port);
    emgCollector emgcollector;
    Socket socketclient(ip, port);

    thread emg(emgThread, ref(emgcollector));
    thread socket(socketThread, ref(socketclient));

    emg.join();
    socket.join();

    return 1;
}
