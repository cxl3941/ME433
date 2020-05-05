#include "imu.h"

void imu_setup(){
char message[50];

   if (readIMU(0b11010111, 0x0F) == 105) {
          
        } else 
            { while (1) {
                sprintf(message, "I am not LSM6DS33"); 
                printString(1,1,message);
                ssd1306_update();
                _CP0_SET_COUNT(0);
                while (_CP0_GET_COUNT() < 24000000 / 2){

            setPin(0b01000000,0x0A, 0x00);
                }
                while (_CP0_GET_COUNT() >= 24000000 / 2 && _CP0_GET_COUNT() < 48000000 / 2){

            setPin(0b01000000,0x0A, 0xFF);

                }
            }

            }
    
   


setPin (0b11010110, 0x10, 0b10000010); //CTRL1_XL
setPin (0b11010110, 0x11, 0b10001010); //CTRL2_G 
setPin (0b11010110, 0x12, 0b00000100); //CTRL3_C

    
    

}

void imu_read(unsigned char reg, signed short * data, int len){
    
    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
    
    // turn the chars into the shorts
    
}

unsigned char readIMU (unsigned char address, unsigned char regist){
    i2c_master_start();
    i2c_master_send(0b11010110);
    i2c_master_send(regist);
    i2c_master_restart();
    i2c_master_send(0b11010111);
    i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    return I2C1RCV;
}