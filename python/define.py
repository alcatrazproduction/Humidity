#!/usr/bin/env python
#/********************************************************************************************************
# * Define for modbus interface
# * $Author	: Yves Huguenin
# * $Created	: 14.01.2014
# * $Version	: 1.0
# *
# *******************************************************************************************************/

ARDUINO_SOFT_VERSION					= '$VER: 14.02.4.0'

def getArVersion():
	return ARDUINO_SOFT_VERSION

def setArVersion( arg ):																	# Does nothing, only a place holder... 
	return

import libdb

# Slave codding:

GLOBAL_SLAVE							= 1
X_AXE_SLAVE								= 99
CHANNEL_SLAVE_BASE						= 100

MAX_CHANNEL_SLAVE						= 49

VOLUME_SLAVE_BASE						= 150
MAX_VOLUME_SLAVE						= 49

# Datatype SIZE
SIZE_CHANNEL_DATA						= (999 + 1)
TYPEDEF_ARRAY							= 10
SIZE_VARCHAR							= ( 31 + 1) / 2
TYPEDEF_STRING							= 1
SIZE_ASCII								= 80 / 2
TYPEDEF_INT								= 2
SIZE_INT								= 2 / 2
TYPEDEF_DOUBLE							= 3
SIZE_DOUBLE								= 4 / 2
TYPEDEF_FLOAT							= 4
SIZE_FLOAT								= 4 / 2

ARDUINO_GLOBAL							= 5
ARDUINO_MESURES							= 6

# Block Base address
DATA_CHANNEL_BASE						= 1000
VARIABLES_BASE							= 100
# Channel define
DATA_CHANNEL_ARRAY						= DATA_CHANNEL_BASE
CHANNEL_BLOCK_NAME						= 'channel'
# Global Variables defines

GLOBAL_BLOCK_NAME						= 'global'
REQCURVE_BASE							= 184
LOADCURVE_BASE							= 186

VA_VERSION_BASE							= VARIABLES_BASE										# 100 
VA_VERSION_SIZE							= SIZE_ASCII

VA_READWRITE_BASE						= VA_VERSION_BASE + VA_VERSION_SIZE						# 140
VA_READWRITE_SIZE						= SIZE_ASCII

VA_CURRENT_INJECTION_BASE				= VA_READWRITE_BASE + VA_READWRITE_SIZE					# 180
VA_CURRENT_INJECTION_SIZE				= SIZE_DOUBLE											#  TODO: MUST CHECK FOR DOUBLE VALUE, INT LIMITED TO 32535 VALUES

VA_PART_NAME_BASE						= VA_CURRENT_INJECTION_BASE + VA_CURRENT_INJECTION_SIZE	# 182
VA_PART_NAME_SIZE						= SIZE_ASCII

VA_PROD_NUM_BASE						= VA_PART_NAME_BASE + VA_PART_NAME_SIZE					# 222
VA_PROD_NUM_SIZE						= SIZE_INT

VA_T1_BASE								= VA_PROD_NUM_BASE + VA_PROD_NUM_SIZE					# 223
VA_T1_SIZE								= SIZE_INT

VA_T2_BASE								= VA_T1_BASE + VA_T1_SIZE								# 224
VA_T2_SIZE								= SIZE_INT

VA_T3_BASE								= VA_T2_BASE + VA_T2_SIZE								# 225
VA_T3_SIZE								= SIZE_INT

VA_CMD_BASE								= VA_T3_BASE + VA_T3_SIZE								# 226
VA_CMD_SIZE								= SIZE_ASCII

VA_NCHAN_BASE							= VA_CMD_BASE + VA_CMD_SIZE								# 266
VA_NCHAN_SIZE							= SIZE_INT

variables_list = [						# str: var mame, ptr read function, ptr write function
	["ARDUINO_SFVERSION",			getArVersion,		setArVersion,		VA_VERSION_BASE, 			VA_VERSION_SIZE				,TYPEDEF_STRING],
	["ARDUINO_READWRITE",			libdb.arduinoRead,	libdb.arduinoWrite,	VA_READWRITE_BASE, 			VA_READWRITE_SIZE			,TYPEDEF_STRING],
	["ARDUINO_CURRENT_INJECTION",	libdb.getIngNum,	libdb.setIngNum,	VA_CURRENT_INJECTION_BASE, 	VA_CURRENT_INJECTION_SIZE	,TYPEDEF_DOUBLE],
	["ARDUINO_PART_NAME",			libdb.getPartName,	libdb.setPartName, 	VA_PART_NAME_BASE, 			VA_PART_NAME_SIZE			,TYPEDEF_STRING],
	["ARDUINO_PROD_NUM",			libdb.getProdNum,	libdb.setProdNum, 	VA_PROD_NUM_BASE, 			VA_PROD_NUM_SIZE			,TYPEDEF_INT],
	["ARDUINO_T1",					libdb.getT1,		libdb.setT1,		VA_T1_BASE, 				VA_T1_SIZE					,TYPEDEF_INT],
	["ARDUINO_T2",					libdb.getT2,		libdb.setT2,		VA_T2_BASE, 				VA_T2_SIZE					,TYPEDEF_INT],
	["ARDUINO_T3",					libdb.getT3,		libdb.setT3,		VA_T3_BASE, 				VA_T3_SIZE					,TYPEDEF_INT],
	["ARDUINO_CMD",					libdb.getLastCmd,	libdb.executeCmd, 	VA_CMD_BASE, 				VA_CMD_SIZE					,TYPEDEF_STRING],
	["ARDUINO_NUM_CHANNEL",			libdb.getNumChannel,libdb.setNumChannel,VA_NCHAN_BASE, 				VA_NCHAN_SIZE				,TYPEDEF_INT],
	]

