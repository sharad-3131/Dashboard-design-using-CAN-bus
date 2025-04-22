#include<LPC21xx.h>
#include "delay.h"
#include "LCD_defines.h"
#include "LCD.h"
#include "adc.h"
#include "adc_defines.h"
#include "types.h"
#include "interrupt.h"
#include "can.h"
#include "can_defines.h"
#include "ds18b20.h"
#include "rtc.h"
#include "rtc_defines.h"
#include "defines.h"
//#include "uart.h"

char cgram_lut[] = {0x0e,0x11,0x11,0x11,0x11,0x11,0x11,0x1f};
char right_arrow[8]  = {0x04, 0x06, 0x1F, 0x06, 0x04, 0x00, 0x00, 0x00};  // Custom Left Arrow
char left_arrow[8] = {0x04, 0x0C, 0x1F, 0x0C, 0x04, 0x00, 0x00, 0x00};  // Custom Right Arrow
// Add these custom characters for different bar levels (0-5)
char bar_0[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // Empty
char bar_1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};  // 1/5 full
char bar_2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};  // 2/5 full
char bar_3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};  // 3/5 full
char bar_4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};  // 4/5 full
char bar_5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};  // Full
extern u8 left_blink, right_blink;  // Declare flags from interrupt.c

s32 hour,min,sec,date,month,year,day;
int temp;
u8 tp,tpd;
u32 flag1=0, flag2=0;
CANF rxF, txF;
int rc_cnd(void)
{
	return (READBIT(C1GSR,RBS_BIT))?1:0;
}


//chatgpt new
void LCD_Create_CustomChars() {
    int i;
    // Left arrow (character 0)
    cmdLCD(0x40);
    for (i = 0; i < 8; i++) charLCD(left_arrow[i]);
    
    // Right arrow (character 1)
    cmdLCD(0x48);
    for (i = 0; i < 8; i++) charLCD(right_arrow[i]);
    
    // Fuel bars (characters 2-6)
    cmdLCD(0x50); for (i = 0; i < 8; i++) charLCD(bar_0[i]);  // Char 2
    cmdLCD(0x58); for (i = 0; i < 8; i++) charLCD(bar_1[i]);  // Char 3
    cmdLCD(0x60); for (i = 0; i < 8; i++) charLCD(bar_2[i]);  // Char 4
    cmdLCD(0x68); for (i = 0; i < 8; i++) charLCD(bar_3[i]);  // Char 5
    cmdLCD(0x70); for (i = 0; i < 8; i++) charLCD(bar_4[i]);  // Char 6
    cmdLCD(0x78); for (i = 0; i < 8; i++) charLCD(bar_5[i]);  // Char 7
    
    cmdLCD(0x80); // Return to DDRAM
    // Display initial arrows
    cmdLCD(0xDE); charLCD(0); // Left arrow
    cmdLCD(0xDD); charLCD(1); // Right arrow
}



main(){

	InitLCD();
	cmdLCD(DSP_ON_CUR_OFF);
	LCD_Create_CustomChars();
	Init_CAN1();
	RTC_Init();
	Enable_EINT0();
    Enable_EINT1();
  //  uart0_init(9600);  // Initialize UART0 at 9600 baud

	rxF.Data1=0;

	SetRTCTime(1,14,0);
	SetRTCDate(02,04,2025);
	SetRTCDay(3);

		cmdLCD(0x0C);

		while(1){

		//for reading, display the date and time
		GetRTCTime(&hour,&min,&sec);
		GetRTCDate(&date,&month,&year);
		GetRTCDay(&day);
		cmdLCD(0x80+6);
		DisplayRTCTime(hour,min,sec);	
		cmdLCD(0xC0);
		DisplayRTCDate(date,month,year);
		DisplayRTCDay(day);

		//for reading and display the temprature
		cmdLCD(0x94);
 		strLCD("TEMP=");
		temp=ReadTemp(); 
		if(temp!=(-20))
		{
			tp  = temp>>4;	 
			tpd=temp&0x08?0x35:0x30;
			s32LCD(tp);
			charLCD('.');
			charLCD(tpd);
			strLCD(" C");
		}
		else
		{
			strLCD("ND");
			strLCD("       ");
		}

	
		if(flag1==1){
		  	flag1=0;
		}
		if(flag2==1){
		  	flag2=0;
		}	   

		//For reading the fuel percentage
		/*if(rc_cnd()) {
			CAN1_Rx(&rxF);
			if(rxF.ID==1){
				cmdLCD(0xD4);
				strLCD("FUEL=");
				u32LCD(rxF.Data1);
				delay_s(1);
			}
		}
*/
        //     NEW PART             For reading the fuel percentage
		/*cmdLCD(0xD4);        // Move to position on LCD
	    	strLCD("FUEL=");     // Print the label
		if(rc_cnd()) {
			CAN1_Rx(&rxF);
			if(rxF.ID == 1) {
				//cmdLCD(0xD4);        // Move to position on LCD
				//strLCD("FUEL=");     // Print the label
				cmdLCD(0xD4 + 5);    // Move cursor after "FUEL="
				strLCD("   ");       // Clear previous 3 digits (if any)

				cmdLCD(0xD4 + 5);    // Move cursor back again
				u32LCD(rxF.Data1);   // Display the actual fuel value

				delay_s(1);          // Wait for visibility
			}
			else
			{
				strLCD("NA");
				strLCD("       ");
			}
		}
		else
		{
			strLCD("ND");
			strLCD("        ");
		}
*/
// Fuel percentage display
    if(rc_cnd()) {
        CAN1_Rx(&rxF);
        if(rxF.ID == 1) {
		Display5BarFuel(rxF.Data1);
		cmdLCD(0xD4);
		strLCD("FUEL: ");
		cmdLCD(0xD4 +16);
		u32LCD(rxF.Data1);
    	charLCD('%');
    	strLCD("  "); 
            
            delay_ms(500);
        }
        else {
            cmdLCD(0xD4);
            strLCD("FUEL: ND            ");
        }
    }
    else {
        //setcursor(4, 0);
		cmdLCD(0xD4);
        strLCD("FUEL: ND            ");
    }
        
        
            if (left_blink) {
                // Blink left arrow
                cmdLCD(0x94 + 16);
                charLCD(' ');
                delay_ms(500);
                cmdLCD(0x94 + 16);
                charLCD(0);
                //delay_ms(500);
            } 
            else {
                // Keep left arrow ON if not blinking
                cmdLCD(0x94 + 16);
                charLCD(0);
            }

            if (right_blink) {
                // Blink right arrow
                cmdLCD(0x94 + 17);
                charLCD(' ');
                delay_ms(500);
                cmdLCD(0x94 + 17);
                charLCD(1);
                //delay_ms(500);
            } 
            else {
                // Keep right arrow ON if not blinking
                cmdLCD(0x94 + 17);
                charLCD(1);
            }
	}

}
