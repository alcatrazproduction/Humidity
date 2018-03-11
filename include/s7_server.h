
/********************************************************************************************************
 * Define for profinet inetrface server
 * $Author	: Yves Huguenin
 * $Created	: 02.05.2014
 * $Version	: 1.0
 *
 * WARNING: Size are differents from MODBUS, MODBUS elements have 16bits, S7 have 8 bits !!!
 *
 * 30.10.2014		Added limit setting on all channel, flags and one flags for calibration over
 *******************************************************************************************************/
#ifndef __S7_SERVER__
#define __S7_SERVER__

#include "snap7.h"

#define ARDUINO_SOFT_VERSION		"$VER: 24.11.14.0"

// Predefinition of some classes
class C_LibDb;
class DB_AxeData;
class DB_AxeX;
class C_Connection;

// Slave codding:

#define	GLOBAL_SLAVE				1

#define	CHANNEL_PROCESS_BASE		100
#define	CHANNEL_TEMPERATURE_BASE	200
#define	CHANNEL_VACCUM_BASE			300
#define	CHANNEL_HVVACCUM_BASE		400
#define	CHANNEL_HUMIDITY_BASE		500
#define	CHANNEL_LIMIT_BASE			600

#define	MAX_CHANNEL_SLAVE		8

#define	VOLUME_SLAVE_BASE		150
#define	MAX_VOLUME_SLAVE		8

// Channel variable DB ...
#define	CHANNEL_MAX				0									//	DB1xx.0
#define CHANNEL_MIN				1									//	DB1xx.2
#define CHANNEL_LCAL			2									//	DB1xx.4
#define	CHANNEL_FLAGS			3									//	DB1xx.6
#define	CH_FLAG_USEA			((unsigned short)(1<<0))			//	DB1xx.6.0
#define CH_FLAG_USEB			((unsigned short)(1<<1))			//	DB1xx.6.1
#define CH_FLAG_CONNECTED		((unsigned short)(1<<2))			//	DB1xx.6.2
#define CH_FLAG_LIMIT_STATUS	((unsigned short)(1<<3))			//	DB1xx.6.3
#define CH_FLAG_LIMIT_ERROR		((unsigned short)(1<<4))			//	DB1xx.6.4
#define CH_FLAG_CAL_ERROR		((unsigned short)(1<<5))			//	DB1xx.6.5
#define	CHANNEL_SAMPLES			4									//	DB1xx.8
#define CHANNEL_CAL				5									//	DB1xx.10
#define CHANNEL_CAL_RES			6									//	DB1xx.12
#define CHANNEL_LIMIT_AVG		7									//	DB1xx.14
#define CHANNEL_LIMIT_VALUE		8									//	DB1xx.16
#define CHANNEL_LIMIT_START		9									//	DB1xx.18
#define CHANNEL_LIMIT_STOP		10									//	DB1xx.20
#define CHANNEL_PHY_ADDR		11									//	DB1xx.22
#define CHANNEL_LOG_ADDR		12									//	DB1xx.24

#define VARIABLES_PAD			13

// Datatype SIZE

#define	SIZE_CHANNEL_DATA		(999 + 1)
#define	TYPEDEF_ARRAY			10
#define	SIZE_VARCHAR			( 31 + 1)
#define	TYPEDEF_STRING			1
#define	SIZE_ASCII				80
#define	TYPEDEF_INT				2
#define	SIZE_INT				2
#define	TYPEDEF_DOUBLE			3
#define	SIZE_DOUBLE				4
#define	TYPEDEF_FLOAT			4
#define	SIZE_FLOAT				4

#define ARDUINO_GLOBAL			5
#define ARDUINO_MESURES			6

// Block Base address
#define DATA_CHANNEL_BASE		0
#define VARIABLES_BASE			0
// Channel define
#define DATA_CHANNEL_ARRAY		DATA_CHANNEL_BASE
#define CHANNEL_BLOCK_NAME		"channel"
// Global Variables defines

#define GLOBAL_BLOCK_NAME		"global"

#define PLAGE_AXE				4


#define VERSION_BASE			( 32  )
#define READWRITE_BASE			( 112 )
#define HVMAXLIMIT				( 126 )
#define HVOUTTOL_BASE			( 128 )
#define HVOUTTOLBYTE_BASE		( 130 )
#define HVOUTTOLBIT_BASE		( 132 )
#define HVCLEAN_BASE			( 134 )
#define HVCLEANBYTE_BASE		( 136 )
#define HVCLEANBIT_BASE			( 138 )
#define CALIBRATIONLIMIT		( 140 )
#define FLAGSBITS				( 142 )
#define SIGNALBITS				( 143 )
#define CHANNEL_2_READ			( 144 )
#define CHANNEL_HUMIDITY		( 146 )
#define CHANNEL_TEMP			( 148 )
#define CHANNEL_VACCUM			( 150 )
#define USEAUTOFACTOR			( 152 )
#define AUTOFACTOR				( 154 )
#define DISPLAYZERO				( 156 )
#define DISPLAYFACTOR			( 158 )
#define CURRENTINJECTION_BASE	( 160 )
#define PARTNAME_BASE			( 164 )
#define PRODNUM_BASE			( 244 )
#define T1_BASE					( 246 )
#define T2_BASE					( 248 )
#define T3_BASE					( 250 )
#define CMD_BASE				( 252 )
#define NUMCHAN_BASE			( 332 )
#define SAMPLERATE_BASE			( 334 )
#define LOADCURVE_BASE			( 336 )
#define REQCURVE_BASE			( 338 )
#define REQUESTINJECTION_BASE	( 340 )
#define HVIP_BASE				( 344 )
#define HVRACK_BASE				( 348 )
#define HVSLOT_BASE				( 350 )
#define HVVACUSTART_BASE		( 352 )
#define HVVACUSTARTBIT_BASE		( 354 )
#define HVDIEOPEN_BASE			( 356 )
#define HVDIEOPENBIT_BASE		( 358 )
#define HVT1_BASE				( 360 )
#define HVT1BYTE_BASE			( 362 )
#define HVT2_BASE				( 364 )
#define HVT2BYTE_BASE			( 366 )
#define HVT3_BASE				( 368 )
#define HVT3BYTE_BASE			( 370 )
#define HVPRODNUM_BASE			( 372 )
#define HVPRODNUMBYTE_BASE		( 374 )
#define HVPARTNUM_BASE			( 376 )
#define HVPARTNUMBYTE_BASE		( 378 )
#define HVPARTNAME_BASE			( 380 )
#define HVPARTNAMEBYTE_BASE		( 382 )
#define HVSHOTNUM_BASE			( 384 )
#define HVSHOTNUMBYTE_BASE		( 386 )

class DB_Global
{
private:
	unsigned short				_Coordination;										// # 000
	unsigned short				_date_heure[6];										// # 002
	unsigned short				_tacheAPI[4];										// # 014
	unsigned short				_Enregistrement[5];									// # 022
	unsigned char				_Version[2];										// # 032
	unsigned char				_Version_data[ SIZE_ASCII - sizeof( _Version ) ];	// # 034 version string of the pcduino firmware
	unsigned short				Reserved_data1;										// # 112
	unsigned short				Reserved_data2;										// # 114
	unsigned short				Reserved_data3;										// # 116
	unsigned short				Reserved_data4;										// # 118
	unsigned short				Reserved_data5;										// # 120
	unsigned short				Reserved_data6;										// # 122
	unsigned short				Reserved_data7;										// # 124
	unsigned short				_hv_max_limit;										// # 126
	unsigned short				_hv_outtol;											// # 128
	unsigned short				_hv_outtol_byte;									// # 130
	unsigned short				_hv_outtol_bit;										// # 132
	unsigned short				_hv_clean;											// # 134
	unsigned short				_hv_clean_byte;										// # 136
	unsigned short				_hv_clean_bit;										// # 138
	unsigned short				_CalibrationLimit;									// # 140
	unsigned char				_FlagsBits;											// # 142 ( saved in __FlagsBits !! )
	unsigned char				_SignalBits;										// # 143
	unsigned short				_Channel2Read;										// # 144
	unsigned short				_ChannelHumidity;									// # 146
	unsigned short				_ChannelTemperature;								// # 148
	unsigned short				_ChannelVaccum;										// # 150
	unsigned short				_UseAutoFactor;										// # 152
	unsigned short				_AutoFactor;										// # 154
	unsigned short				_DisplayZero;										// # 156
	unsigned short				_DisplayFactor;										// # 158
	unsigned long				_CurrentInjection;									// # 160
	unsigned char				_PartName[2];
	unsigned char				_PartName_data[SIZE_ASCII- sizeof( _PartName )];	// # 164
	unsigned short				_ProdNum;											// # 244
	unsigned short				_T1;												// # 246
	unsigned short				_T2;												// # 248
	unsigned short				_T3;												// # 250
	unsigned char				_Cmd[2];
	unsigned char				_Cmd_data[SIZE_ASCII- sizeof( _Cmd )];				// # 252
	unsigned short				_NumChan;											// # 332
	unsigned short				_SampleRate;										// # 334
	unsigned short				_LoadCurve;											// # 336
	unsigned short				_ReqCurve;											// # 338
	unsigned long				_RequestInjection;									// # 340
	unsigned long				_hv_ip;												// # 344
	unsigned short				_hv_rack;											// # 348
	unsigned short				_hv_slot;											// # 350

	unsigned short				_hv_vacustart;										// # 352
	unsigned short				_hv_vacustart_bit;									// # 354

	unsigned short				_hv_dieopen;										// # 356
	unsigned short				_hv_dieopen_bit;									// # 358

	unsigned short				_hv_t1;												// # 360
	unsigned short				_hv_t1_byte;										// # 362


	unsigned short				_hv_t2;												// # 364
	unsigned short				_hv_t2_byte;										// # 366

	unsigned short				_hv_t3;												// # 368
	unsigned short				_hv_t3_byte;										// # 370

	unsigned short				_hv_prodnum;										// # 372
	unsigned short				_hv_prodnum_byte;									// # 374

	unsigned short				_hv_partnum;										// # 376
	unsigned short				_hv_partnum_byte;									// # 378


	unsigned short				_hv_partname;										// # 380
	unsigned short				_hv_partname_byte;									// # 382

	unsigned short				_hv_shotnum;										// # 384
	unsigned short				_hv_shotnum_byte;									// # 386

	unsigned char				_CPUID[2];											// # 388
	unsigned char				_CPUID_data[40];									// # 390
	
	unsigned short				endOfData[10];										// # 430 only to get theEnd
																					// padding tu secure the rest !!!
// Globals variables, to be instructure, not any more in file

	TS7Server					*Server;
	s7_HighVac					*_HighVac;
	DB_AxeX						*axeX;
	C_LibDb						*database;
	DB_AxeData					**axeArray;
	c_List						channelList;
#	ifdef MCP_CAN
	MCP_CAN 					*CAN;
#	else
	CANClass 					*CAN;
#	endif
	unsigned long				hv_injection;
	byte 						cnt;
	bool						dieOpen[2];
	bool						vacuStart[2];
	bool						interruptMCP;
	bool						dataReady;
	bool						codeMode;

	unsigned short				_LimitAverage[MAX_CHANNEL_SLAVE];
	unsigned short				_LimitStart	[MAX_CHANNEL_SLAVE];
	unsigned short				_LimitStop	[MAX_CHANNEL_SLAVE];
	
	uint8_t						sig_dieopen;
	uint8_t						sig__dieopen;
	uint8_t						sig_vacustart;
	uint8_t						sig__vacustart;
	uint8_t						sig_dataready;
	uint8_t						MStorage[2048];
	uint8_t						_MStorage[2048];
	uint8_t						SPI_mode;
	uint8_t						SPI_bits;
	uint16_t					SPI_delay;
	uint32_t					interruptMask;
	uint32_t					SPI_speed;

	unsigned long				cpuid[4];
	unsigned char				__FlagsBits;										// save of _FlagsBits, to check changing state
public:

	DB_Global(void);
static void S7API EventCallBack(void *usrPtr, PSrvEvent PEvent, int Size);

	void			*BaseAddress( void ) 								// return the base of data block
						{ return &_Coordination; };								// only for RegisterArea from class TS7Server
	int				 BlockSize( void )									// return the size of the block
						{ return (int)( (char *)&endOfData - (char *)&_Coordination ); };

	unsigned char	*Version( void );
	
	unsigned short	 CalibrationLimit( void )		{ return ((unsigned short)	be16toh( _CalibrationLimit ) );		};
	unsigned char	 FlagsBits( void )				{ return ((unsigned char)		   (  _FlagsBits ) );			};
	unsigned char	 SignalBits( void )				{ return ((unsigned char)		   (  _SignalBits ) );			};
	unsigned short	 Channel2Read( void )			{ return ((unsigned short)	be16toh( _Channel2Read ) );			};
	unsigned short	 ChannelHumidity( void )		{ return ((unsigned short)	be16toh( _ChannelHumidity ) );		};
	unsigned short	 ChannelTemperature( void )		{ return ((unsigned short)	be16toh( _ChannelTemperature ) );	};
	unsigned short	 ChannelVaccum( void )			{ return ((unsigned short)	be16toh( _ChannelVaccum ) );		};
	unsigned short	 UseAutoFactor( void )			{ return ((unsigned short)	be16toh( _UseAutoFactor ) );		};
	unsigned short	 AutoFactor( void )				{ return ((unsigned short)	be16toh( _AutoFactor ) );			};
	unsigned short	 DisplayZero( void )			{ return ((unsigned short)	be16toh( _DisplayZero ) );			};
	unsigned short	 DisplayFactor( void )			{ return ((unsigned short)	be16toh( _DisplayFactor ) );		};
	unsigned long	 CurrentInjection( void )		{ return ((unsigned long)	be32toh( _CurrentInjection ) ); 	};
	unsigned char	*PartName( void );
	unsigned short	 ProdNum( void )				{ return ((unsigned short)	be16toh( _ProdNum ) );		 		};
	unsigned short	 T1( void )						{ return ((unsigned short)	be16toh( _T1 ) );			 		};
	unsigned short	 T2( void )						{ return ((unsigned short)	be16toh( _T2 ) );	 				};
	unsigned short	 T3( void )						{ return ((unsigned short)	be16toh( _T3 ) ); 					};
	unsigned char	*Cmd( void );
	unsigned short	 NumChan( void )				{ return ((unsigned short)	be16toh( _NumChan ) );		 		};
	unsigned short	 SampleRate( void )				{ return ((unsigned short)	be16toh( _SampleRate ) );	 		};
	unsigned short	 LoadCurve( void )				{ return ((unsigned short)	be16toh( _LoadCurve ) );		 	};
	unsigned short	 ReqCurve( void )				{ return ((unsigned short)	be16toh( _ReqCurve ) );		 		};
	unsigned long	 RequestInjection( void )		{ return ((unsigned long)	be32toh( _RequestInjection ) ); 	};
	
	unsigned long	 HighVacIP( void )				{ return ((unsigned long)	be32toh( _hv_ip ) ); 				};
	unsigned short	 hv_rack( void )				{ return ((unsigned long)	be16toh( _hv_rack ) ); 				};
	unsigned short	 hv_slot( void )				{ return ((unsigned long)	be16toh( _hv_slot ) ); 				};

	unsigned short	 hv_vacustart( void )			{ return ((unsigned long)	be16toh( _hv_vacustart ) ); 		};
	unsigned short	 hv_vacustart_bit( void )		{ return ((unsigned long)	be16toh( _hv_vacustart_bit ) ); 	};

	unsigned short	 hv_dieopen( void )				{ return ((unsigned long)	be16toh( _hv_dieopen ) ); 			};
	unsigned short	 hv_dieopen_bit( void )			{ return ((unsigned long)	be16toh( _hv_dieopen_bit ) ); 		};

	unsigned short	 hv_t1( void )					{ return ((unsigned long)	be16toh( _hv_t1 ) ); 				};
	unsigned short	 hv_t1_byte( void )				{ return ((unsigned long)	be16toh( _hv_t1_byte ) ); 			};


	unsigned short	 hv_t2( void )					{ return ((unsigned long)	be16toh( _hv_t2 ) ); 				};
	unsigned short	 hv_t2_byte( void )				{ return ((unsigned long)	be16toh( _hv_t2_byte ) ); 			};

	unsigned short	 hv_t3( void )					{ return ((unsigned long)	be16toh( _hv_t3 ) ); 				};
	unsigned short	 hv_t3_byte( void )				{ return ((unsigned long)	be16toh( _hv_t3_byte ) ); 			};

	unsigned short	 hv_prodnum( void )				{ return ((unsigned long)	be16toh( _hv_prodnum ) ); 			};
	unsigned short	 hv_prodnum_byte( void )		{ return ((unsigned long)	be16toh( _hv_prodnum_byte ) ); 		};

	unsigned short	 hv_partnum( void )				{ return ((unsigned long)	be16toh( _hv_partnum ) ); 			};
	unsigned short	 hv_partnum_byte( void )		{ return ((unsigned long)	be16toh( _hv_partnum_byte ) ); 		};


	unsigned short	 hv_partname( void )			{ return ((unsigned long)	be16toh( _hv_partname ) ); 			};
	unsigned short	 hv_partname_byte( void )		{ return ((unsigned long)	be16toh( _hv_partname_byte ) ); 	};

	unsigned short	 hv_shotnum( void )				{ return ((unsigned long)	be16toh( _hv_shotnum ) ); 			};
	unsigned short	 hv_shotnumbyte_byte( void )	{ return ((unsigned long)	be16toh( _hv_shotnum_byte ) );	 	};

	unsigned short	 hv_max_limit( void )			{ return ((unsigned long)	be16toh( _hv_max_limit ) );	 	};

	unsigned short	 hv_outtol(  )					{ return ((unsigned long)	be16toh( _hv_outtol ) ); 			};
	unsigned short	 hv_outtol_byte(  )				{ return ((unsigned long)	be16toh( _hv_outtol_byte ) ); 		};
	unsigned short	 hv_outtol_bit(  )				{ return ((unsigned long)	be16toh( _hv_outtol_bit ) ); 		};
	unsigned short	 hv_clean(  )					{ return ((unsigned long)	be16toh( _hv_clean ) ); 			};
	unsigned short	 hv_clean_byte(  )				{ return ((unsigned long)	be16toh( _hv_clean_byte ) ); 		};
	unsigned short	 hv_clean_bit(  )				{ return ((unsigned long)	be16toh( _hv_clean_bit ) ); 		};

	uint8_t			 SPIMode( void )				{ return SPI_mode; 												};
	uint8_t			 SPIBits( void )				{ return SPI_bits; 												};
	uint32_t		 SPISpeed( void )				{ return SPI_speed; 											};
	uint16_t		 SPIDelay( void )				{ return SPI_delay;												};

	bool			 getDieOpen0( void )			{ return dieOpen[0];											};
	bool			 getDieOpen1( void )			{ return dieOpen[1];											};
	bool			 getVacuStart( void )			{ return vacuStart[0];											};
	bool			 get_VacuStart( void )			{ return vacuStart[1];											};
	bool			 getInterruptMCP( void )		{ return interruptMCP;											};
	bool			 getDataReady( void )			{ return dataReady;												};
	uint32_t		 getInterruptMask( void )		{ return interruptMask;											};
	TS7Server		*S7Server(void)					{ return Server;												};
	s7_HighVac		*HighVac(void)					{ return _HighVac;												};
	DB_AxeX			*AxeX(void)						{ return NULL;													};
	DB_AxeData		*AxeData( unsigned short i)		{ return i < ( MAX_CHANNEL_SLAVE) ? \
															axeArray[i] : NULL ;									};
	
	C_LibDb			*DatabaseLibrary( void )		{ return database;												};
	bool			 AxeArrayInited( void )			{ return axeArray != NULL;										};
	bool			 isStandAlone( void )			{ return codeMode;												};
	c_List			*ChannelList( void )			{ return &channelList; 											};
	byte			 Cnt( void )					{ return cnt; 													};
#	ifdef MCP_CAN
	MCP_CAN 		
#	else
	CANClass 		
#	endif
					*Can( void )					{ return CAN; 													};
	uint8_t	 		 SIG_DieOpen( void )			{ return sig_dieopen; 											};
	uint8_t	 		 SIG__DieOpen( void )			{ return sig__dieopen; 											};
	uint8_t	 		 SIG_Vacustart( void )			{ return sig_vacustart; 										};
	uint8_t	 		 SIG__Vacustart( void )			{ return sig__vacustart; 										};
	uint8_t	 		 SIG_DataReady( void )			{ return sig_dataready; 										};

	unsigned long	 HV_Injection( void )			{ return hv_injection; 											};

	unsigned short	 LimitAverage(unsigned short n)	{ return n < MAX_CHANNEL_SLAVE ? _LimitAverage[n] : 0;			};
	unsigned short	 LimitStart(unsigned short n)	{ return n < MAX_CHANNEL_SLAVE ? _LimitStart[n]   : 0;			};
	unsigned short	 LimitStop(unsigned short n)	{ return n < MAX_CHANNEL_SLAVE ? _LimitStop[n] 	 : 0;			};

	
	DB_Global	 	*CalibrationLimit( unsigned short p )		{ _CalibrationLimit 	= htobe16( p ); return this;};
	DB_Global	 	*hv_max_limit( unsigned short p )			{ _hv_max_limit		 	= htobe16( p ); return this;};
	DB_Global	 	*FlagsBits( unsigned char p )				{ _FlagsBits 			= 		 ( p ); return this;};
	DB_Global	 	*SignalBits( unsigned char p )				{ _SignalBits 			= 		 ( p ); return this;};
	DB_Global	 	*Channel2Read( unsigned short p )			{ _Channel2Read 		= htobe16( p ); return this;};
	DB_Global	 	*ChannelHumidity( unsigned short p )		{ _ChannelHumidity 		= htobe16( p ); return this;};
	DB_Global	 	*ChannelTemperature( unsigned short p )		{ _ChannelTemperature	= htobe16( p ); return this;};
	DB_Global	 	*ChannelVaccum( unsigned short p )			{ _ChannelVaccum 		= htobe16( p ); return this;};
	DB_Global	 	*UseAutoFactor( unsigned short p )			{ _UseAutoFactor 		= htobe16( p ); return this;};
	DB_Global	 	*AutoFactor( unsigned short p )				{ _AutoFactor 			= htobe16( p ); return this;};
	DB_Global	 	*DisplayZero( unsigned short p )			{ _DisplayZero 			= htobe16( p ); return this;};
	DB_Global	 	*DisplayFactor( unsigned short p )			{ _DisplayFactor 		= htobe16( p ); return this;};
	DB_Global	 	*CurrentInjection( unsigned long p ) 		{ _CurrentInjection 	= htobe32( p ); return this;};
	DB_Global	 	*PartName( unsigned char * p );
	DB_Global	 	*ProdNum( unsigned short p )				{ _ProdNum 				= htobe16( p ); return this;};
	DB_Global	 	*T1( unsigned short p )						{ _T1 					= htobe16( p ); return this;};
	DB_Global	 	*T2( unsigned short p )						{ _T2 					= htobe16( p ); return this;};
	DB_Global	 	*T3( unsigned short p )						{ _T3 					= htobe16( p ); return this;};
	DB_Global	 	*Cmd( unsigned char * p );
	DB_Global	 	*NumChan( unsigned short p )				{ _NumChan 				= htobe16( p ); return this;};
	DB_Global	 	*SampleRate( unsigned short p )				{ _SampleRate 			= htobe16( p ); return this;};
	DB_Global	 	*LoadCurve( unsigned short p )				{ _LoadCurve 			= htobe16( p ); return this;};
	DB_Global	 	*ReqCurve( unsigned short p )				{ _ReqCurve 			= htobe16( p ); return this;};
	DB_Global	 	*RequestInjection( unsigned long p ) 		{ _RequestInjection 	= htobe32( p );	return this;};
	DB_Global	 	*HighVacIP( unsigned long p ) 				{ _hv_ip 				= htobe32( p );	return this;};
	DB_Global	 	*HighVacIP( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
																{ _hv_ip 				= htobe32( 	(((unsigned long) a ) << 24 ) ||
																									(((unsigned long) b ) << 16 ) ||
																									(((unsigned long) c ) << 8  ) ||
																									(((unsigned long) d ) << 0  ) );
																  return this;};


	DB_Global	 	*SPIMode( uint8_t p )						{ SPI_mode 		= p; 				return this;};
	DB_Global	 	*SPIBits( uint8_t p )						{ SPI_bits 		= p; 				return this;};
	DB_Global	 	*SPISpeed( uint32_t p )						{ SPI_speed 	= p;				return this;};
	DB_Global	 	*SPIDelay( uint16_t p )						{ SPI_delay 	= p;				return this;};

	DB_Global	 	*setDieOpen0( void )						{ dieOpen[0] 	= TRUE;				return this;};
	DB_Global	 	*setDieOpen1( void )						{ dieOpen[1] 	= TRUE;				return this;};
	DB_Global	 	*setVacuStart( void )						{ vacuStart[0] 	= TRUE;				return this;};
	DB_Global	 	*set_VacuStart( void )						{ vacuStart[1] 	= TRUE;				return this;};
	DB_Global	 	*setInterruptMCP( void )					{ interruptMCP 	= TRUE;				return this;};
	DB_Global	 	*setDataReady( void )						{ dataReady 	= TRUE;				return this;};
	DB_Global	 	*clearDieOpen0( void )						{ dieOpen[0] 	= FALSE;			return this;};
	DB_Global	 	*clearDieOpen1( void )						{ dieOpen[1] 	= FALSE;			return this;};
	DB_Global	 	*clearVacuStart( void )						{ vacuStart[0] 	= FALSE;			return this;};
	DB_Global	 	*clear_VacuStart( void )					{ vacuStart[1] 	= FALSE;			return this;};
	DB_Global	 	*clearInterruptMCP( void )					{ interruptMCP 	= FALSE;			return this;};
	DB_Global	 	*clearDataReady( void )						{ dataReady 	= FALSE;			return this;};
	DB_Global	 	*InterruptMask( uint32_t p)					{ interruptMask = p;				return this;};
	DB_Global	 	*S7Server( TS7Server *p)					{ Server = p;						return this;};
	DB_Global	 	*HighVac( s7_HighVac *p)					{ _HighVac = p;						return this;};
	DB_Global	 	*AxeX( DB_AxeX *p)							{ axeX = NULL;						return this;};
	DB_Global	 	*AxeData( unsigned short i, DB_AxeData *p) \
																{ if (i < ( MAX_CHANNEL_SLAVE ) )  \
																  axeArray[i] = p ; return this;	};
	DB_Global	 	*setStandAlone( void )						{ codeMode 		= TRUE;				return this;};
	DB_Global	 	*clearStandAlone( void )					{ codeMode	 	= FALSE;			return this;};
	DB_Global	 	*Cnt( byte b )								{ cnt 			= b;				return this;};

	DB_Global	 	*SIG_DieOpen( uint8_t p )					{ sig_dieopen 	= p; 				return this;};
	DB_Global	 	*SIG__DieOpen( uint8_t p )					{ sig__dieopen 	= p; 				return this;};
	DB_Global	 	*SIG_Vacustart( uint8_t p )					{ sig_vacustart = p; 				return this;};
	DB_Global	 	*SIG__Vacustart( uint8_t p )				{ sig__vacustart = p; 				return this;};
	DB_Global	 	*SIG_DataReady( uint8_t p )					{ sig_dataready = p; 				return this;};

	DB_Global	 	*HV_Injection( unsigned long p )			{ hv_injection	= p; 				return this;};

	DB_Global	 	*LimitAverage(unsigned short n, unsigned short v)
																{ if( n < MAX_CHANNEL_SLAVE ) _LimitAverage[n] = v;	return this;};
	DB_Global	 	*LimitStart(unsigned short n, unsigned short v)
																{ if( n < MAX_CHANNEL_SLAVE ) _LimitStart[n]   = v;	return this;};
	DB_Global	 	*LimitStop(unsigned short n, unsigned short v)
																{ if( n < MAX_CHANNEL_SLAVE ) _LimitStop[n]    = v;	return this;};

// ++++++ Routine to activate the S7 server
public:
	int 			initS7(void );
	int 			startS7(void );
	int 			deleteS7(void );
	int 			clearS7Event( void );

};

class DB_AxeData
{
protected:
	class DB_AxeValue
	{
	protected:
		unsigned short	data[];
	public:
		inline unsigned short	 _MaxValue( void )								{ return this == NULL ? 32765: be16toh( data[ CHANNEL_MAX ] );							};
		inline unsigned short	 _MinValue( void )								{ return this == NULL ? 0    : be16toh( data[ CHANNEL_MIN ] );							};
		inline unsigned short	 _LoadCalibration( void )						{ return this == NULL ? 0    : be16toh( data[ CHANNEL_LCAL ] );							};
		inline unsigned short	 _Calibration( void )							{ return this == NULL ? 0    : be16toh( data[ CHANNEL_CAL ] );							};
		inline unsigned short	 _CalibrationResult( void )						{ return this == NULL ? 0    : be16toh( data[ CHANNEL_CAL_RES ] );						};
		inline unsigned short	 _ChannelFlags( void )							{ return this == NULL ? 0    : be16toh( data[ CHANNEL_FLAGS ] );						};
		inline unsigned short	 _SamplesCount( void )							{ return this == NULL ? 0    : be16toh( data[ CHANNEL_SAMPLES ] );						};
		inline unsigned short	 _LimitAverage( void )							{ return this == NULL ? 0    : be16toh( data[ CHANNEL_LIMIT_AVG ] );					};
		inline unsigned short	 _LimitValue( void )							{ return this == NULL ? 0    : be16toh( data[ CHANNEL_LIMIT_VALUE ] );					};
		inline unsigned short	 _LimitStart( void )							{ return this == NULL ? 0    : be16toh( data[ CHANNEL_LIMIT_START ] );					};
		inline unsigned short	 _LimitStop( void )								{ return this == NULL ? 0    : be16toh( data[ CHANNEL_LIMIT_STOP ] );					};

		inline unsigned short	 _SampleAt( unsigned short index, unsigned short numSample = 0 )
																				{ return this == NULL ? 0xffff : index < numSample ? be16toh( data[ index + VARIABLES_PAD ] ) : 0xfffe; }


		inline DB_AxeValue		*_MaxValue( unsigned short max )				{ if ( this ) data[ CHANNEL_MAX ] 			= htobe16( max ); 			return this;};
		inline DB_AxeValue		*_MinValue( unsigned short min )				{ if ( this ) data[ CHANNEL_MIN ] 			= htobe16( min ); 			return this;};
		inline DB_AxeValue		*_LoadCalibration( unsigned short cal)			{ if ( this ) data[ CHANNEL_LCAL ]			= htobe16( cal & 0x0FFF ); 	return this;};
		inline DB_AxeValue		*_Calibration( unsigned short cal )				{ if ( this ) data[ CHANNEL_CAL ] 			= htobe16( cal & 0x00FF ); 	return this;};
		inline DB_AxeValue		*_CalibrationResult( unsigned short cal )		{ if ( this ) data[ CHANNEL_CAL_RES ]		= htobe16( cal & 0xFFFF ); 	return this;};
		inline DB_AxeValue		*_ChannelFlags( unsigned short flags )			{ if ( this ) data[ CHANNEL_FLAGS ]	 		= htobe16( flags ); 		return this;};
		inline DB_AxeValue		*_SamplesCount( unsigned short count )			{ if ( this ) data[ CHANNEL_SAMPLES ] 		= htobe16( count ); 		return this;};
		inline DB_AxeValue		*_LimitAverage( unsigned short count )			{ if ( this ) data[ CHANNEL_LIMIT_AVG ] 	= htobe16( count ); 		return this;};
		inline DB_AxeValue		*_LimitValue( unsigned short count )			{ if ( this ) data[ CHANNEL_LIMIT_VALUE ] 	= htobe16( count ); 		return this;};
		inline DB_AxeValue		*_LimitStart( unsigned short count )			{ if ( this ) data[ CHANNEL_LIMIT_START ] 	= htobe16( count ); 		return this;};
		inline DB_AxeValue		*_LimitStop( unsigned short count )				{ if ( this ) data[ CHANNEL_LIMIT_STOP ] 	= htobe16( count ); 		return this;};
		
		inline DB_AxeValue		*_SampleAt( unsigned short index, unsigned short spl, unsigned short numSample )
																				{ if (( this ) && ( index < numSample )) data[ index + VARIABLES_PAD ] = htobe16( spl );return this;};
		
	};
protected:
	unsigned short				numSample;
	unsigned short				ID;							// 99 is X axis
	unsigned short				realID;						// this is real ID for channel
	unsigned long				serial;						// serial number of channel
	

	DB_Global					*GLOBALSETSTART;
	DB_AxeValue					*HumidityProcessed;
	DB_AxeValue					*Humidity;
	DB_AxeValue					*Temperature;
	DB_AxeValue					*Vaccum;
	DB_AxeValue					*HV_Vaccum;
	unsigned short				*LimitLine;
	
public:
					 DB_AxeData( unsigned short howMany, unsigned short channelID, DB_Global *glob );
					 DB_AxeData(){numSample = ID =realID =0; serial = -1; Humidity = HumidityProcessed = Temperature = Vaccum = HV_Vaccum = NULL;};
// TODO: create the destructor
	bool			 NumSample( unsigned short );
	unsigned short	 NumSample( void )									{ return numSample; };

	unsigned short	 ChannelID( void )									{ return ID; };
	unsigned short	 ChannelRealID( void )								{ return realID; };
	unsigned short	 ChannelSerialNumber( void )						{ return serial; };

// Processed Humidty values, only the class may write something.....
public:
	
#define		___VARIABLE___					HumidityProcessed
#define		___FUNCVOID___(b)				Process##b( void ) 			{ return ___VARIABLE___->_ ## b ( ) ;}
#define		___FUNCTION___(a,b,c)			Process##a( b c ) 			{ return ___VARIABLE___->_ ## a (c, numSample );}
#define		___PROCEDURE1___(a,b,c)			Process##a( b c ) 			{ ___VARIABLE___->_ ## a ( c );return this;}
#define		___PROCEDURE2___(a,b,c,d,e)		Process##a( b c, d e ) 		{ ___VARIABLE___->_ ## a ( c, e, numSample );return this;}

	DB_AxeData		*ProcessHumidity( unsigned short plage );
	
	unsigned short	 ___FUNCVOID___( MaxValue )					;
	unsigned short	 ___FUNCVOID___( MinValue )					;//		{ return ___VARIABLE___ MinValue();								};
	unsigned short	 ___FUNCVOID___( LoadCalibration )			;//			{ return ___VARIABLE___ LoadCalibration();						};
	unsigned short	 ___FUNCVOID___( Calibration )				;//			{ return ___VARIABLE___ Calibration();							};
	unsigned short	 ___FUNCVOID___( CalibrationResult )		;//			{ return ___VARIABLE___ CalibrationResult();					};
	unsigned short	 ___FUNCVOID___( ChannelFlags )				;//		{ return ___VARIABLE___ ChannelFlags();							};
	unsigned short	 ___FUNCVOID___( SamplesCount )				;//		{ return ___VARIABLE___ SamplesCount();							};
	unsigned short	 ___FUNCVOID___( LimitAverage )				;//	
	unsigned short	 ___FUNCVOID___( LimitValue )				;//
	unsigned short	 ___FUNCVOID___( LimitStop )				;//
	unsigned short	 ___FUNCVOID___( LimitStart )				;//

	bool			 ProcessChannelUseA( void )							{ return ProcessChannelFlags() & CH_FLAG_USEA;			};
	bool			 ProcessChannelUseB( void )							{ return ProcessChannelFlags() & CH_FLAG_USEB;			};
	bool			 ProcessChannelConnected( void  )					{ return ProcessChannelFlags() & CH_FLAG_CONNECTED;		};
	bool			 ProcessChannelLimitActive( void  )					{ return ProcessChannelFlags() & CH_FLAG_LIMIT_STATUS;	};
	bool			 ProcessChannelLimitError( void  )					{ return ProcessChannelFlags() & CH_FLAG_LIMIT_ERROR;	};
	
	unsigned short	 ___FUNCTION___( SampleAt, unsigned short, index);//						{ return ___VARIABLE___->_SampleAt( index, numSample); 			};
	
protected:
  
	DB_AxeData		*___PROCEDURE1___( MaxValue, unsigned short, max )	;//			{ ___VARIABLE___ MaxValue( max ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( MinValue, unsigned short, min )	;//			{ ___VARIABLE___ MinValue( min ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( LoadCalibration, unsigned short, cal);//		{ ___VARIABLE___ LoadCalibration( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( Calibration, unsigned short, cal );//			{ ___VARIABLE___ Calibration( cal ); 				return this;};
	DB_AxeData		*___PROCEDURE1___( CalibrationResult, unsigned short, cal );//		{ ___VARIABLE___ CalibrationResult( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( ChannelFlags, unsigned short, flags );//		{ ___VARIABLE___ ChannelFlags( flags ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( SamplesCount, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitAverage, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitValue, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitStart, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitStop, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};

	DB_AxeData		*ProcessChannelUseA( bool flags )					{
																			return ProcessChannelFlags( ( ProcessChannelFlags() & (~ CH_FLAG_USEA ) ) |
																					( flags ? CH_FLAG_USEA : 0 )
																				);
																		};
	DB_AxeData		*ProcessChannelUseB( bool flags )					{
																			return ProcessChannelFlags( ( ProcessChannelFlags() & (~ CH_FLAG_USEB ) ) |
																					( flags ? CH_FLAG_USEB : 0 )
																				);
																		};
	DB_AxeData		*ProcessChannelConnected( bool flags )				{
																			return ProcessChannelFlags( ( ProcessChannelFlags() & (~ CH_FLAG_CONNECTED ) ) |
																					( flags ? CH_FLAG_CONNECTED : 0 )
																				);
																		};
	DB_AxeData		*ProcessChannelLimitActive( bool flags )				{
																			return ProcessChannelFlags( ( ProcessChannelFlags() & (~ CH_FLAG_LIMIT_STATUS ) ) |
																					( flags ? CH_FLAG_LIMIT_STATUS : 0 )
																				);
																		};
	DB_AxeData		*ProcessChannelLimitError( bool flags )				{
																			return ProcessChannelFlags( ( ProcessChannelFlags() & (~ CH_FLAG_LIMIT_ERROR ) ) |
																					( flags ? CH_FLAG_LIMIT_ERROR : 0 )
																				);
																		};
																		
	DB_AxeData		*___PROCEDURE2___( SampleAt, unsigned short, index, unsigned short, spl );
																		//{ ___VARIABLE___->SampleAt( index ,spl, numSample );	return this;};
#undef ___VARIABLE___
#undef ___FUNCVOID___
#undef ___FUNCTION___
#undef ___PROCEDURE1___
#undef ___PROCEDURE2___
// ****************************************************** Readed Humidity Values, must be processed...*************************************************************************
#define		___VARIABLE___					Humidity
#define		___FUNCVOID___(b)				Humidity##b( void ) 		{ return ___VARIABLE___->_ ## b ( ) ;}
#define		___FUNCTION___(a,b,c)			Humidity##a( b c ) 			{ return ___VARIABLE___->_ ## a (c, numSample);}
#define		___PROCEDURE1___(a,b,c)			Humidity##a( b c ) 			{ ___VARIABLE___->_ ## a ( c );return this;}
#define		___PROCEDURE2___(a,b,c,d,e)		Humidity##a( b c, d e ) 	{ ___VARIABLE___->_ ## a ( c, e, numSample );return this;}
public:

	unsigned short	 ___FUNCVOID___( MaxValue )					;
	unsigned short	 ___FUNCVOID___( MinValue )					;//		{ return ___VARIABLE___ MinValue();								};
	unsigned short	 ___FUNCVOID___( LoadCalibration )			;//			{ return ___VARIABLE___ LoadCalibration();						};
	unsigned short	 ___FUNCVOID___( Calibration )				;//			{ return ___VARIABLE___ Calibration();							};
	unsigned short	 ___FUNCVOID___( CalibrationResult )		;//			{ return ___VARIABLE___ CalibrationResult();					};
	unsigned short	 ___FUNCVOID___( ChannelFlags )				;//		{ return ___VARIABLE___ ChannelFlags();							};
	unsigned short	 ___FUNCVOID___( SamplesCount )				;//		{ return ___VARIABLE___ SamplesCount();							};
	unsigned short	 ___FUNCVOID___( LimitAverage )				;//
	unsigned short	 ___FUNCVOID___( LimitValue )				;//
	unsigned short	 ___FUNCVOID___( LimitStop )				;//
	unsigned short	 ___FUNCVOID___( LimitStart )				;//

	bool			 HumidityChannelUseA( void )						{ return HumidityChannelFlags() & CH_FLAG_USEA;													};
	bool			 HumidityChannelUseB( void )						{ return HumidityChannelFlags() & CH_FLAG_USEB;													};
	bool			 HumidityChannelConnected( void )					{ return HumidityChannelFlags() & CH_FLAG_CONNECTED;											};
	bool			 HumidityChannelLimitError( void )					{ return HumidityChannelFlags() & CH_FLAG_LIMIT_ERROR;											};
	bool			 HumidityChannelLimitActive( void )					{ return HumidityChannelFlags() & CH_FLAG_LIMIT_STATUS;											};
	bool			 HumidityChannelCalibrationError( void )			{ return HumidityChannelFlags() & CH_FLAG_CAL_ERROR;											};
	
	unsigned short	 ___FUNCTION___( SampleAt, unsigned short, index);//						{ return ___VARIABLE___->_SampleAt( index, numSample); 			};

	DB_AxeData		*___PROCEDURE1___( MaxValue, unsigned short, max )	;//			{ ___VARIABLE___ MaxValue( max ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( MinValue, unsigned short, min )	;//			{ ___VARIABLE___ MinValue( min ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( LoadCalibration, unsigned short, cal);//		{ ___VARIABLE___ LoadCalibration( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( Calibration, unsigned short, cal );//			{ ___VARIABLE___ Calibration( cal ); 				return this;};
	DB_AxeData		*___PROCEDURE1___( CalibrationResult, unsigned short, cal );//		{ ___VARIABLE___ CalibrationResult( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( ChannelFlags, unsigned short, flags );//		{ ___VARIABLE___ ChannelFlags( flags ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( SamplesCount, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitAverage, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitValue, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitStart, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( LimitStop, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};

	DB_AxeData		*HumidityChannelUseA( bool flags )					{
																			return HumidityChannelFlags( ( HumidityChannelFlags() & (~ CH_FLAG_USEA ) ) |
																					( flags ? CH_FLAG_USEA : 0 )
																				);
																		};
	DB_AxeData		*HumidityChannelUseB( bool flags )					{
																			return HumidityChannelFlags( ( HumidityChannelFlags() & (~ CH_FLAG_USEB ) ) |
																					( flags ? CH_FLAG_USEB : 0 )
																				);
																		};
	DB_AxeData		*HumidityChannelConnected( bool flags )				{
																			return HumidityChannelFlags( ( HumidityChannelFlags() & (~ CH_FLAG_CONNECTED ) ) |
																					( flags ? CH_FLAG_CONNECTED : 0 )
																				);
																		};
	DB_AxeData		*HumidityChannelLimitError( bool flags )				{
																			return HumidityChannelFlags( ( HumidityChannelFlags() & (~ CH_FLAG_LIMIT_ERROR ) ) |
																					( flags ? CH_FLAG_LIMIT_ERROR : 0 )
																				);
																		};
	DB_AxeData		*HumidityChannelLimitActive( bool flags )				{
																			return HumidityChannelFlags( ( HumidityChannelFlags() & (~ CH_FLAG_LIMIT_STATUS ) ) |
																					( flags ? CH_FLAG_LIMIT_STATUS : 0 )
																				);
																		};
	DB_AxeData		*HumidityChannelCalibrationError( bool flags )				{
																			return HumidityChannelFlags( ( HumidityChannelFlags() & (~ CH_FLAG_CAL_ERROR ) ) |
																					( flags ? CH_FLAG_CAL_ERROR : 0 )
																				);
																		};

	DB_AxeData		*___PROCEDURE2___( SampleAt, unsigned short, index, unsigned short, spl );
//																		{ if (( Humidity ) && ( index < numSample )) Humidity[ index + VARIABLES_PAD ] = htobe16( spl );return this;};
																	
#undef ___VARIABLE___
#undef ___FUNCVOID___
#undef ___FUNCTION___
#undef ___PROCEDURE1___
#undef ___PROCEDURE2___
// ****************************************************** temperatue *****************************************************************************
#define		___VARIABLE___					Temperature
#define		___FUNCVOID___(b)				Temperature##b( void ) 		{ return ___VARIABLE___->_ ## b ( ) ;}
#define		___FUNCTION___(a,b,c)			Temperature##a( b c ) 		{ return ___VARIABLE___->_ ## a (c, numSample);}
#define		___PROCEDURE1___(a,b,c)			Temperature##a( b c ) 		{ ___VARIABLE___->_ ## a ( c );return this;}
#define		___PROCEDURE2___(a,b,c,d,e)		Temperature##a( b c, d e ) 	{ ___VARIABLE___->_ ## a ( c, e, numSample );return this;}
																	
	unsigned short	 ___FUNCVOID___( MaxValue )					;
	unsigned short	 ___FUNCVOID___( MinValue )					;//		{ return ___VARIABLE___ MinValue();								};
	unsigned short	 ___FUNCVOID___( LoadCalibration )			;//			{ return ___VARIABLE___ LoadCalibration();						};
	unsigned short	 ___FUNCVOID___( Calibration )				;//			{ return ___VARIABLE___ Calibration();							};
	unsigned short	 ___FUNCVOID___( CalibrationResult )		;//			{ return ___VARIABLE___ CalibrationResult();					};
	unsigned short	 ___FUNCVOID___( ChannelFlags )				;//		{ return ___VARIABLE___ ChannelFlags();							};
	unsigned short	 ___FUNCVOID___( SamplesCount )				;//		{ return ___VARIABLE___ SamplesCount();							};

	unsigned short	 ___FUNCTION___( SampleAt, unsigned short, index);//						{ return ___VARIABLE___->_SampleAt( index, numSample); 			};

	DB_AxeData		*___PROCEDURE1___( MaxValue, unsigned short, max )	;//			{ ___VARIABLE___ MaxValue( max ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( MinValue, unsigned short, min )	;//			{ ___VARIABLE___ MinValue( min ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( LoadCalibration, unsigned short, cal);//		{ ___VARIABLE___ LoadCalibration( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( Calibration, unsigned short, cal );//			{ ___VARIABLE___ Calibration( cal ); 				return this;};
	DB_AxeData		*___PROCEDURE1___( CalibrationResult, unsigned short, cal );//		{ ___VARIABLE___ CalibrationResult( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( ChannelFlags, unsigned short, flags );//		{ ___VARIABLE___ ChannelFlags( flags ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( SamplesCount, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};

	DB_AxeData		*___PROCEDURE2___( SampleAt, unsigned short, index, unsigned short, spl );
//																		{ if (( Temperature ) && ( index < numSample )) Temperature[ index + VARIABLES_PAD ] = htobe16( spl );return this;};
																	
#undef ___VARIABLE___
#undef ___FUNCVOID___
#undef ___FUNCTION___
#undef ___PROCEDURE1___
#undef ___PROCEDURE2___
// ****************************************************** vaccum *****************************************************************************
#define		___VARIABLE___					Vaccum
#define		___FUNCVOID___(b)				Vaccum##b( void ) 		{ return ___VARIABLE___->_ ## b ( ) ;}
#define		___FUNCTION___(a,b,c)			Vaccum##a( b c ) 		{ return ___VARIABLE___->_ ## a (c, numSample);}
#define		___PROCEDURE1___(a,b,c)			Vaccum##a( b c ) 		{ ___VARIABLE___->_ ## a ( c );return this;}
#define		___PROCEDURE2___(a,b,c,d,e)		Vaccum##a( b c, d e ) 	{ ___VARIABLE___->_ ## a ( c, e, numSample );return this;}
																	
	unsigned short	 ___FUNCVOID___( MaxValue )					;
	unsigned short	 ___FUNCVOID___( MinValue )					;//		{ return ___VARIABLE___ MinValue();								};
	unsigned short	 ___FUNCVOID___( LoadCalibration )			;//			{ return ___VARIABLE___ LoadCalibration();						};
	unsigned short	 ___FUNCVOID___( Calibration )				;//			{ return ___VARIABLE___ Calibration();							};
	unsigned short	 ___FUNCVOID___( CalibrationResult )		;//			{ return ___VARIABLE___ CalibrationResult();					};
	unsigned short	 ___FUNCVOID___( ChannelFlags )				;//		{ return ___VARIABLE___ ChannelFlags();							};
	unsigned short	 ___FUNCVOID___( SamplesCount )				;//		{ return ___VARIABLE___ SamplesCount();							};

	unsigned short	 ___FUNCTION___( SampleAt, unsigned short, index);//						{ return ___VARIABLE___->_SampleAt( index, numSample); 			};

	DB_AxeData		*___PROCEDURE1___( MaxValue, unsigned short, max )	;//			{ ___VARIABLE___ MaxValue( max ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( MinValue, unsigned short, min )	;//			{ ___VARIABLE___ MinValue( min ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( LoadCalibration, unsigned short, cal);//		{ ___VARIABLE___ LoadCalibration( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( Calibration, unsigned short, cal );//			{ ___VARIABLE___ Calibration( cal ); 				return this;};
	DB_AxeData		*___PROCEDURE1___( CalibrationResult, unsigned short, cal );//		{ ___VARIABLE___ CalibrationResult( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( ChannelFlags, unsigned short, flags );//		{ ___VARIABLE___ ChannelFlags( flags ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( SamplesCount, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};

	DB_AxeData		*___PROCEDURE2___( SampleAt, unsigned short, index, unsigned short, spl );
//																		{ if (( Vaccum ) && ( index < numSample )) Vaccum[ index + VARIABLES_PAD ] = htobe16( spl );return this;};

#undef ___VARIABLE___
#undef ___FUNCVOID___
#undef ___FUNCTION___
#undef ___PROCEDURE1___
#undef ___PROCEDURE2___
// ****************************************************** HighVac vaccum *****************************************************************************
#define		___VARIABLE___					HV_Vaccum
#define		___FUNCVOID___(b)				HvVaccum##b( void ) 		{ return ___VARIABLE___->_ ## b ( ) ;}
#define		___FUNCTION___(a,b,c)			HvVaccum##a( b c ) 			{ return ___VARIABLE___->_ ## a (c, numSample);}
#define		___PROCEDURE1___(a,b,c)			HvVaccum##a( b c ) 			{ ___VARIABLE___->_ ## a ( c );return this;}
#define		___PROCEDURE2___(a,b,c,d,e)		HvVaccum##a( b c, d e ) 	{ ___VARIABLE___->_ ## a ( c, e, numSample );return this;}

	unsigned short	 ___FUNCVOID___( MaxValue )					;
	unsigned short	 ___FUNCVOID___( MinValue )					;//		{ return ___VARIABLE___ MinValue();								};
	unsigned short	 ___FUNCVOID___( LoadCalibration )			;//			{ return ___VARIABLE___ LoadCalibration();						};
	unsigned short	 ___FUNCVOID___( Calibration )				;//			{ return ___VARIABLE___ Calibration();							};
	unsigned short	 ___FUNCVOID___( CalibrationResult )		;//			{ return ___VARIABLE___ CalibrationResult();					};
	unsigned short	 ___FUNCVOID___( ChannelFlags )				;//		{ return ___VARIABLE___ ChannelFlags();							};
	unsigned short	 ___FUNCVOID___( SamplesCount )				;//		{ return ___VARIABLE___ SamplesCount();							};

	unsigned short	 ___FUNCTION___( SampleAt, unsigned short, index);//						{ return ___VARIABLE___->_SampleAt( index, numSample); 			};

	DB_AxeData		*___PROCEDURE1___( MaxValue, unsigned short, max )	;//			{ ___VARIABLE___ MaxValue( max ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( MinValue, unsigned short, min )	;//			{ ___VARIABLE___ MinValue( min ); 					return this;};
	DB_AxeData		*___PROCEDURE1___( LoadCalibration, unsigned short, cal);//		{ ___VARIABLE___ LoadCalibration( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( Calibration, unsigned short, cal );//			{ ___VARIABLE___ Calibration( cal ); 				return this;};
	DB_AxeData		*___PROCEDURE1___( CalibrationResult, unsigned short, cal );//		{ ___VARIABLE___ CalibrationResult( cal ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( ChannelFlags, unsigned short, flags );//		{ ___VARIABLE___ ChannelFlags( flags ); 			return this;};
	DB_AxeData		*___PROCEDURE1___( SamplesCount, unsigned short, count );//		{ ___VARIABLE___ SamplesCount( count ); 			return this;};

	DB_AxeData		*___PROCEDURE2___( SampleAt, unsigned short, index, unsigned short, spl );
//																		{ if (( HV_Vaccum ) && ( index < numSample )) HV_Vaccum[ index + VARIABLES_PAD ] = htobe16( spl );return this;};
																		
	DB_AxeData		*SampleAt( unsigned short index, unsigned short hum, unsigned short temp, unsigned short vac )
																		{ 	Humidity->		_SampleAt( index, hum, numSample );
																			Temperature->	_SampleAt( index, temp, numSample );
																			Vaccum->		_SampleAt( index, vac, numSample );
																			return this;
																		};
	DB_AxeData		*SampleAt( unsigned short index, unsigned short hum, unsigned short temp, unsigned short vac, unsigned short hvac )
																		{ 	Humidity->		_SampleAt( index, hum, numSample );
																			Temperature->	_SampleAt( index, temp, numSample );
																			Vaccum->		_SampleAt( index, vac, numSample );
																			HV_Vaccum->		_SampleAt( index, hvac, numSample );
																			return this;
																		};
																		
#undef ___VARIABLE___
#undef ___FUNCVOID___
#undef ___FUNCTION___
#undef ___PROCEDURE1___
#undef ___PROCEDURE2___

	unsigned short	*ProcessBaseAddress( void )							{ return (unsigned short *) HumidityProcessed; };
	unsigned short	*HumidityBaseAddress( void )						{ return (unsigned short *) Humidity; };
	unsigned short	*TemperatureBaseAddress( void )						{ return (unsigned short *) Temperature; };
	unsigned short	*VaccumBaseAddress( void )							{ return (unsigned short *) Vaccum; };
	unsigned short	*HvVaccumBaseAddress( void )						{ return (unsigned short *) HV_Vaccum; };
	unsigned short	*LimitLineBaseAddress( void )						{ return (unsigned short *) LimitLine; };
	
	int				 BlockSize( void )									{ return ( numSample + VARIABLES_PAD ) * sizeof( unsigned short ); };

	int 			 doCalibration( void );

	int 			 createLimitLine( void );
	
};

class DB_AxeX : public DB_AxeData
{
public:
					 DB_AxeX( unsigned short howMany, unsigned short channelID, DB_Global *glob ){};
					 DB_AxeX(  ){};
	void			 buildBlock(){};
};

#endif	