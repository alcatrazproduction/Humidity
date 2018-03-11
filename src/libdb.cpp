/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

#include "humidity.h"

#define	MYSQL_HOST	"tcp://db.arduino.local:3306"
#define MYSQL_USER	"arduinoyun"
#define MYSQL_PASS	"fo1806ch"
#define MYSQL_DB	"USE HumidityV2"

C_Connection::C_Connection( sql::Driver *driver)
{
	try
	{
		used 	= TRUE;
		online	= FALSE;

		if( driver == NULL )
		{
			PRINT_ERROR( "C_Connection::C_Connection, mysql driver is NULL\n" );
			return;
		}
		link 	= driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS );
		if( link == NULL )
		{
			PRINT_ERROR( "C_Connection::C_Connection, no link connection (NULL)\n" );
			return;
		}
		stmt	= link->createStatement();
		if( stmt == NULL )
		{
			fprintf( stderr, "C_Connection::C_Connection, no statement possible!! NULL\n" );
			return;
		}
		stmt->execute( MYSQL_DB );

		stmt_rglobals	= NULL;
		stmt_cglobals	= NULL;
		stmt_uglobals	= NULL;
		stmt_cinjection	= NULL;
		stmt_savedata4	= NULL;
		stmt_ccinfo		= NULL;

		online	= TRUE;
		used	= FALSE;
	}
	catch (const std::exception& e )
	{
			PRINT_ERROR( "Exception C_Connection::C_Connection: init %s \n", e.what() )
			return;
	}

};

bool	C_Connection::checkOpen( void )
{
	try
	{
		if( link )
			if( stmt )
				if( online )
				{
					stmt->execute( MYSQL_DB );
				}
	}
	catch (const std::exception& e )
	{
			PRINT_ERROR( "Exception C_Connection::checkOpen: %s \n", e.what() )
			online 	= FALSE;
			used	= TRUE;
	}
	
	return online;
};

bool	C_Connection::reConnect( sql::Driver *driver )
{
	if( online )
		return TRUE;
	
	used 	= TRUE;
	online	= FALSE;
	
	try
	{

		if( driver == NULL )
		{
			PRINT_ERROR( "C_Connection::C_Connection, mysql driver is NULL\n" );
			return online;
		}
		link 	= driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS );
		if( link == NULL )
		{
			PRINT_ERROR( "C_Connection::C_Connection, no link connection (NULL)\n" );
			return online;
		}
		stmt	= link->createStatement();
		if( stmt == NULL )
		{
			PRINT_ERROR( "C_Connection::C_Connection, no statement possible!! NULL\n" );
			return online;
		}
		stmt->execute( MYSQL_DB );

		stmt_rglobals	= NULL;
		stmt_cglobals	= NULL;
		stmt_uglobals	= NULL;
		stmt_cinjection	= NULL;
		stmt_savedata4	= NULL;
		stmt_ccinfo		= NULL;

		online	= TRUE;
		used	= FALSE;
	}
	catch (const std::exception& e )
	{
			PRINT_ERROR( "Exception C_Connection::reConnect: init %s \n", e.what() )
	}
	
	return online;
};

C_Connection::~C_Connection( )
{
	used 	= TRUE;
	online	= FALSE;

	if( stmt )
		delete stmt;
	if( link )
		delete link;
	link 	= NULL;
	stmt	= NULL;
}
/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::ReadGlobal( const char *theName, void *theValue)
{
		return -11111;
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::ReadGlobal( const char *theName, void *theValue, E_GlobalTypes theType )
{
sql::ResultSet	*res;

	if( theName == NULL )
		return 0x43;
	if( !online )
		return 0x40;
	if( !used )
		if( Obtain() == NULL )
			return 0x41;
	if( !stmt_rglobals )
		try
		{
			stmt_rglobals = link->prepareStatement( STMT_READGLOBALS );
		} catch (sql::SQLException &e)
		{
			PRINT_ERROR( "ERROR in C_C_Connection::ReadGlobal(3)[%s] prepareStatement: %s \n" , theName, e.what() );
			return( 0x42 );
		}
	if( !stmt_rglobals )
		return 0x42;
	try
	{
		stmt_rglobals->setString(1, theName );
		res = stmt_rglobals->executeQuery();
		if( !res )
			return 0x43;
		res->next();
		if( res->isNull( 1 ) )
		{
			PRINT_DEBUG(" ReadGlobal:: NULL value for %s\n", theName )
			return 0x43;
		}
	//	PRINT_DEBUG(" ReadGlobal::value for %s is ", theName )
		switch( theType )
		{
			case INT:
			case BYTE:
			case WORD:
			case LONG:
			{
			uint64_t	u;

				u = res->getUInt64(1);
				*((unsigned long *) theValue ) = (unsigned long ) u&0xffffffff;
				PRINT_DEBUG("%ld,%lx\n",*((unsigned long *) theValue ),*((unsigned long *) theValue ))
				break;
			}
			case FLOAT:
				*((double *) theValue ) = res->getDouble(1);
				PRINT_DEBUG("\n")
				break;
			case TEXT:
				strncpy( (char *)theValue, ((std::string )res->getString(1)).c_str(), SIZE_ASCII - 2 );
				PRINT_DEBUG("%s\n",(char *)theValue)
				break;
			default:
				return 0x43;
				break;
		}
		delete res;
	} catch (sql::SQLException &e)
	{
		PRINT_ERROR( "ERROR in C_C_Connection::ReadGlobal(3)[%s]: %s \n" , theName, e.what() );
		return( 0x43 );
	}
	
	return 0;
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::UpdateGlobal( const char *theName, void *theValue)
{
	if( theName == NULL )
		return 0x46;
	if( !online )
		return 0x44;
	if( !used )
		if( Obtain() == NULL )
			return 0x45;
	if( !stmt_uglobals )
		try
		{
			stmt_uglobals = link->prepareStatement( STMT_UPDATEGLOBALS );
		} catch (sql::SQLException &e)
		{
			PRINT_ERROR( "ERROR in C_C_Connection::UpdateGlobal(3)[%s] prepareStatement: %s \n" , theName, e.what() );
			return( 0x46 );
		}
	if( !stmt_uglobals )
		return 0x46;
	try
	{
		stmt_uglobals->setString( 1, theName );
		stmt_uglobals->setString( 2, (char *)theValue );
		stmt_uglobals->executeUpdate();
		return 0;
	} catch (sql::SQLException &e)
	{
		PRINT_ERROR( "ERROR in C_C_Connection::UpdateGlobal(3)[%s]: %s \n" , theName, e.what() );
		return( 0x46 );
	}
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::UpdateGlobal( const char *theName, void *theValue, E_GlobalTypes theType )
{
	if( theName == NULL )
		return 0x49;
	if( !online )
		return 0x47;
	if( !used )
		if( Obtain() == NULL )
			return 0x48;
	if( !stmt_uglobals )
		try
		{
			stmt_uglobals = link->prepareStatement( STMT_UPDATEGLOBALS );
		} catch (sql::SQLException &e)
		{
			PRINT_ERROR( "ERROR in C_C_Connection::UpdateGlobal(3)[%s] prepareStatement: %s \n" , theName, e.what() );
			return( 0x49 );
		}
	if( !stmt_uglobals )
		return 0x49;
	stmt_uglobals->setString( 1, theName );
	switch( theType )
	{
		case INT:
		case BYTE:
		case WORD:
		case LONG:
			stmt_uglobals->setUInt(2, *((unsigned long *) theValue ));
			break;
		case FLOAT:
			stmt_uglobals->setDouble( 2, *((double *) theValue ));
			break;
		case TEXT:
			stmt_uglobals->setString( 2, (char *)theValue);
			break;
		default:
			return 0x49;
			break;
	}
	try
	{
		stmt_uglobals->executeUpdate();
		return 0;
	} catch (sql::SQLException &e)
	{
		PRINT_ERROR( "ERROR in C_C_Connection::UpdateGlobal(3)[%s]: %s \n" , theName, e.what() );
		return( 0x49 );
	}
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::CreateGlobal( const char *theName, void *theValue, E_GlobalTypes theType )
{
	if( theName == NULL )
		return 0x4c;
	if( !online )
		return 0x4A;
	if( !used )
		if( Obtain() == NULL )
			return 0x4B;
	if( !stmt_cglobals )
		try
		{
			stmt_cglobals = link->prepareStatement( STMT_CREATEGLOBALS );
		} catch (sql::SQLException &e)
		{
			PRINT_ERROR( "ERROR in C_C_Connection::CreateGlobal(3)[%s] prepareStatement: %s \n" , theName, e.what() );
			return( 0x4C );
	}
	if( !stmt_cglobals )
		return 0x4C;
	stmt_cglobals->setString( 1, theName );
	switch( theType )
	{
		case INT:
		case BYTE:
		case WORD:
		case LONG:
			stmt_cglobals->setUInt(		2, *((unsigned long *) theValue ));
			stmt_cglobals->setString(	3, GB_TYPE_INT );
			break;
		case FLOAT:
			stmt_cglobals->setDouble( 	2, *((double *) theValue ));
			stmt_cglobals->setString(	3, GB_TYPE_FLOAT );
			break;
		case TEXT:
			stmt_cglobals->setString( 	2, (char *)theValue);
			stmt_cglobals->setString(	3, GB_TYPE_TEXT );
			break;
		default:
			return 0x4c;
			break;
	}
	try
	{
		stmt_cglobals->executeUpdate();
		return 0;
	} catch (sql::SQLException &e)
	{
		PRINT_ERROR( "ERROR in C_C_Connection::CreateGlobal(3)[%s]: %s \n" , theName, e.what() );
		return( 0x4C );
	}
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::LoadChannelCurve(DB_Global *GLOBALSETSTART,DB_AxeData *theChannel)
{
	if( theChannel == NULL )
	{
		PRINT_ERROR("ERROR in C_Connection::LoadChannelCurve: Channel is NULL \n" )
	}
	
int				theId 	=	theChannel->ChannelID();
char 			*sql	=	new char[ sizeof( READCHANNELFULL ) + 40];
unsigned short	hmin		=	4096;
unsigned short	hmax		=	0;
unsigned short	tmin		=	4096;
unsigned short	tmax		=	0;
unsigned short	vmin		=	4096;
unsigned short	vmax		=	0;
unsigned short	hvmin		=	4096;
unsigned short	hvmax		=	0;
short			hcal		=	444;
short			tcal		=	444;
short			vcal		=	444;


//READCHANNELFULL		"SELECT channel_%c FROM T_Humidity_Value WHERE injection_id = %ld)"

	if( !online )
		return 0x4D;
	if( !used )
		if( Obtain() == NULL )
			return 0x4E;
	snprintf( sql, sizeof( READCHANNELFULL ) + 38, READCHANNELFULL, theId + (unsigned int)'a', theId + (unsigned int)'a', theId + (unsigned int)'a',
																	theId + (unsigned int)'a', GETREQUESTINJECTION );
PRINT_DEBUG("The sql string %s\n", sql )
	try
	{
		if( stmt == NULL )
			stmt = link->createStatement();
		if( stmt == NULL )
			return 0x4F;

	sql::ResultSet	*res;

		res = stmt->executeQuery( sql );
		int index = 0;
		while( ( index < theChannel->NumSample() ) & ( res->next()))
		{
		unsigned short hum  = res->getUInt(1);
		unsigned short tem  = res->getUInt(2);
		unsigned short vac  = res->getUInt(3);
		unsigned short hvac = res->getUInt(4);

//			if( ( cal < 0 ) || (cal > 255 ) )
//				cal = (short)( res->getUInt(2) & 0x00000FFF );
			if( hum   > hmax )
				hmax  = hum;
			if( hum   < hmin )
				hmin  = hum;
			if( tem   > tmax )
				tmax  = tem;
			if( tem   < tmin )
				tmin  = tem;
			if( vac   > vmax )
				vmax  = vac;
			if( vac   < vmin )
				vmin  = vac;
			if( hvac  > hvmax )
				hvmax = hvac;
			if( hvac  < hvmin )
				hvmin = hvac;
			
			theChannel->SampleAt( index++, hum, tem, vac, hvac );
		}
		delete res;

		try
		{
			snprintf( sql, sizeof( READCCINFO ) + 38, READCCINFO,  GETREQUESTINJECTION,theId );
			res = stmt->executeQuery( sql );

			theChannel->HumidityMinValue( 			res->getUInt( 7  ) )
					->HumidityMaxValue( 			res->getUInt( 6  ) )
					->HumidityLoadCalibration( 		res->getUInt( 9  ) )
					->HumiditySamplesCount( 		res->getUInt( 4  ) )
					->TemperatureMinValue( 			res->getUInt( 12 ) )
					->TemperatureMaxValue( 			res->getUInt( 11 ) )
					->TemperatureLoadCalibration( 	res->getUInt( 14 ) )
					->TemperatureSamplesCount( 		res->getUInt( 4  ) )
					->VaccumMinValue( 				res->getUInt( 17 ) )
					->VaccumMaxValue( 				res->getUInt( 16 ) )
					->VaccumLoadCalibration( 		res->getUInt( 19 ) )
					->HvVaccumSamplesCount( 		res->getUInt( 4  ) )
					->HvVaccumMinValue( 			hvmin			   )
					->HvVaccumMaxValue( 			hvmax 			   )
					->HvVaccumLoadCalibration( 		444 )
					->HvVaccumSamplesCount( 		res->getUInt( 4  ) )
					->HumidityLimitAverage(			res->getUInt( 21 ) )
					->HumidityLimitStart(			res->getUInt( 22 ) )
					->HumidityLimitStop(			res->getUInt( 23 ) );

			delete res;
			
		} catch(sql::SQLException &e)
		{	
			theChannel->HumidityMinValue( 			hmin 						)
					->HumidityMaxValue( 			hmax 						)
					->HumidityLoadCalibration( 		hcal 						)
					->HumiditySamplesCount( 		index 						)
					->TemperatureMinValue( 			tmin 						)
					->TemperatureMaxValue( 			tmax 						)
					->TemperatureLoadCalibration( 	tcal 						)
					->TemperatureSamplesCount( 		index 						)
					->VaccumMinValue( 				vmin 						)
					->VaccumMaxValue( 				vmax 						)
					->VaccumLoadCalibration( 		vcal 						)
					->HvVaccumSamplesCount( 		index 						)
					->HvVaccumMinValue( 			hvmin 						)
					->HvVaccumMaxValue( 			hvmax 						)
					->HvVaccumLoadCalibration( 		444 						)
					->HvVaccumSamplesCount( 		index 						)
					->HumidityLimitAverage(			GETLIMITAVERAGE( theId )	)
					->HumidityLimitStart(			GETLIMITSTART( theId ) 		)
					->HumidityLimitStop(			GETLIMITSTOP( theId )		);
		}
		GLOBALSETSTART
		SETREQUESTCURVE( 0X0000 )
		SETLOADCURVE( 0xffff )
		GLOBALSETEND
		
	} catch (sql::SQLException &e)
	{
		PRINT_ERROR( "ERROR in C_Connection::LoadChannelCurve: %s \n" , e.what() );
		return 0x50;
	}
	
	return 0;
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::SaveChannelCurve(DB_Global *GLOBALSETSTART,DB_AxeData *theChannel)
{
	return 0;
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::SaveChannelSample(DB_Global *GLOBALSETSTART, short sample, DB_AxeData *cha, DB_AxeData *chb, DB_AxeData *chc, DB_AxeData *chd )
{// STMT_SAVEDATA4
	if( sample >= 0 )
	{
		if( !online )
			return 0x51;
		if( !used )
			if( Obtain() == NULL )
				return 0x52;
		if( !stmt_savedata4 )
			try
			{
				stmt_savedata4 = link->prepareStatement( STMT_SAVEDATA4 );
			} catch (sql::SQLException &e)
			{
				PRINT_ERROR( "ERROR in C_C_Connection::SaveChannelSample[] prepareStatement: %s \n" , e.what() );
				return( 0x53 );
			}
		if( !stmt_savedata4 )
			return 0x53;
		int index = 1;

		stmt_savedata4->setString(	index++ , 	(char *)GETPARTNAME								);
		stmt_savedata4->setUInt(	index++ , 	GETPRODUCTIONNUMBER								);
		stmt_savedata4->setUInt(	index++ , 	GETCURRENTINJECTION 							);

		stmt_savedata4->setUInt(	index++ , 	cha ? cha->HumiditySampleAt( sample )		: 0	);
		stmt_savedata4->setUInt(	index++ , 	chb ? chb->HumiditySampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ , 	chc ? chc->HumiditySampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ , 	chd ? chd->HumiditySampleAt( sample ) 		: 0	);

		stmt_savedata4->setUInt(	index++ , 	cha ? cha->TemperatureSampleAt( sample )	: 0	);
		stmt_savedata4->setUInt(	index++ , 	chb ? chb->TemperatureSampleAt( sample )	: 0	);
		stmt_savedata4->setUInt(	index++ ,	chc ? chc->TemperatureSampleAt( sample )	: 0	);
		stmt_savedata4->setUInt(	index++ ,	chd ? chd->TemperatureSampleAt( sample )	: 0	);

		stmt_savedata4->setUInt(	index++ , 	cha ? cha->VaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ , 	chb ? chb->VaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ ,	chc ? chc->VaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ ,	chd ? chd->VaccumSampleAt( sample ) 		: 0	);

		stmt_savedata4->setUInt(	index++ , 	cha ? cha->HvVaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ , 	chb ? chb->HvVaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ ,	chc ? chc->HvVaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ ,	chd ? chd->HvVaccumSampleAt( sample ) 		: 0	);
		stmt_savedata4->setUInt(	index++ ,	sample * GETSAMPLERATE							);

		try
		{
			stmt_savedata4->executeUpdate();

			return 0;
		} catch (sql::SQLException &e)
		{
			PRINT_ERROR( "ERROR in C_C_Connection::SaveChannelSample[]: %s \n" , e.what() );
			return( 0x53 );
		}
	}
	else
	{
		if( !online )
			return 0x51;
		if( !used )
			if( Obtain() == NULL )
				return 0x52;
		if( !stmt_ccinfo )
			try
			{
				stmt_ccinfo = link->prepareStatement( STMT_CREATECINFO );
			} catch (sql::SQLException &e)
			{
				PRINT_ERROR( "ERROR in C_C_Connection::SaveChannelSample[-1] prepareStatement: %s \n" , e.what() );
				return( 0x53 );
		}
		if( !stmt_ccinfo )
			return 0x53;

		for( int i = 0; i < 4; i++ ) 
		{
		DB_AxeData *ch;
		
			switch( i )
			{
				case 0:
					ch = cha;
					break;
				case 1:
					ch = chb;
					break;
				case 2: 
					ch = chc;
					break;
				case 3:
					ch = chd;
					break;
				default:
					ch = NULL;
			}
			if( ch )
			{
				int index = 1;

				stmt_ccinfo->setString(	index++ , 	(char *)GETPARTNAME								);
				stmt_ccinfo->setUInt(	index++ , 	GETPRODUCTIONNUMBER								);
				stmt_ccinfo->setUInt(	index++ , 	GETCURRENTINJECTION 							);

				stmt_ccinfo->setUInt(	index++ , 	i					 							);
				stmt_ccinfo->setUInt(	index++ , 	ch->HumiditySamplesCount()						);
				stmt_ccinfo->setUInt(	index++ , 	GETSAMPLERATE		 							);

				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityMaxValue(  )						);
				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityMinValue(  ) 						);
				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityChannelFlags(  ) 					);
				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityCalibration(  ) 					);
				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityCalibrationResult(  ) 				);

				stmt_ccinfo->setUInt(	index++ , 	ch->TemperatureMaxValue(  )						);
				stmt_ccinfo->setUInt(	index++ , 	ch->TemperatureMinValue(  )						);
				stmt_ccinfo->setUInt(	index++ ,	ch->TemperatureChannelFlags(  )					);
				stmt_ccinfo->setUInt(	index++ ,	ch->TemperatureCalibration(  )					);
				stmt_ccinfo->setUInt(	index++ ,	/*ch->TemperatureCalibrationResult(  )*/0			);

				stmt_ccinfo->setUInt(	index++ , 	ch->VaccumMaxValue(  ) 							);
				stmt_ccinfo->setUInt(	index++ , 	ch->VaccumMinValue(  ) 							);
				stmt_ccinfo->setUInt(	index++ ,	ch->VaccumChannelFlags(  ) 						);
				stmt_ccinfo->setUInt(	index++ ,	ch->VaccumCalibration(  ) 						);
				stmt_ccinfo->setUInt(	index++ ,	ch->VaccumCalibrationResult( )					);
	
				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityLimitAverage(  ) 					);
				stmt_ccinfo->setUInt(	index++ , 	ch->HumidityLimitStart(  ) 						);
				stmt_ccinfo->setUInt(	index++ ,	ch->HumidityLimitStop(  ) 						);
	/*			stmt_ccinfo->setUInt(	index++ ,	cha->HvVaccumSampleAt( sample ) 	: 0	);
	*/
				try
				{
					stmt_ccinfo->executeUpdate();
				} catch (sql::SQLException &e)
				{
					PRINT_ERROR( "ERROR in C_C_Connection::SaveChannelInfo Channel a[-1]: %s \n" , e.what() );
				}
			}
		}
	}
	return 0;
};

/*************************************************************************
 *
 * **********************************************************************/


unsigned long C_Connection::CreateNewInjection( DB_Global *GLOBALSETSTART )
{// STMT_CREATEINJECTION
DB_AxeData		*cha = GETAXEDATA( 0 );
DB_AxeData		*chb = GETAXEDATA( 1 );
DB_AxeData		*chc = GETAXEDATA( 2 );
DB_AxeData		*chd = GETAXEDATA( 3 );
unsigned long	ing;

	PRINT_DEBUG("In C_Connection::CreateNewInjection\n")

	if( !online )
		return 0;
	if( !used )
		if( Obtain() == NULL )
			return 0;
	if( !stmt_cinjection )
		try
		{
			stmt_cinjection = link->prepareStatement( STMT_CREATEINJECTION );
		}
		catch (sql::SQLException &e)
		{
			PRINT_ERROR( "ERROR in C_C_Connection::CreateNewInjection[] prepareStatement: %s \n" , e.what() );
		}
	if( !stmt_cinjection )
		return 0;

	ing = getIngNum();
	
	PRINT_DEBUG("In C_Connection::CreateNewInjection: Setting the params \n")

	stmt_cinjection->setString(	1, 	( char * )GETPARTNAME								);
	stmt_cinjection->setUInt(	2, 	GETPRODUCTIONNUMBER									);
	stmt_cinjection->setUInt(	3, 	ing + 1												);
	stmt_cinjection->setUInt(	4, 	GETT1												);
	stmt_cinjection->setUInt(	5, 	GETT2												);
	stmt_cinjection->setUInt(	6, 	GETT3												);
	stmt_cinjection->setUInt(	7, 	cha ? cha->HumidityCalibration( ) : 4111			);		// calibartion A. TODO: make sure tu pout the good value
	stmt_cinjection->setUInt(	8, 	chb ? chb->HumidityCalibration( ) : 4222			);		// calibartion B
	stmt_cinjection->setUInt(	9, 	chc ? chc->HumidityCalibration( ) : 4333			);		// calibartion C
	stmt_cinjection->setUInt(	10,	chd ? chd->HumidityCalibration( ) : 4444			);		// calibartion D
	stmt_cinjection->setUInt(	11, GETHVINJECTION	 									);

	PRINT_DEBUG("In C_Connection::CreateNewInjection: Do query\n")

	try
	{
		stmt_cinjection->executeUpdate();
	}
	catch (sql::SQLException &e)
	{
		PRINT_ERROR( "ERROR in C_C_Connection::CreateNewInjection[] executeUpdate: %s \n" , e.what() );
	}
	PRINT_DEBUG("In C_Connection::CreateNewInjection: Setting the new ing num\n")

	GLOBALSETSTART

	SETCURRENTINJECTION( ing + 1 )

	GLOBALSETEND

	setIngNum( GETCURRENTINJECTION );

	return GETCURRENTINJECTION;
}

/*************************************************************************
 *
 * **********************************************************************/

void C_Connection::getPartName( unsigned char *buffer, unsigned short &length)
{
	ReadGlobal( GB_VN_PARTNAME, buffer, GB_VT_PARTNAME );
}

/*************************************************************************
 *
 * **********************************************************************/

unsigned short C_Connection::getProdNum( )
{
unsigned long	buffer;
	
	ReadGlobal( GB_VN_PRODNUM, &buffer, GB_VT_PRODNUM );
	return (unsigned short)buffer;
}

/*************************************************************************
 *
 * **********************************************************************/

unsigned long C_Connection::getIngNum( )
{
unsigned long	buffer;

	ReadGlobal( GB_VN_INGNUM, &buffer, GB_VT_INGNUM );
	return (unsigned long)buffer;
}

/*************************************************************************
 *
 * **********************************************************************/

unsigned short C_Connection::getT1(  )
{
unsigned long	buffer;

	ReadGlobal( GB_VN_T1, &buffer, GB_VT_T1 );
	return (unsigned short)buffer;

}

/*************************************************************************
 *
 * **********************************************************************/

unsigned short C_Connection::getT2(  )
{
unsigned long	buffer;

	ReadGlobal( GB_VN_T2, &buffer, GB_VT_T2 );
	return (unsigned short)buffer;
}

/*************************************************************************
 *
 * **********************************************************************/

unsigned short C_Connection::getT3(  )
{
unsigned long	buffer;

	ReadGlobal( GB_VN_T3, &buffer, GB_VT_T3 );
	return (unsigned short)buffer;
}

/*************************************************************************
 *
 * **********************************************************************/

unsigned short C_Connection::getNumChannel( )
{
unsigned long	buffer;

	ReadGlobal( GB_VN_NUMCHANNEL, &buffer, GB_VT_NUMCHANNEL );
	PRINT_DEBUG(" Number of Channel from db is: %d\n",(unsigned short)buffer)
	return (unsigned short)buffer;
}

/*************************************************************************
 *
 * **********************************************************************/

unsigned short C_Connection::getSampleRate( )
{
unsigned long	buffer;

	ReadGlobal( GB_VN_SAMPLERATE, &buffer, GB_VT_SAMPLERATE );
	return (unsigned short)buffer;
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setPartName( unsigned char *buffer )
{
	return CreateGlobal( GB_VN_PARTNAME, buffer, GB_VT_PARTNAME );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setProdNum( unsigned short value)
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_PRODNUM, &buffer, GB_VT_PRODNUM );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setIngNum( unsigned long value)
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_INGNUM, &buffer, GB_VT_INGNUM );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setNumChannel( unsigned short value)
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_NUMCHANNEL, &buffer, GB_VT_NUMCHANNEL );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setSampleRate( unsigned short value)
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_SAMPLERATE, &buffer, GB_VT_SAMPLERATE );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setT1( unsigned short value )
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_T1, &buffer, GB_VT_T1 );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setT2( unsigned short value )
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_T2, &buffer, GB_VT_T2 );
}

/*************************************************************************
 *
 * **********************************************************************/

int C_Connection::setT3( unsigned short value )
{
unsigned long	buffer = (unsigned long) value;

	return CreateGlobal( GB_VN_T3, &buffer, GB_VT_T3 );
}

/*************************************************************************
 *
 * **********************************************************************/

C_LibDb::C_LibDb()
{
//	driver =  sql::mysql::get_driver_instance();
PRINT_DEBUG( "Trying to load mysql driver\n" )
	driver =  get_driver_instance();
	if( driver == NULL )
	{
		PRINT_ERROR( "C_LibDb::C_LibDb, cannot initialise mysql driver\n" );
	}
PRINT_DEBUG( "Initialising connection array\n" )
	for( unsigned int i = 0; i < sizeof( connectionList ) / sizeof( connectionList[0] ); i++ )
	{
PRINT_DEBUG( "\tInitialising connection %d\n",i )
		connectionList[ i ] = new C_Connection( driver );
	}
};

/*************************************************************************
 *
 * **********************************************************************/

C_Connection * C_LibDb::getDb()
{
	if( driver == NULL )
	{
	PRINT_DEBUG( "Trying to reload mysql driver\n" )
		driver =  get_driver_instance();
		if( driver == NULL )
		{
			PRINT_ERROR( "C_LibDb::getDb, cannot initialise mysql driver\n" );
			return NULL;
		}
	PRINT_DEBUG( "Initialising connection array\n" )
		for( unsigned int i = 0; i < sizeof( connectionList ) / sizeof( connectionList[0] ); i++ )
		{
	PRINT_DEBUG( "\tInitialising connection %d\n",i )
			connectionList[ i ]->reConnect( driver );
		}
	}
	for( unsigned int i = 0; i < sizeof( connectionList ) / sizeof( connectionList[0] ); i++ )
	{
		if( !connectionList[ i ]->checkOpen() )
			connectionList[ i ]->reConnect( driver );
		if( connectionList[ i ]->Obtain() )
			return connectionList[ i ];
	}
	return NULL;
};

/*************************************************************************
 *
 * **********************************************************************/

void C_LibDb::releaseDb( C_Connection *theLink )
{
	if( theLink )
		theLink->Release();
};
