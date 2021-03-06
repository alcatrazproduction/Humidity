/*****************************************************************************
 * File:		interrupt.cpp
 * Author:		Yves Huguenin
 * Date:		27.06.2014
 * Version:		0.1
 * Description:	All the stuff for the interrupt process, and mesurement
 *
 * **************************************************************************/


#include "humidity.h"

extern	pid_t		mainPid;
extern	pid_t		childPid;

/****************************************************************************
 * void dieOpenInterrupt()
 *
 * called when the signal DieOpen get active.
 *
 * Input:	None
 * Output:	None
 *
 ***************************************************************************/

void dieOpenInterrupt()
{
	if( globals == NULL )
		return;
	if( GETDIEOPENRAISING )	// Check if we allready get in for this cycle..
		return;

	GLOBALSETSTART
	SETDIEOPENRAISING
	CLEARVACUSTART
	CLEARDIEOPENFALLING
	GLOBALSETEND
	
	PRINT_DEBUG("In First DieOpenHandler interrupt\n")
	
//	stopTimer();

	if( GETDATAREADY )
		raise( GETSIG_DATAREADY );
	
	if( GETSTANDALONE )
		raise( GETSIG_DIEOPEN );
	else
	{
		stopTimer();
		sendSIGUSR1();
	}
}

/****************************************************************************
 * void _dieOpenInterrupt()
 *
 * called when the signal DieOpen get inactive.
 *
 * Input:	None
 * Output:	None
 *
 ***************************************************************************/

void _dieOpenInterrupt()
{
	if( globals == NULL )
		return;
	if( GETDIEOPENFALLING )									// Check ..
		return;
	
	GLOBALSETSTART
	SETDIEOPENFALLING
	GLOBALSETEND
	
	PRINT_DEBUG(" Loosing Die Open .... \n ")

	if( GETSTANDALONE )
		raise( GETSIG__DIEOPEN );
	else
		sendSIGUSR1();
	
	if( GETHIGHVAC )
		if( GETHIGHVAC->openConnection() )
		{
			GETHIGHVAC->setOutOfTolerance( FALSE );
			GETHIGHVAC->closeConnection();
		}
}

/****************************************************************************
 * void vacuStartInterrut()
 *
 * called when the signal VacuStart get active.
 * We must have the signal DieOpen before ( if we have the signal of loosing dieOpen, all is OK )
 *
 * Input:	None
 * Output:	None
 *
 ***************************************************************************/

void vacuStartInterrut()
{
	if( globals == NULL )
		return;
	if( !( GETDIEOPENRAISING || GETDIEOPENFALLING ))									// we must have a die Open tu begin the cycle
		return;
	if( GETVACUSTART )									// Check ..
		return;
	PRINT_DEBUG(" VacuStart .... \n ")

	GLOBALSETSTART
	
	SETREQUESTCURVE( 0X0000 )
	SETLOADCURVE( 0X0000 )
	
	CLEARDIEOPENRAISING
	CLEARDIEOPENFALLING
	SETVACUSTART

	GLOBALSETEND
	for( int i =0; i < GETNUMBEROFCHANNEL; i++)
		try
		{
			DB_AxeData *axe = GETAXEDATA( i );
			if( axe );
			{
				axe->HumidityLimitAverage	( GETLIMITAVERAGE	( i ) );
				axe->HumidityLimitStart		( GETLIMITSTART		( i ) );
				axe->HumidityLimitStop		( GETLIMITSTOP		( i ) );
			}
		}
		catch(const std::exception& e )
		{
		}
	startTimer();									// Start the Timer
}
/****************************************************************************
 * void dispatchInterrupt()
 *
 * called when one signal is changing, and the dispatch to the good handler.
 *
 * Input:	None
 * Output:	None
 *
 ***************************************************************************/

void dispatchInterrupt()
{
	if( getpid() != mainPid )
	{
		PRINT_DEBUG(" Not in mainPid doing nothing .... \n ")
		return;
	}
	if( ( digitalRead( PIN_DIEOPEN ) == 0 ) && ( GETDIEOPENRAISING == FALSE ) )
		dieOpenInterrupt();
	
	if( ( digitalRead( PIN_VACUSTART ) == 0 ) && ( GETVACUSTART == FALSE ))
		vacuStartInterrut();
	
	if( digitalRead( PIN_MCP_INT ) == 0 )
		MCPInterrupt();
	
	if( ( digitalRead( PIN_DIEOPEN ) == 1 ) && ( GETDIEOPENRAISING == TRUE ) && ( GETDIEOPENFALLING == FALSE ) )	// we are loosing dieOpen signal
		_dieOpenInterrupt();

	digitalWrite( PIN_INTACK, HIGH );
	digitalWrite( PIN_INTACK, LOW );
	
}

/****************************************************************************
 * void MCPInterrupt()
 *
 * called when the MCP require something
 * actually not used.
 *
 * Input:	None
 * Output:	None
 *
 ***************************************************************************/

void MCPInterrupt()
{
   GLOBALSETSTART
   SETMCPINTERRUPT
   GLOBALSETEND
   
   PRINT_DEBUG("Got MCP interrupt( 2)\n")

   GLOBALSETSTART
   CLEARMCPINTERRUPT
   GLOBALSETEND
}




static struct sigaction readLoop;
static struct sigaction oldact;
static itimerval 		interrupt_time;
static int 				howMany_sample 	= -1;
static int				sample_index	=  0;

void sendSIGUSR1()
{
	try
	{
		interrupt_time.it_value.tv_sec 		= 0;					// wait T1 start
		interrupt_time.it_value.tv_usec 	= 100;					// is in microseconde, periode in millisecond, so adjust it ( need at lease 1 clock !!! )

		interrupt_time.it_interval.tv_sec	= 0;
		interrupt_time.it_interval.tv_usec 	= 0;					// is in microseconde, periode in millisecond, so adjust it

		readLoop.sa_sigaction 	= &raiseInterruptAct;
		sigemptyset( &readLoop.sa_mask );
		readLoop.sa_flags		= SA_SIGINFO;
		readLoop.sa_restorer	= NULL;

		sigaction( SIGALRM, &readLoop, &oldact );					// place holder
		setitimer( ITIMER_REAL,&interrupt_time, NULL);				// start interrupt
	}
	catch (const std::exception& e )
	{
		PRINT_DEBUG("sendSIGUSR1: got an error... \n")
	}
};

void startTimer()
{
c_Node			*node = NULL;

	try
	{
		node = GETCHANNELLIST->getHead();
		if( !node )
		{
			PRINT_DEBUG("startTimer: list is NULL... \n")
			return;
		}
		do															// parse the list to clear the max and min values
		{
		int 		channel 	= ((c_Channel *) node )->getChannelNum();
		DB_AxeData	*axe = GETAXEDATA( channel );
		
			((c_Channel *) node )->resetMinValue();
			((c_Channel *) node )->resetMaxValue();
			if( axe )
			{
				((c_Channel *) node )	->setChannelConnected( 	axe->HumidityChannelConnected() 	)
										->setChannelRxA(		axe->HumidityChannelUseA()			)
										->setChannelRxB(		axe->HumidityChannelUseB()			);
				axe	->HumidityChannelLimitError( FALSE )			// Reset error flags on channel
					->HumidityLimitValue( 4096 );					// Reset min value of limit to max possible
			}
			else
				((c_Channel *) node )	->setChannelConnected( 	FALSE							 	)
										->setChannelRxA(		FALSE								)
										->setChannelRxB(		FALSE								);
			node = node->getSucc();
		}
		while ( !GETCHANNELLIST->isAtEnd( node ) );
	}
	catch (const std::exception& e )
	{
			PRINT_DEBUG("startTimer: error getting list Head... \n")
			return;
	}
	

	interrupt_time.it_value.tv_sec 		= 0;			// wait T1 start
	interrupt_time.it_value.tv_usec 	= ( GETT1 * 1000 ) + 1;
														// is in microseconde, periode in millisecond, so adjust it ( need at lease 1 clock !!! )
	
	interrupt_time.it_interval.tv_sec	= 0;
	interrupt_time.it_interval.tv_usec 	= GETSAMPLERATE * 1000 + 1;		// is in microseconde, periode in millisecond, so adjust it

	readLoop.sa_sigaction 	= &timeInterruptAct;
	sigemptyset( &readLoop.sa_mask );
	readLoop.sa_flags		= SA_SIGINFO;
	readLoop.sa_restorer	= NULL;
	
	howMany_sample = ( ( GETT2 + GETT3 ) / ( GETSAMPLERATE == 0 ? 1 : GETSAMPLERATE ) ) + 1;
	
//	if( howMany_sample > 990 )
//		howMany_sample = 990;
	
	sample_index = 0;
	sigaction( SIGALRM, &readLoop, &oldact );					// place holder
	setitimer( ITIMER_REAL,&interrupt_time, NULL);				// start interrupt
};

void stopTimer()
{
	interrupt_time.it_value.tv_sec 		= 0;					// wait T1 start
	interrupt_time.it_value.tv_usec 	= 0;					// is in microseconde, periode in millisecond, so adjust it
	interrupt_time.it_interval.tv_sec	= 0;
	interrupt_time.it_interval.tv_usec 	= 0;					// is in microseconde, periode in millisecond, so adjust it

	setitimer( ITIMER_REAL,&interrupt_time, NULL);				// stop interrupt
	delay( 1);
	sigaction( SIGALRM, &oldact, NULL );
	howMany_sample = -1;
}

static bool inTimeInt = FALSE;

void raiseInterruptAct( int signum, siginfo_t *sinfo, void *pad)
{
	if( signum != SIGALRM )
	{
		PRINT_DEBUG("Bad signal %d \n",(int) signum )
		return;
	}
	raise( SIGUSR1 );
}

void timeInterruptAct( int signum, siginfo_t *sinfo, void *pad)
{
	if( signum != SIGALRM )
	{
		PRINT_DEBUG("Bad signal %d \n",(int) signum )
		return;
	}

	if( inTimeInt )
		return;
	inTimeInt = TRUE;
	if( GETCHANNELLIST->IsListEmpty() )
	{
		PRINT_DEBUG("List empty, do nothing\n")
		stopTimer();
		return;
	}
	
c_Node				*node = GETCHANNELLIST->getHead();
#ifndef __NEWHARDWARE__
c_IrRxStatus		cmd;
#else
c_GetMeasurement	cmd;
#endif
unsigned short	theTime = sample_index * GETSAMPLERATE;

	do
	{
	int 		channel 	= ((c_Channel *) node )->getChannelNum();
	bool		isActive	= ((c_Channel *) node )->getChannelConnected();
	uint8_t		ret;

		if( isActive )
		{
			cmd.setId( channel );
			ret = cmd.sendCmd();					// Send the GetInfo Command
			if( ret )
			{
				PRINT_DEBUG("\n\tSendcmd return during send IrRxStatus to %d, %d \n", (int)channel,(int)ret)
			}
			ret = cmd.isResponseOK();
			if( ret > 0x80)
			{
				PRINT_ERROR("\n\tError during rx IrRxStatus to %d, %d \n", (int)channel,(int)ret);
			}
			if( !GETSTANDALONE )
			{// TODO: Add the value for the temperature and vaccum
#ifndef __NEWHARDWARE__
					((c_Channel *) node)->addValue( (c_Node *) new c_Value( cmd.getChannelA(), cmd.getChannelB(), (uint32_t) sinfo->si_stime ) );
#else
					((c_Channel *) node)->addValue( (c_Node *) new c_Value( cmd.getHumidity(), cmd.getPressure(), (uint32_t) sinfo->si_stime ) );
#endif
			}
			else
			{
			DB_AxeData	*axe;
			unsigned short	theHumidity;
			
				axe = GETAXEDATA( channel );
				if( axe )
				{
				
#ifndef __NEWHARDWARE__
					axe->HumiditySampleAt( sample_index, theHumidity = ( axe->HumidityChannelFlags() & CH_FLAG_USEA ? cmd.getChannelA() : cmd.getChannelB() ) );
#else
					axe	->SampleAt	( sample_index,	theHumidity = cmd.getHumidity(),
													cmd.getTemperature(),
													cmd.getPressure());
#endif
					if( ( theTime >= axe->HumidityLimitStart() ) & ( theTime <= axe->HumidityLimitStop() ) )
					{
						if( axe->HumidityLimitValue() > theHumidity )
							axe->HumidityLimitValue( theHumidity );
					}
				}
			}
#ifndef __NEWHARDWARE__
			((c_Channel *) node )->setHumidityMinMaxValue( cmd.getChannelA(), cmd.getChannelB() );
#else
			((c_Channel *) node )->setMinMaxValue( cmd.getHumidity(), cmd.getTemperature(), cmd.getPressure() );
#endif
		}
		else
		{
		}
		node = node->getSucc();
	}
	while ( !GETCHANNELLIST->isAtEnd( node ) );

	sample_index++;
	if( howMany_sample-- < 0 )
	{
		stopTimer();

		GLOBALSETSTART
		SETDATAREADY
		CLEARVACUSTART
		GLOBALSETEND
		
		if( GETSTANDALONE )
		{
//			raise( GETSIG_DATAREADY );
			node = GETCHANNELLIST->getHead();
			do
			{
			int 		channel = ((c_Channel *) node )->getChannelNum();
			DB_AxeData	*axe;

				axe = GETAXEDATA( channel );
				if( axe )
				{
#ifndef __NEWHARDWARE__
					axe	->HumidityMaxValue( ((c_Channel *) node )->getHumidityMaxValue() )
					    ->HumidityMinValue( ((c_Channel *) node )->getHumidityMinValue() )
						->HumiditySamplesCount( sample_index );
#else
					axe	->HumidityMaxValue( 			((c_Channel *) node )->getHumidityMaxValue() 		)
					    ->HumidityMinValue( 			((c_Channel *) node )->getHumidityMinValue() 		)
						->HumiditySamplesCount( 		sample_index 										)
						->TemperatureMaxValue( 			((c_Channel *) node )->getTemperatureMaxValue() 	)
					    ->TemperatureMinValue( 			((c_Channel *) node )->getTemperatureMinValue() 	)
						->TemperatureSamplesCount( 		sample_index 										)
						->VaccumMaxValue( 				((c_Channel *) node )->getVaccumMaxValue() 			)
					    ->VaccumMinValue( 				((c_Channel *) node )->getVaccumMinValue() 			)
						->VaccumSamplesCount( 			sample_index 										);

#endif
					if( ( axe->HumidityChannelLimitActive() ) &
						( ( axe->HumidityMaxValue() - axe->HumidityLimitValue() ) > axe->HumidityLimitAverage() ) )
						axe->HumidityChannelLimitError( TRUE );
				}
				node = node->getSucc();
			}
			while ( !GETCHANNELLIST->isAtEnd( node ) );
		}
		else
			raise( SIGUSR1 );
		

	}
	inTimeInt = FALSE;
}

/****************************************************************************
 * void setup()
 *
 * called during the initialisaton of the arduino library.
 *
 * Input:	None
 * Output:	None
 *
 ***************************************************************************/

void setup()
{
	PRINT_DEBUG("in setup\n")

		pinMode( PIN_MCP_INT, 		INPUT);
		pinMode( PIN_INTREQ, 		INPUT);
		pinMode( PIN_DIEOPEN, 		INPUT);
		pinMode( PIN_RTC_INT, 		INPUT);
		pinMode( PIN_VACUSTART,		INPUT);
		pinMode( PIN_SPARE_INT,		INPUT);
		pinMode( PIN_RTC_INT2, 		INPUT);
		pinMode( PIN_INTACK, 		OUTPUT);

		digitalWrite( PIN_INTACK, HIGH );

	if( GETCHANNELLIST )
		for( int i=0; i < MAX_CHANNEL_SLAVE; i++ )
			GETCHANNELLIST->AddTail( (c_Node *) new c_Channel( i ) );

	if( digitalRead( PIN_DIEOPEN) == 0 )			// set the die open state
		GLOBALSETSTART
		SETDIEOPENRAISING
		GLOBALSETEND
	else
		GLOBALSETSTART
		CLEARDIEOPENRAISING
		GLOBALSETEND
}
