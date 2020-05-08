#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h" 
#include "i2c_master_noint.h"
#include "font.h"
#include "ssd1306.h"
#include <math.h>
#include "ws2812b.h"


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
   
   

    
    
    __builtin_enable_interrupts();
    
    
  
   
   
 
   
   int i = 0;
    
    while (1) {
            
         wsColor c[4];
        int b, a, d;
        
        b = i + 120;
        a = i + 240;
        d = i + 70;
        
       if (i>360) {
            i = 0;
            
        }
        if (b>360) {
            b = b - 360;
            
        }
        if (a>360) {
            a = a - 360;
            
        }
        if (d>360) {
            d = d - 360;
            
        }
        
       
        
        
        
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()< 48000)
        {
        //initial state of LED
        c[0] = HSBtoRGB(i,1,0.05); 
        c[1] = HSBtoRGB(b,1,0.05);
        c[2] = HSBtoRGB(a,1,0.05);
        c[3] = HSBtoRGB(d,1,0.05);
        wsColor *pointer;
        pointer=&c;
        ws2812b_setColor(pointer,4);
        }
        
        
        
        
        i++;
        
        
        
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

