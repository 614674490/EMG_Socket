#ifndef FSRCOLLECTOR_H_INCLUDED
#define FSRCOLLECTOR_H_INCLUDED

# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <termios.h>
# include <memory.h>
# include <unistd.h>
# include <time.h>
# include <vector>
# include <string>

# include <errno.h>
# include <stdio.h>
# include "FeatureExtraction.h"

#include "global.h"

using namespace std;

class fsrCollector{
public:
    struct termio{
        unsigned short  c_iflag;       /* 输入模式标志 */
        unsigned short  c_oflag;        /* 输出模式标志 */
        unsigned short  c_cflag;        /* 控制模式标志*/
        unsigned short  c_lflag;        /* local mode flags */
        unsigned char   c_line;          /* line discipline */
        unsigned char   c_cc[NCCS];       /* control characters */
    };


    int fsr_fd;
    char fsr_dev[13] = "/dev/ttyAMA0";

	std::vector<int> fsr0array;
	std::vector<int> fsr1array;
	std::vector<int> fsr2array;
	std::vector<int> fsr3array;
	std::vector<int> fsr4array;
    std::vector<vector <int> > fsrarray;

    double FsrAva[fsr_num], FsrMeans[fsr_num], FsrRms[fsr_num], FsrSd[fsr_num], FsrSsd[fsr_num];
    double FsrAr1[fsr_num], FsrAr2[fsr_num], FsrAr3[fsr_num], FsrAr4[fsr_num], FsrPa[fsr_num];

    FeatureExtraction FsrExtraction;

    ofstream fsrfile; //以输出方式打开文件
    ofstream FsrOri;
	ofstream FsrFea;
	ostringstream FsrOrifileName;
	ostringstream FsrFeafileName;

    fsrCollector();
	int OpenDev(char *Dev);
	int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
	int init_fsr_hardware();

	int read_fsr_data();

    void GetFsrFea();

	void saveToFsrOri(vector <vector <int>> fsrarray, int fsr_len);
	void saveToFsrFea(string name,double *array,int len);
	~fsrCollector();
};

#endif
