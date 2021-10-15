#ifndef ACTIVEDETECT_H_INCLUDED
#define ACTIVEDETECT_H_INCLUDED
#define Py_NO_ENABLE_SHARED

#include <python2.7/Python.h>
#include <vector>
#include <stdexcept>
#include <time.h>
#include <cmath>

#include "FeatureExtraction.h"
#include "fsrCollector.h"
#include "emgCollector.h"
#include "imuCollector.h"
#include "global.h"

using namespace std;

class ActiveDetect{
private:
	int cnt; //size of window
	int ener_cnt;//the number of windows which more than T
	int gesture_counts;//gesture label
	FeatureExtraction extraction;

public:
	static const int T = 50000;
	double feature[featurecnt];//the fea of emg and fsr

	int preemg[emg_num][windowsize];
	double emgarray[emg_num][maxlen];

	int emgarray_len;
	static int len2;

	queue<int> emgque0;
	queue<int> emgque1;
	queue<int> emgque2;
	queue<int> emgque3;
	queue<int> emgque4;
	queue<int> emgque5;
	queue<int> emgque6;
	queue<int> emgque7;

	/***********************************************************************
     save the file
	************************************************************************/
	ofstream EmgOri;
	ofstream EmgFea;
	ostringstream EmgOrifileName;
	ostringstream EmgFeafileName;

public:
	ActiveDetect();
	void saveTofileOri();//save the emg fsr and imu ori data to .csv
	void saveToFileFea(string name,double *array,int len);//save the emg and fsr fea to .csv

	void quetoarr();
	void CollectorData(fsrCollector &fsrcollector, imuCollector &imucollector, int emg[emg_num][windowsize]);
	double cal_energy();//get the energy of windows
	void pre_pro(int cnt, int *emg);  //put the ori data to preemg prefsr preimu(pre is a windows)

	bool push_que();// put the preemg prefsr preimu to emgque fsrque
	bool pop_que(int cnt);
	void FeaToString(double EmgAva[emg_num], double EmgRms[emg_num], double EmgSsd[emg_num], double EmgAr1[emg_num],
                       double EmgAr2[emg_num], double EmgAr3[emg_num], double EmgAr4[emg_num], double EmgPa[emg_num],
                       double FsrMeans[fsr_num], double FsrRms[fsr_num], double FsrSd[fsr_num], double FsrAr1[fsr_num],
                       double FsrAr2[fsr_num], double FsrAr3[fsr_num], double FsrAr4[fsr_num], double FsrPa[fsr_num],
                       char (&FeaString)[1024]);

	void featureScale(double *originalFeature,double *scaledFeature,double *feature_min,double *feature_max,double y_upper,double y_lower);
	void resultGestureDisplay(int label); //get the result of gesture
	~ActiveDetect();
};

#endif // ACTIVEDETECT_H_INCLUDED
