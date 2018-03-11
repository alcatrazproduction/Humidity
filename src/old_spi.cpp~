/* SPI testing utility (see copyright beow)
 *	adapted for use in Python
 * 	by Louis Thiery
 * 	Lots more flexibility and cleanup by Connor Wolf (imaginaryindustries.com)
 *
 * compile for Python using: "python setup.py build"
 * compiled module will be in "./build/lib.linux-armv6l-2.7/spi.so"
 *
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 */

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

#include "mcp_can.h"

#define PIN_MODE_SPI 2

#define GPIO_MODE_PATH "/sys/devices/virtual/misc/gpio/mode/"
#define GPIO_FILENAME "gpio"
//static

static const 	char 		*default_device 	= "/dev/spidev0.0";
static			char		*device				= (char *)default_device;
static			uint8_t 	mode				= 0;
static			uint8_t 	bits 				= 8;
static			uint32_t 	speed 				= 500000;
static			uint16_t 	thedelay			= 0;

				int 		ret 		= 0;
				int 		fd;

void writeFile( int fileID, int value );
static void setPinMode( int pinID, int mode )
{
	writeFile( pinID, mode );
}

static PyObject* openSPI(PyObject *self, PyObject *args, PyObject *kwargs)
{

	static char* kwlist[] =
							{	(char *)"device",
								(char *)"mode",
								(char *)"bits",
								(char *)"speed",
								(char *)"delay",
								NULL
							};

	// Adding some sort of mode parsing would probably be a nice idea for the future, so you don't have to specify it as a bitfield
	// stuffed into an int.
	// For the moment the default mode ("0"), will probably work for 99% of people who need a SPI interface, so I'm not working on that
	//


	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|siiii:keywords", kwlist, &device, &mode, &bits, &speed, &thedelay))
		return NULL;
	// It's not clearly documented, but it seems that PyArg_ParseTupleAndKeywords basically only modifies the values passed to it if the
	// keyword pertaining to that value is passed to the function. As such, the defaults specified by the variable definition are used
	// unless you pass a kwd argument.
	// Note that there isn't any proper bounds-checking, so if you pass a value that exceeds the variable size, it's just truncated before
	// being stuffed into  the avasilable space. For example, passing a bits-per-word of 500 gets truncated to 244. Unfortunately, the
	// PyArg_ParseTupleAndKeywords function only seems to support ints of 32 bits.

	PyErr_Clear();

	// printf("Mode: %i, Bits: %i, Speed: %i, Delay: %i\n", mode, bits, speed, delay);

	if( !strcmp( default_device, device ) )
	{
		char path[256];
		for( int i = 10; i<= 13; i++ )
		{
			memset( path, 0, sizeof( path ) );
			sprintf( path, "%s%s%d", GPIO_MODE_PATH, GPIO_FILENAME, i );
			int pinMode = open( path, O_RDWR );
			setPinMode( pinMode, PIN_MODE_SPI );
			close( pinMode );
		}
	}

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * Setup SPI mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	// Stuff the various initilization parameters into a dict, and return that.
	// Note that the returned values may not be completely real. It seems that, at least for the speed value,
	// the hardware only has several possible settings (250000, 500000, 1000000, etc...) Strangely enough, the
	// ioctl for setting the speed *returns the speed you specify*. However, the hardware seems to default to the
	// closest avalable value *below* the specified rate. (i.e. you will never get a speed faster then you spec),
	// but you may get a slower value.

	//It would probably be a good idea to bin-down the passed arguement to the available values, and return
	// that.

	PyObject* retDict;
	retDict = PyDict_New();

	PyDict_SetItem(retDict, PyString_FromString("mode"), PyInt_FromLong((long)mode));
	PyDict_SetItem(retDict, PyString_FromString("bits"), PyInt_FromLong((long)bits));
	PyDict_SetItem(retDict, PyString_FromString("speed"), PyInt_FromLong((long)speed));
	PyDict_SetItem(retDict, PyString_FromString("delay"), PyInt_FromLong((long)thedelay));


	return retDict;
}



static PyObject* transfer(PyObject* self, PyObject* arg)
{
	PyObject* transferTuple;

	if(!PyArg_ParseTuple(arg, "O", &transferTuple))		// "O" - Gets non-NULL borrowed reference to Python argument.
		return NULL;					// As far as I can tell, it's mostly just copying arg[0] into transferTuple
								// and making sure at least one arg has been passed (I think)

	if(!PyTuple_Check(transferTuple))			// The only argument we support is a single tuple.
		pabort("Only accepts a single tuple as an argument\n");


	uint32_t tupleSize = PyTuple_Size(transferTuple);

	uint8_t tx[tupleSize];
	uint8_t rx[tupleSize];
	PyObject* tempItem;

	uint16_t i=0;

	while(i < tupleSize)
	{
		tempItem = PyTuple_GetItem(transferTuple, i);		//
		if(!PyInt_Check(tempItem))
		{
			pabort("non-integer contained in tuple\n");
		}
		tx[i] = (uint8_t)PyInt_AsSsize_t(tempItem);

		i++;

	}

	struct spi_ioc_transfer tr =
	{
		/*.tx_buf 			= */(unsigned long)tx,
		/*.rx_buf 			= */(unsigned long)rx,
		/*.len 				= */tupleSize,
		/*.delay_usecs 			= */thedelay,
		/*.speed_hz 			= */speed,
		/*.bits_per_word		= */bits,
		/*.cs_change 			=*/ 1,
						    0
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	transferTuple = PyTuple_New(tupleSize);
	for(i=0;i<tupleSize;i++)
		PyTuple_SetItem(transferTuple, i, Py_BuildValue("i",rx[i]));

	return transferTuple;
}


static PyObject* closeSPI(PyObject* self,PyObject* args)
{
	close(fd);
	Py_RETURN_NONE;
}

// MCP.....
static PyObject* mcp2515_begin(PyObject* self,PyObject* args)
{
INT8U	speed;

	PyArg_ParseTuple( args, "b", &speed );
	return Py_BuildValue( "b", CAN.begin( speed ) );
}

static PyObject* mcp2515_init_Mask(PyObject* self,PyObject* args)
{
INT8U	num;
INT8U	ext;
INT32U	ulData;

	PyArg_ParseTuple( args, "bbk", &num,&ext,&ulData );
	return Py_BuildValue( "b", CAN.init_Mask( num,ext,ulData ) );
}

static PyObject* mcp2515_init_Filt(PyObject* self,PyObject* args)
{
INT8U	num;
INT8U	ext;
INT32U	ulData;

	PyArg_ParseTuple( args, "bbk", &num,&ext,&ulData );
	return Py_BuildValue( "b", CAN.init_Filt( num,ext,ulData ) );
}
static PyObject* mcp2515_sendMsgBuf(PyObject* self,PyObject* args)
{
INT32U	id;
INT8U	ext;
INT8U	len;
INT8U	*buf = NULL;

	PyArg_ParseTuple( args, "kby#", &id,&ext, &buf, &len );
	return Py_BuildValue( "b", CAN.sendMsgBuf( id,ext,len,buf ) );
}
static PyObject* mcp2515_readMsgBuf(PyObject* self,PyObject* args)
{

INT8U	len;
INT8U	*buf[MAX_CHAR_IN_MESSAGE ];
INT8U	ret;

	ret = CAN.readMsgBuf( &len,(INT8U *)buf );
	return Py_BuildValue( "by#",ret,buf,len );
}

static PyObject* mcp2515_checkReceive(PyObject* self,PyObject* args)
{
	return Py_BuildValue( "b", CAN.checkReceive( ) );
}

static PyObject* mcp2515_checkError(PyObject* self,PyObject* args)
{
	return Py_BuildValue( "b", CAN.checkError(  ) );
}

static PyObject* mcp2515_getCanId(PyObject* self,PyObject* args)
{
	return Py_BuildValue( "b", CAN.getCanId(  ) );
}

static PyMethodDef SpiMethods[] =
{
	{(char *)"openSPI", 		(PyCFunction)openSPI, 				METH_KEYWORDS, 	(char *)"Open SPI Port."},
	{(char *)"transfer", 		(PyCFunction)transfer, 				METH_VARARGS, 	(char *)"Transfer data."},
	{(char *)"closeSPI", 		(PyCFunction)closeSPI, 				METH_NOARGS, 	(char *)"Close SPI port."},
	{(char *)"MCPbegin", 		(PyCFunction)mcp2515_begin, 		METH_VARARGS, 	(char *)"init can."},
	{(char *)"MCPinit_Mask",	(PyCFunction)mcp2515_init_Mask, 	METH_VARARGS, 	(char *)"init Mask."},
	{(char *)"MCPinit_Filt", 	(PyCFunction)mcp2515_init_Filt, 	METH_VARARGS, 	(char *)"init filters."},
	{(char *)"MCPsendMsgBuf", 	(PyCFunction)mcp2515_sendMsgBuf, 	METH_VARARGS, 	(char *)"Send buffers."},
	{(char *)"MCPreadMsgBuf", 	(PyCFunction)mcp2515_readMsgBuf, 	METH_NOARGS, 	(char *)"read Buffers."},
	{(char *)"MCPcheckReceive",	(PyCFunction)mcp2515_checkReceive, 	METH_NOARGS, 	(char *)"check if something arived."},
	{(char *)"MCPcheckError", 	(PyCFunction)mcp2515_checkError, 	METH_NOARGS, 	(char *)"check error."},
	{(char *)"MCPgetCanId", 	(PyCFunction)mcp2515_getCanId, 		METH_NOARGS, 	(char *)"get can id when receive."},
	{(char *)NULL, 				(PyCFunction)NULL, 						0, 				(char *)NULL}
};

PyMODINIT_FUNC

initspi(void)
{
	init(); 					// Init all the stuff for hw pin....
	(void) Py_InitModule("spi", SpiMethods);
}
