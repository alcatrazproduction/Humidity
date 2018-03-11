/*
* RTC test program
*/
#include "humidity.h"
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>

// Wire Librariy

#define BUFFER_LENGTH 32
#define pabort( a )		PRINT_ERROR( a )

	int i2c_handle					= 0 ;
    uint8_t rxBuffer[BUFFER_LENGTH];
    uint8_t rxBufferIndex			= 0;
    uint8_t rxBufferLength			= 0;

    uint8_t txBuffer[BUFFER_LENGTH];
    uint8_t txBufferIndex			= 0;
    uint8_t txBufferLength			= 0;

    uint8_t transmitting			= 0;
	bool	setsystime				= FALSE;


    void begin();
//    void setBusFreq(unsigned int);
    void beginTransmission(uint8_t);
    void beginTransmission(int);
    uint8_t endTransmission(void);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(int, int);
    int write(byte);
    size_t write(const uint8_t *, size_t);
    int available(void);
    int read(void);
    int peek(void);
    void flush(void);

void begin(void)
{
  rxBufferIndex 	= 0;
  rxBufferLength 	= 0;

  txBufferIndex 	= 0;
  txBufferLength 	= 0;

  if (!i2c_handle) {
     if ((i2c_handle = open("/dev/i2c-2", O_RDWR)) < 0)
        pabort("can't open device");
  }
}

uint8_t requestFrom(int address, int quantity)
{
  uint8_t read_out = 0;

  if(!i2c_handle)
    return 0;

  // clamp to buffer length
  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }

  // 7 or 10 bits address
  if (address <= 0x7F) {
     if (ioctl(i2c_handle, I2C_TENBIT, 0) < 0)
       pabort("Set I2C 7-bits addres flag fail");
  }else if(address <= 0x3FF){
     if (ioctl(i2c_handle, I2C_TENBIT, 1) < 0)
       pabort("Set I2C 10-bits addres flag fail");
  }

  if (ioctl(i2c_handle, I2C_SLAVE, address) < 0)
    pabort("Set I2C slave addres fail");

  // perform blocking read into buffer
  int ret = read(i2c_handle, rxBuffer, quantity);
  if (ret > 0)
     read_out = ret;
  else
     read_out = 0;

  // set rx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = read_out;

  return read_out;
}

void beginTransmission(int address)
{
  if(!i2c_handle)
    return;

  // indicate that we are transmitting
  transmitting = 1;

  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;

  // 7 or 10 bits address
  if (address <= 0x7F) {
     if (ioctl(i2c_handle, I2C_TENBIT, 0) < 0)
       pabort("Set I2C 7-bits addres flag fail");
  }else if(address <= 0x3FF){
     if (ioctl(i2c_handle, I2C_TENBIT, 1) < 0)
       pabort("Set I2C 10-bits addres flag fail");
  }

  if (ioctl(i2c_handle, I2C_SLAVE, address) < 0)
    pabort("Set I2C slave addres fail");
}

int i2cWrite(byte data)
{
  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(txBufferLength >= BUFFER_LENGTH)
	{
    //  setWriteError();
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer
    txBufferLength = txBufferIndex;
  }else{
  // in slave send mode
    // reply to master
  }
  return 1;
}

uint8_t endTransmission(void)
{
  if(!i2c_handle)
    return 0;

  // transmit buffer (blocking)
  int8_t ret = write(i2c_handle, txBuffer, txBufferLength);
  if (ret != txBufferLength) {
     printf("i2c transaction failed\r\n");
     ret = 4;
  }else
     ret = 0;

  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
  // indicate that we are done transmitting
  transmitting = 0;
  return ret;
}

int i2cRead(void)
{
  int value = -1;

  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}

#define  DS1307_I2C_ADDRESS  0x68 // This is the I2C address

int command = 0; // This is the command char, in ascii form, sent from the serial port
int zero = 0;
//long previousMillis = 0; // will store last time Temp was updated byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; byte test;
//Convert normal decimal numbers to binary coded decimal
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte test;

byte decToBcd(byte val)
{
byte 	a = (val/10)%10;
byte	b =	(val%10);

   return ( (a*16) + (b) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
byte	a = (val/16)%10;
byte	b =	(val%16)%10;
  return ( (a*10) + (b) );
}

void setDateDs1307()
{
  beginTransmission(DS1307_I2C_ADDRESS);
  i2cWrite(zero);
  i2cWrite(decToBcd(second));     // 0 to bit 7 starts the clock
  i2cWrite(decToBcd(minute));
  i2cWrite(decToBcd(hour));           // If you want 12 hour am/pm you need to set
                                  // bit 6 (also need to change readDateDs1307)
  i2cWrite(decToBcd(dayOfWeek));
  i2cWrite(decToBcd(dayOfMonth));
  i2cWrite(decToBcd(month));
  i2cWrite(decToBcd(year));
  endTransmission();

}

 //Gets the date and time from the ds1307 and prints result
void getDateDs1307()
{
    char str_cmd[1000];

    // Reset the register pointer
    beginTransmission(DS1307_I2C_ADDRESS);
    i2cWrite(zero);
    endTransmission();
    requestFrom(DS1307_I2C_ADDRESS, 7);
    // A few of these need masks because certain bits are control bits
    second     = bcdToDec(i2cRead() & 0x7f);
    minute     = bcdToDec(i2cRead());
    hour       = bcdToDec(i2cRead() & 0x3f);  // Need to change this if 12 hour am/pm
    dayOfWeek  = bcdToDec(i2cRead());
    dayOfMonth = bcdToDec(i2cRead());
    month      = bcdToDec(i2cRead());
    year       = bcdToDec(i2cRead());

    sprintf(str_cmd, "date -s  %d/%d/%d\n", month, dayOfMonth, year);
	
    if( setsystime )
	{
		if( system(str_cmd) )
			;
	}
	else
		puts( str_cmd );
	
    sprintf(str_cmd, "date -s  %d:%d:%d\n", hour, minute, second);
	
    if( setsystime )
	{
		if( system(str_cmd) )
			;
	}
	else
		puts( str_cmd );
	
}

int  main( int argc, char **argv)
{
PRINT_DEBUG( "%d, %s, %s \n",argc, argv[0], argc == 2 ? argv[1]:"")
	if( argc > 2 )
	{
		PRINT_ERROR("USAGE: %s [svr] : s = set rtc time \n v = version \n r = only read, not set system time \n", argv[0])
		exit(0);
	}
time_t	sec;
	sec = time( &sec );
struct	tm	*theTime = localtime( &sec  );

	if( theTime )
	{
		second		= theTime->tm_sec;
		minute		= theTime->tm_min;
		hour		= theTime->tm_hour;
		dayOfMonth	= theTime->tm_mday;
		dayOfWeek	= theTime->tm_wday;
		month		= theTime->tm_mon+1;
		year		= theTime->tm_year;
	}
	PRINT_ERROR("Current date is (MM/DD/YY): %d/%d/%d %d:%d:%d\n", month, dayOfMonth, year, hour, minute, second )
    begin();
PRINT_DEBUG( "Processing argument\n")
	if( argc == 1 )
	{
		PRINT_ERROR("Setting sys time from rtc\n")
		setsystime = TRUE;
		getDateDs1307();
	}
	else
	{
		if( argv[1][0] == '-' )
		{
			switch( argv[1][1] )
			{
				case 'v':
					PRINT_ERROR("%s VERSION 1.0 (c) Yves Huguenin\n", argv[0])
					break;
				case 's':
					PRINT_ERROR("Setting rtc time from sys\n")
					setDateDs1307();
					break;
				case 'r':
					setsystime = FALSE;
					getDateDs1307();
					break;
				default:
					PRINT_ERROR("USAGE: %s [svr] : s = set rtc time \n v = version \n r = only read, not set system time \n", argv[0])
					break;
			}
		}
		else
		{
			PRINT_ERROR("USAGE: %s [svr] : s = set rtc time \n v = version \n r = only read, not set system time \n", argv[0])
			exit(0);
		}
	}
	return( 0);
}
