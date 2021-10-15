#include "emgCollector.h"

int emg[emg_num][windowsize];

ActiveDetect active_detect;
fsrCollector fsrcollector;
imuCollector imucollector;

Socket_click Socket_SendFeaToPython;
using namespace std;

void emgCollector::Init_EmgHardWork()
{
    I2CFile = open("/dev/i2c-1", O_RDWR);
    printf("I2CFile = %d\n", I2CFile);
    if (I2CFile < 0)
        printf("Open I2C failed");
    if (ioctl(I2CFile, I2C_TENBIT, 0) < 0)
        printf("Couldn't set I2C device 7 bit");
    if (ioctl(I2CFile, I2C_SLAVE, ADS_address) < 0)
        printf("Couldn't find I2C device");

    uint8_t writeBuf[3];
    writeBuf[0] = 0b00000100; //Pointer Byte, ADC pin configuration
    writeBuf[1] = 0b00000000; //Data high
    writeBuf[2] = 0b11111111; //Data low

    int result = write(I2CFile, writeBuf, 3);
    printf("write 1 result = %d, errno=%d\n", result, errno);

    //select ADCs for conversion
    writeBuf[0] = 0b00000010; //selects ADCS for conversion
    writeBuf[1] = 0b00000010;
    writeBuf[2] = 0b11111111;
    result = write(I2CFile, writeBuf, 3);
    printf("write 2 result = %d, errno=%d\n", result, errno);

    // 0100 ADC readback
    writeBuf[0] = 0b01000000;
    result = write(I2CFile, writeBuf, 1);
    printf("write 3 result = %d, errno=%d\n", result, errno);
    usleep(1000); //usleep(sleeptime);
}

void emgCollector::ReadOneEmgDate(int count)
{
    memset(readBuf, 0, sizeof(readBuf));
    int nread = 0;
    while (nread <= 0)
    {
        nread = read(I2CFile, readBuf, sizeof(readBuf));
    }
    int i = 0;
    int tem_i = 0;
    char tem[5];
    for (int i = 0; i < 8; i++)
    {
        memset(readBuf, 0, sizeof(readBuf));
        int result_R = read(I2CFile, readBuf, 2);
        int16_t buff = readBuf[0] << 8 | readBuf[0];
        //printf("%d,%d", i, buff);
        int num = buff >> 12;
        printf("%d", num);
        emg[num][count] = buff & (0b0000111111111111);
        usleep(1000);
        printf("%d", count);
    }
    count++;
}

void emgCollector::start_collect_emg()
{
    /*/set socket communition
    char *Python_Address ="127.0.0.1";
    int Port_SendFeaToPython = 2000;
    Socket_SendFeaToPython.set_socket(Python_Address, Port_SendFeaToPython);


    /*open the fsr uart:/dev/ttyAMA0*/
    int fsrInitResult = fsrcollector.init_fsr_hardware(); //0:sucussed  -1:failed
    if (fsrInitResult < 0)
    {
        std::cout << "Init Fsr Dev failed!" << std::endl;
        exit(1);
    }

    int imuInitResult = imucollector.init_imu_hardware(); //0:sucussed  -1:failed
    if (imuInitResult < 0)
    {
        std::cout << "Init IMU Dev failed!" << std::endl;
        exit(1);
    }

    Init_EmgHardWork();
    while (1)
    {
        for (int i = 0; i < windowsize; i++)
            ReadOneEmgDate(i);

        active_detect.CollectorData(fsrcollector, imucollector, emg);

        /*send '0' to fsr and quit the fsr*/
        char stop_comdata[1] = {'0'};
        if (stop_signal)
        {
            int nwrite = write(fsrcollector.fsr_fd, stop_comdata, 1);
            if (!nwrite)
                cout << "Stop FSR failed!" << endl;
            else
                cout << "Stop FSR." << endl;
            exit(1);
        }
    }
}

emgCollector::~emgCollector()
{
    close(I2CFile);
};
