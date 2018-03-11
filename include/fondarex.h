/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/
#pragma once

#ifndef __FONDAREX__
#define __FONDAREX__



#ifdef DEBUG
#	if DEBUG > 1
#		define	PRINT_DEBUG( ... )	PRINTF ( stderr, __VA_ARGS__ );
#	else
#		define	PRINT_DEBUG( ... )	;
#	endif
#	define	PRINT_WARNING( ... )	PRINTF ( stderr, __VA_ARGS__ );
#else
#	define	PRINT_DEBUG( ... )		;
#endif
#	define	PRINT_WARNING( ... )	PRINTF ( stderr, __VA_ARGS__ );
#define		PRINT_ERROR( ... )		PRINTF ( stderr, __VA_ARGS__ );

#define MAKE_ID(a,b,c,d) 	((a<<24L) | (b<<16L) | (c<<8L) | d)

#define GET_WORD(a,b) 		( (a[b+1]<<8L) | a[b])
#define GET_LONG(a,b) 		((a[b+3]<<24L) | (a[b+2]<<16L) | (a[b+1]<<8L) | a[b])
// System includes

#include <Python.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <endian.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
// globals Define

#ifndef TRUE
	#define TRUE (1==1)
#endif
#ifndef FALSE
	#define FALSE (1==0)
#endif

// Define for pin usage....

#define	PIN_ST7036_RS	GPIO0
#define	PIN_ST7036_CSB	GPIO1
#define	PIN_MCP_INT		GPIO2
#define PIN_INTREQ		GPIO3
#define PIN_DIEOPEN		GPIO4
#define PIN_RTC_INT		GPIO5
#define PIN_VACUSTART	GPIO6
#define PIN_SPARE_INT	GPIO7
#define PIN_RTC_INT2	GPIO8
#define PIN_INTACK		GPIO9
#define PIN_MCP_CS		GPIO10
#define PIN_RESERVED	GPIO11
#define PIN_ST7036_SI	GPIO12
#define PIN_ST7036_SCL	GPIO13

// CAN speed is 500k, 1 TQ = 2microsseconds
// every bit use 25 TQ
// every bytes use 200 TQ
// one frame as 6 bytes control + datas...
#define CAN_BYTE_TIME			(200 * 2 )
#define CAN_BYTES_HEADER		6
#define RETRY					4

#define	DIEOPEN_MASK			( 1<<0  )
#define VACUSTART_MASK			( 1<<1  )
#define INTMCP_MASK				( 1<<2  )
#define DATAREADY_MASK			( 1<<3  )
#define _DIEOPEN_MASK			( 1<<4  )
#define READWRITE_MASK			( 1<<5  )
#define CURRENTINJECTION_MASK	( 1<<6  )
#define PARTNAME_MASK			( 1<<7  )
#define PRODNUM_MASK			( 1<<8  )
#define T1_MASK					( 1<<9  )
#define T2_MASK					( 1<<10 )
#define T3_MASK					( 1<<11 )
#define CMD_MASK				( 1<<12 )
#define NUMCHAN_MASK			( 1<<13 )
#define SAMPLERATE_MASK			( 1<<14 )
#define LOADCURVE_MASK			( 1<<15 )		// May be not to be used, only internaly for graph display
#define REQCURVE_MASK			( 1<<16 )		// May be not to be used, only internaly for graph display

// define for sandalone mode
/*
#define	SIG_DIEOPEN				34
#define SIG__DIEOPEN		 	35
#define	SIG_VACUSTART			36
#define	SIG_DATAREADY			37
*/

// ******** Local Include ********

#include "Endian.h"
//#include "mcp_can.h"
//#include "CAN.h"
#include "CAN_MCP2515.h"
#include "solutec.h"
#include "s7_client.h"
#include "s7_server.h"
#include "libdb.h"
#include "globals.h"
#include "st7036.h"

bool			decodeVarName(  const char *info, char **name, E_GlobalTypes &theType, char &mode , int &size, unsigned short db = 0, unsigned short addr = 0 );
const char 		*getVarName(unsigned short db, unsigned short addr);

/*******************************************************************************
 *
 * Declaration for Fondarex Python extension library
 *
 ******************************************************************************/ 

#ifdef ___DECLARE_LIB__

#define KeyFunc( func,com ) {(char *)#func,(PyCFunction)func,METH_KEYWORDS,(char *)#com},
#define VarArgsFunc( func,com ) {(char *)#func,(PyCFunction)func,METH_VARARGS,(char *)#com},
#define NoArgsFunc( func,com ) {(char *)#func,(PyCFunction)func,METH_NOARGS,(char *)#com},
#define PythonFunction(__func__) static PyObject* __func__(PyObject* self, PyObject* args)
#define _PythonFunction(__func__) static PyObject* __func__(PyObject* self, PyObject* args, PyObject *kwargs)
//static Prototype of function

_PythonFunction( openSPI );
PythonFunction( transfer );
PythonFunction( closeSPI );
PythonFunction( MCPbegin );
PythonFunction( MCPinit_Mask );
PythonFunction( MCPinit_Filt );
PythonFunction( MCPsendMsgBuf );
PythonFunction( MCPreadMsgBuf );
PythonFunction( MCPcheckReceive );
PythonFunction( MCPcheckError );
PythonFunction( MCPgetCanId );
PythonFunction( getModuleInfo );
PythonFunction( setModuleInfo );
PythonFunction( getModuleStatus );
PythonFunction( setModuleStatus );
PythonFunction( getIrTxStatus );
PythonFunction( setIrTxStatus );
PythonFunction( getIrRxStatus );
PythonFunction( setIrRxStatus );
PythonFunction( getIrStatus );
PythonFunction( setIrStatus );
PythonFunction( MCPReset );
PythonFunction( installInterrupt );
PythonFunction( removeInterrupt );
PythonFunction( getInterrupt );
PythonFunction( getData );
PythonFunction( setTimerValue );
PythonFunction( doCalibration );
PythonFunction( getMinMax );

// ******************* for S7 module ********************
// getter
PythonFunction( getVersion );
PythonFunction( getReadWrite );
PythonFunction( getCurrentInjection );
PythonFunction( getPartName );
PythonFunction( getProdNum );
PythonFunction( getT1 );
PythonFunction( getT2 );
PythonFunction( getT3 );
PythonFunction( getCmd );
PythonFunction( getNumChan );
PythonFunction( getSampleRate );
PythonFunction( getLoadCurve );
PythonFunction( getReqCurve );
// setter
PythonFunction( setVersion );
PythonFunction( setReadWrite );
PythonFunction( setCurrentInjection );
PythonFunction( setPartName );
PythonFunction( setProdNum );
PythonFunction( setT1 );
PythonFunction( setT2 );
PythonFunction( setT3 );
PythonFunction( setCmd );
PythonFunction( setNumChan );
PythonFunction( setSampleRate );
PythonFunction( setLoadCurve );
PythonFunction( setReqCurve );
// utility
PythonFunction( initS7 );
PythonFunction( startS7 );
PythonFunction( deleteS7 );
PythonFunction( clearS7Event );
static PyMethodDef SpiMethods[] =
{
	KeyFunc( openSPI,Open SPI Port.)
	VarArgsFunc(transfer,Transfer data.)
	NoArgsFunc(closeSPI,Close SPI port.)
				
	VarArgsFunc(MCPbegin,init can.)
	VarArgsFunc(MCPinit_Mask,init Mask.)
	VarArgsFunc(MCPinit_Filt,init filters.)
	VarArgsFunc(MCPsendMsgBuf,Send buffers.)
				
	NoArgsFunc(MCPreadMsgBuf,read Buffers.)
	NoArgsFunc(MCPcheckReceive,check if something arived.)
	NoArgsFunc(MCPcheckError,check error.)
	NoArgsFunc(MCPgetCanId,get can id when receive.)
				
	VarArgsFunc(getModuleInfo,Send buffers.)
	VarArgsFunc(setModuleInfo,Send buffers.)
	VarArgsFunc(getModuleStatus,Send buffers.)
	VarArgsFunc(setModuleStatus,Send buffers.)
	VarArgsFunc(getIrTxStatus,Send buffers.)
	VarArgsFunc(setIrTxStatus,Send buffers.)
	VarArgsFunc(getIrRxStatus,Send buffers.)
	VarArgsFunc(setIrRxStatus,Send buffers.)
	VarArgsFunc(getIrStatus,Send buffers.)
	VarArgsFunc(setIrStatus,Send buffers.)
				
	NoArgsFunc(MCPReset,Send buffers.)
	NoArgsFunc(installInterrupt,Send buffers.)
	NoArgsFunc(removeInterrupt,Send buffers.)
	NoArgsFunc(getInterrupt,Send buffers.)
	NoArgsFunc(getData,Send buffers.)
				
	VarArgsFunc(setTimerValue,Send buffers.)
	VarArgsFunc(doCalibration,Send buffers.)
				
	NoArgsFunc(getMinMax,Get minimum and maximum values for all channel.)
	NoArgsFunc(getVersion,check if something arived.)
	NoArgsFunc(getReadWrite,check if something arived.)
	NoArgsFunc(getCurrentInjection,check if something arived.)
	NoArgsFunc(getPartName,check if something arived.)
	NoArgsFunc(getProdNum,check if something arived.)
	NoArgsFunc(getT1,check if something arived.)
	NoArgsFunc(getT2,check if something arived.)
	NoArgsFunc(getT3,check if something arived.)
	NoArgsFunc(getCmd,check if something arived.)
	NoArgsFunc(getNumChan,check if something arived.)
	NoArgsFunc(getSampleRate,check if something arived.)
	NoArgsFunc(getLoadCurve,check if something arived.)
	NoArgsFunc(getReqCurve,check if something arived.)
				
	VarArgsFunc(setVersion,Send buffers.)
	VarArgsFunc(setReadWrite,Send buffers.)
	VarArgsFunc(setCurrentInjection,Send buffers.)
	VarArgsFunc(setPartName,Send buffers.)
	VarArgsFunc(setProdNum,Send buffers.)
	VarArgsFunc(setT1,Send buffers.)
	VarArgsFunc(setT2,Send buffers.)
	VarArgsFunc(setT3,Send buffers.)
	VarArgsFunc(setCmd,Send buffers.)
	VarArgsFunc(setNumChan,Send buffers.)
	VarArgsFunc(setSampleRate,Send buffers.)
	VarArgsFunc(setLoadCurve,Send buffers.)
	VarArgsFunc(setReqCurve,Send buffers.)
				
	NoArgsFunc(initS7,check if something arived.)
	NoArgsFunc(startS7,check if something arived.)
	NoArgsFunc(deleteS7,check if something arived.)
	NoArgsFunc(clearS7Event,check if something arived.)

 	{(char *)NULL,			(PyCFunction)NULL,				0, 			(char *)NULL}
};

PyMODINIT_FUNC initfondarex(void);

#endif // ___DECLARE_LIB__
#endif // __FONDAREX__
