#include "derivative.h" /* include peripheral declarations */

#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

//Time definitions
#define nt15_msec	5500
#define nt40_usec	100

//LCD Control
#define nIns	0
#define nData	1

#define PortLCD    	GPIOC_PDOR
//Enable connected to portb_01
#define Enable_1	GPIOB_PDOR |= 0x01
#define Enable_0	GPIOB_PDOR &= 0xFE
#define RS_1   		GPIOB_PDOR |= 0x02
#define RS_0   		GPIOB_PDOR &= 0xFD

#define	Set_GPIOB_PDOR(x)	(GPIOB_PDOR |= (1 << (x-1)))


#define n_clock ((GPIOD_PDIR & 0x0F) == 0x00)
#define n_data	((GPIOD_PDIR & 0x0F) == 0x08)

//Declare Variables

int int_Temp;


int paquete[11];
int data[8];
int i;
int break_flag = 0;
char data_to_send;
int capital_letter = 0;
int xdir = 0x80;
int count = 0;
int counter = 0;
int mayus = 0;

//Cursor Blink off initialization
const unsigned char InitializeLCD[5] = {0x38, 0x38, 0x38, 0x0C, 0x01};
//--------------------------------------------------------------
//Declare Prototypes
/* Functions */
void cfgPorts(void);
void initLCD(void);
void delay(long time);
void sendCode(int Code, int Data);

void decode_data (void);

int main(void)
{	cfgPorts();	
	initLCD();

	for(;;)
	{	if(n_clock)
		{	int bit_weight;
			bit_weight = 1;
			
			for(i=0;i<11;i++)
			{	paquete[i] = n_data;
				delay(172); //Sampling Time
			}
			for(i=1;i<=9;i++)
			{	data[i-1] = paquete[i];
			}			
			for (i=0;i<8;i++)
			{	if(data[i] == 1)
				{	data_to_send = data_to_send | bit_weight;
					bit_weight = bit_weight * 2;
				}
				else
				{	bit_weight = bit_weight * 2;
				}
			}	
			
			//Print corresponding code			
			decode_data();
			if (data_to_send == 0xf0)
			{
				break_flag = 2;
			}
			
			if ( (data_to_send != 0xf0) && (break_flag == 0) )
			{	break_flag = 1;
				sendCode(nIns, xdir);
				
				sendCode(nData, data_to_send);
				
				xdir++;
			}
			
			if ((break_flag == 2) && (data_to_send != 0xf0))
			{	break_flag = 0;
			}
			
			//Reset all variables
			for(i=0;i<9;i++)
			{	data[i] = 0;
			}		
			data_to_send = 0;
		}
	}
	return 0;
}

void cfgPorts(void)
{
	//Turn on clock for portb
	SIM_SCGC5 = SIM_SCGC5_PORTB_MASK;
	//Turn on clock for portd
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;	
	////Turn on clock for portc
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	/* Set pins of PORTB as GPIO */
	PORTB_PCR0= PORT_PCR_MUX(1);
	PORTB_PCR1= PORT_PCR_MUX(1);
	PORTB_PCR2= PORT_PCR_MUX(1);
	PORTB_PCR3= PORT_PCR_MUX(1);
	PORTB_PCR4= PORT_PCR_MUX(1);
	PORTB_PCR5= PORT_PCR_MUX(1);
	PORTB_PCR6= PORT_PCR_MUX(1);
	PORTB_PCR7= PORT_PCR_MUX(1);
	
	/* Set pins of PORTC as GPIO */
	PORTC_PCR0= PORT_PCR_MUX(1);
	PORTC_PCR1= PORT_PCR_MUX(1);
	PORTC_PCR2= PORT_PCR_MUX(1);
	PORTC_PCR3= PORT_PCR_MUX(1);
	PORTC_PCR4= PORT_PCR_MUX(1);
	PORTC_PCR5= PORT_PCR_MUX(1);
	
	PORTC_PCR6= PORT_PCR_MUX(1);
	PORTC_PCR7= PORT_PCR_MUX(1);
	PORTC_PCR8= PORT_PCR_MUX(1);
	PORTC_PCR9= PORT_PCR_MUX(1);
	PORTC_PCR10= PORT_PCR_MUX(1);
	PORTC_PCR11= PORT_PCR_MUX(1);
	PORTC_PCR12= PORT_PCR_MUX(1);
	PORTC_PCR13= PORT_PCR_MUX(1);
		
	/* Set pins of PORTD as GPIO */
	PORTD_PCR0= PORT_PCR_MUX(1);
	PORTD_PCR1= PORT_PCR_MUX(1);
	PORTD_PCR2= PORT_PCR_MUX(1);
	PORTD_PCR3= PORT_PCR_MUX(1);
	
//Initialize Ports
	GPIOB_PDOR = 0x00;
	GPIOC_PDOR = 0x00;
	GPIOD_PDOR = 0x00;

//Configure PortBs
	GPIOB_PDDR = 0xFF;
	GPIOC_PDDR = 0xFFFF;
	GPIOD_PDDR = 0xFFF0;	
}

void initLCD(void)
{
	int i;
	delay(nt15_msec);
	
	/* Send initialization instructions */
	/* Loop for sending each character from the array */
	for(i=0;i<5;i++)
	{	sendCode(nIns, InitializeLCD[i]);	/* send initialization instructions */			
	}	
}

void sendCode(int Code, int Data)
{
	//Assign a value to pin RS
	/*HINT: When RS is 1, then the LCD receives a data
	when RS is 0, then the LCD receives an instruction */
	// Initialize RS and Enable with 0
	RS_0;
	Enable_0;
	//Assign the value we want to send to the LCD
	PortLCD = (Data << 6);	
	
	//We make the algorithm to establish if its an instruction we start with 0 on RS value, otherwise if its a data command we start with RS as 1;
	if (Code == nIns)
	{
		Enable_1;
		delay(nt40_usec);
		Enable_0;
		RS_0;
	}		
	else if(Code == nData)
	{
		RS_1;
		Enable_1;
		delay(nt40_usec);
		Enable_0;
		RS_0;
	}
}
void delay(long time)
{	while (time > 0)
	{
		time--;
	}	
}

void decode_data (void)
{	if (break_flag != 1)
	{	if(mayus == 1)
		{	switch(data_to_send)
			{	//capital letter 
				case 0x1C: 
					data_to_send = 'A';
					break;	
				case 0x32:
					data_to_send = 'B';
					break;
				case 0x21:
					data_to_send = 'C';
					break;
				case 0x23:
					data_to_send = 'D';
					break;
				case 0x24:
					data_to_send = 'E';
					break;
				case 0x2B:
					data_to_send = 'F';
					break;
				case 0x34:
					data_to_send = 'G';
					break;
				case 0x33:
					data_to_send = 'H';
					break;
				case 0x43:
					data_to_send = 'I';
					break;
				case 0x3B: 
					data_to_send = 'J';
					break;
				case 0x42:
					data_to_send = 'K';
					break;
				case 0x4B:
					data_to_send = 'L';
					break;
				case 0x3A:
					data_to_send = 'M';
					break;
				case 0x31:
					data_to_send = 'N';
					break;
				case 0x44:
					data_to_send = 'O';
					break;
				case 0x4D:
					data_to_send = 'P';
					break;
				case 0x15:
					data_to_send = 'Q';
					break;
				case 0x2D:
					data_to_send = 'R';
					break;
				case 0x1B:
					data_to_send = 'S';
					break;
				case 0x2C:
					data_to_send = 'T';
					break;
				case 0x3C:
					data_to_send = 'U';
					break;
				case 0x2A:
					data_to_send = 'V';
					break;
				case 0x1D:
					data_to_send = 'W';
					break;
				case 0x22:
					data_to_send = 'X';
					break;
				case 0x35:
					data_to_send = 'Y';
					break;
				case 0x1A:
					data_to_send = 'Z';
					break;
				//numbers
				case 0x45:
					data_to_send = '0';
					break;
				case 0x16:
					data_to_send = '1';
					break;
				case 0x1E:
					data_to_send = '2';
					break;
				case 0x26:
					data_to_send = '3';
					break;
				case 0x25:
					data_to_send = '4';
					break;
				case 0x2E:
					data_to_send = '5';
					break;
				case 0x36:
					data_to_send = '6';
					break;
				case 0x3D:
					data_to_send = '7';
					break;
				case 0x3E:
					data_to_send = '8';
					break;
				case 0x46:
					data_to_send = '9';
					break;
				//other keys
				case 0x29:
					data_to_send = ' ';
					break;	
				case 0x5A:
					data_to_send = ' ';
					xdir = 0xC0;
					break;
				case 0x66:
					data_to_send = ' ';				
					xdir--;
					break;
			}	
		}
		else
		{	
			switch(data_to_send)
			{	//lowercase letter 
				case 0x1C: 
					data_to_send = 'a';
					break;	
				case 0x32:
					data_to_send = 'b';
					break;
				case 0x21:
					data_to_send = 'c';
					break;
				case 0x23:
					data_to_send = 'd';
					break;
				case 0x24:
					data_to_send = 'e';
					break;
				case 0x2B:
					data_to_send = 'f';
					break;
				case 0x34:
					data_to_send = 'g';
					break;
				case 0x33:
					data_to_send = 'h';
					break;
				case 0x43:
					data_to_send = 'i';
					break;
				case 0x3B: 
					data_to_send = 'j';
					break;
				case 0x42:
					data_to_send = 'k';
					break;
				case 0x4B:
					data_to_send = 'l';
					break;
				case 0x3A:
					data_to_send = 'm';
					break;
				case 0x31:
					data_to_send = 'n';
					break;
				case 0x44:
					data_to_send = 'o';
					break;
				case 0x4D:
					data_to_send = 'p';
					break;
				case 0x15:
					data_to_send = 'q';
					break;
				case 0x2D:
					data_to_send = 'r';
					break;
				case 0x1B:
					data_to_send = 's';
					break;
				case 0x2C:
					data_to_send = 't';
					break;
				case 0x3C:
					data_to_send = 'u';
					break;
				case 0x2A:
					data_to_send = 'v';
					break;
				case 0x1D:
					data_to_send = 'w';
					break;
				case 0x22:
					data_to_send = 'x';
					break;
				case 0x35:
					data_to_send = 'y';
					break;
				case 0x1A:
					data_to_send = 'z';
					break;
				//numbers
				case 0x45:
					data_to_send = '0';
					break;
				case 0x16:
					data_to_send = '1';
					break;
				case 0x1E:
					data_to_send = '2';
					break;
				case 0x26:
					data_to_send = '3';
					break;
				case 0x25:
					data_to_send = '4';
					break;
				case 0x2E:
					data_to_send = '5';
					break;
				case 0x36:
					data_to_send = '6';
					break;
				case 0x3D:
					data_to_send = '7';
					break;
				case 0x3E:
					data_to_send = '8';
					break;
				case 0x46:
					data_to_send = '9';
					break;
				//other keys
				case 0x29:
					data_to_send = ' ';
					break;	
				case 0x5A:
					data_to_send = ' ';
					xdir = 0xC0;
					break;
				case 0x66:
					data_to_send = ' ';				
					xdir--;
					
				case 0x58:
					mayus = mayus^1;
					//data_to_send = 0xF0;
					break;
			}	
		}
	}
}




