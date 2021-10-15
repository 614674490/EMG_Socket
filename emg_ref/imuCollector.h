#ifndef IMUCOLLECTOR_H_INCLUDED
#define IMUCOLLECTOR_H_INCLUDED

# include <stdio.h>
# include <malloc.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <termios.h>
# include <memory.h>
# include <unistd.h>
# include <time.h>
# include <inttypes.h>// uint8_t, etc
# include <linux/spi/spidev.h>
# include <linux/types.h>
# include <getopt.h>
# include <sys/ioctl.h>
# include <stdint.h>

#include "global.h"

using namespace std;

class imuCollector{
public:
    int imu_fd;
    int imu_len;

    ofstream ImuOri;
	ostringstream ImuOrifileName;

    vector<vector<int>> imuarray;

    vector<int> imu0array;
    vector<int> imu1array;
    vector<int> imu2array;
    vector<int> imu3array;
    vector<int> imu4array;
    vector<int> imu5array;
    vector<int> imu6array;
    vector<int> imu7array;
    vector<int> imu8array;
    vector<int> imu9array;
    vector<int> imu10array;
    vector<int> imu11array;
    vector<int> imu12array;
    vector<int> imu13array;
    vector<int> imu14array;
    vector<int> imu15array;

    imuCollector();

    int spi_init(char filename[40], uint8_t mode, uint8_t lsb, uint8_t bits, uint32_t speed);
    int init_imu_hardware();

	int write_one_byte(int imu_fd, uint8_t address, uint8_t *writebuf);
	int read_one_byte(int imu_fd, uint8_t address, uint8_t *readbuf);
	int read_all_byte(int imu_fd, uint8_t *readbuf);

	int read_imu_data();
	void saveToImuOri(vector <vector <int>> imuarray, int imu_len);
	~imuCollector();
};

#endif // IMUCOLLECTOR_H_INCLUDED
