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
// Prtototype definition for local function
void	DieOpenHandler		( int );			// RealTime 1, #34
void 	_DieOpenHandler		( int ); 			// RealTime 2, #35
void	VacuStartHandler	( int );			// RealTime 3, #36
void	DataReady			( int );			// RealTime 4, #37

pid_t				mainPid 	= 0;
pid_t				daemondPid 	= 0;
pid_t				terminalPid	= -1;

bool				isDaemon	= FALSE;
bool				isService	= FALSE;
volatile bool		running		= TRUE;

char				*logFile	= ( char *)&"/var/log/Humidity.log";

FILE 				*_stdin		= NULL;
FILE 				*_stdout	= NULL;
FILE 				*_stderr	= NULL;
int 				sockfd		= 0;
int 				newsockfd	= -1;
int 				portno		= 10080;

struct sockaddr_in 	serv_addr;
struct sockaddr_in 	cli_addr;
/*************************************************************************
 *
 * **********************************************************************/
void 	doExit( int sig )
{
	PRINT_ERROR(" I'am Sorry, must go.....\n");
	running = FALSE;
	close( 0 );
};

void	DieOpenHandler		( int sig )			// RealTime 1, #34
{
	if( sig == GETSIG_DIEOPEN )
		{
		short i;

			stopTimer();
			for( i = 0; i < GETNUMBEROFCHANNEL ; i++ )
			try
			{
			DB_AxeData *theChannel = GETAXEDATA(i);

				PRINT_DEBUG(" sending Calibration to channel %d \n", i )

				if( theChannel )
					theChannel->doCalibration( );
			} catch(std::exception &e )
			{
				PRINT_ERROR( "Error in DieOpen signal, calibration %s\n",  e.what() )
			}
			if( GETHIGHVAC )
			{
				if( GETHIGHVAC->openConnection() )
				{
					GLOBALSETSTART

					SETT1( GETHIGHVAC->getT1() )
					SETT2( GETHIGHVAC->getT2() )
					SETT3( GETHIGHVAC->getT3() )
					SETPRODUCTIONNUMBER( GETHIGHVAC->getProdNumber() )

					GLOBALSETEND
					GETHIGHVAC->closeConnection();
				}
				else
					PRINT_DEBUG("ERROR connecting to HV\n")
			}
		}
	else
		raise( sig );
}

/*************************************************************************
 *
 * **********************************************************************/

void 	_DieOpenHandler		( int sig ) 			// RealTime 2, #35
{
	if( sig == GETSIG__DIEOPEN )
		{
			PRINT_DEBUG("In Second _DieOpenHandler signal (main PID )\n")

			if( GETHIGHVAC )
			{
				if( GETHIGHVAC->openConnection() )
				{
				char	buffer[256];
					
					PRINT_DEBUG("In Second _DieOpenHandler signal Getting value from HV\n")

					GLOBALSETSTART

					SETT1( GETHIGHVAC->getT1() )
					SETT2( GETHIGHVAC->getT2() )
					SETT3( GETHIGHVAC->getT3() )
					SETPRODUCTIONNUMBER( GETHIGHVAC->getProdNumber() )

					GLOBALSETEND
					if( GETHIGHVAC->getPartName( &buffer[0], sizeof( buffer ) ) )
					{
						GLOBALSETSTART
						SETPARTNAME( (unsigned char *)&buffer[0] )
						GLOBALSETEND
					}
					GETHIGHVAC->setOutOfTolerance( FALSE );
					GETHIGHVAC->setNeed2Clean( FALSE );
					GETHIGHVAC->closeConnection();
				}
				else
					PRINT_DEBUG("ERROR connecting to HV\n")
			}
		}
	else
		raise( sig );
}

/*************************************************************************
 *
 * **********************************************************************/
void	VacuStartHandler	( int sig )			// RealTime 3, #36
{
	if( sig == GETSIG_VACUSTART )
		{
			PRINT_DEBUG("In VacuStartHandler signal (main PID )\n")
			PRINT_DEBUG("In VacuStartHandler signal (child PID )\n")
		}
	else
		raise( sig );
}

/*************************************************************************
 *
 * **********************************************************************/

void	DataReady			( int sig )			// RealTime 4, #37
{
	if( sig ==  GETSIG_DATAREADY )
	{ // TODO: 		Make the save in db routine.....
		PRINT_DEBUG("In DataReady signal\n")
	short			i;
	short			sample;
	C_Connection 	*con;
	DB_AxeData		**axe;
	bool			hvok 		= false;
	unsigned short	plage 		= PLAGE_AXE;
	unsigned short	ch_ready 	= 0xffff;
	unsigned short	max_limit	= 0;

		
		con = GETDATABASELIBRARY->getDb();

		if( con == NULL )
		{
			PRINT_WARNING("Signal DataReady, Error no mysql free connection\n")
			return;
		}
		axe = ( DB_AxeData ** ) new char[ ( GETNUMBEROFCHANNEL + 4) * sizeof( DB_AxeData *) ];		// TODO: for now, we must have 4 channel....
		
		if( axe == NULL )
		{
			PRINT_WARNING("Signal DataReady, no axe structure\n")
			return;
		}
		
		for( i = 0; i < ( GETNUMBEROFCHANNEL + 4); i++)
			axe[i] = NULL;
			
		sample 			= (  GETT2 + GETT3 ) / GETSAMPLERATE;

		con->setT1( GETT1 );
		con->setT2( GETT2 );
		con->setT3( GETT3 );
		con->setProdNum( GETPRODUCTIONNUMBER );
		con->setPartName( GETPARTNAME );
		
		PRINT_DEBUG("Signal DataReady, Creating injection\n")
		
		if( !con->CreateNewInjection( globals ) )
			return;

		PRINT_DEBUG("Signal DataReady, Saving Datas\n")

		if( GETHIGHVAC )
			hvok = GETHIGHVAC->openConnection();
		else
			PRINT_DEBUG("ERROR connecting to HV\n")
			
		for( i = 0; i < GETNUMBEROFCHANNEL ; i++ )
			if( ( axe[i] = GETAXEDATA( i ) ) != NULL )
			{
			unsigned short	max = axe[ i ]->HumidityMaxValue();
			unsigned short	min = axe[ i ]->HumidityMaxValue();
		
				if( plage < (( ( ( max - min ) / PLAGE_AXE ) + 1 ) * PLAGE_AXE ) )
					plage = (( ( ( max - min ) / PLAGE_AXE ) + 1 ) * PLAGE_AXE );
				ch_ready |= ( 1 << i ); 
				axe[ i ]->HumidityLoadCalibration( axe[ i ]->HumidityCalibration( ) ); // copy calibration data
				if( hvok )
				{
					PRINT_DEBUG("Signal DataReady, Getting Block from HV, %d sample, of channel %d\n", sample, i)
					GETHIGHVAC->getHVBlock( i, sample == 998 ? sample : 998 , &(axe[ i ]->HvVaccumBaseAddress()[VARIABLES_PAD]) );
					if( axe[i]->HumidityChannelLimitError() )
						GETHIGHVAC->setOutOfTolerance( TRUE );
				}
			}
			
		if( hvok )
			GETHIGHVAC->closeConnection();
		
		con->SaveChannelSample( globals, -1, axe[0], axe[1], axe[2], axe[3] );
		
		for( i = 0; i < sample; i++ )
		{
			con->SaveChannelSample( globals, i, axe[0], axe[1], axe[2], axe[3] );
		}
		
		for( i = 0; i < GETNUMBEROFCHANNEL ; i++ )
			if(  axe[i] != NULL )
			{
			unsigned short	tmp;
		
				axe[ i ]->ProcessHumidity( plage );
				
				tmp = ( axe[ i ]->HumidityMaxValue() - axe[ i ]->HumidityLimitValue() );

				if( tmp > max_limit )
					max_limit = tmp;
			}		
		PRINT_DEBUG("Signal DataReady, Saved %d of %d\n",i ,sample )
		con->Release();
		
		GLOBALSETSTART
		SETHVMAXLIMIT( max_limit )
		CLEARDATAREADY
		SETREQUESTINJECTION( GETCURRENTINJECTION - 1 )
		SETLOADCURVE( ch_ready )
		SETREQUESTCURVE( 0x00000 )
		GLOBALSETEND
		
		delete axe;

	}
	else
		raise( sig );
}

int readOneChar(short timeout )
{
int 	c;


	while( (  ( c  = fgetc( stdin ) ) <= 0 ) && ( timeout > 0 ) )
	{
//							fprintf( _stderr,".");
		timeout --;
		delayMicroseconds( 500 * 1000 );
	}
	return c;
}

/*************************************************************************
 *
 *	Main entry point for standalone version
 * 
 * **********************************************************************/
bool	checkConfig( void );

int main( int argc, char **argv)
{
int 		error;



	PRINT_DEBUG("In main fuction ( Entry PID is %d )\n", getpid())

	PRINT_DEBUG("Process arguments\n")

	try
	{
		for(int  i = 1; i < argc ; i++ )
		{
			char *thearg = argv[i];

			if( thearg[0] == '-' )
			{
				switch( thearg[1] )
				{
					case '?':
						PRINT_ERROR("Usage: %s [-DVd] \n", argv[0]);
						exit( 0 );
						break;
					case 'V':
						PRINT_ERROR("Version: %s \n", ARDUINO_SOFT_VERSION);
						break;
					case 'D':
						PRINT_DEBUG("  --> Daemon Mode \n");
						isDaemon = TRUE;
						break;
					case 'S':
						PRINT_DEBUG("  --> Service Mode \n");
						isService = TRUE;
						break;
					case 'd':
						PRINT_DEBUG("  --> Database selection \n");
						break;
					case 'l':
						PRINT_DEBUG("  --> Log file selection\n");
						if( strlen( thearg ) > 2 )
							logFile = &thearg[2];
						else
							if( ++i < argc )
								if( argv[i] )
								{
									logFile = argv[i];
									break;
								}
						PRINT_ERROR("No log file specified using default one %s\n",logFile );
						break;
					default:
						PRINT_ERROR("  ->> Unknow option %s\n",thearg );
				}
			}
			else
			{
			}
		}
	}catch(std::exception &e )
	{
		PRINT_ERROR( "Error Processing arguments  (  %s )\n",  e.what() );
	}
	
	
	mainPid 	= getpid();
	daemondPid 	= 0;
	terminalPid	= 0;

	if( isDaemon )
	{
		int rien;
		
        mainPid = fork();
		
        if (mainPid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (mainPid > 0) {
                exit(EXIT_SUCCESS);
        }
        /* Change the file mode mask */
        umask(0);

        /* Open any logs here */

        /* Create a new SID for the child process */
        daemondPid = setsid();
        if (daemondPid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }

		mainPid = daemondPid;

        /* Change the current working directory */
        if ((chdir("/")) < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }

        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        /* Daemon-specific initialization goes here */
		open ("/dev/null", O_RDWR);
		/* stdin */
		if( open (logFile, O_RDWR) == 0)
			rien = dup( 0 );
		/* stdout */
		rien = dup( 1 );
		/* stderror */
		if( rien )
		{
		}
	}
	
	init(); 					// Init all the stuff for hw pin....
	
	PRINT_DEBUG("In main:  initGlobals\n")
	
	if( ( error = initGlobals() ) )
	{
		PRINT_ERROR(  "Error initialising library %8.8x ( %s )\n", error, CAN_error[error] )
		if( error != 0x83 )
		{
			PRINT_ERROR("ERROR TOO BAD, Sorry ByeBye\n")
			return error;
		}
	}
	PRINT_DEBUG("In main:  Checking copy\n")
	if( !checkConfig() )
	{
		PRINT_ERROR("UNREGISTERED... Sorry ByeBye\n")
		return error;
	}
		
	GLOBALSETSTART

	SETSTANDALONE

	GLOBALSETEND
	
	PRINT_DEBUG("In main: Setup\n")
	
	setup();
	
	PRINT_DEBUG("In main: initialisaing Snap7\n")
	
	GLOBALSETSTART->initS7();
		
	PRINT_DEBUG("In main, initialising hardware\n")

	try
	{
	short i;
	
		hw_pinMode( PIN_MCP_INT, 		INPUT);
		hw_pinMode( PIN_INTREQ, 		INPUT);
		hw_pinMode( PIN_DIEOPEN, 		INPUT);
		hw_pinMode( PIN_RTC_INT, 		INPUT);
		hw_pinMode( PIN_VACUSTART,		INPUT);
		hw_pinMode( PIN_SPARE_INT,		INPUT);
		hw_pinMode( PIN_RTC_INT2, 		INPUT);
		hw_pinMode( PIN_INTACK, 		OUTPUT);

		digitalWrite( PIN_INTACK, LOW );
		
		PRINT_DEBUG("  Starting CAN Bus\n")
		
		GETCAN->begin( CAN_BPS_500K );			// Initialise CAN BUS. TODO: move absolute value to globals, and to de set by the s7
		
		PRINT_DEBUG("  Initialising channel (%d)\n", GETNUMBEROFCHANNEL )
		
		for( i = 0; i < GETNUMBEROFCHANNEL; i++ )
		{
		DB_AxeData *theChannel = GETAXEDATA(i);

			PRINT_DEBUG(" sending Calibration to channel %d (%s)\n", i, theChannel  	? "Inited":"NULL" )
			
			if( theChannel )
				theChannel->doCalibration( );
		}

	} catch(std::exception &e )
	{
		GLOBALSETSTART->deleteS7();
		PRINT_ERROR( "Error initialising Hardware %10x ( %s, %s )\n", error, CAN_error[error], e.what() );
		return error;
	}

	try
	{
	short	i;
	C_Connection *con;

		con = GETDATABASELIBRARY->getDb();

		if( con == NULL )
		{
			PRINT_WARNING("S7API EventCallBack, REQUESTINJECTION, Error no mysql free connection\n")
		}
		else
		{
		bool			found = FALSE;
		unsigned short 	max;
		unsigned short 	min;
		unsigned short 	plage 		= PLAGE_AXE;
		unsigned short	ch_ready 	= 0xffff;
		DB_AxeData		*axe;
		
			PRINT_DEBUG("Main, REQUESTINJECTION, loading data into buffer\n")

			GLOBALSETSTART
			SETREQUESTINJECTION( con->getIngNum() -1 )
			GLOBALSETEND
			for( i = 0; i < GETNUMBEROFCHANNEL ; i++ )
			{
				axe = GETAXEDATA(i);
				PRINT_DEBUG("    Main, REQUESTINJECTION, loading %ld in axe %d %s\n", GETREQUESTINJECTION, i, axe == NULL ? "Axe is NULL!!!":"")
				con->LoadChannelCurve( GLOBALSETSTART, axe );
				
				if( axe )
				{
					if( i == 0 )
					{
						if( axe->HumidityLoadCalibration() == 444 )
						{
							if( GETREQUESTINJECTION <= 1 )
								i = 9999;
							found = TRUE;
						}
						else
						{
							if( !found )
							{
								if( GETREQUESTINJECTION > 0 )
								{
									i = -1;
									GLOBALSETSTART
									SETREQUESTINJECTION( GETREQUESTINJECTION - 1 )
									GLOBALSETEND
								}
								else
									i = 9999;
							}
						}
					}
					if( found )
					{
						max = axe->HumidityMaxValue();
						min = axe->HumidityMaxValue();

						if( plage < (( ( ( max - min ) / PLAGE_AXE ) + 1 ) * PLAGE_AXE ) )
							plage = (( ( ( max - min ) / PLAGE_AXE ) + 1 ) * PLAGE_AXE );
						ch_ready |= ( 1 << i );
					}
				}
				else
					if( ( GETREQUESTINJECTION <= 1 ) & ( i == 0 ) )
						i = 9999;
			}
			con->Release();
			
			PRINT_DEBUG("Main, REQUESTINJECTION, Processing datas\n")
			
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
		}
	}
	catch(std::exception &e )
	{
		GLOBALSETSTART->deleteS7();
		PRINT_ERROR( "Error loading last shoot %10x ( %s, %s )\n", error, CAN_error[error], e.what() );
		return error;
	}
	
	PRINT_DEBUG( "Create Profinet connection to CPU \n")

	{
	char			ip[]		=" 192.168.001.010";
	unsigned long	theIp = GETHIGHVACIP;

		snprintf((char *)&ip,16,"%d.%d.%d.%d",(int)((theIp>>24)&0x00ff),(int)((theIp>>16)&0x00ff),(int)((theIp>>8)&0x00ff),(int)((theIp>>0)&0x00ff));

		PRINT_DEBUG("   Host info: IP: %s(%8.8lX), Rack: %d, Slot %d\n",ip,theIp,GETHVRACK, GETHVSLOT)

		GLOBALSETSTART
		SETHIGHVAC( new s7_HighVac( (char *)&ip,GETHVRACK, GETHVSLOT ) );	// MK1: 0 2; MK2 0 0
		GLOBALSETEND
	}
	PRINT_DEBUG( "Open Profinet connection to CPU ")
	
	if( GETHIGHVAC )
		if( GETHIGHVAC->openConnection() )
		{
			PRINT_DEBUG( "OK \n")
			GETHIGHVAC->closeConnection();
		}
		else
			PRINT_DEBUG( "Error\n")
	else
		PRINT_DEBUG( "no client structure\n")

	GETHIGHVAC->setT1Db( GETHVT1 )					->setT1Byte( GETHVT1BYTE )				->setT1DbSize( 4 )				->
				setT2Db( GETHVT2 )					->setT2Byte( GETHVT2BYTE )				->setT2DbSize( 4 )				->
				setT3Db( GETHVT3 )					->setT3Byte( GETHVT3BYTE )				->setT3DbSize( 4 )				->
				setProdNameDb( GETHVPARTNAME )		->setProdNameByte( GETHVPARTNAMEBYTE )	->setProdNameDbSize( 0x2a )		->
				setProdNumberDb( GETHVPRODNUM )		->setProdNumberByte( GETHVPRODNUMBYTE )	->setProdNumberDbSize( 2 )		->
				setShotNumberDb( GETHVSHOTNUM )		->setShotNumberByte( GETHVSHOTNUMBYTE )	->setShotNumberDbSize( 4 )		->
				setInVacuStartIn( GETHVVACUSTART )	->setInVacuStartBit( GETHVVACUSTARTBIT )								->
				setInDieOpenIn( GETHVDIEOPEN )		->setInDieOpenBit( GETHVDIEOPENBIT)										->
				setOuttol( GETHVOUTTOL )			->setOuttolByte( GETHVOUTTOLBYTE)		->setOuttolBit( GETHVOUTTOLBIT)	->
				setClean( GETHVCLEAN )				->setCleanByte( GETHVCLEANBYTE)			->setCleanBit( GETHVCLEANBIT);
	
	PRINT_DEBUG( "Initialise Signal Handler \n")
	
	signal( GETSIG_DIEOPEN, 	DieOpenHandler );			// RealTime 1, #34
	signal( GETSIG__DIEOPEN, 	_DieOpenHandler ); 			// RealTime 2, #35
	signal( GETSIG_VACUSTART, 	VacuStartHandler );			// RealTime 3, #36
	signal( GETSIG_DATAREADY, 	DataReady );				// RealTime 4, #37
	// we must catch all exit possible signals....
	signal( SIGQUIT,			doExit );					// do the exit on signal
	
	terminalPid = fork();

	_stdin	= stdin;
	_stdout	= stdout;
	_stderr	= stderr;

	PRINT_DEBUG("Pid of all: main(%d), daemon( %d ), terminal( %d ) active( %d ) globals = %lX\n", mainPid, daemondPid, terminalPid, getpid(), (long)globals );
	if(  terminalPid == 0 )
	{
		try
		{
			sockfd = socket(AF_INET, SOCK_STREAM , 0);
			if (sockfd < 0)
			{
				PRINT_ERROR("ERROR creating socket %d\n",errno);
				exit(1);
			}
			bzero((char *) &serv_addr, sizeof(serv_addr));
			
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = INADDR_ANY;
			serv_addr.sin_port = htons(portno);
			
			if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
			{
				PRINT_ERROR("ERROR binding socket %d\n",errno);
				exit(2);
			}
			
			listen( sockfd ,1 );
		}
		catch(std::exception &e )
		{
			PRINT_ERROR( "Error creating terminal access %s\n",e.what() );
			return -1000;
		}
	}
	else
	{	// we are in maim program....
		PRINT_DEBUG("In main: Starting Snap7\n")

		GLOBALSETSTART->startS7();

		PRINT_DEBUG( "In main: Initialise Interrupt Handler \n")

		attachInterrupt(INT_DIEOPEN, 	dispatchInterrupt, FALLING);

	}
	newsockfd = 0;
	while( running )
	{
		if( ( isDaemon || isService )&& !( 0 == terminalPid ))
		{
			sleep( 60 );
		}
		else
		{
			try
			{
			unsigned char	cmd;
			socklen_t 		clilen;
			
				if( 0 == terminalPid )
				{
					if( newsockfd >= 0 )
					{
						fflush(stdout);
//						stdin	= _stdin;
//						stdout 	= _stdout;
//						stderr 	= _stderr;
					}
					clilen		= sizeof( cli_addr );
					if( newsockfd == 0 )
						newsockfd 	= accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					if (newsockfd < 0)
					{
						stdin	= _stdin;
						stdout 	= _stdout;
						stderr 	= _stderr;
						cmd 	= 0;
					}
					else
					{
					short	wait = 150;
					int 	flags;
					int 	c;
					
						if( stdin == _stdin )
						{
							stdin	= fdopen( newsockfd,"a+");
							stdout	= stdin;
							stderr 	= stdin;
							flags 	= fcntl(newsockfd, F_GETFL, 0); 
							flags 	|= O_NONBLOCK;	
							fcntl(newsockfd, F_SETFL, flags);
						}
						while( (  ( c  = fgetc( stdin ) ) <= 0 ) && ( wait > 0 ) )
						{
//							fprintf( _stderr,".");
							wait --;
							delayMicroseconds( 500 * 1000 );
						}
//						fprintf( _stderr," (%x)\n",c );
						if( c < 2)
						{
							close( newsockfd );
							stdin		= _stdin;
							stdout 		= _stdout;
							stderr 		= _stderr;
							cmd 		= 0;
							newsockfd 	= 0;
							continue;
						}
						else
							cmd  = (unsigned char)c;
					}
				}
				else
				{
					stdin	= _stdin;
					stdout 	= _stdout;
					stderr 	= _stderr;
					cmd = getchar();
				}

				switch( cmd )
				{
					case 'q':
					case 'Q':
						running = FALSE;
						break;
						
					case 'a':
					case 'A':
						{
						bool	state = false;

						int 	signal = readOneChar( 2 );

							if( signal == 'h' )
								for( short i = 0; i < GETNUMBEROFCHANNEL; i++ )
								{
								DB_AxeData *theChannel = GETAXEDATA(i);

									PRINT_DEBUG(" Setting Error signal  " )

									if( theChannel )
										if( theChannel->HumidityChannelLimitActive() )
										{
											theChannel->HumidityChannelLimitError( cmd == 'A' );
											PRINT_DEBUG(" %s on channel %d\n ", cmd == 'A' ? "TRUE" : "FALSE", i  )

											if( cmd == 'A' )
												state = true;
										}
										else
										{
											PRINT_DEBUG(" ( not active on channel %d )\n",i)
										}
									else
									{
										PRINT_DEBUG(" Inactive channel (NULL) %d\n",i)
									}
								}
							else
							{
								if ( cmd == 'A' )
									state = true;
							}
							if( !GETHIGHVAC)
								break;
							if( GETHIGHVAC->openConnection() )
							{
								switch( signal )
								{
									case 'c':
									case 'C':
										GETHIGHVAC->setNeed2Clean( state )->closeConnection();
										break;
									case 'h':
									case 'H':
										GETHIGHVAC->setOutOfTolerance( state )->closeConnection();
										break;
									default:
										GETHIGHVAC->setOutOfTolerance( state )->setNeed2Clean( state )->closeConnection();
								}
							}							
						}
						break;

						break;
					case 'p':
						PRINTF (stdout,"Part name is %s \n", GETPARTNAME );
						break;
					case 'n':
						PRINTF (stdout,"Prodution number is %d \n", GETPRODUCTIONNUMBER );
						break;
					case 'i':
						PRINTF (stdout,"Injection number is %ld \n", GETCURRENTINJECTION );
						break;
					case 'I':
						PRINTF (stdout,"Part name is %s, Prodution number is %d, Injection number is %ld \n",
								GETPARTNAME, GETPRODUCTIONNUMBER, GETCURRENTINJECTION );
						break;
					case 's':
						PRINTF (stdout,"Read values from MK2: \n" );
						if( GETHIGHVAC->openConnection() )
						{
							PRINTF (stdout,"      DIE OPEN  ( active  ): %s  \n", GETHIGHVAC->getDieOpenState()  		? "HIGH":"LOW" );
							PRINTF (stdout,"      DIE OPEN  ( loosing ): %s  \n", GETHIGHVAC->getVaccumStartState() 	? "HIGH":"LOW" );
							PRINTF (stdout,"      T1                   : %ld \n", GETHIGHVAC->getT1() );
							PRINTF (stdout,"      T2                   : %ld \n", GETHIGHVAC->getT2() );
							PRINTF (stdout,"      T3                   : %ld \n", GETHIGHVAC->getT3() );
							PRINTF (stdout,"      Production Number    : %ld \n", GETHIGHVAC->getProdNumber() );
							PRINTF (stdout,"      Shot Number          : %ld \n", GETHIGHVAC->getShotNumber() );
							{
							char *buff = (char *)new char[0x30];

								GETHIGHVAC->getPartName( buff, 0x2a );
								int size = buff[1];
								if( size > 0x2a )
									size = 0x2a;
								buff[2+size] = 0;
								PRINTF (stdout,"      Part name            : %s\n", &buff[2]);
								delete buff;
							}
							GETHIGHVAC->closeConnection();
						}
						else
							PRINTF ( stdout, "      ERROR connecting to HV\n");
						PRINTF (stdout,"Read values from Local: \n" );
						PRINTF (stdout,"      T1                   : %d \n", GETT1 );
						PRINTF (stdout,"      T2                   : %d \n", GETT2 );
						PRINTF (stdout,"      T3                   : %d \n", GETT3 );
						PRINTF (stdout,"      Production Number    : %d \n", GETPRODUCTIONNUMBER );
						break;
					case 'S':
						PRINTF (stdout,"Active signals are: \n" );
						PRINTF (stdout,"      DIE OPEN  ( active  ): %s \n", GETDIEOPENRAISING  	? "HIGH":"LOW" );
						PRINTF (stdout,"      DIE OPEN  ( loosing ): %s \n", GETDIEOPENFALLING  	? "HIGH":"LOW" );
						PRINTF (stdout,"      VACUSTART ( active  ): %s \n", GETVACUSTART 			? "HIGH":"LOW" );
						PRINTF (stdout,"      DIE OPEN PIN         : %s \n", digitalRead( PIN_DIEOPEN )	? "HIGH":"LOW" );
						PRINTF (stdout,"      VACUSTART PIN        : %s \n", digitalRead( PIN_VACUSTART )	? "HIGH":"LOW" );
						break;
					case 'C':
						PRINTF (stdout,"User forced calibration: \n" );
						for( short i = 0; i < GETNUMBEROFCHANNEL; i++ )
						{
						DB_AxeData *theChannel = GETAXEDATA(i);

							PRINT_DEBUG(" sending Calibration to channel %d \n", i )

							if( theChannel )
								theChannel->doCalibration( );
						}
						break;
					case 'c':
						PRINTF (stdout,"Channels info ( %d ): \n", GETNUMBEROFCHANNEL );
						for( short i = 0; i < GETNUMBEROFCHANNEL; i++ )
						{
						DB_AxeData *theChannel = GETAXEDATA(i);

							PRINTF (stdout,"  Setting of channel %d \n", i );

							if( theChannel )
							{
								PRINTF (stdout,"     Connected             : %s \n", 			theChannel->HumidityChannelConnected() ? "TRUE":"FALSE" );
								PRINTF (stdout,"     Use A TX/RX           : %s \n", 			theChannel->HumidityChannelUseA() ? "TRUE":"FALSE" );
								PRINTF (stdout,"     Use B TX/RX           : %s \n", 			theChannel->HumidityChannelUseB() ? "TRUE":"FALSE" );
								PRINTF (stdout,"     Calibration           : %3.3d / 255\n", 	theChannel->HumidityCalibration());
								PRINTF (stdout,"     Calibration ( loaded) : %3.3d / 255\n", 	theChannel->HumidityLoadCalibration());
								PRINTF (stdout,"     Calibration Limit     : %3.3d / 255\n", 	theChannel->HumidityLoadCalibration());
								PRINTF (stdout,"     Limit Active          : %s \n", 			theChannel->HumidityChannelLimitActive() ? "TRUE":"FALSE" );
								PRINTF (stdout,"     Limit Error           : %s \n", 			theChannel->HumidityChannelLimitError() ? "TRUE":"FALSE" );
								PRINTF (stdout,"     Limit Actual Imposed  : %3.3d / 100\n",	theChannel->HumidityLimitAverage() );
								PRINTF (stdout,"     Limit Actual Value    : %4.3d / 100\n",	theChannel->HumidityLimitValue() );
								PRINTF (stdout,"     Limit Starting at     : %4.4d ms\n", 		theChannel->HumidityLimitStart() );
								PRINTF (stdout,"     Limit Stopping at     : %4.4d ms\n", 		theChannel->HumidityLimitStop() );
							}
							else
								PRINTF (stdout,"    No Record !\n");
						}
						break;
					case 'y':
						if( !GETHIGHVAC)
							break;
						PRINTF (stdout,"Active signals on HV ( from Profinet ) are: \n" );
						if( GETHIGHVAC->openConnection() )
						{
							PRINTF (stdout,"      DIE CLOSE            : %s \n", GETHIGHVAC->getDieCloseState()  	? "HIGH":"LOW" );
							PRINTF (stdout,"      EJECTOR OUT          : %s \n", GETHIGHVAC->getEjectorOutState()  	? "HIGH":"LOW" );
							PRINTF (stdout,"      SWITCH OVER          : %s \n", GETHIGHVAC->getSwitchOverState() 	? "HIGH":"LOW" );
							PRINTF (stdout,"      DIE OPEN             : %s \n", GETHIGHVAC->getDieOpenState()		? "HIGH":"LOW" );
							PRINTF (stdout,"      VACUSTART            : %s \n", GETHIGHVAC->getVaccumStartState()	? "HIGH":"LOW" );
							GETHIGHVAC->closeConnection();
						}
						else
							PRINTF ( stdout, "      ERROR connecting to HV\n");
						break;
					case 'r':
						PRINTF (stdout,"User Value reading: \n" );
						for( short i = 0; i < GETNUMBEROFCHANNEL; i++ )
						{
						c_GetMeasurement	cmd;
						int 				ret;

							PRINTF (stdout,"\n\t Channel: %d (%x) \n", i,i +0x20 );

							cmd.setId( i );
							ret = cmd.sendCmd();					// Send the GetInfo Command
							if( ret == 0 )
							{
								PRINT_ERROR("\t\tError during send c_GetMeasurement to %d, %d \n", (int)i,(int)ret);
							}
							delayMicroseconds( 20000 );
							ret = cmd.isResponseOK();
							if( ret > 0x80)
							{
								PRINT_ERROR("\t\tError during rx c_GetMeasurement to %d \n", (int)i );
							}
							PRINTF ( stdout, "\t\t Humidity Value           : %d \n", cmd.getHumidity( )								);
							PRINTF ( stdout, "\t\t Presure Value            : %d \n", cmd.getPressure( )								);
							PRINTF ( stdout, "\t\t Temperature Value        : %d \n", cmd.getTemperature( )							 	);
							PRINTF ( stdout, "\t\t TxLevel Value            : %d \n", cmd.getTxLevel( )								 	);
							PRINTF ( stdout, "\t\t AutoSetPower State       : %s \n", cmd.getAutoSetPower( ) 		? "TRUE":"FALSE" 	);
							PRINTF ( stdout, "\t\t AutoSetActive State      : %s \n", cmd.getAutoSetActive( ) 		? "TRUE":"FALSE" 	);
							PRINTF ( stdout, "\t\t ConfigurationError State : %s \n", cmd.getConfigurationError( ) 	? "TRUE":"FALSE" 	);
							PRINTF ( stdout, "\t\t AutoSetError State       : %s \n", cmd.getAutoSetError( ) 		? "TRUE":"FALSE" 	);
							PRINTF ( stdout, "\t\t TemperatureError State   : %s \n", cmd.getTemperatureError( ) 	? "TRUE":"FALSE" 	);
							PRINTF ( stdout, "\t\t ChannelUse Value         : %d \n", cmd.getChannelUse( )							 	);

						}
						break;
					case '+':		// signals forcing ( simulations )
						try
						{
						unsigned char	cmd;
						bool			v_status 	= false;
						bool			d_status	= false;
						bool			dr_status	= false;

							do
							{
								cmd = getchar();

								switch( cmd )
								{
									case 'v':
									case 'V':
										v_status 	= true;
										break;
									case 'd':
									case 'D':
										d_status	= true;
										break;
									case 'R':
										if( v_status )
											dr_status = true;
										break;
									default:
										break;
								};
							} while( !( (cmd == 10) || ( cmd ==13 ) ) );

							if( ( !GETDIEOPENRAISING ) 	&&
	//							( !GETDIEOPENFALLING ) 	&&
								( d_status )
	//							( v_status )	 		&&
	//							( GETVACUSTART )
								)	// getting dieOpen
							{
								GLOBALSETSTART
								SETDIEOPENRAISING
								CLEARVACUSTART
								CLEARDIEOPENFALLING
								GLOBALSETEND
								kill( mainPid, GETSIG_DIEOPEN );
								if( daemondPid != 0 )
									kill( daemondPid, GETSIG_DIEOPEN );
							}

							if( ( GETDIEOPENRAISING ) 	&&
								( !GETDIEOPENFALLING ) 	&&
								( !d_status ) 			&&
								( !v_status )	 		&&
								( !GETVACUSTART )
								)	// loosing dieOpen
							{
								GLOBALSETSTART
								SETDIEOPENRAISING
								SETDIEOPENFALLING
								GLOBALSETEND
								kill( mainPid, GETSIG__DIEOPEN );
							}

							if( ( GETDIEOPENRAISING ) 	&&
								( GETDIEOPENFALLING ) 	&&
								( !d_status ) 			&&
								( v_status )	 		&&
								( !GETVACUSTART )
								)	// gettingVacustart
							{
								GLOBALSETSTART
								CLEARDIEOPENRAISING
								SETVACUSTART
								CLEARDIEOPENFALLING
								GLOBALSETEND
								{
								short			i,j;
								short			sample;
								DB_AxeData		*axe[ GETNUMBEROFCHANNEL ];

									sample 			= (  GETT2 + GETT3 ) / GETSAMPLERATE;

									for( i = 0; i < GETNUMBEROFCHANNEL ; i++ )
										if( ( axe[i] = GETAXEDATA( i ) ) != NULL )
										{
											axe[ i ]->HumidityLoadCalibration( axe[ i ]->HumidityCalibration( ) ); // copy calibration data
											for(j = 0; j<sample; j++ )
											{
												axe[ i ]->SampleAt( j, 0x03ff & random(), 0x03ff & random(), 0x03ff & random() );
											}
											axe[ i ]->HumidityMaxValue(		  		0x03ff	)
													->HumidityMinValue( 			0	 	)
													->HumiditySamplesCount( 		sample	)
													->TemperatureMaxValue( 			0x03ff 	)
													->TemperatureMinValue( 			0 		)
													->TemperatureSamplesCount( 		sample	)
													->VaccumMaxValue( 				0x03ff 	)
													->VaccumMinValue( 				0 		)
													->VaccumSamplesCount( 			sample	);
										}
								}
								kill( mainPid, GETSIG_VACUSTART );
							}

							if( ( !GETDIEOPENRAISING ) 	&&
								( !GETDIEOPENFALLING ) 	&&
								( !d_status ) 			&&
								( !v_status )	 		&&
								( GETVACUSTART )
								)	// loosing Vacustart
							{
	//							GLOBALSETSTART
	//							CLEARDIEOPENRAISING
	//							CLEARVACUSTART
	//							CLEARDIEOPENFALLING
	//							GLOBALSETEND
	//							kill( mainPid, GETSIG_VACUSTART );
							}

							if( ( !GETDIEOPENRAISING ) 	&&
								( !GETDIEOPENFALLING ) 	&&
								( !d_status ) 			&&
								( v_status )	 		&&
								( dr_status )	 		&&
								( GETVACUSTART )
								)	// simul dataready
							{
	//							GLOBALSETSTART
	//							CLEARDIEOPENRAISING
	//							CLEARVACUSTART
	//							CLEARDIEOPENFALLING
	//							GLOBALSETEND
								kill( mainPid, GETSIG_DATAREADY );
							}

						}
						catch( ... )
						{
							PRINT_WARNING("Error in sim signals \n")
						}
						break;
					case 10:
					case 13:
					case 0:
						break;
					default:
						break;
					case '?':
						PRINTF ( stdout,"Pid is  %d \n", getpid() );
						PRINTF ( stdout,"Commands:\n"
										"\t[q,Q]\t	\t	\t	Quit the programm gracefully\n"
										"\t[p]\t	\t	\t	\t	Print the actual part name\n"
										"\t[n]\t	\t	\t	\t	Print the actual internal production number\n"
										"\t[i]\t	\t	\t	\t	Print the actual injection number\n"
										"\t[I]\t	\t	\t	\t	Print Info ( part name, production number and injection\n"
										"\t[s]\t	\t	\t	\t	Print some value read from MKII\n"
										"\t[S]\t	\t	\t	\t	Print the state of the signal on the board ( Hardware input )\n"
										"\t[c]\t	\t	\t	\t	Print the actual calibration value\n"
										"\t[C]\t	\t	\t	\t	Request a forced calibration of all connected humidity sensor\n"
										"\t[y]\t	\t	\t	\t	Try to read some signal via Profinet on the HV and print\n"
										"\t[r]\t	\t	\t	\t	Read and printout the actual values of the sensors\n"
										"\t[+]\t	\t	\t	\t	Simulate signal for testing:\n"
										"\t	\t[v,V]\t	\t	Vacustart\n"
										"\t	\t[d,D]\t	\t	DieOpen\n"
										"\t	\t[R]\t	\t	\t	Data Ready to be saved in DB\n\n"

										"\t	\t	\t	\t	\tWhen a signal is not set, assumed to be FALSE\n"
										"\t	\t	\t	\t	\texample: +Rd = Data Ready, die Open and no vacustart\n"
										"\t[a,A]\t	\t	\t	\t	Simulate alarm for humidity level [c,h]\n"
										"\t[W]db addr value	\t	Write a variable\n"
										"\t[R]db addr \t	\t	Read a variable\n"

								);
						break;
				}
			}
			catch( ... )
			{
				PRINT_WARNING("Error in main loop \n")
			}
		}
	}

	GLOBALSETSTART->deleteS7();
	GETCAN->end();
	if( daemondPid )
	{
		PRINT_DEBUG(" Killing daemond PID: %d\n", daemondPid )
		kill( daemondPid, SIGQUIT );
		daemondPid = 0;
	}
	if( terminalPid )
	{
		PRINT_DEBUG(" Killing terminal PID: %d\n", terminalPid )
		kill( terminalPid, SIGQUIT );
		kill( terminalPid, 9 );
		terminalPid = 0;
	}
	if( mainPid )
	{
		PRINT_DEBUG(" Killing main PID: %d\n", mainPid )
		kill( mainPid, SIGQUIT );
		mainPid = 0;
	}
	PRINT_DEBUG(" Exiting, all done, bye bye\n")
	return 0;
}
