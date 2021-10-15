# include "fsrCollector.h"

using namespace std;

fsrCollector::fsrCollector(){
	FsrOrifileName << "FsrOrigin" << ".csv";
	FsrOri.open(FsrOrifileName.str(),ios::out);

	FsrFeafileName<< "FsrFea" << ".csv";
	FsrFea.open(FsrFeafileName.str(),ios::out);
}

void fsrCollector::GetFsrFea(){
    fsrarray.push_back(fsr0array);
    fsrarray.push_back(fsr1array);
    fsrarray.push_back(fsr2array);
    fsrarray.push_back(fsr3array);
    fsrarray.push_back(fsr4array);
    int fsr_len = fsrarray[0].size()-1;

    FsrExtraction.RMS_SD_SSD_vector(fsrarray, fsr_len, fsr_num, FsrMeans, FsrAva, FsrRms, FsrSd, FsrSsd);
    FsrExtraction.AR_vector(fsrarray, fsr_len, fsr_num, FsrAr1, FsrAr2, FsrAr3, FsrAr4);
    FsrExtraction.PA_vector(fsrarray, fsr_len, fsr_num, FsrPa);

    fsr0array.clear();
    fsr1array.clear();
    fsr2array.clear();
    fsr3array.clear();
    fsr4array.clear();
}

int fsrCollector::read_fsr_data(){
    char buf[1024];
    int cnt_col = 0;
    int tem_i = 0, nwrite;
    char tem[5];//将缓冲区数据发送FSR数组的一个过渡
    char c;
    int count = 0;
    //send start_comdata and the arduino begin to collector fsr data
    char start_comdata[1]={'1'};
    cout<<"fsr_fd: "<<fsr_fd<<endl;
    nwrite=write(fsr_fd,start_comdata,1);
    if (!nwrite){
        cout << "Wirte FSR comdata failed!" << endl;
        exit(1);
    }

    else
        cout << "Begin to read FSR data." << endl;

    while (!gesture_start){};
    cout<<"start to collector fsr data:"<<endl;
    while(!gesture_end){
        memset(buf, 0, sizeof(buf));
        int nread = read(fsr_fd,buf,sizeof(buf));
        if (nread ==  0){
            sleep(1);
            continue;
        }
        int buf_i = 0;//对缓冲区的数据进行计数
        while(buf_i>=0 && buf_i < nread){
            char c = buf[buf_i];
            switch(c){
                case'a':
                    tem[tem_i] = '\0';
                    fsr0array.push_back(atoi(tem));
                    tem_i = 0;
                    count++;
                    //cout<<"FSR: "<<fsr0array.back()<<' ';
                    break;
                case'b':
                    tem[tem_i] = '\0';
                    fsr1array.push_back(atoi(tem));
                    //cout<<fsr1array.back()<<' ';
                    tem_i = 0;
                    break;
                case'c':
                    tem[tem_i] = '\0';
                    fsr2array.push_back(atoi(tem));
                    tem_i = 0;
                    //cout<<fsr2array.back()<<' ';
                    break;
                case'd':
                    tem[tem_i] = '\0';
                    fsr3array.push_back(atoi(tem));
                    tem_i = 0;
                    //cout<<fsr3array.back()<<' ';
                    break;
                case'e':
                    tem[tem_i] = '\0';
                    fsr4array.push_back(atoi(tem));
                    tem_i = 0;
                    //cout<<fsr4array.back()<<endl;
                    break;
                default:
                    tem[tem_i] = c;
                    tem_i++;
            }
            buf_i++;
        }

    }
    cout<<"FSR END"<<endl;
    GetFsrFea();
    return fsr0array.size();
}



void fsrCollector::saveToFsrOri(vector <vector <int> > fsrarray, int fsr_len){
	for(int i = 0;i< fsr_num;i++){
        FsrOri<<"Fsr"<<i<<',';
        for (int j = 0; j < fsr_len; j++){
            FsrOri<<fsrarray[i][j]<<',';
        }
        FsrOri<<endl;
	}
}

void fsrCollector::saveToFsrFea(string name,double *array,int len){
	FsrFea<<name<<',';
	for(int i = 0;i<len;i++){
		FsrFea<<array[i]<<',';
	}
}

int fsrCollector::OpenDev(char *Dev){
    int fd = open( Dev, O_RDWR|O_NOCTTY|O_NDELAY );
    sleep(1);
    if(isatty(STDIN_FILENO)==0)
        printf("FSR input is not a terminal device\n");
    else
        printf("FSR isatty success!\n");
    if (-1 == fd)
    {
        perror("Can't Open FSR Serial Port");
        return -1;
    }
    else
        return fd;
}

int fsrCollector::set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop){
    struct termios newtio, oldtio;
    /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if ( tcgetattr( fd,&oldtio) != 0){
         perror("SetupSerial 1");
         return -1;
     }
    bzero( &newtio, sizeof( newtio ) );
    /*步骤一，设置字符大小*/
    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_cflag &= ~CSIZE;
    /*设置data位*/
    switch( nBits ){
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
    }
/*设置奇偶校验位*/
    switch( nEvent ){
        case 'O': //奇数
        case 'o':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
        break;
        case 'E': //偶数
        case 'e':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
        break;
        case 'N': //无奇偶校验位
        case 'n':
            newtio.c_cflag &= ~PARENB;
        break;
    }
    /*设置波特率*/
    switch( nSpeed ){
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
        break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
        break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
        break;
        case 19200:
            cfsetispeed(&newtio, B19200);
            cfsetospeed(&newtio, B19200);
        break;
        case 38400:
            cfsetispeed(&newtio, B38400);
            cfsetospeed(&newtio, B38400);
        break;
        case 57600:
            cfsetispeed(&newtio, B57600);
            cfsetospeed(&newtio, B57600);
        break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
        break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
        break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
        break;
    }
    /*设置停止位*/
    switch(nStop){
        case 1:
            newtio.c_cflag &= ~CSTOPB;
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;
            break;
    }
    /*设置等待时间和最小接收字符*/
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    /*处理未接收字符*/
    tcflush(fd,TCIFLUSH);
    /*激活新配置*/
    int n = tcsetattr(fd,TCSANOW,&newtio);
    if (n !=0){
        perror("FSR Serial Port set error");
        return -1;
    }
    else{
        printf("FSR Serial Port set done!\n");
        return 0;
    }
}

int fsrCollector::init_fsr_hardware(){
    fsr_fd = OpenDev(fsr_dev);
    //cout<<"emg init : fsr_fd :"<<fsr_fd<<endl;
    int n = set_opt(fsr_fd, 19200, 8, 'N', 1);
    return n;
}

fsrCollector::~fsrCollector(){
    close(fsr_fd);
}
