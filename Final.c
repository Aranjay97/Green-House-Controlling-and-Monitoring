#include <p18f4550.h>
#include "glcd.h"
#include "isr.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/
unsigned long datlow,dathigh,data,data1,data2;
char character_set[]="AT+CSCS=\"GSM\"\r";
char sms_format[]="AT+CMGF=1\r";
char sms_write[]="AT+CMGS=\"+919921960511\"\r";
char sms1[]="TEMPERATURE IS CROSSING 40°C ; MOTOR IS TURNED ON\r";
char sms2[]="TEMPERATURE IS BELOW 40°C ; MOTOR IS TURNED OFF\r";
void delay(unsigned int itime);
void anatodig(void);
void serialTX(unsigned char x);
void transmit(const char *out);
void sendsms(unsigned int q);
#define MOTOR PORTDbits.RD7
#define BUZZER PORTDbits.RD6

/*-------------------------------------------------------------------------------------------------------------------------------------*/
void delay(unsigned int itime)
{
int i,j;
for(i=0;i<itime;i++)
for(j=0;j<710;j++);
}
/*-------------------------------------------------------------------------------------------------------------------------------------*/
//ADC FUNCTION
void anatodig(void)
{
ADCON0bits.GO=1;
while(ADCON0bits.DONE==1);
datlow=ADRESL;
dathigh=ADRESH;
dathigh=dathigh<<8;
data=datlow|dathigh;
data1=data*100;
data2=data1/1023;
delay(10);
}
/*-------------------------------------------------------------------------------------------------------------------------------------*/
//GSM FUNCTIONS
void serialTX(unsigned char x)
{
while (PIR1bits.TXIF==0);
TXREG = x;
}

void transmit(const char *out) // to send strings to tx_data method
{
do
{
serialTX(*out);
out++;
}while(*out);
}

void sendsms(unsigned int q)
{
TXSTAbits.TXEN=1;
RCSTAbits.SPEN=1;
serialTX(0x1A);
serialTX(0x0D);
transmit(sms_format);	
delay(5);
transmit(character_set); 
delay(5);
transmit(sms_write);	
delay(5);
if(q > 407)
{
transmit(sms1);  
delay(5);
}
else if(q<407)
{
transmit(sms2);  
delay(5);
}
serialTX(0x1A);
}


/*-------------------------------------------------------------------------------------------------------------------------------------*/
//MAIN FUNCTION
void main()
{
//INITIALIZATION
int i,j,ii,gbm,p,q,conv,conv1;
TRISAbits.RA0=1;
TRISCbits.RC6=0;
TRISD = 0x00;
TRISB = 0x00;
TRISE = 0x00;
ADCON0bits.CHS0=0;
ADCON0=0x01;
ADCON1=0x0E;
ADCON2=0xCE;
TXSTA = 0x20;      
SPBRG = 12;
GLCDinit();
anatodig();

while(1)
{	
loadimg(&image[0], 1024);      
for(i=0;i<49;i++)
{
anatodig();
if(i==``0)
{
sendsms(data);
}
CS1 = 0;
CS2 = 1;
gbm=(55-data2);
if(gbm<8)
{
p=abc[gbm];
}
else
{
q=gbm%8;
p=abc[q];
}

GLCDcmd(0x4E + i );
if(0<=gbm && gbm<=7)
{GLCDcmd(0xB8 );}
else if(8<=gbm && gbm<=15)
{GLCDcmd(0xB9 );}
else if(16<=gbm && gbm<=23)
{GLCDcmd(0xBA );}
else if(24<=gbm && gbm<=31)
{GLCDcmd(0xBB );}
else if(32<=gbm && gbm<=39)
{GLCDcmd(0xBC );}
else if(40<=gbm && gbm<=47)
{GLCDcmd(0xBD );}
else if(48<=gbm && gbm<=55)
{GLCDcmd(0xBE );}
GLCDdata(p);
delay(10);
CS1 = 1;
CS2 = 0;
conv=data2%10;
conv1=(data2/10)%10;

for(ii=0;ii<5;ii++)
{
GLCDcmd(0xBA);
GLCDcmd(0x57+ii);
GLCDdata(numbers[(conv1*5)+ii]);
GLCDcmd(0x5D+ii);
GLCDdata(numbers[(conv*5)+ii]);
}
if(data > 407)
{
MOTOR=1;
BUZZER=1;

for(j=0;j<19;j++)
{
GLCDcmd(0xBE);
GLCDcmd(0x57+j);
GLCDdata(ON[j]); 
}
}

else
{
MOTOR=0;
BUZZER=0;
for(j=0;j<19;j++)
{
GLCDcmd(0xBE);
GLCDcmd(0x57+j);
GLCDdata(OFF[j]); 
}
}

}//for



}//while
}//main

/*-------------------------------------------------------------------------------------------------------------------------------------*/

