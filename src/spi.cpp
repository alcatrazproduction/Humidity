// include definition for python lib

#define ___DECLARE_LIB__

#include "humidity.h"

//static


_PythonFunction( openSPI )
{

	static char* kwlist[] =
							{
								(char *)"mode",
								(char *)"bits",
								(char *)"speed",
								(char *)"delay",
								NULL
							};
	uint8_t 		mode	= globals->getSPIMode();
	uint8_t 		bits	= globals->getSPIBits();
	uint32_t 		speed	= globals->getSPISpeed();
	uint16_t 		thedelay= globals->getSPIDelay();

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|iiii:keywords", kwlist,  &mode, &bits, &speed, &thedelay))
		return NULL;

	PyErr_Clear();

	globals->setSPIMode( mode )->setSPIBits( bits )->setSPISpeed( speed )->setSPIDelay( thedelay );
	
	SPI.begin();		// Initialise SPI libraries
	 
	SPI.setDataMode( mode );
	SPI.setBitOrder( bits );
	SPI.setClockDivider( speed );

	PyObject* retDict;
	retDict = PyDict_New();

	PyDict_SetItem(retDict, PyString_FromString("mode"), PyInt_FromLong((long)mode));
	PyDict_SetItem(retDict, PyString_FromString("bits"), PyInt_FromLong((long)bits));
	PyDict_SetItem(retDict, PyString_FromString("speed"), PyInt_FromLong((long)speed));
	PyDict_SetItem(retDict, PyString_FromString("delay"), PyInt_FromLong((long)thedelay));

	return retDict;
}

PythonFunction( transfer )
{
	PyObject* transferTuple;

	if(!PyArg_ParseTuple(args, "O", &transferTuple))		// "O" - Gets non-NULL borrowed reference to Python argument.
		return NULL;					// As far as I can tell, it's mostly just copying arg[0] into transferTuple
								// and making sure at least one arg has been passed (I think)

	if(!PyTuple_Check(transferTuple))			// The only argument we support is a single tuple.
		pabort("Only accepts a single tuple as an argument\n");


	uint32_t tupleSize = PyTuple_Size(transferTuple);

	uint8_t rx[tupleSize];
	PyObject* tempItem;

	volatile unsigned int index = 0;

	while( index < tupleSize )
	{
		tempItem = PyTuple_GetItem( transferTuple, index );		//
		if(!PyInt_Check(tempItem))
		{
			pabort("non-integer contained in tuple\n");
		}
		rx[ index  ] = SPI.transfer(
			(uint8_t)PyInt_AsSsize_t(tempItem),
			( index + 1) < tupleSize ? SPI_CONTINUE:SPI_LAST ) ;
		index ++;

	}


	transferTuple = PyTuple_New(tupleSize);
	for(unsigned int i=0;i<tupleSize;i++)
		PyTuple_SetItem(transferTuple, i, Py_BuildValue("i",rx[i]));

	return transferTuple;
}


PythonFunction( closeSPI )
{
	SPI.end();
	Py_RETURN_NONE;
}

// MCP.....
PythonFunction( MCPbegin )
{
INT8U	speed;

	PyArg_ParseTuple( args, "b", &speed );
	return Py_BuildValue( "b", globals->getCan()->begin( speed ) );
}

PythonFunction( MCPinit_Mask )
{
INT8U	num;
INT8U	ext;
INT32U	ulData;

	PyArg_ParseTuple( args, "bbk", &num,&ext,&ulData );
	return Py_BuildValue( "b", globals->getCan()->init_Mask( num,ext,ulData ) );
}

PythonFunction( MCPinit_Filt )
{
INT8U	num;
INT8U	ext;
INT32U	ulData;

	PyArg_ParseTuple( args, "bbk", &num,&ext,&ulData );
	return Py_BuildValue( "b", globals->getCan()->init_Filt( num,ext,ulData ) );
}
PythonFunction( MCPsendMsgBuf )
{
INT32U		id;
INT8U		ext;
Py_ssize_t	len;
INT8U		*buf;

	PyArg_ParseTuple( args, "lbs#", &id,&ext, &buf, &len );
	printf(" >>>%s<<<(%d,%d,%d)\n ",buf,(int)id,(int)ext,(int)len );
	return Py_BuildValue( "b", globals->getCan()->sendMsgBuf( id,ext,len,buf ) );
}
PythonFunction( MCPreadMsgBuf )
{

INT8U		len;
INT8U		theBuffer[MAX_CHAR_IN_MESSAGE ];
INT8U		*buf = &theBuffer[0];
INT8U		ret;

	ret = globals->getCan()->readMsgBuf( &len,(INT8U *)buf );
	return Py_BuildValue( "bs#",ret,buf,(Py_ssize_t)len );
}

PythonFunction( MCPcheckReceive )
{
INT8U ret =globals->getCan()->checkReceive( );
	return Py_BuildValue( "bs", ret, CAN_error[ret] );
}

PythonFunction( MCPcheckError )
{
INT8U ret = globals->getCan()->checkError(  );
	return Py_BuildValue( "bs", ret , CAN_error[ret]);
}

PythonFunction( MCPgetCanId )
{
	return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}

PythonFunction( getModuleInfo )
{
INT32U			id;
c_moduleinfo	cmd;
INT8U			ret;

		PyArg_ParseTuple( args, "l", &id );
		printf(" (%d) ",(int)id );

		cmd.setId( id );
		ret = cmd.sendCmd();					// Send the GetInfo Command
		if( ret )
		{
			printf("\n\tError during send GetInfo to %d, %d \n", (int)id,(int)ret);
//			return Py_BuildValue( "b", ret );
		}
		ret = cmd.isResponseOK();
		if( ret == 0xFF )
		{
			printf("\n\tTimeout on GetInfo to %d\n", (int)id);
//			return Py_BuildValue( "b", ret );
		}
		else if( ret > 0x80 )
		{
			printf("\n\tError during rx GetInfo to %d, %d \n", (int)id,(int)ret);
//			return Py_BuildValue( "b", ret );
		}
        return Py_BuildValue( "blllb", ret, cmd.getSerial(), cmd.getHardWareVersion() , cmd.getSoftWareVersion() ,cmd.getStatus() );
}

PythonFunction( setModuleInfo )
{
        return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}

PythonFunction( getModuleStatus )
{
        return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}

PythonFunction( setModuleStatus )
{
        return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}

PythonFunction( getIrTxStatus )
{
int          	id;
c_IrTxStatus    cmd;
int           	ret;

	PyArg_ParseTuple( args, "i", &id );
	cmd.setId( id );
	ret = cmd.sendCmd(  );                                     // Send the GetInfo Command
	if( ret )
	{
			printf("Error during send getIrTxStatus to %d, %d \n", (int)id,(int)ret);
//                      return Py_BuildValue( "b", ret );
	}
	ret = cmd.isResponseOK();
	if( ret == 0xff)
	{
		printf("\n\tTimeout on getIrTxStatus to %d\n", (int)id);
//			return Py_BuildValue( "b", ret );
	} else
	if( ret > 0x80 )
	{
			printf("Error during rx getIrTxStatus to %d, %d \n", (int)id,(int)ret);
//                      return Py_BuildValue( "b", ret );
	}

	return Py_BuildValue( "iii", (int)ret, (int)cmd.getChannelA(), (int)cmd.getChannelB() );

}

PythonFunction( setIrTxStatus )
{
int          	id;
c_IrTxStatus    cmd;
int           	ret;
int  			channel_a  = 0;
int  			channel_b   = 0;

	PyArg_ParseTuple( args, "iii", &id, &channel_a, &channel_b );
	cmd.setId( id );
	cmd.setChannelA( channel_a );
	cmd.setChannelB( channel_a );

	ret = cmd.sendParam();                                     // Send the GetInfo Command
	if( ret )
	{
			printf("Error during send TX value to %d, %d \n", (int)id,(int)ret);
	//                      return Py_BuildValue( "b", ret );
	}
	ret = cmd.isAckOK( );
	if( ret == 0xff )
	{
		printf("\n\tTimeout on setIrTxStatus to %d\n", (int)id);
	//			return Py_BuildValue( "b", ret );
	}else
	if( ret > 0x80 )
	{
			printf("Error during rx setIrTxStatus to %d, %d \n", (int)id,(int)ret);
	//                      return Py_BuildValue( "b", ret );
	}
	return Py_BuildValue( "b", (int)ret );
}


PythonFunction( getIrRxStatus )
{
INT32U          id;
c_IrRxStatus	cmd;
INT8U           ret;

                PyArg_ParseTuple( args, "i", &id );
				cmd.setId( id );
                ret = cmd.sendCmd(  );                                     // Send the GetInfo Command
                if( ret )
                {
                        printf("Error during send getIrRxStatus to %d, %d \n", (int)id,(int)ret);
//                      return Py_BuildValue( "b", ret );
                }
				ret = cmd.isResponseOK(  );
				if( ret == 0xff )
				{
					printf("\n\tTimeout on getIrRxStatus to %d\n", (int)id);
		//			return Py_BuildValue( "b", ret );
				}
                if( ret > 0x80 )
                {
                        printf("Error during rx getIrRxStatus to %d, %d \n", (int)id,(int)ret);
//                      return Py_BuildValue( "b", ret );
                }
				return Py_BuildValue( "(iii)", (int)ret, (int)cmd.getChannelA(), (int)cmd.getChannelB() );

}

PythonFunction( setIrRxStatus )
{
        return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}


PythonFunction( getIrStatus )
{
        return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}

PythonFunction( setIrStatus )
{
        return Py_BuildValue( "b", globals->getCan()->getCanId(  ) );
}

PythonFunction( MCPReset )
{
        globals->getCan()->mcp2515_reset();
	Py_RETURN_NONE;

}
PythonFunction( installInterrupt )
{
	pinMode( PIN_IN1, 			INPUT);
	pinMode( PIN_IN2, 			INPUT);
	pinMode( PIN_MCP_INT, 		INPUT);
	pinMode( 3,			 		INPUT_PULLUP);

	attachInterrupt(INT_DIEOPEN, 	dispatchInterrupt, CHANGE);
//	attachInterrupt(INT_MCP, 		MCPInterrupt, FALLING);

	Py_RETURN_NONE;
}

PythonFunction( removeInterrupt )
{
	detachInterrupt(INT_DIEOPEN);
//	detachInterrupt(INT_MCP);

	Py_RETURN_NONE;
}

PythonFunction( getInterrupt )
{
uint32_t ret = 0;

	if( globals->getDieOpen0() )
		ret += DIEOPEN_MASK;	// 0
	if( globals->getVacuStart() )
		ret += VACUSTART_MASK;	// 1
	if( globals->getInterruptMCP() )
		ret += INTMCP_MASK;		// 2
	if( globals->getDataReady() )
		ret += DATAREADY_MASK;	// 3
// 	if( globals->getDieOpen1() )
		ret += _DIEOPEN_MASK;	// 4
printf("Int flags: %d \n", (int) ret );
	return Py_BuildValue( "l", ret );
}

PythonFunction( getData )
{
PyObject* 	transferTuple 	= PyTuple_New(2);
c_Node		*node 			= globals->getChannelList()->getHead();
	do
	{
	c_Value	*val;

		val = (c_Value *) ((c_Channel *)node)->remNextValue();
		if( val == NULL )
		{
			globals->clearDataReady();
			Py_RETURN_NONE;
		}
		else
		{
		uint32_t v = val->getValues();
		
			PyTuple_SetItem(transferTuple, 0, Py_BuildValue("i",(v&0xffff)));
			PyTuple_SetItem(transferTuple, 1, Py_BuildValue("i",(((v&0xffff0000)>>16)&0xffff)));
		}
		node = node->getSucc();
	}
	while ( !globals->getChannelList()->isAtEnd( node ) );
	return transferTuple;
}
PythonFunction( setTimerValue )
{
INT32U		t1 	= 65535;
INT32U  	t2  = 65535;
INT32U  	t3	= 65535;

	PyArg_ParseTuple( args, "lll", &t1, &t2, &t3 );
	if( t1 != 65535 )
		globals->setT1( t1 );
	if( t2 != 65535 )
		globals->setT1( t2 );
	if( t3 != 65535 )
		globals->setT1( t3 );
	Py_RETURN_NONE;
}

PythonFunction( doCalibration )
{
int				oldCal,  id;
DB_AxeData		*axe;

	PyArg_ParseTuple( args, "ii", &id, &oldCal );

	axe = globals->getAxeData( id );
	if ( axe )
		return Py_BuildValue( "i",axe->doCalibration( ));
	else
		return Py_BuildValue( "i",oldCal );
}

PythonFunction( getMinMax )
{
PyObject* 	transferTuple 	= PyTuple_New(2);
c_Node		*node 			= globals->getChannelList()->getHead();
	do
	{
		PyTuple_SetItem(transferTuple, 0, Py_BuildValue("i",( ((c_Channel *) node )->getMinValue() &0xffff)));
		PyTuple_SetItem(transferTuple, 1, Py_BuildValue("i",( ((c_Channel *) node )->getMaxValue()&0xffff)));
		node = node->getSucc();
	}
	while ( !globals->getChannelList()->isAtEnd( node ) );
	return transferTuple;
}
// ******************* for S7 module ********************
// getter
/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getVersion )
{
//	return Py_BuildValue( "s", globals->getVersion() );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getReadWrite )
{
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getCurrentInjection )
{
	return Py_BuildValue( "l", globals->getCurrentInjection() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getPartName )
{
	return Py_BuildValue( "s", globals->getPartName() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getProdNum )
{
	return Py_BuildValue( "i", (int) globals->getProdNum() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getT1 )
{
	return Py_BuildValue( "i", (int)globals->getT1() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getT2 )
{
	return Py_BuildValue( "i", (int)globals->getT2() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getT3 )
{
	return Py_BuildValue( "i", (int)globals->getT3() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getCmd )
{
	return Py_BuildValue( "s", (int)globals->getCmd() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getNumChan )
{
	return Py_BuildValue( "i", (int)globals->getNumChan() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getSampleRate )
{
	return Py_BuildValue( "i", (int)globals->getSampleRate() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getLoadCurve )
{
	return Py_BuildValue( "i", (int)globals->getLoadCurve() );
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( getReqCurve )
{
	return Py_BuildValue( "i", (int)globals->getReqCurve() );
}

// setter

/*********************************************************
 * SetVersion will never be implemented
 *
 * ******************************************************/

PythonFunction( setVersion ) 
{
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setReadWrite )
{
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setCurrentInjection )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setCurrentInjection( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setPartName )
{
unsigned char	*param1 	= NULL;

	PyArg_ParseTuple( args, "s", &param1);
	if( param1 != NULL )
		globals->setPartName( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setProdNum )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setProdNum( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setT1 )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setT1( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setT2 )
{
	INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setT2( param1 );

	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setT3 )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setT3( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setCmd )
{
unsigned char	*param1 	= NULL;

	PyArg_ParseTuple( args, "s", &param1);
	if( param1 != NULL )
		globals->setCmd( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setNumChan )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
printf("setNumChan  %ld \n", param1);
	if( param1 != 65535 )
		globals->setNumChan( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setSampleRate )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setSampleRate( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setLoadCurve )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setLoadCurve( param1 );
	Py_RETURN_NONE;
}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( setReqCurve )
{
INT32U		param1 	= 65535;

	PyArg_ParseTuple( args, "l", &param1);
	if( param1 != 65535 )
		globals->setReqCurve( param1 );
	Py_RETURN_NONE;
}

// utility

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( initS7 )
{
int ret = initS7(  );

	return Py_BuildValue( "bs", ret , CAN_error[ret]);

}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( startS7 )
{
int ret = startS7(  );

	return Py_BuildValue( "bs", ret , CAN_error[ret]);

}

/*********************************************************
 *
 *
 * ******************************************************/

PythonFunction( deleteS7 )
{
int ret = deleteS7(  );

	return Py_BuildValue( "bs", ret , CAN_error[ret]);
}

/*********************************************************
 *
 *
 * ******************************************************/
PythonFunction( clearS7Event )
{
	clearS7Event();
	Py_RETURN_NONE;
}
/*********************************************************
 *
 *
 * ******************************************************/

PyMODINIT_FUNC initfondarex(void)
{
int error;

PRINT_DEBUG("In Init fuction\n")
	init(); 					// Init all the stuff for hw pin....
PRINT_DEBUG("In initGlobals\n")
	if( ( error = initGlobals() ) )
		fprintf( stderr, "Error initialising library %d ( %s )\n", error, CAN_error[error] );
	globals->clearStandAlone();
PRINT_DEBUG("In Init python fuction\n")
	if( error == 0 )
		(void) Py_InitModule("fondarex", SpiMethods);
PRINT_DEBUG("In Init Setup\n")
	setup();
PRINT_DEBUG(" end of Init\n")
}

