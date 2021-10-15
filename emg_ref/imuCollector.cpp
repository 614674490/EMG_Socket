#include "imuCollector.h"
#include <bitset>

using namespace std;

int16_t imu[imu_num];

imuCollector::imuCollector(){
}

int imuCollector::spi_init(char filename[40], uint8_t mode, uint8_t lsb, uint8_t bits, uint32_t speed){
    int result, fd, ret;
    fd = open(filename, O_RDWR);
    result = fd;
    if (fd < 0){
        printf("Can't open SPI device.");
        result = -1;
    }

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret < 0){
        printf("SPI device set mode failed");
        result = -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb);
    if (ret < 0){
        printf("SPI device set LSB_FIRST failed");
        result = -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret < 0){
        printf("SPI device set BITS_PER_WORD failed");
        result = -1;
    }

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret < 0){
        printf("SPI device set WR_MAX_SPEED_HZ failed");
        result = -1;
    }
    return result;
}


void imuCollector::saveToImuOri(vector <vector <int>> imuarray, int imu_len){
    ImuOrifileName.str("");
	ImuOrifileName << "ImuOrigin" << ".csv";
	ImuOri.open(ImuOrifileName.str(),ios::out);
    for (int i = 0; i < 3; i++) {
		//ImuOri << "Acc" << i << ',';
		for (int j = 0; j < imu_len; j++) {
			ImuOri << imuarray[i][j] << ',';
		}
		ImuOri << endl;
	}
	for (int i = 3; i < 6; i++) {
		//ImuOri << "GYR" << i << ',';
		for (int j = 0; j < imu_len; j++)
			ImuOri << imuarray[i][j] << ',';
		ImuOri << endl;
	}
	for (int i = 6; i < 9; i++) {
        //ImuOri << "COMPASS" << i << ',';
		for (int j = 0; j < imu_len; j++)
			ImuOri << imuarray[i][j] << ',';
		ImuOri << endl;
	}
	for (int i = 9; i < 12; i++) {
        //ImuOri << "RPY" << i << ',';
		for (int j = 0; j < imu_len; j++)
			ImuOri << imuarray[i][j] << ',';
		ImuOri << endl;
	}
    for (int i = 12; i < 16; i++) {
       // ImuOri << "Q" << i << ',';
		for (int j = 0; j < imu_len; j++)
			ImuOri << imuarray[i][j] << ',';
		ImuOri << endl;
	}
}

int imuCollector::write_one_byte(int imu_fd, uint8_t address, uint8_t *writebuf){
    uint8_t add[] = {address};
    int num_write = write(imu_fd, add, 1);
    if (num_write == -1)
        printf("Write write_one_byte address failed!\n");

    num_write = write(imu_fd, writebuf, 1);
    if (num_write == -1)
        printf("Write writebuf failed!\n");
    return num_write;
}

int imuCollector::read_one_byte(int imu_fd, uint8_t address, uint8_t *readbuf){
    uint8_t add[1] = {address};
    int num_read = write(imu_fd, add, 1);
    if (num_read == -1)
        printf("Write read_one_byte address failed!\n");
    num_read = read(imu_fd, readbuf, 1);
    if (num_read == -1)
        printf("Read_one_byte failed!\n");
    return num_read;
}

int imuCollector::read_all_byte(int imu_fd, uint8_t *readbuf){
    uint8_t address[1] = {0xC1};
    int num_read = write(imu_fd, address, 1);
    if (num_read == -1)
        printf("Write read_all_byte address failed!\n");
    num_read = read(imu_fd, readbuf, 41);
    if (num_read != 41)
        printf("Read_all_byte failed!\n");
    return num_read;
}

int imuCollector::read_imu_data(){
    uint8_t readdata[41];
    int num;
    int i = 0;
    float a=0, b=0, c=0;
	//while(!gesture_start){};
    cout<<"start to collect imu data:"<<endl;
    while(!gesture_end){
        char accsum = 0, gyrsum = 0, compasssum = 0, rpysum = 0, qsum = 0;
        num = read_all_byte(imu_fd, readdata);

        if (readdata[34] == 0x0d){
            for (int i=0; i<6; i++){
                accsum += readdata[2+i];
                gyrsum += readdata[8+i];
                compasssum += readdata[14+i];
                rpysum += readdata[20+i];
                qsum += readdata[26+i];
            }
            qsum = qsum + readdata[32] + readdata[33];
            if ((accsum == readdata[36]) && (gyrsum == readdata[37]) &&(compasssum == readdata[38])&&
                (rpysum == readdata[39]) && (qsum == readdata[40])) {
                int16_t imudata[16];
                imudata[0] = (readdata[2]<<8|readdata[3])-1200; //accx
                imudata[1] = (readdata[4]<<8|readdata[5])+700;//accy
                imudata[2] = (readdata[6]<<8|readdata[7])-90;//accz
                imudata[3] = (readdata[8]<<8|readdata[9]);//gyrx
                imudata[4] = (readdata[10]<<8|readdata[11]);//gyry
                imudata[5] = (readdata[12]<<8|readdata[13]);//gyrz
                imudata[6] = (readdata[14]<<8|readdata[15]);//compassx
                imudata[7] = (readdata[16]<<8|readdata[17]) ;//compassy
                imudata[8] = (readdata[18]<<8|readdata[19]) ;//compassz
                imudata[9] = (readdata[20]<<8|readdata[21]);//roll
                imudata[10] = (readdata[22]<<8|readdata[23]);//pitch
                imudata[11] = (readdata[24]<<8|readdata[25]);//yaw
                imudata[12] = (readdata[26]<<8|readdata[27]);//q0
                imudata[13] = (readdata[28]<<8|readdata[29]);//q1
                imudata[14] = (readdata[30]<<8|readdata[31]);//q2
                imudata[15] = (readdata[32]<<8|readdata[33]);//q3
                //for(int i=0; i<16; i++)
                   //cout<<imudata[i]<<"  ";
                //cout<<endl;
                imu0array.push_back(imudata[0]);
                imu1array.push_back(imudata[1]);
                imu2array.push_back(imudata[2]);
                imu3array.push_back(imudata[3]);
                imu4array.push_back(imudata[4]);
                imu5array.push_back(imudata[5]);
                imu6array.push_back(imudata[6]);
                imu7array.push_back(imudata[7]);
                imu8array.push_back(imudata[8]);
                imu9array.push_back(imudata[9]);
                imu10array.push_back(imudata[10]);
                imu11array.push_back(imudata[11]);
                imu12array.push_back(imudata[12]);
                imu13array.push_back(imudata[13]);
                imu14array.push_back(imudata[14]);
                imu15array.push_back(imudata[15]);
                usleep(5000);
            }
        }
	}

    cout<<"IMU END"<<endl;
	imuarray.push_back(imu0array);
	imuarray.push_back(imu1array);
	imuarray.push_back(imu2array);
	imuarray.push_back(imu3array);
	imuarray.push_back(imu4array);
	imuarray.push_back(imu5array);
	imuarray.push_back(imu6array);
	imuarray.push_back(imu7array);
	imuarray.push_back(imu8array);
	imuarray.push_back(imu9array);
	imuarray.push_back(imu10array);
	imuarray.push_back(imu11array);
	imuarray.push_back(imu12array);
	imuarray.push_back(imu13array);
	imuarray.push_back(imu14array);
	imuarray.push_back(imu15array);

	imu0array.clear();
    imu1array.clear();
    imu2array.clear();
    imu3array.clear();
    imu4array.clear();
    imu5array.clear();
    imu6array.clear();
    imu7array.clear();
    imu8array.clear();
    imu9array.clear();
    imu10array.clear();
    imu11array.clear();
    imu12array.clear();
    imu13array.clear();
    imu14array.clear();
    imu15array.clear();
   // cout<<"IMU END"<<endl;
    return imuarray[0].size();
}


int imuCollector:: init_imu_hardware(){
    static uint8_t mode = SPI_MODE_3;
    static uint8_t lsb = 0;//设置传输数据是否先传输低比特位。0表示高比特在前，非0表低比特在前
    static uint8_t bits = 8;
    static uint32_t speed = 125000;
    static uint16_t delay;
    char *devname = "/dev/spidev0.0";
    imu_fd = spi_init(devname, mode, lsb, bits, speed);
    return imu_fd;
}

imuCollector::~imuCollector(){
    close(imu_fd);
}

