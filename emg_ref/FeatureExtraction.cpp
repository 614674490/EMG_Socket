#include "FeatureExtraction.h"
#define channel 4
using namespace std;

void FeatureExtraction::Means_AVA(double (*array)[maxlen],int len,int rows,double *means,double *ava){
	double temp;
	double avatemp = 0.0;
	for(int i = 0;i<rows;i++){
		temp = 0.0;
		avatemp = 0.0;
		for(int j = 0;j < len;j++){
			temp += array[i][j];
			avatemp += abs(array[i][j]);
		}
		means[i] = (temp/len);
		ava[i] = avatemp/len;
	}
}


void  FeatureExtraction::RMS_SD_SSD(double (*array)[maxlen],int len,int rows,double *means,double *ava,double *rms,double *sd,double *ssd){

	double rmstemp = 0.0;
	double sdtemp = 0.0;
	double ssdtemp;
	double val = 0.0;
	double a =2.0;
	Means_AVA(array,len,rows,means,ava);

	for(int i = 0;i < rows;i++){
		rmstemp = 0.0;
		sdtemp = 0;
		ssdtemp =0.0;
		for(int j = 0;j < len;j++){
			val = array[i][j];
			rmstemp += val*val;
			sdtemp += (val-means[i])*(val-means[i]);
			ssdtemp+=(abs(val)-ava[i])*(abs(val)-ava[i]);
		}
		rms[i] = sqrt(rmstemp/len);
		sd[i] =  sdtemp/len;
		ssd[i] = sqrt(ssdtemp/len);
	}
}


void  FeatureExtraction::PA(double (*array)[maxlen],int len,int rows,double *pa){

	double maxval = 0.0;
	for(int i = 0;i < rows;i++){
		maxval = 0.0;
		for(int j = 0;j < len-1;j++){
			maxval = max(array[i][j+1],array[i][j]);
		}
		pa[i] = maxval;
	}
}

void FeatureExtraction::Rxx(const double *a, int N, double *xocrr){
	for(int i = 0;i<N;i++){
		for(int j = 0;j<N;j++){
			if(i+j<N)
				xocrr[i] += (*(a+j)) * (*(a+i+j));

		}
		xocrr[i] = xocrr[i]/(N-i);
	}
	for(int i = 0;i<N/2;i++){
		double temp;
		temp = xocrr[i];
		xocrr[i] = xocrr[N-1-i];
		xocrr[N-1-i]=temp;
		//	cout<<xocrr[i]<<endl;
	}
	for(int i = N;i<2*N-1;i++){
		xocrr[i]= xocrr[2*N-2-i];
	}
	//for(int i =0;i<2*N-1;i++)
	//	cout<<xocrr[i]<<endl;
}

void FeatureExtraction::FFT(const double *x, double *y,int len){
	double *in;
	fftw_complex *out;
	fftw_plan p;
	in = (double *)fftw_malloc(sizeof(double) * len);
	out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * len);
	for(int i = 0; i < len; i++){
		in[i] = x[i];
	}

	p = fftw_plan_dft_r2c_1d(len, in, out, FFTW_ESTIMATE); // Ò»Î¬ÊµÊýŸÝµÄDFT
	fftw_execute(p);

	for(int i = 0; i < len; i++)
		y[i] = abs(out[i][0]);//¶ÔfftµÄœá¹ûÈ¥Ÿø¶ÔÖµ£¬µÃµœ¹ŠÂÊÆ×ÃÜ¶È

	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
}

double FeatureExtraction::sum1(const double *x,int len){

	double sum = 0.0;
	for(int i = 0; i < len; i++){
		sum += x[i];
	}
	return sum;
}

double FeatureExtraction::sum2(const double *x,int len){

	double sum = 0.0;
	for(int i = 1; i <= len; i++){
		sum += i * x[i];
	}
	return sum;
}

void FeatureExtraction::MPF_and_MF2(double (*array)[maxlen],int len,int rows,double *mpf2,double *mf2){
	double *y = new double[len];
	double *x = new double[len];
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < len; j++)
			x[j] = double(array[i][j]);
		FFT(x, y,len);
		mpf2[i] = sum2(y,len) / sum1(y,len);
		mf2[i] =  10*log10l(sum1(y,len) / 2);
	}
	delete x;
	delete y;
}

void FeatureExtraction::sumcal(double *a,int N,double& sum11,double& sum22){
	for(int i = 1;i<=N;i++)
		sum11 += a[i]*i;
	for(int  i =0;i<N;i++){
		sum22 += a[i];
	}
	//sum2 = sum2/2;
}

void FeatureExtraction::MPF_and_MF1(double (*array)[maxlen],int len,int rows,double *mpf1,double *mf1){

	double *y = new double[len];
	double *x = new double[len];
	double sum11 = 0.0;
	double sum22 = 0.0;
	for(int i = 0; i < rows; i++){
		sum11 = 0.0;
		sum22 = 0.0;
		for(int j = 0; j < len; j++)
			x[j] = double(array[i][j]);
		//double xocrr[2*len-1]={0.0};
		int size = 2*len-1;
		double *xocrr = new double[size];
		for(int J = 0; J < size; J++)
			xocrr[J] = 0.0;
		Rxx(x,len,xocrr);
		FFT(xocrr, y,len);
		sumcal(y,len,sum11,sum22);
		mpf1[i] = sum11/ sum22;
		mf1[i] = 10*log10l(sum22 / 2);
	}
	delete x;
	delete y;
}

void FeatureExtraction::Burg_AR(double *x,int N,int p,double *a,double *sigma2){

	int i,k;
	double R0,sumd,sumn,*b,*ef,*eb,*garma;
	b=(double*)malloc((p+1)*sizeof(double));
	ef=(double*)malloc(N*sizeof(double));
	eb=(double*)malloc(N*sizeof(double));
	garma=(double*)malloc((p+1)*sizeof(double));

	//³õÊŒ»¯,0œ×ARÄ£ÐÍ
	a[0]=1.0;
	b[0]=1.0;
	for (i=1;i<=p;i++)
	{
		a[i]=b[i]=0;
	}

	R0=0.0;
	for   (i=0;i<N;i++)
	{
		R0+=double(x[i])*double(x[i])/N;
	}
	sigma2[0]=R0;

	for (i=0;i<N;i++)
	{
		ef[i]=double(x[i]);
		eb[i]=double(x[i]);
	}

	//Öðœ×ŒÆËã

	for (k=1;k<=p;k++)
	{
		sumn=0.0;
		sumd=0.0;
		for (i=k;i<N;i++)
		{
			sumn+=ef[i]*eb[i-1];
			sumd+=ef[i]*ef[i]+eb[i-1]*eb[i-1];
		}
		if(sumd != 0){
			garma[k]=-2*sumn/sumd;
		}

		for (i=1;i<=k;i++)
		{
			b[i]=a[i]+garma[k]*a[k-i];//ŒÆËãÐÂµÄkœ×ARÏµÊý
		}
		for (i=1;i<=k;i++)
		{
			a[i]=b[i];//ÐÂµÄkœ×ARÏµÊýž³Öµ
		}

		sigma2[k]=(1.0-garma[k]*garma[k])*sigma2[k-1];//°×ÔëÉù¹ŠÂÊ

		//ŒÆËã±Ÿœ×ARÄ£ÐÍµÄÇ°ÏòºÍºóÏòÔ€²âÎó²î
		//ŽÓºóÏòÇ°ŒÆËã¿ÉÒÔœÚÔŒŽæŽ¢¿ÕŒä
		for (i=(N-1);i>=k;i--)
		{
			eb[i]=eb[i-1]+garma[k]*ef[i];
			ef[i]=ef[i]+garma[k]*eb[i-1];
		}
	}

	free(b);
	free(ef);
	free(eb);
	free(garma);

}

void FeatureExtraction::AR(double (*array)[maxlen],int len,int rows,double *ar1,double *ar2,double *ar3,double *ar4){
	double *a,*sigma2;
	//int sig[10]={1,2,3,78,45,23,-45,-12,2};
	int p=4;
	a=(double*)malloc((p+1)*sizeof(double));
	sigma2=(double*)malloc((p+1)*sizeof(double));
	for(int i =0 ;i < rows;i++){
		//Burg_AR(sig,10,p,a,sigma2);
		Burg_AR(array[i],len,p,a,sigma2);

		ar1[i] = a[1];
		ar2[i] = a[2];
		ar3[i] = a[3];
		ar4[i] = a[4];
	}
	delete a;
	delete sigma2;
}

void FeatureExtraction::save_result(string val_name, double *val){
	out << val_name << ",";
	for(int i = 0; i < channel; ++i){
		out << val[i] << ",";
	}
	out << endl;
}

void FeatureExtraction::save_origin(string val_name, double *val,int len){

	out << val_name << ",";
	for(int i = 0; i < len; ++i){
		if(i %2 == 0)
			out << abs(val[i]) << ",";
	}
	out << endl;
}



void FeatureExtraction::Means_AVA_vector(vector< vector <int> > array,int len,int rows,double *means,double *ava){
	double temp;
	double avatemp = 0.0;
	for(int i = 0;i<rows;i++){
		temp = 0.0;
		avatemp = 0.0;
		for(int j = 0;j < len;j++){
			temp += array[i][j];
			avatemp += abs(array[i][j]);
		}
		means[i] = (temp/len);
		ava[i] = avatemp/len;
	}
}


void  FeatureExtraction::RMS_SD_SSD_vector(vector <vector <int> > array,int len,int rows,double *means,double *ava,double *rms,double *sd,double *ssd){

	double rmstemp = 0.0;
	double sdtemp = 0.0;
	double ssdtemp;
	double val = 0.0;
	double a =2.0;
	Means_AVA_vector(array,len,rows,means,ava);

	for(int i = 0;i < rows;i++){
		rmstemp = 0.0;
		sdtemp = 0;
		ssdtemp =0.0;
		for(int j = 0;j < len;j++){
			val = array[i][j];
			rmstemp += val*val;
			sdtemp += (val-means[i])*(val-means[i]);
			ssdtemp+=(abs(val)-ava[i])*(abs(val)-ava[i]);
		}
		rms[i] = sqrt(rmstemp/len);
		sd[i] =  sdtemp/len;
		ssd[i] = sqrt(ssdtemp/len);
	}
}


void  FeatureExtraction::PA_vector(vector <vector <int> > array,int len,int rows,double *pa){
	double maxval = 0.0;
	for(int i = 0;i < rows;i++){
		maxval = 0.0;
		for(int j = 0;j < len-1;j++){
			maxval = max(array[i][j+1],array[i][j]);
		}
		pa[i] = maxval;
	}
}


void FeatureExtraction::MPF_and_MF2_vector(vector <vector <int> > array,int len,int rows,double *mpf2,double *mf2){
	double *y = new double[len];
	double *x = new double[len];
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < len; j++)
			x[j] = double(array[i][j]);
		FFT(x, y,len);
		mpf2[i] = sum2(y,len) / sum1(y,len);
		mf2[i] =  10*log10l(sum1(y,len) / 2);
	}
	delete x;
	delete y;
}

void FeatureExtraction::MPF_and_MF1_vector(vector <vector <int> > array,int len,int rows,double *mpf1,double *mf1){

	double *y = new double[len];
	double *x = new double[len];
	double sum11 = 0.0;
	double sum22 = 0.0;
	for(int i = 0; i < rows; i++){
		sum11 = 0.0;
		sum22 = 0.0;
		for(int j = 0; j < len; j++)
			x[j] = double(array[i][j]);
		int size = 2*len-1;
		double *xocrr = new double[size];
		for(int J = 0; J < size; J++)
			xocrr[J] = 0.0;
		Rxx(x,len,xocrr);
		FFT(xocrr, y,len);
		sumcal(y,len,sum11,sum22);
		mpf1[i] = sum11/ sum22;
		mf1[i] = 10*log10l(sum22 / 2);
	}
	delete x;
	delete y;
}

void FeatureExtraction::AR_vector(vector <vector <int> > array,int len,int rows,double *ar1,double *ar2,double *ar3,double *ar4){
	double *a,*sigma2;
	//int sig[10]={1,2,3,78,45,23,-45,-12,2};
	int p=4;
	a=(double*)malloc((p+1)*sizeof(double));
	sigma2=(double*)malloc((p+1)*sizeof(double));
	for(int i =0 ;i < rows;i++){
		//Burg_AR(sig,10,p,a,sigma2);
		double tem[len];
		for (int j = 0; j < len; j++)
            tem[j] = array[i][j];
		Burg_AR(tem,len,p,a,sigma2);

		ar1[i] = a[1];
		ar2[i] = a[2];
		ar3[i] = a[3];
		ar4[i] = a[4];
	}
	delete a;
	delete sigma2;
}


