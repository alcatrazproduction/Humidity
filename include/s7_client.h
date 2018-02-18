
/********************************************************************************************************
 * Define for profinet inetrface client
 * $Author	: Yves Huguenin
 * $Created	: 02.05.2014
 * $Version	: 1.0
 *
 * WARNING: Size are differents from MODBUS, MODBUS elements have 16bits, S7 have 8 bits !!!
 *******************************************************************************************************/
#ifndef __S7_CLIENT__
#define __S7_CLIENT__

#include "snap7.h"

/*******************************************************************************************************
 * HV variables
 * *****************************************************************************************************/
#define MAX_HV_CH				4
// INPUT

#define	HV_DIECLOSE_ADDR		0
#define HV_DIECLOSE_BIT			0

#define	HV_EJECTOUT_ADDR		0
#define HV_EJECTOUT_BIT			1

#define	HV_SWITCHOVER_ADDR		0
#define HV_SWITCHOVER_BIT		2

#define	HV_SETTING_ADDR			0
#define HV_SETTING_BIT			3

#define	HV_VACCUMLOCK_ADDR		0
#define HV_VACCUMLOCK_BIT		4

#define	HV_DIEOPEN_ADDR			0
#define HV_DIEOPEN_BIT			5

#define	HV_VACCUMSTART_ADDR		0
#define HV_VACCUMSTART_BIT		6

#define	HV_SPRAYER_ADDR			0
#define HV_SPRAYER_BIT			7

#define	HV_AIRDIRECT_ADDR		1
#define HV_AIRDIRECT_BIT		0

#define	HV_VACCUMDIRECT_ADDR	1
#define HV_VACCUMDIRECT_BIT		1

// OUTPUT

#define	HV_GREEN_ADDR			0
#define HV_GREEN_BIT			2

#define	HV_RED_ADDR				0
#define HV_RED_BIT				3

#define	HV_GREENEXT_ADDR		0
#define HV_GREENEXT_BIT			4

#define	HV_REDEXT_ADDR			0
#define HV_REDEXT_BIT			5

// DB...

class s7_HighVac : private TS7Client				// will be deriveted from TS7Client
{
private:
	char			*IP;
	int 			theRack;
	int 			theSlot;
	bool			connected;
//	Input state
	bool			dieOpenState;
	bool			vacuStartState;
	bool			dieCloseState;

	bool			useProfinet;
	pthread_t		thread;
	pthread_attr_t	attr;

	int 			vacuStart_byte;
	int 			vacuStart_bit;

	int 			dieOpen_byte;
	int 			dieOpen_bit;

	int 			green_byte;
	int 			green_bit;
	
	int 			red_byte;
	int 			red_bit;
	
	int 			greenext_byte;
	int 			greenext_bit;

	int 			redext_byte;
	int 			redext_bit;
	
	int 			hv_T1_DB;
	int 			hv_T1_byte;
	int 			hv_T1_Size;
	
	int 			hv_T2_DB;
	int 			hv_T2_byte;
	int 			hv_T2_Size;

	int 			hv_T3_DB;
	int 			hv_T3_byte;
	int 			hv_T3_Size;

	int 			hv_ProductionName_DB;
	int 			hv_ProductionName_byte;
	int 			hv_ProductionName_Size;
	
	int 			hv_outtol;											// # 128
	int 			hv_outtol_byte;										// # 130
	int 			hv_outtol_bit;										// # 132
	
	int 			hv_clean;											// # 134
	int 			hv_clean_byte;										// # 136
	int 			hv_clean_bit;										// # 138

	int 			hv_PartNum_DB;
	int 			hv_PartNum_byte;
	int 			hv_PartNum_Size;

	int 			hv_ShotNum_DB;
	int 			hv_ShotNum_byte;
	int 			hv_ShotNum_Size;

	int 			hv_Vaccum_DB[MAX_HV_CH];
	int 			hv_Vaccum_byte[ MAX_HV_CH ];

//	address in CPU
public:
				s7_HighVac( const char *RemAddress, int Rack, int Slot );
				
	bool		openConnection( void );
	bool		closeConnection( void );
	
	s7_HighVac	*setAddress( const char *RemAddress );
	s7_HighVac	*setRack( int Rack );
	s7_HighVac	*setSlot( int Slot );
	
	s7_HighVac	*setT1Db( int db );
	s7_HighVac	*setT1Byte( int db );
	s7_HighVac	*setT1DbSize( int db );
	
	s7_HighVac	*setT2Db( int db );
	s7_HighVac	*setT2Byte( int db );
	s7_HighVac	*setT2DbSize( int db );

	s7_HighVac	*setT3Db( int db );
	s7_HighVac	*setT3Byte( int db );
	s7_HighVac	*setT3DbSize( int db );

	s7_HighVac	*setProdNameDb( int db );
	s7_HighVac	*setProdNameByte( int db );
	s7_HighVac	*setProdNameDbSize( int db );
	
	s7_HighVac	*setProdNumberDb( int db );
	s7_HighVac	*setProdNumberByte( int db );
	s7_HighVac	*setProdNumberDbSize( int db );

	s7_HighVac	*setShotNumberDb( int db );
	s7_HighVac	*setShotNumberByte( int db );
	s7_HighVac	*setShotNumberDbSize( int db );

	s7_HighVac	*setInVacuStartIn( int db );
	s7_HighVac	*setInVacuStartBit( int db );

	s7_HighVac	*setInDieOpenIn( int db );
	s7_HighVac	*setInDieOpenBit( int db );

	s7_HighVac	*setHvVaccumDb( int channel, int db );
	s7_HighVac	*setHvVaccumByte( int channel, int db );
	
	s7_HighVac	*setGreenOut( int db );
	s7_HighVac	*setGreenBit( int db );

	s7_HighVac	*setRedOut( int db );
	s7_HighVac	*setRedBit( int db );

	s7_HighVac	*setGreenExtOut( int db );
	s7_HighVac	*setGreenExtBit( int db );

	s7_HighVac	*setRedExtOut( int db );
	s7_HighVac	*setRedExtBit( int db );

	s7_HighVac	*setOuttol( int db );
	s7_HighVac	*setOuttolByte( int db );
	s7_HighVac	*setOuttolBit( int db );

	s7_HighVac	*setClean( int db );
	s7_HighVac	*setCleanByte( int db );
	s7_HighVac	*setCleanBit( int db );

	int16_t		getInputByte( int address );			// if negative, ERROR
	
	int16_t		getOutputByte( int address );			// if negative, ERROR
	bool		setOutputByte( int address, uint8_t buffer );
	
	int16_t		getDbByte( short db, short address );			// if negative, ERROR
	bool		setDbByte( short db, short address, uint8_t buffer );
	
	bool		setMBit( short MAddr, short Mbit, bool state );
	bool		getMBit( short MAddr, short Mbit, bool &state );

	bool		getDieCloseState( void );
	bool		getEjectorOutState( void );
	bool		getSwitchOverState( void );
	bool		getPressSettingState( void );
	bool		getVaccumLockState( void );
	bool		getDieOpenState( void );
	bool		getVaccumStartState( void );
	bool		getSprayerState( void );
	bool		getAirDirectState( void );
	bool		getVaccumDirectState( void );

	s7_HighVac	*setGreenLamp( bool state );
	s7_HighVac	*setGreenExtLamp( bool state );
	s7_HighVac	*setRedLamp( bool state );
	s7_HighVac	*setRedExtLamp( bool state );
	s7_HighVac	*setOutOfTolerance( bool state );
	s7_HighVac	*setNeed2Clean( bool state );
			
	long		getT1( void );
	long		getT2( void );
	long		getT3( void );
	long		getProdNumber( void );
	long		getShotNumber( void );

	bool		getPartName( char *buffer, int len = 0);

	bool		getHVBlock( int channel, int size, void *buffer );
	bool		getHostAlive( void );
};

bool checkHost( const char *RemAddress );

#endif