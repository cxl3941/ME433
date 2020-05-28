#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h" 
#include "i2c_master_noint.h"
#include "font.h"
#include "ssd1306.h"
#include <math.h>
#include "ws2812b.h"
#include "adc.h"
#include "rtcc.h"


// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = ON // disable secondary oscillator
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

    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 1;
    
    
    i2c_master_setup();
    ssd1306_setup();
    ws2812b_setup();
    adc_setup();
    ctmu_setup();
    rtcc_setup(0x17394500,0x20052804);
    
   
   

    
    
    __builtin_enable_interrupts();
    
    
  
   
   
 
   
   int i = 0, temp;
   char message[100];
   char message1[100];
   char message2[10];
   char day[15];
  
   
   
  

   
    
    while (1) {
        
   if (temp != RTCCONbits.HALFSEC){
            temp = RTCCONbits.HALFSEC;
            i++;
        }
        
   rtccTime time = readRTCC (); 
   dayOfTheWeek(time.wk, &day);
   
   sprintf(message2, "%d  ", i);
   
   
   
   sprintf(message, "%d%d:%d%d:%d%d   ", time.hr10, time.hr01, time.min10, time.min01, time.sec10, time.sec01); 
  
   
   sprintf(message1, "%s, %d%d/%d%d/%d%d              ", day, time.mn10, time.mn01, time.dy10, time.dy01, time.yr10, time.yr01);
   printString(0, 20, message1);
   printString(0,0,message2);
   
    printString(1,10,message);
    
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
            
        } else {
            ssd1306_drawPixel ((x + j), (y + k), 0);
    
    }
    
    
    }
    
}
}

void printString (unsigned char x, unsigned char y, char *message) {
    
 int s = 0;
 
 
 while (message[s] != 0) {
     if (s <= 25) {
     drawLetter ((s*5) + x, 0 + y, message[s]);
 }
     else if (s > 25 && s <= 50) {
     drawLetter (((s-25)*5) + x, 8 + y, message[s]);
 }
     else if (s > 50 && s <= 75) {
     drawLetter (((s-50)*5) + x, 16 + y, message[s]);
 }
     else if (s > 75 && s <= 100) {
     drawLetter (((s-75)*5) + x, 24 + y, message[s]);
 }
     s++;
    
}
 
 
}