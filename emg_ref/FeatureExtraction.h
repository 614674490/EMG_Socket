#ifndef FEATUREEXTRACTION_H_INCLUDED
#define FEATUREEXTRACTION_H_INCLUDED

#include <cstdlib>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <time.h>
#include <memory.h>

#include <fftw3.h>
#include <vector>

#include "global.h"
using namespace std;

class FeatureExtraction{
private:
	//ofstream rmsout,sdout,measnout,paout,avaout,mfout,mpfout,ar1out,ar2out,ar3out;
	ofstream out;
	//ostringstream out_file;
	std::string out_file;
	void Rxx(const double *a, int N,double *xocrr);//×ÔÏà¹Øº¯Êý
	void FFT(const double *x, double *y,int len);//žµÀïÒ¶±ä»»
	double sum1(const double *x,int len);//P(f)
	double sum2(const double *x,int len);//fP(f)
	void sumcal(double *a,int N,double& sum1,double& sum2);
	void save_result(string val_name, double *val);

public:
	FeatureExtraction(){};
	FeatureExtraction(std::string name):out_file(name){
		out.open(out_file.c_str(), std::ios::app);
	};
	void open_file(string name){//Žò¿ªÎÄŒþ
		out.open(name.c_str(), ios::app); //app:ÔÚÃ¿ŽÎÐŽÖ®Ç°ÕÒµœÎÄŒþÎ²£¬ŒŽ£º×ÜÊÇÔÚÔ­ÎÄŒþÎ²ÐŽÈë²Ù×÷£šÎÄŒþ²»ŽæÔÚÔòŽŽœš£©
	}
	void writetofile();//ÐŽÈëÎÄŒþ
	void save_origin(string val_name, double *val,int len);//±£ŽæÔ­ÊŒÊýŸÝ
	~FeatureExtraction(){
		out.close();
	}

public:
	//void Means_MAV(double emgarray[8][maxlen],int len);//ÆœŸùÖµºÍŸø¶Ô»ý·ÖÆœŸùÖµ
	void Means_AVA(double (*array)[maxlen],int len,int rows,double *means,double *ava);//ÆœŸùÖµºÍŸø¶Ô»ý·ÖÆœŸùÖµ
	void RMS_SD_SSD(double (*array)[maxlen],int len,int rows,double *means,double *ava,double *rms,double *sd,double *ssd); //Ÿù·œžùÖµºÍ±ê×Œ²î
	void PA(double (*array)[maxlen],int len,int rows,double *pa);
	//ÆµÓòŒÆËã
	void MPF_and_MF1(double (*array)[maxlen],int len,int rows,double *mpf1,double *mf1);
	void MPF_and_MF2(double (*array)[maxlen],int len,int rows,double *mpf2,double *mf2);//ŒÆËãÆœŸù¹ŠÂÊÆµÂÊ
	//Ê±ÆµÓòŒÆËã
	void Burg_AR(double *x,int N,int p,double *a,double *sigma2);//ARÄ£ÐÍ
	void AR(double (*array)[maxlen],int len,int rows,double *ar1,double *ar2,double *ar3,double *ar4);//ŒÆËãARÄ£ÐÍÏµÊý

	void Means_AVA_vector(vector< vector <int> > array,int len,int rows,double *means,double *ava);
    void RMS_SD_SSD_vector(vector <vector <int> > array,int len,int rows,double *means,double *ava,double *rms,double *sd,double *ssd);
    void PA_vector(vector <vector <int> > array,int len,int rows,double *pa);
    void MPF_and_MF2_vector(vector <vector <int> > array,int len,int rows,double *mpf2,double *mf2);
    void MPF_and_MF1_vector(vector <vector <int> > array,int len,int rows,double *mpf1,double *mf1);
    void AR_vector(vector <vector <int> > array,int len,int rows,double *ar1,double *ar2,double *ar3,double *ar4);
};

#endif // FEATUREEXTRACTION_H_INCLUDED
