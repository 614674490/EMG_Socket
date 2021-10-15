#include "ActiveDetect.h"

int pa_index = 0;
int ActiveDetect::len2 = 0;
const int Time = 20;
bool gesture_start = false;
bool gesture_end = true;
bool gesture_available = false;

thread fsrcollectorthread;
thread imucollectorthread;
extern Socket_click Socket_SendFeaToPython;

/*creat the EmgFsrOrigin.csv ImuOrigun.csv EmgFsrFea.csv*/
ActiveDetect::ActiveDetect():emgarray_len(0), cnt(0), ener_cnt(0), gesture_counts(0){
	time_t timestamp = time(0);
	EmgOrifileName << "EmgOrigin" << ".csv";
	EmgOri.open(EmgOrifileName.str(),ios::out);

	EmgFeafileName<< "EmgFea" << ".csv";
	EmgFea.open(EmgFeafileName.str(),ios::out);
}

/*save the collected data(EMG and ACC)*/
void ActiveDetect::saveTofileOri(){
	for(int i = 0;i< emg_num;i++){
		EmgOri<<"Emg"<<i<<',';
		for(int j = 0;j< len2;j++)
			EmgOri<<abs(emgarray[i][j])<<',';
		EmgOri<<endl;
	}
}

/*save the emg feature data to ofstring EmgFea*/
void ActiveDetect::saveToFileFea(string name,double *array,int len){
	EmgFea<<name<<',';
	for(int i = 0;i<len;i++){
		EmgFea<<array[i]<<',';
	}
}

/*create the fsr thread to collector fsr data*/
int CreatPthreadToCollectorFsrData(fsrCollector &fsr){
    //cout<<"active; CreatPthreadToCollectorFsrData fsr_fd: "<<fsr.fsr_fd<<"  "<<fsr.fsr_dev<<endl;
    int fsr_size  = fsr.read_fsr_data();
    return fsr_size;
    //pthread_exit(NULL);
}


int CreatPthreadToCollectorImuData(imuCollector &imu){
    int imu_length  = imu.read_imu_data();
    return imu_length;
   // pthread_exit(NULL);
}


void ActiveDetect::FeaToString(double EmgAva[emg_num], double EmgRms[emg_num], double EmgSsd[emg_num], double EmgAr1[emg_num],
        double EmgAr2[emg_num], double EmgAr3[emg_num], double EmgAr4[emg_num], double EmgPa[emg_num],
        double FsrMeans[fsr_num], double FsrRms[fsr_num], double FsrSd[fsr_num], double FsrAr1[fsr_num],
        double FsrAr2[fsr_num], double FsrAr3[fsr_num], double FsrAr4[fsr_num], double FsrPa[fsr_num], char (&FeaString)[1024]){

    FeaString[0] = '\0';
    strcpy(FeaString, "Begin:");
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgAva[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgRms[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgSsd[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgAr1[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgAr2[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgAr3[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgAr4[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<emg_num; i++){
        char tmp[10];
        gcvt(EmgPa[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }

    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrMeans[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrRms[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrSd[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrAr1[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrAr2[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrAr3[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrAr4[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    for(int i=0; i<fsr_num; i++){
        char tmp[10];
        gcvt(FsrPa[i], 7, tmp);
        strcat(tmp, ",");
        strcat(FeaString, tmp);
    }
    strcat(FeaString, "end");

}

/*feature normalized*/
void ActiveDetect::featureScale(double *originalFeature,double *scaledFeature,double *feature_min,double *feature_max,double y_upper,double y_lower){
	for(int i =0;i < featurecnt;i++){
		if(feature_max[i] == feature_min[i]){
			scaledFeature[i] = originalFeature[i];
			return;
		}
		if(abs(originalFeature[i]-feature_min[i])<=0.0000001)
			scaledFeature[i] = y_lower;
		else if(abs(originalFeature[i]-feature_max[i])<=0.0000001)
			scaledFeature[i] = y_upper;
		else scaledFeature[i] = y_lower+(y_upper-y_lower)*
			(originalFeature[i] - feature_min[i])/(feature_max[i]-feature_min[i]);
	}
}


void ActiveDetect::quetoarr(){
	int i = 0;//emgarray_len;
	cout<<"quetoarr 0"<<endl;
	while(!emgque0.empty()){
		emgarray[0][i] = (double)(emgque0.front());
		emgarray[1][i] = (double)(emgque1.front());
		emgarray[2][i] = (double)(emgque2.front());
		emgarray[3][i] = (double)(emgque3.front());

		emgarray[4][i] = (double)(emgque4.front());
		emgarray[5][i] = (double)(emgque5.front());
		emgarray[6][i] = (double)(emgque6.front());
		emgarray[7][i] = (double)(emgque7.front());

		i++;
		emgque0.pop();
		emgque1.pop();
		emgque2.pop();
		emgque3.pop();

		emgque4.pop();
		emgque5.pop();
		emgque6.pop();
		emgque7.pop();

	}
	emgarray_len  = i;
	len2 = i;
}


void ActiveDetect::pre_pro(int cnt,int emg[emg_num]){
	for(int i = 0; i < emg_num;i++)
        preemg[i][cnt] = emg[i];
}


double ActiveDetect::cal_energy(){
	double energy = 0;
	int n = emg_num;
	int m = windowsize;
	for(int i = 0;i < n;i++){
		for(int j = 0;j < m;j++)
		{
			energy += (preemg[i][j]*preemg[i][j]);
		}

    }
	return (energy/(m*n));
}

bool ActiveDetect::push_que(){
	int m = windowsize;
	for(int j = 0;j< m;j++){
		emgque0.push(preemg[0][j]);
		emgque1.push(preemg[1][j]);
		emgque2.push(preemg[2][j]);
		emgque3.push(preemg[3][j]);

		emgque4.push(preemg[4][j]);
		emgque5.push(preemg[5][j]);
		emgque6.push(preemg[6][j]);
		emgque7.push(preemg[7][j]);

	}
	return true;
}

bool ActiveDetect::pop_que(int count){
	while(!emgque0.empty()){
		emgque0.pop();
		emgque1.pop();
		emgque2.pop();
		emgque3.pop();

		emgque4.pop();
		emgque5.pop();
		emgque6.pop();
		emgque7.pop();

	}
	return true;
}

/*Display the result of gesture recognition.*/
void ActiveDetect::resultGestureDisplay(int label){
	switch(label){
	case(-1):
		cout << "Recognition failed!!" << endl;
		return;
	case(0):
		cout<<"Take Off·­"<<endl;
		return;
	case(1):
		cout<<"Landing."<<endl;
		return;
	case(2):
		cout<<"Hovering.­"<<endl;
		return;
	case(3):
		cout<<"Go Ahead­."<<endl;
		return;
	case(4):
		cout<<"Retreat.­"<<endl;
		return;
	case(5):
		cout<<"Rising."<<endl;
		return;
	case(6):
		cout<<"Decline."<<endl;
		return;
	case(7):
		cout<<"Left."<<endl;
		return;
	case(8):
		cout<<"Right."<<endl;
		return;
	case(9):
		cout<<"Back."<<endl;
		return;
	case(10):
		cout<<"Anticlockwise."<<endl;
		return;
	case(11):
		cout<<"Clockwise."<<endl;
		return;
	case(12):
		cout<<"Take a Picture."<<endl;
		return;
	case(13): //ÏÖÔÚÈ¥µôÁË
		cout<<"Video."<<endl;
		return;
	case(14):
		cout<<"Stop."<<endl;
		return;
	}
}


void ActiveDetect::CollectorData(fsrCollector &fsrcollector, imuCollector &imucollector, int emg[emg_num][windowsize]) {
	for(int i=0; i<emg_num; i++)
        for(int j=0; j<windowsize; j++){
            preemg[i][j] = emg[i][j];
            //cout<<emg[0][j]<<"  "<<emg[1][j]<<"  "<<emg[2][j]<<"  "<<emg[3][j]<<endl;
            //cout<<emg[2][j]<<endl;
        }
	    double energy = 0.0;
        energy = cal_energy();
        //cout<<"energy: "<<energy<<endl;
        if ( (energy > T) && (ener_cnt < Time)){
            gesture_start = true;
            gesture_end = false;
            push_que();//put the emg data and imu data to queue
            ener_cnt++;
        }
        else{
            gesture_start = false;
            gesture_end = true;
            //cout<<"EMG END"<<endl;
        }

        if ((ener_cnt == 1) && (gesture_start)){
            fsrcollectorthread = thread(CreatPthreadToCollectorFsrData, ref(fsrcollector));
            imucollectorthread = thread(CreatPthreadToCollectorImuData, ref(imucollector));
            //cout<<"fsr_thread_id: "<<fsrcollectorthread.get_id()<<"  imu_thread_id: "<<imucollectorthread.get_id()<<endl;
        }

        if (gesture_end){
            if(ener_cnt < minlen/windowsize)
                gesture_available = false;
            else
                gesture_available = true;
            if (fsrcollectorthread.joinable()){
                fsrcollectorthread.join();
                //cout<<"fsrcollectorthread.join();"<<endl;
            }
            if (imucollectorthread.joinable()){
                imucollectorthread.join();
                //cout<<"imucollectorthread.join();"<<endl;
            }

        }

        if (!gesture_available && gesture_end){
            pop_que(ener_cnt * windowsize);
            ener_cnt = 0;
            while (!fsrcollector.fsrarray.empty())
                fsrcollector.fsrarray.pop_back();
            while (!imucollector.imuarray.empty())
                imucollector.imuarray.pop_back();
        }
        else if (gesture_available && gesture_end){
            cout<<"The length of "<<gesture_counts<<" gesture is: "<<ener_cnt<<endl;
            ++gesture_counts;
            cout<<"The gesture is end."<< endl;
            quetoarr();
            cout<<"11111"<<endl;
            //compute the EMG fea
			double EmgAva[emg_num], EmgMeans[emg_num], EmgRms[emg_num], EmgSd[emg_num], EmgSsd[emg_num], EmgAr1[emg_num], EmgAr2[emg_num];
			double EmgAr3[emg_num], EmgAr4[emg_num], EmgPa[emg_num], EmgMpf1[emg_num], EmgMf1[emg_num], EmgMpf2[emg_num], EmgMf2[emg_num];

			extraction.RMS_SD_SSD(emgarray, len2-1, emg_num, EmgMeans, EmgAva, EmgRms, EmgSd, EmgSsd);
			extraction.AR(emgarray, len2-1, emg_num, EmgAr1, EmgAr2, EmgAr3, EmgAr4);
			extraction.PA(emgarray, len2-1, emg_num, EmgPa);

			extraction.MPF_and_MF1(emgarray, len2-1, emg_num, EmgMpf1, EmgMf1);
			extraction.MPF_and_MF2(emgarray, len2-1, emg_num, EmgMpf2, EmgMf2);
            cout<<"EMGAVA: "<<EmgAva[0]<<"  "<<EmgAva[1]<<"  "<<EmgAva[2]<<"  "<<EmgAva[3]<<endl;
            cout<<"FSRAVA: "<<fsrcollector.FsrAva[0]<<"  "<<fsrcollector.FsrAva[1]<<"  "<<endl;
			saveToFileFea("EmgAva", EmgAva, emg_num);
			//saveToFileFea("EMGrms", EmgRms, emg_num);
			saveToFileFea("EmgRms", EmgRms, emg_num);
			saveToFileFea("Emgssd", EmgSsd, emg_num);
			saveToFileFea("EmgAr1", EmgAr1, emg_num);
			saveToFileFea("EmgAr2", EmgAr2, emg_num);
			saveToFileFea("EmgAr3", EmgAr3, emg_num);
			saveToFileFea("EmgAr4", EmgAr4, emg_num);
			saveToFileFea("EmgPa", EmgPa, emg_num);
			EmgFea << endl;
            saveTofileOri();

			fsrcollector.saveToFsrFea("FsrMeans", fsrcollector.FsrMeans, fsr_num);
            fsrcollector.saveToFsrFea("FsrRms", fsrcollector.FsrRms, fsr_num);
            fsrcollector.saveToFsrFea("FsrSd", fsrcollector.FsrSd, fsr_num);
            fsrcollector.saveToFsrFea("FsrAr1", fsrcollector.FsrAr1, fsr_num);
            fsrcollector.saveToFsrFea("FsrAr2", fsrcollector.FsrAr2, fsr_num);
            fsrcollector.saveToFsrFea("FsrAr3", fsrcollector.FsrAr3, fsr_num);
            fsrcollector.saveToFsrFea("FsrAr4", fsrcollector.FsrAr4, fsr_num);
            fsrcollector.saveToFsrFea("FsrPa", fsrcollector.FsrPa, fsr_num);
            fsrcollector.FsrFea << endl;
            int fsr_len = fsrcollector.fsrarray[0].size();
            fsrcollector.saveToFsrOri(fsrcollector.fsrarray, fsr_len);

            int imu_len = imucollector.imuarray[0].size();
            cout<<imucollector.imuarray[0][0]<<endl;
            imucollector.saveToImuOri(imucollector.imuarray, imu_len);
            imucollector.ImuOri.close();


            ifstream in("../ImuOrigin.csv");
            char buffer[256];
            in.getline(buffer, 100);
            cout<<buffer<<endl;
            char FeaString[1024] = {0};
			FeaToString(EmgAva, EmgRms, EmgSsd, EmgAr1, EmgAr2,EmgAr3, EmgAr4, EmgPa, fsrcollector.FsrMeans,
                        fsrcollector.FsrRms, fsrcollector.FsrSd, fsrcollector.FsrAr1,fsrcollector.FsrAr2,
                        fsrcollector.FsrAr3, fsrcollector.FsrAr4,fsrcollector.FsrPa, FeaString);
            Socket_SendFeaToPython.send_data(FeaString);
            cout<<FeaString<<endl;
            char data[4] = {'0'};
            do{
                Socket_SendFeaToPython.rece_data(data);
            }while((data[0] != 'e')  || (data[1] != 'n')  || (data[2] != 'd') );
            cout<<data<<endl;

            for (int i=0; i<fsr_num; i++)
                fsrcollector.fsrarray.pop_back();
            for (int i=0; i<imu_num; i++)
                imucollector.imuarray.pop_back();
            pop_que(ener_cnt * windowsize);
            ener_cnt = 0;
        }

    //cnt = 0;
	//}
}


ActiveDetect::~ActiveDetect(){
	EmgFea.close();
	EmgOri.close();
}
