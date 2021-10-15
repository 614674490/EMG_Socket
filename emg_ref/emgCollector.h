#ifndef EMGCOLLECTOR_H_INCLUDED
#define EMGCOLLECTOR_H_INCLUDED

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <memory.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>// uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
#include <sys/ioctl.h>
//#include "serial.h"
#include <stdio.h>
#include <inttypes.h>// uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions

#include <cinttypes>
#include "ActiveDetect.h"
#include "FeatureExtraction.h"
#include "Socket_click.h"
#include "global.h"

using namespace std;

class emgCollector{
public:

	std::queue<int> emgque0;
	std::queue<int> emgque1;
	std::queue<int> emgque2;
	std::queue<int> emgque3;
	std::queue<int> emgque4;
	std::queue<int> emgque5;
	std::queue<int> emgque6;
	std::queue<int> emgque7;

	std::ofstream emgfile; //以输出方式打开文件
	int emg_cnts;//目前做出的手势数目
	int emg_ener_cnt;// 当前大于阈值的窗口数
	int emgarray_len; //当前手势活动段长度
	static int emgT; //窗口阈值

	int emgpre[emg_num][windowsize];
	//int emg[emg_num][windowsize];

    uint8_t readBuf[32];

    int ADS_address = 0x11;
    int I2CFile;
    double radl = 6.144/32768.0;
    int16_t val;

    emgCollector():emg_cnts(0),emg_ener_cnt(0){};
    void Init_EmgHardWork();
    void ReadOneEmgDate(int count);
	void start_collect_emg();
	~emgCollector();
};

#endif // EMGCOLLECTOR_H_INCLUDED
