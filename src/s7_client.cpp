/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

#include "fondarex.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;;

/*************************************************************************
 *
 * **********************************************************************/

bool checkHost( const char *RemAddress )
{
SOCKET 				sock;
struct hostent 		*hostinfo 	= NULL;
SOCKADDR_IN 		sin 		= { 0 }; /* initialise la structure avec des 0 */

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
			return FALSE;
	}

	hostinfo = gethostbyname( RemAddress ); /* on récupère les informations de l'hôte auquel on veut se connecter */
	if (hostinfo == NULL) /* l'hôte n'existe pas */
	{
		PRINT_ERROR( "Unknown host %s.\n", RemAddress )
		return FALSE;
	}
	PRINT_DEBUG("Host is : %s\n", RemAddress )
	
	sin.sin_addr 	= *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port 	= htons(102); /* on utilise htons pour le port */
	sin.sin_family 	= AF_INET;

	if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		return FALSE;
	}
	close( sock );

	return TRUE;

}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac::s7_HighVac( const char *RemAddress, int Rack, int Slot )
{
	IP = NULL;
	setAddress( RemAddress )
	->setRack( Rack )
	->setSlot( Slot );

	connected = false;

	if( MAX_HV_CH >= 4 )
	{
		hv_Vaccum_DB[0]		= 122;
		hv_Vaccum_byte[0]	= 0;
		hv_Vaccum_DB[1]		= 123;
		hv_Vaccum_byte[1]	= 0;
		hv_Vaccum_DB[2]		= 522;
		hv_Vaccum_byte[2]	= 0;
		hv_Vaccum_DB[3]		= 523;
		hv_Vaccum_byte[3]	= 0;
	}

	green_byte		= HV_GREEN_ADDR;
	green_bit		= HV_GREEN_BIT;
	red_byte		= HV_RED_ADDR;
	red_bit			= HV_RED_BIT;
	greenext_byte	= HV_GREENEXT_ADDR;
	greenext_bit	= HV_GREENEXT_BIT;
	redext_byte		= HV_REDEXT_ADDR;
	redext_bit		= HV_REDEXT_BIT;
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::openConnection( void )
{
int res;

	if( IP == NULL )
		return false;
	
	PRINT_DEBUG( "Check if host alive \n")

	if( !checkHost( IP ) )
	{
		connected = false;
		return FALSE;
	}
	if( connected )
		return true;
	
	PRINT_DEBUG( "Open real S7 connection \n")
	
	res = ConnectTo( IP, theRack, theSlot );
	if( res )
	{
		PRINT_ERROR(" s7_HighVac::openConnection( void ) ERROR connecting to %s, %d, %d \n\t\t%s\n",
					IP, theRack, theSlot, CliErrorText( res ).c_str() );
		connected = false;
		return false;
	}
	connected = true;
	return true;
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::closeConnection( void )
{
	if( connected )
	{
		Disconnect();
		connected = false;
	}
	return false;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setAddress( const char *RemAddress )
{
	if( IP )
		delete IP;
	IP = new char[ strlen( RemAddress ) + 2 ];
	if( IP )
		strcpy( IP, RemAddress );
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRack( int Rack )
{
	theRack = Rack;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setSlot( int Slot )
{
	theSlot = Slot;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setOuttol( int db )
{
	hv_outtol = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/
s7_HighVac	*s7_HighVac::setOuttolByte( int db )
{
	hv_outtol_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/
s7_HighVac	*s7_HighVac::setOuttolBit( int db )
{
	hv_outtol_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setClean( int db )
{
	hv_clean = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/
s7_HighVac	*s7_HighVac::setCleanByte( int db )
{
	hv_clean_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/
s7_HighVac	*s7_HighVac::setCleanBit( int db )
{
	hv_clean_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

int16_t		s7_HighVac::getInputByte( int address )
{
uint8_t	buffer[4];

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0x8000;
	}
	
	if( EBRead(  address, 1,  &buffer ) )
		return 0x8000 + buffer[0];
	return 0x0000 + buffer[0];
}

/*************************************************************************
 *
 * **********************************************************************/

int16_t		s7_HighVac::getOutputByte( int address )
{
uint8_t	buffer[4];

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0x8000;
	}

	if( ABRead(  address, 1,  &buffer ) )
		return 0x8000 + buffer[0];
	return 0x0000 + buffer[0];
}

/*************************************************************************
 *
 * **********************************************************************/

bool	s7_HighVac::setOutputByte( int address, uint8_t buffer )
{
	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return false;
	}

	if( ABWrite(  address, 1,  &buffer ) )
		return false;
	return true;
}

/*************************************************************************
 *
 * **********************************************************************/

int16_t		s7_HighVac::getDbByte( short db, short address )			// if negative, ERROR
{
uint8_t			buffer[4];
unsigned int 	ret;
	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0x8000;
	}

	PRINT_DEBUG("int16_t s7_HighVac::getDbByte( short db, short address ) Db %d ; Addr: %d ( %d ) -> ",db,address, address & 0xFFFE)
		
	if( ( ret = DBRead(  db, address  , 1,  &buffer ) ) )
	{
		PRINT_DEBUG(" Error: %x \n",ret)
		return 0x8000 + buffer[ 0 ];
	}
	PRINT_DEBUG(" %x\n",  0x0000 + buffer[ 0 ] )
	return 0x0000 + buffer[ 0 ];
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::setMBit( short MAddr, short Mbit, bool state )
{
unsigned char	buffer;
unsigned char	mask;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return false;
	}
	if( MBRead(  MAddr, 1,  &buffer ) )
		return false;

	mask = ( 1<<Mbit );
	
	PRINT_DEBUG("int16_t s7_HighVac::setMBit( short MAddr, short Mbit, bool state ) M %d . %d ( %2x, mask: %2x ~%2x ) -> ",MAddr,Mbit, buffer & 0xFFFF, (short)mask, (short)~mask)

	buffer = (( buffer & (~ mask ) ) );
	
	PRINT_DEBUG(" %2x -> ",    buffer )
	
	buffer = buffer | ( state ? mask : 0);
	
	PRINT_DEBUG(" %2x\n",    buffer )
	
	if( MBWrite(  MAddr, 1,  &buffer ) )
		return false;
	return true;
}
/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getMBit( short MAddr, short Mbit, bool &state )
{
unsigned char	buffer;
	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return false;
	}
	
	if( MBRead(  MAddr, 1,  &buffer ) )
		return false;

	state = (( buffer & (( 1<<Mbit ) ) ) ) ? TRUE : FALSE;
	return true;
}
/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::setDbByte( short db, short address, uint8_t buffer )
{
	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return false;
	}

	if( DBWrite(  db, address, 1,  &buffer ) )
		return false;
	return true;
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getDieCloseState( void )
{
	return getInputByte( HV_DIECLOSE_ADDR ) 		& (1 << HV_DIECLOSE_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getEjectorOutState( void )
{
	return getInputByte( HV_EJECTOUT_ADDR ) 		& (1 << HV_EJECTOUT_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getSwitchOverState( void )
{
	return getInputByte( HV_SWITCHOVER_ADDR )		& (1 << HV_SWITCHOVER_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getPressSettingState( void )
{
	return getInputByte( HV_SETTING_ADDR ) 		& (1 << HV_SETTING_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getVaccumLockState( void )
{
	return getInputByte( HV_VACCUMLOCK_ADDR )		& (1 << HV_VACCUMLOCK_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getDieOpenState( void )
{
	return getInputByte( dieOpen_byte ) 		& (1 << dieOpen_bit 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getVaccumStartState( void )
{
	return getInputByte( vacuStart_byte )	& (1 << vacuStart_bit	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getSprayerState( void )
{
	return getInputByte( HV_SPRAYER_ADDR ) 		& (1 << HV_SPRAYER_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getAirDirectState( void )
{
	return getInputByte( HV_AIRDIRECT_ADDR ) 		& (1 << HV_AIRDIRECT_BIT 	);
};

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getVaccumDirectState( void )
{
	return getInputByte( HV_VACCUMDIRECT_ADDR )	& (1 << HV_VACCUMDIRECT_BIT);
};

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setGreenLamp( bool state )
{
int16_t	b = getOutputByte( green_byte )	& (~(1 << green_bit));

	if( b < 0 )
	{
		if( state )
			setOutputByte( green_byte, b | (1 << green_bit) );
		else
			setOutputByte( green_byte, b  );
	}
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setGreenExtLamp( bool state )
{
int16_t	b = getOutputByte( greenext_byte )	& (~(1 << greenext_bit));

	if( b < 0 )
	{
		if( state )
			setOutputByte( greenext_byte, b | (1 << greenext_bit) );
		else
			setOutputByte( greenext_byte, b  );
	}
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRedLamp( bool state )
{
int16_t	b = getOutputByte( red_byte )	& (~(1 << red_bit));

	if( b < 0 )
	{
		if( state )
			setOutputByte( red_byte, b | (1 << red_bit) );
		else
			setOutputByte( red_byte, b  );
	}
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRedExtLamp( bool state )
{
int16_t	b = getOutputByte( redext_byte )	& (~(1 << redext_bit));

	if( b < 0 )
	{
		if( state )
			setOutputByte( redext_byte, b | (1 << redext_bit) );
		else
			setOutputByte( redext_byte, b  );
	}
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setOutOfTolerance( bool state )
{
	/*
int16_t	b = getDbByte( hv_outtol, hv_outtol_byte )	& (~(1 << hv_outtol_bit ));

	if( b >= 0 )
	{
		if( state )
			setDbByte( hv_outtol, hv_outtol_byte, b | (1 << hv_outtol_bit) );
		else
			setDbByte( hv_outtol, hv_outtol_byte, b  );
	}*/
	setMBit( hv_outtol_byte, hv_outtol_bit, state );
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/
s7_HighVac	*s7_HighVac::setNeed2Clean( bool state )
{/*
int16_t	b = getDbByte( hv_clean, hv_clean_byte )	& (~(1 << hv_clean_bit));

	if( b >= 0 )
	{
		if( state )
			setDbByte( hv_clean, hv_clean_byte, b | (1 << hv_clean_bit) );
		else
			setDbByte( hv_clean, hv_clean_byte, b  );
	}
	else
	{
		PRINT_DEBUG( "s7_HighVac *s7_HighVac::setNeed2Clean( bool state ) [DB%d:%d.%d] error on Read (%8x,%8x)\n",hv_clean, hv_clean_byte, hv_clean_bit, b, getDbByte( hv_clean, hv_clean_byte ) );
	}*/
	
	setMBit( hv_clean_byte, hv_clean_bit, state );
	
	return this;
}
	
/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT1Db( int db )
{
	hv_T1_DB = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT1Byte( int db )
{
	hv_T1_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT1DbSize( int db )
{
	hv_T1_Size = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT2Db( int db )
{
	hv_T2_DB = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT2Byte( int db )
{
	hv_T2_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT2DbSize( int db )
{
	hv_T2_Size = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT3Db( int db )
{
	hv_T3_DB = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT3Byte( int db )
{
	hv_T3_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setT3DbSize( int db )
{
	hv_T3_Size = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setProdNameDb( int db )
{
	hv_ProductionName_DB = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setProdNameByte( int db )
{
	hv_ProductionName_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setProdNameDbSize( int db )
{
	hv_ProductionName_Size = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setProdNumberDb( int db )
{
	hv_PartNum_DB = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setProdNumberByte( int db )
{
	hv_PartNum_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setProdNumberDbSize( int db )
{
	hv_PartNum_Size = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setShotNumberDb( int db )
{
	hv_ShotNum_DB = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setShotNumberByte( int db )
{
	hv_ShotNum_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setShotNumberDbSize( int db )
{
	hv_ShotNum_Size = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setInVacuStartIn( int db )
{
	vacuStart_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setInVacuStartBit( int db )
{
	vacuStart_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setInDieOpenIn( int db )
{
	dieOpen_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setInDieOpenBit( int db )
{
	dieOpen_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setGreenOut( int db )
{
	green_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/
s7_HighVac	*s7_HighVac::setGreenBit( int db )
{
	green_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRedOut( int db )
{
	red_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRedBit( int db )
{
	red_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setGreenExtOut( int db )
{
	greenext_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setGreenExtBit( int db )
{
	greenext_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRedExtOut( int db )
{
	redext_byte = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setRedExtBit( int db )
{
	redext_bit = db;
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

long		s7_HighVac::getT1( void )
{
unsigned long buff;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0;
	}

	DBRead(hv_T1_DB, hv_T1_byte, 4, &buff);
	return be32toh( buff );
}

/*************************************************************************
 *
 * **********************************************************************/

long		s7_HighVac::getT2( void )
{
unsigned long buff;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0;
	}

	DBRead(hv_T2_DB, hv_T2_byte, 4, &buff);
	return be32toh( buff );
}

/*************************************************************************
 *
 * **********************************************************************/

long		s7_HighVac::getT3( void )
{
unsigned long buff;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0;
	}

	DBRead(hv_T3_DB, hv_T3_byte, 4, &buff);
	return be32toh( buff );
}

/*************************************************************************
 *
 * **********************************************************************/

long		s7_HighVac::getProdNumber( void )
{
unsigned short buff;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0;
	}

	DBRead(hv_ProductionName_DB, hv_ProductionName_byte, 2, &buff);
	return be16toh( buff );
}

/*************************************************************************
 *
 * **********************************************************************/

long		s7_HighVac::getShotNumber( void )
{
unsigned long buff;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0;
	}

	DBRead(hv_ShotNum_DB, hv_ShotNum_byte, 4, &buff);
	return be32toh( buff );
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getPartName( char *buffer, int len)
{
	if( len > hv_ProductionName_Size )
		len = hv_ProductionName_Size;

	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return 0;
	}

	DBRead( hv_ProductionName_DB, hv_ProductionName_byte, len, buffer);
	return 0;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setHvVaccumDb( int channel, int db )
{
	if( channel < MAX_HV_CH )
	{
		hv_Vaccum_DB[channel] = db;
	}
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

s7_HighVac	*s7_HighVac::setHvVaccumByte( int channel, int db )
{
	if( channel < MAX_HV_CH )
	{
		hv_Vaccum_byte[channel] = db;
	}
	return this;
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getHVBlock( int channel, int size, void *buffer )
{
	if( !connected )
	{
		PRINT_DEBUG( "no connection \n")
		return false;
	}
	if( channel < MAX_HV_CH )
	{
	int ret;

		for( int j=0; j < size ; j +=100 )
		{
			if( (ret = DBRead( hv_Vaccum_DB[channel], hv_Vaccum_byte[channel] + j, j < ( size - 100 ) ? 100 : size - j,
						  &(((char *)buffer)[j])) ) )
			{
				PRINT_DEBUG("s7_HighVac::getHVBlock error %x (%s) of %d (DB %d.%d  )\n", ret,CliErrorText( ret ).c_str(), size, hv_Vaccum_DB[channel], hv_Vaccum_byte[channel] + j)
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

/*************************************************************************
 *
 * **********************************************************************/

bool		s7_HighVac::getHostAlive( void )
{
	return checkHost( IP );
}

