#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h" 
#include "i2c_master_noint.h"
#include "font.h"
#include "ssd1306.h"
#include <math.h>
#include "imu.h"


// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

 void setPin (unsigned char address, unsigned char regist, unsigned char value);
 unsigned char readPin (unsigned char address, unsigned char regist);
 void drawLetter (unsigned char x, unsigned char y, unsigned char letter);
 void printString (unsigned char x, unsigned char y, char *message);
 unsigned char readIMU (unsigned char address, unsigned char regist);
 void IMU_init (void);
 void I2C_read_multiple(unsigned char address, unsigned char register, unsigned char * data, int length);
 void bar_x (signed short x);
 
int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here

    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    
    i2c_master_setup();
    ssd1306_setup();
    imu_setup();
   
   

    
    
    __builtin_enable_interrupts();
    
    
    
    unsigned char wAdd,value,regist, rAdd;
    
    
    
    
    wAdd = 0b01000000; 
    rAdd = 0b01000001;
    regist = 0x00;
    value = 0x00;
    
    setPin (wAdd,regist, value); //initialise A as output 
   
    regist = 0x10;
    value = 0xFF;
   
    setPin (wAdd,regist, value); //initialise B as input
    
    regist = 0x0A;
    value = 0xFF;
    
    setPin (wAdd,regist, value);
    
    char message[50];
    char frames[50];
    
    unsigned char output[50];
    
   
    
    
    while (1) {
      
       
     _CP0_SET_COUNT(0);     //for blinking
                while (_CP0_GET_COUNT() < 2400000 / 2){

            setPin(wAdd,0x0A, 0x00);
                }
                while (_CP0_GET_COUNT() >= 2400000 / 2 && _CP0_GET_COUNT() < 4800000 / 2){

            setPin(wAdd,0x0A, 0xFF);

                }
         
       
     I2C_read_multiple(0b1101011, 0x20, output, 14); //reading of inputs, and also saving done here
     
    signed short temp = (int)((output[1]) << 8) | output[0];
    signed short xG = (int)((output[3]) << 8) | output[2];
    signed short yG = (int)((output[5]) << 8) | output[4];
    signed short zG = (int)((output[7]) << 8) | output[6];
         
    signed short xAcc = (int)((output[9]) << 8) | output[8];
    signed short yAcc = (int)((output[11]) << 8) | output[10];
    signed short zAcc = (int)((output[13]) << 8) | output[12];
     
     
    /*
    
    sprintf(message, "temp: %d", temp);             //this part used to print to screen the values necessary 
                printString(1,1,message);
    sprintf(message, "g: %d %d %d", xG, yG, zG); 
                printString(1,10,message);
    sprintf(message, "a: %d %d %d", xAcc, yAcc, zAcc); 
                printString(1,20,message);
                
                 ssd1306_update();
    
       */
    
    ssd1306_clear();
      bar_x (xAcc);
      bar_y (yAcc);
      
      ssd1306_update();
      
      
      
     
    
    
  
    }
}
       
         
       
     
        
        
        
  
    



void setPin (unsigned char address, unsigned char regist, unsigned char value) {
    i2c_master_start();
    i2c_master_send(address);
    i2c_master_send(regist);
    i2c_master_send(value);
    i2c_master_stop();
}

unsigned char readPin (unsigned char address, unsigned char regist){
    i2c_master_start();
    i2c_master_send(0b01000000);
    i2c_master_send(regist);
    i2c_master_restart();
    i2c_master_send(0b01000001);
    i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    return I2C1RCV;
}



void drawLetter (unsigned char x, unsigned char y, unsigned char letter) {
    int j, k;

 
    
    for (k = 0; k <=7; k++) {
    
    for (j = 0; j <=7; j++) {
   
        if (((ASCII[letter - 0x20][j])>> k ) & 1) {
            ssd1306_drawPixel ((x + j), (y + k), 1);
            
        } else 
            ssd1306_drawPixel ((x + j), (y + k), 0);
    
    }
    
    
    }
    
}


void printString (unsigned char x, unsigned char y, char *message) {
    
 int s = 0;
 
 
 while (message[s] != 0) {
     if (s <= 25) {
     drawLetter ((s*5) + x, 0 + y, message[s]);
     s++;
 }
     else if (s > 25 && s <= 50) {
     drawLetter (((s-25)*5) + x, 8 + y, message[s]);
     s++;
 }
     else if (s > 50 && s <= 75) {
     drawLetter (((s-50)*5) + x, 16 + y, message[s]);
     s++;
 }
     else if (s > 75 && s <= 100) {
     drawLetter (((s-75)*5) + x, 24 + y, message[s]);
     s++;
 }
     
    
}
 
 
}





void I2C_read_multiple(unsigned char address, unsigned char regist, unsigned char * data, int length) {
    int i = 0;
    
    i2c_master_start();
    i2c_master_send((address << 1) | 0);
    i2c_master_send(regist);
    i2c_master_restart();
    i2c_master_send((address << 1) | 1);
    
        for (i = 0; i <= length; i++){
        data[i]= i2c_master_recv();      
        if (i < length){
            i2c_master_ack(0);
        } else {
            i2c_master_ack(1);
        }
        }
    
    
   
    i2c_master_stop();    
           
    


    
    }
    
    
    
void bar_x (signed short x) {
    float bar;
    int i = 0;
    
    
    bar = (float)x/32767 * 32;
    
    if (bar >= 0){
    while (i <= bar){
        ssd1306_drawPixel (62 , 17 + i, 1);
        i ++;
      
    }
    } else if (bar < 0) {
        bar = -bar;
        while (i <= bar){
        ssd1306_drawPixel (62 , 17 - i, 1);
        i ++;
      
    }
        
    }
    
    
    
   
        
       
    
}

void bar_y (signed short y) {
    float bar;
    int i = 0;
    
    
    bar = (float)y/32767 * 128;
    
    if (bar >= 0){
    while (i <= bar){
        ssd1306_drawPixel (62 - i , 17, 1);
        i ++;
      
    }
    } else if (bar < 0) {
        bar = -bar;
        while (i <= bar){
        ssd1306_drawPixel (62 + i , 17, 1);
        i ++;
      
    }
        
    }
    
    
    
      
        
       
    
}
    
    
