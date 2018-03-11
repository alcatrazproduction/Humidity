/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/
#define S7_STATIC
#include "fondarex.h"
#include "s7_server_name.h"

					
void	encode( unsigned long a, unsigned long b, unsigned long c, unsigned long d );
void dieOpenInterrupt();
void _dieOpenInterrupt();
void vacuStartInterrut();

DB_Global::DB_Global()
{
unsigned char		buffer[256];
E_GlobalTypes		theType;
char				*theName;
char				theMode;
int 				theSize;

	try
	{
		memcpy( (char *)&_Version_data, (char *)ARDUINO_SOFT_VERSION,
				strlen((char *)ARDUINO_SOFT_VERSION) < sizeof( _Version_data ) ?  strlen((char *)ARDUINO_SOFT_VERSION) : sizeof( _Version_data ));
		
		_Version[0] 		= sizeof( _Version_data );
		_Version[1] 		= strlen( ARDUINO_SOFT_VERSION );
		_PartName[0]		= sizeof( _PartName_data );
		_PartName[1]		= 0;
		_Channel2Read		= 0;
		_ChannelHumidity	= 0;
		_ChannelTemperature	= 0;
		_ChannelVaccum		= 0;
		_Cmd[0]				= sizeof( _Cmd_data );
		_Cmd[1]				= 0;
		_T1					= 0;
		_T2					= htobe16( 10 );
		_T3					= htobe16( 10 );
		_NumChan			= htobe16( 4 );
		_SampleRate			= htobe16( 20 );
		_ReqCurve			= 0;
		_LoadCurve			= 0;
		SPI_mode			= 0;
		SPI_bits			= 8;
		SPI_speed			= 500000;
		SPI_delay			= 0;
		dieOpen[0]			= FALSE;
		dieOpen[1]			= FALSE;
		vacuStart[0]		= FALSE;
		vacuStart[1]		= FALSE;
		interruptMCP		= FALSE;
		dataReady			= FALSE;
		Server				= NULL;
		axeArray			= new DB_AxeData *[MAX_CHANNEL_SLAVE];

		database			= new C_LibDb();
		
#if	defined(PCDUINO)		
		CAN 				= new CAN_MCP2515( PIN_MCP_CS );
#endif
		SIG_DieOpen			(SIGRTMIN + 0);
		SIG__DieOpen		(SIGRTMIN + 1);
		SIG_Vacustart		(SIGRTMIN + 2);
		SIG__Vacustart		(SIGRTMIN + 4);
		SIG_DataReady		(SIGRTMIN + 3);

		memset( &MStorage, 0, sizeof( MStorage ));
 		memset( &_MStorage, 0, sizeof( _MStorage ));

		_CPUID[0]			= sizeof( _CPUID_data );
		try
		{
		unsigned char	buffer[256];
		FILE			*in;

			in = fopen("/proc/cpuid","r");

			if( in )
			{
				while( fgets( (char *)&buffer, sizeof( buffer ) -10, in ) )
					if( ( buffer[0] == 'I' ) && ( buffer[1] =='D' ))
					{
						strncpy((char *) &_CPUID_data,  (char *)&buffer, _CPUID[0]);
						_CPUID[1] = strlen(  (char *)&buffer ) - 1;
					}
				fclose( in );
			}
			else
			{
				strncpy((char *) &_CPUID_data,  "ID:BAD", _CPUID[0]);
				_CPUID[1] = strlen(  "ID:BAD" );
			}
		}
		catch (const std::exception& e )
		{
				PRINT_ERROR( "Exception DB_Global::DB_Global:get /proc/cpuid ) %s \n", e.what() )
				exit(-1);
		}
		try
		{
			cpuid[0] = cpuid[1] = cpuid[2] = cpuid[3] = 0;
			sscanf( (char *)&_CPUID_data[3], "%8lx%8lx%8lx%8lx", &cpuid[0],&cpuid[1],&cpuid[2],&cpuid[3] );
			encode( cpuid[0],cpuid[1],cpuid[2],cpuid[3] );
		}
		catch (const std::exception& e )
		{
				PRINT_ERROR( "Exception DB_Global::DB_Global: sscanf( _CPUID_data[3].. ) %s \n", e.what() )
		}
		PRINT_DEBUG("CPU ID is: %s \n", _CPUID_data );
		
		if( database )
		{
		C_Connection *con	= database->getDb();
		
			if( con )
			{
			unsigned short	length = sizeof( _PartName_data );

				PRINT_DEBUG(" Charging values saved in DB \n")

				CurrentInjection( 		con->getIngNum() 		);
										con->getPartName( &buffer[0], length ); // load part name in buffer
				PartName( 				&buffer[0]				);
				ProdNum( 				con->getProdNum()		);
				T1(						con->getT1()			);
				T2(						con->getT2()			);
				T3(						con->getT3()			);
				NumChan(				con->getNumChannel()	);
				
#				ifndef __USE_MANY_LIMIT__
				
				
			unsigned short avg		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500, CHANNEL_LIMIT_AVG * sizeof( short )  ) ? theName : NULL ,
													   &buffer, theType ) ?
											100 	: (unsigned short) (*((unsigned long *)&buffer));
			
			unsigned short start	= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500, CHANNEL_LIMIT_START * sizeof( short )  ) ? theName : NULL ,
													   &buffer, theType ) ?
											0 		: (unsigned short) (*((unsigned long *)&buffer));
			
			unsigned short stop		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500, CHANNEL_LIMIT_STOP * sizeof( short )  ) ? theName : NULL ,
													   &buffer, theType ) ?
											4000 	: (unsigned short) (*((unsigned long *)&buffer));

				PRINT_DEBUG(" Charging Limit value saved in DB (%d, %d, %d) \n",avg,start,stop)
				
				for( int i = 0; i < MAX_CHANNEL_SLAVE; i++)
				{
					_LimitAverage[i] = avg;
					_LimitStart[i]	 = start;
					_LimitStop[i]	 = stop;
				}
#				else
				for( int i = 0; i < MAX_CHANNEL_SLAVE; i++)
				{
				unsigned short avg;
				unsigned short start;
				unsigned short stop;
					switch( i )
					{
						case 1:
							avg		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_AVG * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									100 	: (unsigned short) (*((unsigned long *)&buffer));

							start	= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_START * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									0 		: (unsigned short) (*((unsigned long *)&buffer));

							stop		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_STOP * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									4000 	: (unsigned short) (*((unsigned long *)&buffer));
							break;
						case 2:
							avg		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_AVG * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									100 	: (unsigned short) (*((unsigned long *)&buffer));

							start	= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_START * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									0 		: (unsigned short) (*((unsigned long *)&buffer));

							stop		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_STOP * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									4000 	: (unsigned short) (*((unsigned long *)&buffer));
							break;
						case 3:
							avg		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_AVG * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									100 	: (unsigned short) (*((unsigned long *)&buffer));

							start	= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_START * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									0 		: (unsigned short) (*((unsigned long *)&buffer));

							stop		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+i, CHANNEL_LIMIT_STOP * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									4000 	: (unsigned short) (*((unsigned long *)&buffer));
							break;
						case 0:
						default:
							avg		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+0, CHANNEL_LIMIT_AVG * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									100 	: (unsigned short) (*((unsigned long *)&buffer));

							start	= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+0, CHANNEL_LIMIT_START * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									0 		: (unsigned short) (*((unsigned long *)&buffer));

							stop		= con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, 500+0, CHANNEL_LIMIT_STOP * sizeof( short )  ) ? theName : NULL ,
														&buffer, theType ) ?
									4000 	: (unsigned short) (*((unsigned long *)&buffer));
							break;
					}
					
					PRINT_DEBUG(" Charging Limit value saved in DB  for channel %d (%d, %d, %d) \n",i,avg,start,stop)

					_LimitAverage[i] = avg;
					_LimitStart[i]	 = start;
					_LimitStop[i]	 = stop;
				}
#				endif
DO_INIT_GLOBAL
				con->Release();
			}
		}


	}
	catch (const std::exception& e )
	{
			PRINT_ERROR( "Exception DB_Global::DB_Global: init %s \n", e.what() )
			return;
	}

}

// some static buffer, only for local use

static	unsigned char *Version_buf 	= new unsigned char[SIZE_ASCII];
static	unsigned char *PartName_buf	= new unsigned char[SIZE_ASCII];
static	unsigned char *Cmd_buf		= new unsigned char[SIZE_ASCII];

unsigned char	*DB_Global::Version( void )
{
unsigned short	len = _Version[1];

	if( len > SIZE_ASCII - sizeof( short ) )
		len = SIZE_ASCII - sizeof( short );
	
	memcpy( Version_buf, &_Version_data, len );
	Version_buf[ len ] = 0;
	return Version_buf;
};

unsigned char	*DB_Global::PartName( void )
{
unsigned short	len = _PartName[1];

	if( len > SIZE_ASCII - sizeof( short ) )
		len = SIZE_ASCII - sizeof( short );

	memcpy( PartName_buf, &_PartName_data, len );
	PartName_buf[ len ]= 0;
	return PartName_buf;
};

unsigned char	*DB_Global::Cmd( void )
{
unsigned short	len = _Cmd[1];

	if( len > SIZE_ASCII - sizeof( short ) )
		len = SIZE_ASCII - sizeof( short );

	memcpy( Cmd_buf, &_Cmd_data, len );
	Cmd_buf[len] = 0;
	return Cmd_buf;
};

DB_Global 		*DB_Global::PartName( unsigned char * p )
{
	if( p == NULL )
		return this;
unsigned short	len = strlen( (char *) p );

	if( len > SIZE_ASCII - sizeof( short ) )
		len = SIZE_ASCII - sizeof( short );

	_PartName[1] = (unsigned char) len ;
	memcpy( &_PartName_data, p, len );
	return this;
};

DB_Global 		*DB_Global::Cmd( unsigned char * p )
{
	if( p == NULL )
		return this;
unsigned short	len = strlen( (char * )p );

	if( len > SIZE_ASCII - sizeof( short ) )
		len = SIZE_ASCII - sizeof( short );

	_Cmd[1] = (unsigned char) len ;
	memcpy( &_Cmd_data, p, len );
	return this;
};

DB_AxeData::DB_AxeData( unsigned short howMany, unsigned short channelID, DB_Global *glob )
{
C_Connection *con	= GETDATABASELIBRARY->getDb();

	numSample 			= howMany;
	ID					= channelID;
	GLOBALSETSTART		= glob;

	HumidityProcessed	= ( DB_AxeData::DB_AxeValue * )	calloc( numSample 		+ VARIABLES_PAD , sizeof( unsigned short ) );
	Humidity			= ( DB_AxeData::DB_AxeValue * )	calloc( numSample 		+ VARIABLES_PAD , sizeof( unsigned short ) );
	Temperature			= ( DB_AxeData::DB_AxeValue * )	calloc( numSample 		+ VARIABLES_PAD , sizeof( unsigned short ) );
	Vaccum				= ( DB_AxeData::DB_AxeValue * )	calloc( numSample 		+ VARIABLES_PAD , sizeof( unsigned short ) );
#define 	CASE_THE_DB		case ___THE_DB___:																	\
							if( addr < ( sizeof( ___THE_DBNNAME___( ___THE_DB___ ) ) / sizeof( char * ) ) )		\
								return ___THE_DBNNAME___( ___THE_DB___ )[addr];									\
							break;
	HV_Vaccum			= ( DB_AxeData::DB_AxeValue * )	calloc( numSample 		+ VARIABLES_PAD , sizeof( unsigned short ) );
	LimitLine			= ( unsigned short * ) 			calloc( numSample * 2	+ VARIABLES_PAD , sizeof( unsigned short ) );

	HumidityLimitAverage( 	GETLIMITAVERAGE( channelID ) );
	HumidityLimitStart( 	GETLIMITSTART  ( channelID ) );
	HumidityLimitStop( 		GETLIMITSTOP   ( channelID ) );

	if( con )
	{
	unsigned char		buffer[256];
	E_GlobalTypes		theType;
	char				*theName;
	char				theMode;
	int 				theSize;
	
		HumidityChannelLimitActive((  ( con->ReadGlobal( decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVCLEANBIT_BASE  ) ? theName : NULL ,
														&buffer, theType ) ?
#if DEBUG > 0
											CH_FLAG_LIMIT_STATUS
#else
											0
#endif
											: (unsigned short) (*((unsigned long *)&buffer)) ) & CH_FLAG_LIMIT_STATUS )
								);
		con->Release();
	}
}

bool DB_AxeData::NumSample( unsigned short howMany)
{
	if( howMany < 999 )
		howMany = 999;
	numSample 		= howMany;

	HumidityProcessed	= ( DB_AxeData::DB_AxeValue * )	realloc( HumidityProcessed	, this->BlockSize() );
	Humidity			= ( DB_AxeData::DB_AxeValue * )	realloc( Humidity 			, this->BlockSize() );
	Temperature			= ( DB_AxeData::DB_AxeValue * )	realloc( Temperature 		, this->BlockSize() );
	Vaccum				= ( DB_AxeData::DB_AxeValue * )	realloc( Vaccum	 			, this->BlockSize() );
	HV_Vaccum			= ( DB_AxeData::DB_AxeValue * )	realloc( HV_Vaccum	 		, this->BlockSize() );
	LimitLine			= ( unsigned short * ) 			realloc( LimitLine	 		, this->BlockSize() * 2 );

	return (( HumidityProcessed ? TRUE : FALSE ) & ( Humidity ? TRUE : FALSE ) & ( Temperature ? TRUE : FALSE ) & ( Vaccum ? TRUE : FALSE ) & ( HV_Vaccum ? TRUE : FALSE ));
}

DB_AxeData *DB_AxeData::ProcessHumidity( unsigned short plage )
{
	if( ( HumidityProcessed == NULL ) | ( Humidity == NULL ) )
	  return this;
  
 unsigned short		max 	= HumidityMaxValue();
 unsigned short		min 	= HumidityMinValue();
 unsigned short		cnt 	= HumiditySamplesCount();
// unsigned short		prev	= HumiditySampleAt( 0 );
// unsigned short		next	= HumiditySampleAt( 0 );

	memcpy( HumidityProcessed, Humidity, VARIABLES_PAD * sizeof( unsigned short ) );

	if( plage < ( max -min ) )
		plage = max - min;

	plage 	= 100 ;																// TODO: Read from globals...
	min		= HumiditySampleAt( 0 ) - 90;										// TODO: Read from globals, actualy set to 90%
	
//	PRINT_DEBUG(" Plage is %d (%d,%d)-> ",plage,min,max );

	plage = 4096 / plage ;

//	PRINT_DEBUG(" Factor is %d (%d,%d)\n",plage, 0, (max - min ) *plage );

	ProcessMaxValue( 		( HumidityMaxValue() 		- min ) * plage );
	ProcessMinValue( 		( HumidityMinValue() 		- min ) * plage );
	ProcessLimitAverage( 	( HumidityLimitAverage() 	      ) * plage );
	
	ProcessCalibrationResult( min * plage );
	
	for( int i = 0; i < cnt ;  )
	{
	unsigned short	cur	= HumiditySampleAt( i );

		ProcessSampleAt( i, ( cur - min ) * plage );
		
#if FALSE
		prev = cur;
		if( ( ++i ) <  cnt  )
		{
			next = HumiditySampleAt( i + 1 );
		}
		else
		{
			next = HumiditySampleAt( i );
		}
#else
		i++;
#endif
	}
	createLimitLine();
	PRINT_DEBUG("\n")
	return this;
}

int DB_AxeData::createLimitLine( void )
{
	ProcessLimitAverage( 	( HumidityLimitAverage() 	      ) * (4096 / 100) );
unsigned short theValue =  ProcessMaxValue() - ProcessLimitAverage();
unsigned short theStart = ( HumidityLimitStart() / GETSAMPLERATE ) + 1000 ;
unsigned short theStop  = ( HumidityLimitStop()  / GETSAMPLERATE ) + 1000 ;

	PRINT_DEBUG("int DB_AxeData::createLimitLine( void ), Create the data: %d for %d samples (Start %d ; Stop %d )\n",  theValue , numSample, theStart, theStop )



	theValue = htobe16( theValue >  4096 ? 0 : theValue );
	
	for( int i = 0; i < 1999; i++ )
		if ( i < 1000 )
			LimitLine[i] =   theValue ;
		else
			if ( ( ( i == theStart  ) ) | ( ( i == theStop  )  ) )
				LimitLine[i] = 0x7f7f;
			else
				LimitLine[i] = 0x0000;


	return 0; // TODO:  define how to make it...
}

bool	decodeVarName(  const char *info, char **name, E_GlobalTypes &theType, char &mode , int &size, unsigned short db , unsigned short addr   )
{
	if( name == NULL )
		return false;
	if( info == NULL )
		if( ( info = getVarName( db, addr ) ) == NULL )
			return false;

	switch( info[0] )
	{
		case '?':
		case 0:
			return false;
		case 'B':
		case 'b':
			theType = (E_GlobalTypes)BYTE;
			break;
		case 'W':
		case 'w':
			theType = (E_GlobalTypes)WORD;
			break;
		case 'L':
		case 'l':
			theType = (E_GlobalTypes)LONG;
			break;
		case 'D':
		case 'd':
		case 'F':
		case 'f':
			theType = (E_GlobalTypes)FLOAT;
			break;
		case 'S':
		case 's':
			theType = (E_GlobalTypes)TEXT;
			size 	= (int)info[2];
		default:
			return false;
	}
	*name = (char *)&info[3];
	mode 			= info[1];
	return true;
}




void S7API DB_Global::EventCallBack(void *usrPtr, PSrvEvent PEvent, int Size)
{
	PRINT_DEBUG("DB_Global::EventCallBack %3.3d.%3.3d.%3.3d.%3.3d -> Event:%8x( Type: %d, DB: %d, ADDR: %d, SIZE: %d )[%s]->",
				0xff&(PEvent->EvtSender),0xff&(PEvent->EvtSender>>8),0xff&(PEvent->EvtSender>>16),0xff&(PEvent->EvtSender>>24),
			 PEvent->EvtCode,PEvent->EvtParam1 ,PEvent->EvtParam2,PEvent->EvtParam3,PEvent->EvtParam4,
			 getVarName( PEvent->EvtParam2, PEvent->EvtParam3) )
#if DEBUG==2
	if( ( PEvent->EvtParam1==S7AreaDB ) && ( PEvent->EvtCode == evcDataWrite ) )
	{
	char 		*buf;
	DB_AxeData	*axe;
	
		switch( PEvent->EvtParam2  )
		{
			case GLOBAL_SLAVE:
				buf = (char *)GLOBALSETSTART->BaseAddress( );
				break;
			case CHANNEL_PROCESS_BASE + 0:
			case CHANNEL_PROCESS_BASE + 1:
			case CHANNEL_PROCESS_BASE + 2:
			case CHANNEL_PROCESS_BASE + 3:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_PROCESS_BASE );
				buf = axe ? ((char *)axe->ProcessBaseAddress()) : NULL;
				break;
			case CHANNEL_TEMPERATURE_BASE + 0:
			case CHANNEL_TEMPERATURE_BASE + 1:
			case CHANNEL_TEMPERATURE_BASE + 2:
			case CHANNEL_TEMPERATURE_BASE + 3:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_TEMPERATURE_BASE );
				buf = axe ? ((char *)axe->TemperatureBaseAddress()) : NULL;
				break;
			case CHANNEL_VACCUM_BASE +0:
			case CHANNEL_VACCUM_BASE +1:
			case CHANNEL_VACCUM_BASE +2:
			case CHANNEL_VACCUM_BASE +3:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_VACCUM_BASE );
				buf = axe ? ((char *)axe->VaccumBaseAddress()) : NULL;
				break;
			case CHANNEL_HVVACCUM_BASE + 0:
			case CHANNEL_HVVACCUM_BASE + 1:
			case CHANNEL_HVVACCUM_BASE + 2:
			case CHANNEL_HVVACCUM_BASE + 3:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_HVVACCUM_BASE );
				buf = axe ? ((char *)axe->HvVaccumBaseAddress()) : NULL;
				break;
			case CHANNEL_HUMIDITY_BASE + 0:
			case CHANNEL_HUMIDITY_BASE + 1:
			case CHANNEL_HUMIDITY_BASE + 2:
			case CHANNEL_HUMIDITY_BASE + 3:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_HUMIDITY_BASE );
				buf = axe ? ((char *)axe->HumidityBaseAddress()) : NULL;
				break;
			case CHANNEL_LIMIT_BASE + 0:
			case CHANNEL_LIMIT_BASE + 1:
			case CHANNEL_LIMIT_BASE + 2:
			case CHANNEL_LIMIT_BASE + 3:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_LIMIT_BASE );
				buf = axe ? ((char *)axe->LimitLineBaseAddress()) + 0000 : NULL;
				break;
			case CHANNEL_LIMIT_BASE + 50:
			case CHANNEL_LIMIT_BASE + 51:
			case CHANNEL_LIMIT_BASE + 52:
			case CHANNEL_LIMIT_BASE + 53:
				axe = GETAXEDATA( PEvent->EvtParam2 - CHANNEL_LIMIT_BASE - 50 );
				buf = axe ? ((char *)axe->LimitLineBaseAddress()) + 1000 : NULL;
				break;
			default:
				buf = NULL;
				break;
		};
		if( buf )
			for( int i=PEvent->EvtParam3; i<(PEvent->EvtParam3 + PEvent->EvtParam4 );i++)
				PRINT_DEBUG(" %2.2X(%c)",buf[i], isprint( buf[i] ) ?  buf[i] : '.' )

	}
	PRINT_DEBUG("\n")
#endif
	if( ( PEvent->EvtParam1==S7AreaMK ) && ( PEvent->EvtCode == evcDataWrite ) )
	{
		uint8_t	theSignal;
		int theAddr		= PEvent->EvtParam3;
		int theSize		= PEvent->EvtParam4;
		PRINT_DEBUG("Accessing M block ")

		switch( theAddr )
		{
			case 0:		// Profinet signal
				theSignal = ( ( GLOBALSETSTART->_MStorage[0] ) xor ( GLOBALSETSTART->MStorage[0] ));

				if( theSignal == 0 )
					break;

				if( theSignal bitand 0x01 ) // Vacustart
				{
					if( GLOBALSETSTART->MStorage[0] bitand 0x01 )	// Rising
					{
						PRINT_DEBUG("Got profinet VacuStart Signal Raising M0.0")
						vacuStartInterrut();
					}
					else
					{
						PRINT_DEBUG("Got profinet VacuStart Signal Falling M0.0")
					}
				}
				if( theSignal bitand 0x02 ) //Die
				{
					if( GLOBALSETSTART->MStorage[0] bitand 0x02 )	// Rising
					{
						PRINT_DEBUG("Got profinet dieOpen Signal Raising M0.1")
						dieOpenInterrupt();
					}
					else
					{
						PRINT_DEBUG("Got profinet dieOpen Signal Falling M0.1")
						_dieOpenInterrupt();
					}
				}
				break;
				
			default:
				break;
		}
		PRINT_DEBUG("\n")
		for( int i=theAddr;i<( theAddr + theSize); i++)
			GLOBALSETSTART->_MStorage[i] = GLOBALSETSTART->MStorage[i];

	}

	if (( PEvent->EvtParam1==S7AreaDB ) && ( PEvent->EvtCode == evcDataWrite ) )
	{
                // As example the DB requested is filled before transferred
                // EvtParam1 contains the DB number.
		switch (PEvent->EvtParam2)
		{
		case 	GLOBAL_SLAVE :						// we access the globals... may be we do something here
			try
			{
			char			*theName;
			E_GlobalTypes	 theType;
			char			 theMode;
			int 			 theSize;

				if( decodeVarName(/* PEvent->EvtParam2,*/ db001_name[PEvent->EvtParam3], &theName, theType, theMode, theSize ) )
					if( theMode == 'w' )
					try
					{
					C_Connection 	*con;
					char 			*buf 			= (char *)GLOBALSETSTART->BaseAddress(  );
					unsigned long	theInt;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, Save setting, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, Save setting, loading data into buffer %s \n", theName)

						switch( theType )
						{
							case (E_GlobalTypes)BYTE:
							case (E_GlobalTypes)WORD:
							case (E_GlobalTypes)LONG:
								switch ( theType )
								{
									case (E_GlobalTypes)BYTE:
										theInt = (unsigned long ) buf[PEvent->EvtParam3];
										break;
									case (E_GlobalTypes)WORD:
										theInt = (unsigned long ) be16toh( *((unsigned short *)&buf[PEvent->EvtParam3]));
										break;
									case (E_GlobalTypes)LONG:
										theInt = (unsigned long ) be32toh( *((unsigned long *)&buf[PEvent->EvtParam3]));
										break;
									default:
										break;
								}
								con->CreateGlobal( theName, &theInt, (E_GlobalTypes)INT );
								break;
							case (E_GlobalTypes)FLOAT:
								break;
							case (E_GlobalTypes)TEXT:
								break;
							default:
								break;
						}

						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: set Number of Channel  %s \n", e.what() )
							return;
					}
			}
			catch (const std::exception& e )
			{
					PRINT_ERROR(  "Exception S7API EventCallBack: Save setting %s \n", e.what() )
					return;
			}
			switch (PEvent->EvtParam3 )				// Param3 is address base
			{
				case FLAGSBITS:
					{
					unsigned char	flags	= globals->__FlagsBits xor globals->_FlagsBits;

						globals->__FlagsBits = globals->_FlagsBits;			// save it

						if( flags bitand ( 1 << 1 ) )						// DIE OPEN changed state
						{
							if( globals->_FlagsBits bitand ( 1 << 0 ) )		// change to active
							{
								PRINT_DEBUG("Got profinet Variable Flags Raising 0.1")
								dieOpenInterrupt();
							}
							else
							{
								PRINT_DEBUG("Got profinet Variable Flags Falling 0.1")
								_dieOpenInterrupt();
							}						}
						
						if( flags bitand ( 1 << 0 ) )						// Vacustart changed state
						{
							if( globals->_FlagsBits bitand ( 1 << 1 ) )		// change to active
							{
								PRINT_DEBUG("Got Variable Flags VacuStart Raising 0.0")
								vacuStartInterrut();
							}
							else
							{
								PRINT_DEBUG("Got Variable Flags VacuStart Falling 0.0")
							}
						}
					}
					break;
				case SIGNALBITS:
					if( globals->SignalBits() & (1<< 0) )	// CMD RESET ALL ALARM
					{
						if( !GETHIGHVAC)
							break;
						if( GETHIGHVAC->openConnection() )
						{
							GETHIGHVAC->setOutOfTolerance( FALSE )->setNeed2Clean( FALSE )->closeConnection();
						}						
					}
					if( globals->SignalBits() & (1<< 1) )	// CMD RESET Out Tolerance
					{
						if( !GETHIGHVAC)
							break;
						if( GETHIGHVAC->openConnection() )
						{
							GETHIGHVAC->setOutOfTolerance( FALSE )->closeConnection();
						}
					}
					if( globals->SignalBits() & (1<< 2) )	// CMD RESET Calibration
					{
						if( !GETHIGHVAC)
							break;
						if( GETHIGHVAC->openConnection() )
						{
							GETHIGHVAC->setNeed2Clean( FALSE )->closeConnection();
						}
					}
					globals->SignalBits(0);
					break;
				case HVIP_BASE:					// 	( 344 )
				case HVRACK_BASE:				//	( 348 )
				case HVSLOT_BASE:				//	( 350 )
					if( !GETHIGHVAC )
						break;
					{
					char			ip[]		=" 192.168.001.010";
					unsigned long	theIp 		= GETHIGHVACIP;

						snprintf((char *)&ip,16,"%d.%d.%d.%d",(int)((theIp>>24)&0x00ff),(int)((theIp>>16)&0x00ff),(int)((theIp>>8)&0x00ff),(int)((theIp>>0)&0x00ff));

						PRINT_DEBUG(" (IN S7 SRV)  Host info: IP: %s(%8.8lX), Rack: %d, Slot %d\n",ip,theIp,GETHVRACK, GETHVSLOT)

						GETHIGHVAC->setAddress((const char *) &ip )->setRack( GETHVRACK )->setSlot( GETHVSLOT );
					}
					break;
				case HVOUTTOL_BASE:				// ( 128 )
				case HVOUTTOLBYTE_BASE:			// ( 130 )
				case HVOUTTOLBIT_BASE:			// ( 132 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setOuttol( GETHVOUTTOL )			->setOuttolByte( GETHVOUTTOLBYTE)		->setOuttolBit( GETHVOUTTOLBIT);
					break;
				case HVCLEAN_BASE:				// ( 134 )
				case HVCLEANBYTE_BASE:			// ( 136 )
				case HVCLEANBIT_BASE:			// ( 138 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setClean( GETHVCLEAN )				->setCleanByte( GETHVCLEANBYTE)			->setCleanBit( GETHVCLEANBIT);
					break;
				case HVVACUSTART_BASE:			// ( 352 )
				case HVVACUSTARTBIT_BASE:		// ( 354 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setInVacuStartIn( GETHVVACUSTART )	->setInVacuStartBit( GETHVVACUSTARTBIT );
					break;
				case HVDIEOPEN_BASE:			// ( 356 )
				case HVDIEOPENBIT_BASE:			// ( 358 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setInDieOpenIn( GETHVDIEOPEN )		->setInDieOpenBit( GETHVDIEOPENBIT);
					break;
				case HVT1_BASE:					// ( 360 )
				case HVT1BYTE_BASE:				// ( 362 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setT1Db( GETHVT1 )					->setT1Byte( GETHVT1BYTE )				->setT1DbSize( 4 );
					break;
				case HVT2_BASE:					// ( 364 )
				case HVT2BYTE_BASE:				// ( 366 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setT2Db( GETHVT2 )					->setT2Byte( GETHVT2BYTE )				->setT2DbSize( 4 );
					break;
				case HVT3_BASE:					// ( 368 )
				case HVT3BYTE_BASE:				// ( 370 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setT3Db( GETHVT3 )					->setT3Byte( GETHVT3BYTE )				->setT3DbSize( 4 );
					break;
				case HVPRODNUM_BASE:			// ( 372 )
				case HVPRODNUMBYTE_BASE:		// ( 374 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setProdNumberDb( GETHVPRODNUM )		->setProdNumberByte( GETHVPRODNUMBYTE )	->setProdNumberDbSize( 2 );
					break;
				case HVPARTNUM_BASE:			// ( 376 )
				case HVPARTNUMBYTE_BASE:		// ( 378 )
					if( !GETHIGHVAC )
						break;
//					GETHIGHVAC-> //TODO:
					break;
				case HVPARTNAME_BASE:			// ( 380 )
				case HVPARTNAMEBYTE_BASE:		// ( 382 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setProdNameDb( GETHVPARTNAME )		->setProdNameByte( GETHVPARTNAMEBYTE )	->setProdNameDbSize( 0x2a );
					break;
				case HVSHOTNUM_BASE:			// ( 384 )
				case HVSHOTNUMBYTE_BASE:		// ( 386 )
					if( !GETHIGHVAC )
						break;
					GETHIGHVAC->setShotNumberDb( GETHVSHOTNUM )		->setShotNumberByte( GETHVSHOTNUMBYTE )	->setShotNumberDbSize( 4 );
					break;
				case CURRENTINJECTION_BASE:
					try
					{
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, CURRENTINJECTION_BASE, Error no mysql free connection\n")
							break;
						}
						PRINT_DEBUG("S7API EventCallBack, CURRENTINJECTION_BASE, loading data into buffer\n")
						con->setIngNum( GETCURRENTINJECTION );
						con->Release();
					}					
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: Set Current Injection %s \n", e.what())
							return;
					}
					break;
				case PARTNAME_BASE:
					try
					{
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, PARTNAME_BASE, Error no mysql free connection\n")
							break;
						}
						PRINT_DEBUG("S7API EventCallBack, PARTNAME_BASE, loading data into buffer\n")
						con->setPartName( GETPARTNAME );
						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR( "Exception S7API EventCallBack:  set PartName %s \n", e.what())
							return;
					}
					break;
				case PRODNUM_BASE:
					try
					{
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, PRODNUM_BASE, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, PRODNUM_BASE, loading data into buffer\n")
						con->setProdNum( GETPRODUCTIONNUMBER );
						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR( "Exception S7API EventCallBack: set Production number %s \n", e.what())
							return;
					}
					break;
				case T1_BASE:
					try
					{
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, T1_BASE, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, T1_BASE, loading data into buffer\n")
						con->setT1( GETT1 );
						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack:  set T1 %s \n", e.what() )
							return;
					}
					break;
				case T2_BASE:
					try
					{
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, T2_BASE, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, T2_BASE, loading data into buffer\n")
						con->setT2( GETT2 );
						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR( "Exception S7API EventCallBack: set T2  %s \n", e.what() )
							return;
					}
					break;
				case T3_BASE:
					try
					{
					C_Connection *con;


						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, T3_BASE, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, T3_BASE, loading data into buffer\n")
						con->setT3( GETT3 );
						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR( "Exception S7API EventCallBack: set T3  %s \n", e.what() )
							return;
					}
					break;
				case NUMCHAN_BASE:
					try
					{
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, NUMCHAN_BASE, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, NUMCHAN_BASE, loading data into buffer\n")
						con->setNumChannel( GETNUMBEROFCHANNEL );
						con->Release();
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: set Number of Channel  %s \n", e.what() )
							return;
					}
					break;
				case SAMPLERATE_BASE:
					try
					{
					C_Connection 	*con;
					short			samples;

						con = GETDATABASELIBRARY->getDb();

						if( GETSAMPLERATE == 0 )	// 0 is not authorized !!!
							GLOBALSETSTART

							SETSAMPLERATE(1)

							GLOBALSETEND
							
						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, SAMPLERATE_BASE, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, SAMPLERATE_BASE, loading data into buffer\n")
						con->setSampleRate( GETSAMPLERATE );
						con->Release();
						// TODO: Check for the size of the block
						samples = ( GETT2 + GETT3 )/ GETSAMPLERATE;
						if( samples < 999 )
							samples = 999;
						for( short i = 0; i < GETNUMBEROFCHANNEL; i++ )
						{
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_PROCESS_BASE);						// 10c
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_TEMPERATURE_BASE);					// 20c
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_VACCUM_BASE);							// 30c
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HVVACCUM_BASE);						// 40c
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HUMIDITY_BASE);						// 50c
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE);							// 60c
							GETS7SERVER->LockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE + 50);							// 60c
							
							if( GETAXEDATA( i )->NumSample( samples ) )
							try
							{
								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_PROCESS_BASE );				// 10c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_PROCESS_BASE ,
																	   GETAXEDATA(i)->ProcessBaseAddress(),
																	   GETAXEDATA(i)->BlockSize() );
								
								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_TEMPERATURE_BASE );			// 20c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_TEMPERATURE_BASE ,
																	   GETAXEDATA(i)->TemperatureBaseAddress(),
																	   GETAXEDATA(i)->BlockSize() );
								
								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_VACCUM_BASE );				// 30c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_VACCUM_BASE ,
																	   GETAXEDATA(i)->VaccumBaseAddress(),
																	   GETAXEDATA(i)->BlockSize() );
								
								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HVVACCUM_BASE );			// 40c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HVVACCUM_BASE ,
																	   GETAXEDATA(i)->HvVaccumBaseAddress(),
																	   GETAXEDATA(i)->BlockSize() );

								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HUMIDITY_BASE );			// 50c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HUMIDITY_BASE ,
																	   GETAXEDATA(i)->HumidityBaseAddress(),
																	   GETAXEDATA(i)->BlockSize() );

								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE );				// 60c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE ,
																	   GETAXEDATA(i)->LimitLineBaseAddress(),
																	   ( GETAXEDATA(i)->BlockSize() - ( VARIABLES_PAD * sizeof( unsigned short ) ) )  );
								
								GETS7SERVER->UnregisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE + 50);				// 65c
								GETS7SERVER->RegisterArea(  srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE + 50 ,
																	   GETAXEDATA(i)->LimitLineBaseAddress() + 1000,
																	   ( GETAXEDATA(i)->BlockSize() - ( VARIABLES_PAD * sizeof( unsigned short ) ) )  );
							}
							catch (const std::exception& e )
							{
									PRINT_ERROR( "Exception S7API EventCallBack: Changing channel data size ID = %d ; %s \n", i, e.what() )
									return;
							}
							else
								PRINT_WARNING( "WARNING S7API EventCallBack: Changing channel data size ID = %d \n", i )
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_PROCESS_BASE);
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_TEMPERATURE_BASE);
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_VACCUM_BASE);
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HVVACCUM_BASE);
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HUMIDITY_BASE);
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE);
							GETS7SERVER->UnlockArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE + 50);
						}
					}
					catch (const std::exception& e )
					{
#define ___THE_DBNAME___(a,b) db ##a##b
							PRINT_ERROR( "Exception S7API EventCallBack: set number of samples  %s \n", e.what() )
							return;
					}
					break;
				case LOADCURVE_BASE:

					break;
				case REQCURVE_BASE:
					
					if( GETREQUESTCURVE != 0 )
					{
						GLOBALSETSTART
//						SETLOADCURVE( GETLOADCURVE | GETREQUESTCURVE )
						SETLOADCURVE( 0xffff )
						SETREQUESTCURVE( 0x0000)
						GLOBALSETEND
					}
					break;
					
				case REQUESTINJECTION_BASE:			// if we write here, we must load the data, but check for a flag may be good
#ifndef DEBUG
					if( GETSIG_DATAREADY | GETSIG_VACUSTART )
					{
						PEvent->EvtRetCode = evrFragmentRejected;
						break;
					}
#endif
					try
					{
					short	i;
					unsigned short 	max;
					unsigned short 	min;
					unsigned short 	plage 		= PLAGE_AXE;
					unsigned short	ch_ready 	= 0xffff;
					DB_AxeData		*axe;
					
					C_Connection *con;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, REQUESTINJECTION, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, REQUESTINJECTION, loading data into buffer (%ld)\n", GETREQUESTINJECTION)
						for( i = 0; i < GETNUMBEROFCHANNEL; i++ )
						{
							axe = GETAXEDATA(i);
							con->LoadChannelCurve( GLOBALSETSTART, axe  );
							if( axe )
							{
								max = axe->HumidityMaxValue();
								min = axe->HumidityMaxValue();

								if( plage < (( ( ( max - min ) / PLAGE_AXE ) + 1 ) * PLAGE_AXE ) )
									plage = (( ( ( max - min ) / PLAGE_AXE ) + 1 ) * PLAGE_AXE );
								ch_ready |= ( 1 << i );
							}
						}
						con->Release();
						for( i = 0; i < GETNUMBEROFCHANNEL; i++ )
						{
							axe = GETAXEDATA(i);
							if( axe )
							{
								axe->ProcessHumidity( plage );
							}
						}
						GLOBALSETSTART
						SETLOADCURVE( ch_ready )
						SETREQUESTCURVE( 0x0000)
						GLOBALSETEND
						PRINT_DEBUG("S7API EventCallBack, REQUESTINJECTION, data loaded into buffer,\n")

					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: Request Injection %s \n", e.what() )
							return;
					}
					break;
				default:

					break;
			}
			break;
#define ___THE_AXE___ 0			
		case ___THE_AXE___+500:
			try
			{
			char			*theName;
			E_GlobalTypes	 theType;
			char			 theMode;
			int 			 theSize;

				if( decodeVarName( /*PEvent->EvtParam2,*/ db500_name[PEvent->EvtParam3], &theName, theType, theMode, theSize ) )
					if( theMode == 'w' )
					try
					{
					C_Connection 	*con;
					char 			*buf 			= (char *)(GETAXEDATA( ___THE_AXE___ )->HumidityBaseAddress());
					unsigned long	theInt;

						if( buf == NULL )
							break;
						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, Save setting, Error no mysql free connection\n")
							break;
						}
#define ___THE_DBNNAME___(a) 	___THE_DBNAME___(a,_name)
						PRINT_DEBUG("S7API EventCallBack, Save setting, Saving CH 0 setting in DB %s ( %d ) %x \n", theName, theType, *((unsigned short *)&buf[PEvent->EvtParam3]) )

						switch( theType )
						{
							case (E_GlobalTypes)BYTE:
							case (E_GlobalTypes)WORD:
							case (E_GlobalTypes)LONG:
								switch ( theType )
								{
									case (E_GlobalTypes)BYTE:
										theInt = (unsigned long ) buf[PEvent->EvtParam3];
										break;
									case (E_GlobalTypes)WORD:
										theInt = (unsigned long ) ( be16toh( *((unsigned short *)(&buf[PEvent->EvtParam3]))) & 0x0000FFFF);
										break;
									case (E_GlobalTypes)LONG:
										theInt = (unsigned long ) be32toh( *((unsigned long *)&buf[PEvent->EvtParam3]));
										break;
									default:
										break;
								}
								con->CreateGlobal( theName, &theInt, (E_GlobalTypes)INT );
								break;
							case (E_GlobalTypes)FLOAT:
								break;
							case (E_GlobalTypes)TEXT:
								break;
							default:
								break;
						}
						con->Release();
						switch(  PEvent->EvtParam3  )
						{
							case CHANNEL_LIMIT_AVG   *2:
							case CHANNEL_LIMIT_START *2:
							case CHANNEL_LIMIT_STOP  *2:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
										if( axe->HumidityLimitAverage() > 100 )		// TODO: get a global define
											axe->HumidityLimitAverage( 100 );		// maximum 100.....
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___)
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityLimitAverage( axe->HumidityLimitAverage() );
													axe2->HumidityLimitStart(	axe->HumidityLimitStart()	);
													axe2->HumidityLimitStop(	axe->HumidityLimitStop()	);
												}
											}
										}
		#endif
										axe->createLimitLine();
										GLOBALSETSTART
										SETLOADCURVE( 0xffff )
										SETREQUESTCURVE( 0x0000)
										SETLIMITAVERAGE	( ___THE_AXE___, axe->HumidityLimitAverage() )
										SETLIMITSTART	( ___THE_AXE___, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( ___THE_AXE___, axe->HumidityLimitStop()	 )
		#ifndef __USE_MANY_LIMIT__
										SETLIMITAVERAGE	( 1, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 1, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 1, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 2, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 2, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 2, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 3, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 3, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 3, axe->HumidityLimitStop()	 )
		#endif
										GLOBALSETEND
									}
								}
								break;
							case CHANNEL_FLAGS * 2:
							case (CHANNEL_FLAGS * 2) + 1:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___)
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityChannelLimitActive( axe->HumidityChannelLimitActive() );
												}
											}
										}
		#endif
		#ifndef __USE_MANY_LIMIT__
		#endif
									}
								}
								break;
							default:
								break;
						}
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: Save setting, Saving CH 0 setting in DB  %s \n", e.what() )
							return;
					}
			}
			catch (const std::exception& e )
			{
#undef ___THE_AXE___
				PRINT_ERROR(  "Exception S7API EventCallBack: Save setting, Saving CH 1 setting in DB %s \n", e.what() )
				return;
			}
			break;
#define ___THE_AXE___ 1
		case ___THE_AXE___+500:
			try
			{
			char			*theName;
			E_GlobalTypes	 theType;
			char			 theMode;
			int 			 theSize;

				if( decodeVarName( /*PEvent->EvtParam2,*/ db501_name[PEvent->EvtParam3], &theName, theType, theMode, theSize ) )
					if( theMode == 'w' )
					try
					{
					C_Connection 	*con;
					char 			*buf 			= (char *)(GETAXEDATA( ___THE_AXE___ )->HumidityBaseAddress());
					unsigned long	theInt;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, Save setting, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, Save setting, Save setting, Saving CH 1 setting in DB %s %x \n", theName, *((unsigned short *)&buf[PEvent->EvtParam3]))

						switch( theType )
						{
							case (E_GlobalTypes)BYTE:
							case (E_GlobalTypes)WORD:
							case (E_GlobalTypes)LONG:
								switch ( theType )
								{
									case (E_GlobalTypes)BYTE:
										theInt = (unsigned long ) buf[PEvent->EvtParam3];
										break;
									case (E_GlobalTypes)WORD:
										theInt = (unsigned long ) be16toh( *((unsigned short *)&buf[PEvent->EvtParam3]));
										break;
									case (E_GlobalTypes)LONG:
										theInt = (unsigned long ) be32toh( *((unsigned long *)&buf[PEvent->EvtParam3]));
										break;
									default:
										break;
								}
								con->CreateGlobal( theName, &theInt, (E_GlobalTypes)INT );
								break;
							case (E_GlobalTypes)FLOAT:
								break;
							case (E_GlobalTypes)TEXT:
								break;
							default:
								break;
						}
						con->Release();
						switch(  PEvent->EvtParam3  )
						{
							case CHANNEL_LIMIT_AVG   *2:
							case CHANNEL_LIMIT_START *2:
							case CHANNEL_LIMIT_STOP  *2:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
										if( axe->HumidityLimitAverage() > 100 )		// TODO: get a global define
											axe->HumidityLimitAverage(100);		// maximum 100.....
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___ )
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityLimitAverage( axe->HumidityLimitAverage() );
													axe2->HumidityLimitStart(	axe->HumidityLimitStart()	);
													axe2->HumidityLimitStop(	axe->HumidityLimitStop()	);
												}
											}
										}
		#endif
										axe->createLimitLine();
										GLOBALSETSTART
										SETLOADCURVE( 0xffff )
										SETREQUESTCURVE( 0x0000)
										SETLIMITAVERAGE	( ___THE_AXE___, axe->HumidityLimitAverage() )
										SETLIMITSTART	( ___THE_AXE___, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( ___THE_AXE___, axe->HumidityLimitStop()	 )
		#ifndef __USE_MANY_LIMIT__
										SETLIMITAVERAGE	( 0, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 0, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 0, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 2, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 2, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 2, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 3, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 3, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 3, axe->HumidityLimitStop()	 )
		#endif
										GLOBALSETEND
									}
								}
								break;
							case CHANNEL_FLAGS * 2:
							case (CHANNEL_FLAGS * 2) + 1:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___)
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityChannelLimitActive( axe->HumidityChannelLimitActive() );
												}
											}
										}
		#endif
		#ifndef __USE_MANY_LIMIT__
		#endif
									}
								}
								break;
							default:
								break;
						}
					}
					catch (const std::exception& e )
					{
#undef ___THE_AXE___
							PRINT_ERROR(  "Exception S7API EventCallBack: set Number of Channel  %s \n", e.what() )
							return;
					}
			}
			catch (const std::exception& e )
			{
				PRINT_ERROR(  "Exception S7API EventCallBack: Save setting %s \n", e.what() )
				return;
			}
			break;
#define ___THE_AXE___ 2
			case ___THE_AXE___+500:
			try
			{
			char			*theName;
			E_GlobalTypes	 theType;
			char			 theMode;
			int 			 theSize;

				if( decodeVarName( /*PEvent->EvtParam2,*/ db502_name[PEvent->EvtParam3], &theName, theType, theMode, theSize ) )
					if( theMode == 'w' )
					try
					{
					C_Connection 	*con;
					char 			*buf 			= (char *)(GETAXEDATA( ___THE_AXE___ )->HumidityBaseAddress());
					unsigned long	theInt;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, Save setting, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, Save setting, loading data into buffer %s \n", theName)

						switch( theType )
						{
							case (E_GlobalTypes)BYTE:
							case (E_GlobalTypes)WORD:
							case (E_GlobalTypes)LONG:
								switch ( theType )
								{
									case (E_GlobalTypes)BYTE:
										theInt = (unsigned long ) buf[PEvent->EvtParam3];
										break;
									case (E_GlobalTypes)WORD:
										theInt = (unsigned long ) be16toh( *((unsigned short *)&buf[PEvent->EvtParam3]));
										break;
									case (E_GlobalTypes)LONG:
										theInt = (unsigned long ) be32toh( *((unsigned long *)&buf[PEvent->EvtParam3]));
										break;
									default:
										break;
								}
								con->CreateGlobal( theName, &theInt, (E_GlobalTypes)INT );
								break;
							case (E_GlobalTypes)FLOAT:
								break;
							case (E_GlobalTypes)TEXT:
								break;
							default:
								break;
						}
						con->Release();
						switch(  PEvent->EvtParam3  )
						{
							case CHANNEL_LIMIT_AVG   *2:
							case CHANNEL_LIMIT_START *2:
							case CHANNEL_LIMIT_STOP  *2:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
										if( axe->HumidityLimitAverage() > 100 )		// TODO: get a global define
											axe->HumidityLimitAverage( 100 );		// maximum 100.....
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___)
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityLimitAverage( axe->HumidityLimitAverage() );
													axe2->HumidityLimitStart(	axe->HumidityLimitStart()	);
													axe2->HumidityLimitStop(	axe->HumidityLimitStop()	);
												}
											}
										}
		#endif
										axe->createLimitLine();
										GLOBALSETSTART
										SETLOADCURVE( 0xffff )
										SETREQUESTCURVE( 0x0000)
										SETLIMITAVERAGE	( ___THE_AXE___, axe->HumidityLimitAverage() )
										SETLIMITSTART	( ___THE_AXE___, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( ___THE_AXE___, axe->HumidityLimitStop()	 )
		#ifndef __USE_MANY_LIMIT__
										SETLIMITAVERAGE	( 1, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 1, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 1, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 0, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 0, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 0, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 3, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 3, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 3, axe->HumidityLimitStop()	 )
		#endif
										GLOBALSETEND
									}
								}
								break;
							case CHANNEL_FLAGS * 2:
							case (CHANNEL_FLAGS * 2) + 1:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___)
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityChannelLimitActive( axe->HumidityChannelLimitActive() );
												}
											}
										}
		#endif
		#ifndef __USE_MANY_LIMIT__
		#endif
									}
								}
								break;
							default:
								break;
						}
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: set Number of Channel  %s \n", e.what() )
							return;
					}
			}
			catch (const std::exception& e )
			{
#undef ___THE_AXE___
					PRINT_ERROR(  "Exception S7API EventCallBack: Save setting %s \n", e.what() )
					return;
			}
			break;
#define ___THE_AXE___ 3
		case ___THE_AXE___+500:
			try
			{
			char			*theName;
			E_GlobalTypes	 theType;
			char			 theMode;
			int 			 theSize;

				if( decodeVarName( /*PEvent->EvtParam2, */db503_name[PEvent->EvtParam3], &theName, theType, theMode, theSize ) )
					if( theMode == 'w' )
					try
					{
					C_Connection 	*con;
					char 			*buf 			= (char *)(GETAXEDATA( ___THE_AXE___ )->HumidityBaseAddress());
					unsigned long	theInt;

						con = GETDATABASELIBRARY->getDb();

						if( con == NULL )
						{
							PRINT_WARNING("S7API EventCallBack, Save setting, Error no mysql free connection\n")
							break;
						}

						PRINT_DEBUG("S7API EventCallBack, Save setting, loading data into buffer %s \n", theName)

						switch( theType )
						{
							case (E_GlobalTypes)BYTE:
							case (E_GlobalTypes)WORD:
							case (E_GlobalTypes)LONG:
								switch ( theType )
								{
									case (E_GlobalTypes)BYTE:
										theInt = (unsigned long ) buf[PEvent->EvtParam3];
										break;
									case (E_GlobalTypes)WORD:
										theInt = (unsigned long ) be16toh( *((unsigned short *)&buf[PEvent->EvtParam3]));
										break;
									case (E_GlobalTypes)LONG:
										theInt = (unsigned long ) be32toh( *((unsigned long *)&buf[PEvent->EvtParam3]));
										break;
									default:
										break;
								}
								con->CreateGlobal( theName, &theInt, (E_GlobalTypes)INT );
								break;
							case (E_GlobalTypes)FLOAT:
								break;
							case (E_GlobalTypes)TEXT:
								break;
							default:
								break;
						}
						con->Release();
						switch(  PEvent->EvtParam3  )
						{
							case CHANNEL_LIMIT_AVG   *2:
							case CHANNEL_LIMIT_START *2:
							case CHANNEL_LIMIT_STOP  *2:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
										if( axe->HumidityLimitAverage() > 100 )		// TODO: get a global define
											axe->HumidityLimitAverage( 100 );		// maximum 100.....
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___ )
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityLimitAverage( axe->HumidityLimitAverage() );
													axe2->HumidityLimitStart(	axe->HumidityLimitStart()	);
													axe2->HumidityLimitStop(	axe->HumidityLimitStop()	);
												}
											}
										}
		#endif
										axe->createLimitLine();
										GLOBALSETSTART
										SETLOADCURVE( 0xffff )
										SETREQUESTCURVE( 0x0000)
										SETLIMITAVERAGE	( ___THE_AXE___, axe->HumidityLimitAverage() )
										SETLIMITSTART	( ___THE_AXE___, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( ___THE_AXE___, axe->HumidityLimitStop()	 )
		#ifndef __USE_MANY_LIMIT__
										SETLIMITAVERAGE	( 1, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 1, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 1, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 2, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 2, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 2, axe->HumidityLimitStop()	 )
										SETLIMITAVERAGE	( 0, axe->HumidityLimitAverage() )
										SETLIMITSTART	( 0, axe->HumidityLimitStart()	 )
										SETLIMITSTOP	( 0, axe->HumidityLimitStop()	 )
		#endif
										GLOBALSETEND
									}
								}
								break;
							case CHANNEL_FLAGS * 2:
							case (CHANNEL_FLAGS * 2) + 1:
								{
								DB_AxeData *axe = GETAXEDATA( ___THE_AXE___ );

									if( axe )
									{
		#ifndef __USE_MANY_LIMIT__
										for(short i=0; i<GETNUMBEROFCHANNEL;i++)
										{
											if( i != ___THE_AXE___)
											{
											DB_AxeData *axe2 = GETAXEDATA( i );

												if( axe2 )
												{
													axe2->HumidityChannelLimitActive( axe->HumidityChannelLimitActive() );
												}
											}
										}
		#endif
		#ifndef __USE_MANY_LIMIT__
		#endif
									}
								}
								break;
							default:
								break;
						}
					}
					catch (const std::exception& e )
					{
							PRINT_ERROR(  "Exception S7API EventCallBack: set Number of Channel  %s \n", e.what() )
							return;
					}
			}
			catch (const std::exception& e )
			{
#undef ___THE_AXE___
					PRINT_ERROR(  "Exception S7API EventCallBack: Save setting %s \n", e.what() )
					return;
			}
			break;
		default:
			break;
		}
	}
	//((TS7Server *)usrPtr)->ClearEvents();
};

int DB_Global::initS7(void )
{
//    int Error;

	if( GLOBALSETSTART == NULL )
		return -1;

    GLOBALSETSTART
    SETS7SERVER( new TS7Server )
	GLOBALSETEND
	
	if( GETS7SERVER == NULL )					// may be not created
	{
		PRINT_ERROR( "Error creating S7 server \n" )
		return -1;
	}

	PRINT_DEBUG(" >>>>> VERSION %s <<<<<<<<<\n", GETVERSION )
	
	GETS7SERVER->RegisterArea(srvAreaDB, GLOBAL_SLAVE, 			GETBASEADDRESS, 	GETBLOCKSIZE	);

	GETS7SERVER->RegisterArea(srvAreaMK, 0,						&MStorage,			sizeof( MStorage ));
	if( GETAXEX )
	{
		GETAXEX->buildBlock();
		GETS7SERVER->RegisterArea(srvAreaDB, GETAXEX->ChannelID(), 	GETAXEX->HumidityBaseAddress(), 		GETAXEX->BlockSize()	);
	}

	for( int i = 0; i < GETNUMBEROFCHANNEL ; i++ )
	{
		GLOBALSETSTART
		SETAXEDATA( i,  new DB_AxeData( SIZE_CHANNEL_DATA, i, globals ) )
		GLOBALSETEND
		
		if( GETAXEDATA( i ) )
		{
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_PROCESS_BASE ,				// 10c
												 GETAXEDATA(i)->ProcessBaseAddress(),
												 GETAXEDATA(i)->BlockSize() );
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_TEMPERATURE_BASE ,			// 20c
												 GETAXEDATA(i)->TemperatureBaseAddress(),
												 GETAXEDATA(i)->BlockSize() );
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_VACCUM_BASE ,					// 30c
												 GETAXEDATA(i)->VaccumBaseAddress(),
												 GETAXEDATA(i)->BlockSize() );
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HVVACCUM_BASE ,				// 40c
												 GETAXEDATA(i)->HvVaccumBaseAddress(),
												 GETAXEDATA(i)->BlockSize() );
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_HUMIDITY_BASE ,				// 50c
												 GETAXEDATA(i)->HumidityBaseAddress(),
												 GETAXEDATA(i)->BlockSize() );
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE ,					// 60c
												 GETAXEDATA(i)->LimitLineBaseAddress(),
												 ( GETAXEDATA(i)->BlockSize() - ( VARIABLES_PAD * sizeof( unsigned short ) ))  );
			GETS7SERVER->RegisterArea(srvAreaDB, GETAXEDATA(i)->ChannelID() + CHANNEL_LIMIT_BASE + 50 ,					// 65c
												 GETAXEDATA(i)->LimitLineBaseAddress() + 1000 ,
												 ( GETAXEDATA(i)->BlockSize() - ( VARIABLES_PAD * sizeof( unsigned short ) ))  );
		}
		else
			PRINT_ERROR( "Error creating S7 channel data ID = %d \n", i )
	}
	
    GETS7SERVER->SetEventsMask(~evcDataRead);				// Only catch the write event
    GETS7SERVER->SetEventsCallback(GLOBALSETSTART->EventCallBack, GETS7SERVER);
    // Set the Read Callback
//    Server->SetReadEventsCallback(ReadEventCallBack, NULL);  // do not modify value on entry !

	//*******************  Setting Server Parameters *****************************
int32_t	PValue;

	PValue = 8;
	globals->S7Server()->SetParam( p_i32_MaxClients, &PValue);			// Set max clients access
	return 0;
};

int DB_Global::startS7( void )
{

int Error;
	
	if( GETS7SERVER == NULL )					// may be not created
	{
		PRINT_ERROR( "Error starting S7 server, no server initialised \n" )
		return 0x60;
	}
	
    Error = GETS7SERVER->StartTo("0.0.0.0");
	
    if (Error==0)
    {
		return 0;
    }
    PRINT_ERROR("%s\n",SrvErrorText(Error).c_str())
	GETS7SERVER->SetCpuStatus( S7CpuStatusRun );
	return 0x61;
};

int DB_Global::deleteS7( void )
{
	if( GETS7SERVER != NULL )
	{
		GETS7SERVER->SetCpuStatus( S7CpuStatusStop );
		PRINT_DEBUG(  "Destroing S7 server \n" )
		GETS7SERVER->Stop(); // <- not strictly needed, every server is stopped on deletion
						//    and every client is gracefully disconnected.
		delete GETS7SERVER;

		GLOBALSETSTART
		SETS7SERVER( NULL )
		GLOBALSETEND
	}
	return 0;
}

int DB_Global::clearS7Event( void )
{
	if( GETS7SERVER )
		GETS7SERVER->ClearEvents();
	return 0;
};

static const int	emptyString=0x0;

const char 		*getVarName(unsigned short db, unsigned short addr)
{
	switch( db )
	{
#define ___THE_DB___ 001
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 100
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 101
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 102
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 103
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 200
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 201
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 202
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 203
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 300
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 301
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 302
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 303
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 400
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 401
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 402
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 403
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 500
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 501
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 502
CASE_THE_DB
#undef  ___THE_DB___
#define ___THE_DB___ 503
CASE_THE_DB
		default:
			break;
	};
	return (const char *)&emptyString;

}