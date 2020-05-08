#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h" 
#include "i2c_master_noint.h"
#include "font.h"
#include "ssd1306.h"
#include <math.h>
#include "ws2812b.h"
#include "adc.h"


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
    ws2812b_setup();
    adc_setup();
    ctmu_setup();
   
   

    
    
    __builtin_enable_interrupts();
    
    
  
   
   
 
   
   int i = 0;
   char message[100];
   char message1[50];
   
   
  
   int delay = 30;
   
    
    while (1) {
        
   int AnA = (ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay)) + (ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay) + ctmu_read (4, delay));
   int AnB = (ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay)) + (ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay) + ctmu_read (5, delay));
  
   float DeltaA = (12090 - AnA);
   float DeltaB = (12100 - AnB);
   float LeftPos = (DeltaA*100)/(DeltaA + DeltaB);
   float RightPos = ((1-DeltaB)*100)/(DeltaA + DeltaB);
   float Pos = (((LeftPos + RightPos))/200) + 0.5;
   
   
   
   sprintf(message, "A: %d     B: %d          Pos:%f     ", AnA, AnB, Pos); 
  
   
    printString(1,1,message);
    
    ssd1306_update();
            
         wsColor c[4];
        int b, a, d;
      
        float power = .1;
            
    
        //initial state of LED
        
        if (AnA < 3000){
        
        c[0] = HSBtoRGB(0,0,power);
        } else
        {
         c[0] = HSBtoRGB(0,0,0);   
        }
        
         if (AnB < 3000){
        
        c[1] = HSBtoRGB(0,0,power);
        } else
        {
         c[1] = HSBtoRGB(0,0,0);   
        }
        
        
        
        if (AnA < 3000 || AnB <3000) {
        
        c[2] = HSBtoRGB(0,0,(Pos*power));
        }
        else {
            
            c[2] = HSBtoRGB(0,0,0);
        }
        
        
        if (AnA < 3000 || AnB <3000) {
        
        c[3] = HSBtoRGB(Pos*180,.5,power);
        }
        else {
            
            c[3] = HSBtoRGB(0,0,0);
        }
        
        
       
       
        
        
        wsColor *pointer;
        pointer=&c;
        ws2812b_setColor(pointer,4);
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