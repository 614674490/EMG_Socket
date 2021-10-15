#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <queue>
#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define emg_num 8
#define imu_num 16
#define fsr_num 5

#define minlen 160
#define windowsize 32
#define maxlen 1200
#define featurecnt  34
#define fea_num 8

#define FALSE -1
#define TRUE 1

using namespace std;

extern int emg[emg_num][windowsize];
extern int16_t imu[imu_num];

extern double feature_min[featurecnt];
extern double feature_max[featurecnt];


extern bool gesture_avaliable;
extern bool gesture_start;
extern bool gesture_end;

extern int stop_signal;

extern char FsrFeaString[1024];

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t gesture_start_signal;
    pthread_cond_t gesture_end_signal;
    pthread_cond_t gesture_available_signal;
}Gesture_Signal;

//extern pthread_mutex_t lock;
extern Gesture_Signal gesture_signal;

extern queue<string> FsrFea_To_Emg;

extern pthread_t fsrid;
extern int fsrrc;
extern void *thrd_fsr_ret;
//the fsr thread parameter
#endif
