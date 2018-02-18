/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

#include "fondarex.h"

DB_Global 	*GLOBALSETSTART					= NULL;

static		char *		_empty_string 		= (char *)"";
 
const		char *		CAN_error[] 		=
							{//************************** mcp_can error ***********************************************************************************
								(char *)"NO ERROR, ALL GOOD",																						// 0x00
								(char *)"CAN_FAIL_INIT",																							// 0x01
								(char *)"CAN_FAIL_TX",																								// 0x02
								(char *)"CAN_MSG_AVAIL",																							// 0x03
								(char *)"CAN_NO_MSG",																								// 0x04
								(char *)"CAN_CTRL_ERROR",																							// 0x05
								(char *)"CAN_GET_TX_BF_TIMEOUT",																					// 0x06
								(char *)"CAN_SEND_MSG_TIMEOUT",																						// 0x07
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x0f
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x17
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x1f
							//*************************** Interrupt error *********************************************************************************
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x27
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x2f
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x37
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x3f
							//*************************** libDb error *************************************************************************************
								(char *)"C_Connection::ReadGlobal, 			where are not online!",													// 0x40
								(char *)"C_Connection::ReadGlobal, 			cann't obtain connection",												// 0x41
								(char *)"C_Connection::ReadGlobal, 			Statemment not initialized",											// 0x42
								(char *)"C_Connection::ReadGlobal, 			Query don't have success",												// 0x43
								(char *)"C_Connection::UpdateGlobal(2),		where are not online!",													// 0x44
								(char *)"C_Connection::UpdateGlobal(2),		cann't obtain connection",												// 0x45
								(char *)"C_Connection::UpdateGlobal(2),		Statemment not initialized",											// 0x46
								(char *)"C_Connection::UpdateGlobal(3),		where are not online!",													// 0x47
								(char *)"C_Connection::UpdateGlobal(3),		cann't obtain connection",												// 0x48
								(char *)"C_Connection::UpdateGlobal(3),		Statemment not initialized",											// 0x49
								(char *)"C_Connection::CreateGlobal, 		where are not online!",													// 0x4A
								(char *)"C_Connection::CreateGlobal, 		cann't obtain connection",												// 0x4B
								(char *)"C_Connection::CreateGlobal, 		Statemment not initialized",											// 0x4C
								(char *)"C_Connection::LoadChannelCurve,	where are not online!",													// 0x4D
								(char *)"C_Connection::LoadChannelCurve, 	cann't obtain connection",												// 0x4E
								(char *)"C_Connection::LoadChannelCurve, 	Statemment not initialized",											// 0x4F
								(char *)"C_Connection::LoadChannelCurve, 	Sql error",																// 0x50
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x57
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x5f
							//*************************** s7_server error *********************************************************************************
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x67
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x6f
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x77
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x7f
							//*************************** Globals error ***********************************************************************************
								(char *)"initGlobals, Can't init DB_Globals",																		// 0x80
								(char *)"initGlobals, Can't init Axe X",																			// 0x81
								(char *)"initGlobals, Can't init Array of Axe",																		// 0x82
								(char *)"initGlobals, Can't init mysql connection",																	// 0x83
								_empty_string,_empty_string,_empty_string,_empty_string,															// 0x87
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x8f
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x97
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0x9f
							//*************************** Solutec error ***********************************************************************************
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xa7
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xaf
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xb7
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xbf
							//*************************** Spi error ***************************************************************************************
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xc7
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xcf
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xd7
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xdf
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xe7
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xef
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xf7
								_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,_empty_string,	// 0xfe
								(char *)"CAN_FAIL, General ERROR",
								NULL
							};

char * getArVersion( void)
{
	 return (char *)ARDUINO_SOFT_VERSION;
};

int initGlobals( void )
{
	try
	{
		PRINT_DEBUG("initGlobals, Creating DB_Global class\n")

		GLOBALSETSTART = new DB_Global();

		if( GLOBALSETSTART == NULL )
			return 0x80;

		PRINT_DEBUG("initGlobals, Creating DB_AxeX class\n")

		GLOBALSETSTART

	//	SETAXEX( new DB_AxeX( SIZE_CHANNEL_DATA , 99, globals ) )

		SETAXEX( NULL )

		GLOBALSETEND

	//	if( GETAXEX == NULL )		// no more axe X ( never used )
	//		return 0x81;

		PRINT_DEBUG("initGlobals, Creating DB_AxeData array class\n")

		if( !GETAXEARRAYINITED )
			return 0x82;

		PRINT_DEBUG("initGlobals, Checking C_LibDb class\n")

		if(  GETDATABASELIBRARY == NULL )
			return 0x83;

		return 0;
	}
	catch( std::exception &e )
	{
		PRINT_ERROR(" Exception catched in initGlobals\n")
		return 0x80;
	}	
}