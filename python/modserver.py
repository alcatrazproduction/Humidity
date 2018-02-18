#!/usr/bin/env python

from __future__ import with_statement

import sys

#add logging capability

import struct
import modbus_tk.utils as utils
import logging
import threading
from modbus_tk.utils import threadsafe_function
from modbus_tk.hooks import call_hooks

import modbus_tk
import modbus_tk.defines as cst
import modbus_tk.modbus as modbus
import modbus_tk.modbus_tcp as modbus_tcp

import libdb
import database
import define
import hardware
import signal
import saveIndb
import fondarex
# Utility routine

#####################################################################################
#	Class: 		None																#
#	Member:		string2int															#
#	Parameters:	theString			the string to convert in an array of int		#
#	Return:		array of integer													#
#####################################################################################

def string2int( theString):

	values = []
	try:
		if( len( theString )%2 ):
			theString = theString + '\0'
		while len( theString ) > 0 :
			value = ord( theString[1:2] ) * 256
			value = value + ord( theString[0:1])
			values.append( value )
			theString = theString[2:]
	except:
		print "Unexpected error (string2int): %s "%( theString ), sys.exc_info()
	try:
		while len( values ) < define.SIZE_ASCII:
				values.append( 0 )
	except:
		print "Unexpected error (string2int, in padding): %s "%( theString ), sys.exc_info()		
	return values
	
#####################################################################################
#	Class: 		None																#
#	Member:		int2string															#
#	Parameters:	theValues			array of integer defining the string			#
#	Return:		the string															#
#####################################################################################

def int2string( theValues ):

	theString = ''
	try:
		while len( theValues ) > 0 :
			value = theValues.pop(0)
			theString += chr( value & 0x00ff )
			theString += chr( value / 0x0100 )
	except:
		print "Unexpected error (int2string): %s "%( theString ), sys.exc_info()
	return theString

# Adruino Server Class
#-------------------------------------------------------------------------------
class ArduinoSlave( modbus.Slave ):
	
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data	#
#	Return:		None																#
#####################################################################################

	def __init__(self, id):
		
		self._id = id
		self._blocks = {}
		self._memory = {	cst.COILS:[],
							cst.DISCRETE_INPUTS:[],
							cst.HOLDING_REGISTERS:[],
							cst.ANALOG_INPUTS:[],
							define.ARDUINO_GLOBAL:[],
							define.ARDUINO_MESURES:[]
						}
		self._data_lock = threading.Lock()
		self._fn_code_map = {	cst.READ_COILS: 				self._read_coils,
								cst.READ_DISCRETE_INPUTS: 		self._read_discrete_inputs,
								cst.READ_INPUT_REGISTERS: 		self._read_input_registers,
								cst.READ_HOLDING_REGISTERS:		self._read_holding_registers,
								cst.WRITE_SINGLE_COIL: 			self._write_single_coil,
								cst.WRITE_SINGLE_REGISTER: 		self._write_single_register,
								cst.WRITE_MULTIPLE_COILS: 		self._write_multiple_coils,
								cst.WRITE_MULTIPLE_REGISTERS: 	self._write_multiple_registers,
							 }
							 
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		_get_block_and_offset												#
#	Parameters:	block_type			type of the block								#
#				address				memory address of the block						#
#				length				size of the block								#
#	Return:		block pointer and offset from address								#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def _get_block_and_offset(self, block_type, address, length):
		
		if ( block_type == cst.HOLDING_REGISTERS ) :
			if( self._id == define.GLOBAL_SLAVE ):
				block_type = define.ARDUINO_GLOBAL
			elif ( self._id >= define.CHANNEL_SLAVE_BASE ):
				block_type = define.ARDUINO_MESURES
				
		for block in self._memory[block_type]:
			if address >= block.starting_address:
				offset = address - block.starting_address
				if block.size >= offset+length:
					return block, offset

		raise modbus.ModbusError(cst.ILLEGAL_DATA_ADDRESS)
	
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		_read_digital														#
#	Parameters:	block_type			type of the block								#
#				request_pdu			the request from the master						#
#	Return:		the response														#
#	Raise:		ModbusError, ILLEGAL_DATA_VALUE										#
#####################################################################################

	def _read_digital(self, block_type, request_pdu):
		
		(starting_address, quantity_of_x) = struct.unpack(">HH", request_pdu[1:5])

		if (quantity_of_x <= 0) or (quantity_of_x>2000):
			# maximum allowed size is 2000 bits in one reading
			raise modbus.ModbusError(cst.ILLEGAL_DATA_VALUE)

		block, offset = self._get_block_and_offset(block_type, starting_address, quantity_of_x)

		values = block[offset:offset+quantity_of_x]

		#pack bits in bytes
		byte_count = quantity_of_x / 8
		if (quantity_of_x % 8) > 0:
			byte_count += 1

		# write the response header
		response = struct.pack(">B", byte_count)

		i, byte_value = 0, 0
		for coil in values:
			if coil:
				byte_value += (1 << i)
			if i >= 7:
				# write the values of 8 bits in a byte
				response += struct.pack(">B", byte_value)
				#reset the counters
				i, byte_value = 0, 0
			else:
				i += 1

		#if there is remaining bits: add one more byte with their values
		if i > 0:
			response += struct.pack(">B", byte_value)
		return response
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def _read_coils(self, request_pdu):

		return self._read_digital(cst.COILS, request_pdu)
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def _read_discrete_inputs(self, request_pdu):

		return self._read_digital(cst.DISCRETE_INPUTS, request_pdu)
		
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		_read_globals														#
#	Parameters:	starting_address	Address to start reading						#
#				quantity_of_x		size of data to read							#
#	Return:		the formated response												#
#	Raise:		None																#
#####################################################################################

	def _read_globals(self,starting_address, quantity_of_x):

		try:
			theValues = []
			end_addr = starting_address + quantity_of_x
			while starting_address < end_addr:
				flag = 0
				for var in define.variables_list:
					if var[3] == starting_address:
						theValue = var[1]( var )
						if var[5] == define.TYPEDEF_INT:
							try:
								theValues.append( int( theValue ) )
							except:
								theValues.append( 0 )
								print "Unexpected error (_read_globals, TYPEDEF_INT): %d, %d, %s "%( starting_address, end_addr, theValue ), sys.exc_info()
						elif var[5] == define.TYPEDEF_STRING:
							for v in string2int( theValue ):
								theValues.append( v )
						elif var[5] == define.TYPEDEF_DOUBLE:
							try:
								theValue = long( theValue )
							except:
								print "Unexpected error (_read_globals, TYPEDEF_DOUBLE): %d, %d, %s "%( starting_address, end_addr, theValue ), sys.exc_info()
								theValue = 0x00000000
							theValues.append( int( theValue & 0x0000ffff ))
							theValues.append( int( ( theValue >> 16) & 0x0000ffff ) )
						starting_address += var[4]
						flag = 1
						if starting_address >= end_addr:
							return theValues
				if flag == 0:
						starting_address += 1
		except:
				print "Unexpected error (_read_globals): %d, %d "%( starting_address, end_addr ), sys.exc_info()
		return theValues
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		_read_globals														#
#	Parameters:	starting_address	Address to start reading						#
#				quantity_of_x		size of data to read							#
#	Return:		the formated response											  	#
#	Raise:		None																#
#####################################################################################

	def _read_one_channel(self,theChannel,starting_address, quantity_of_x):

		if ( starting_address < define.DATA_CHANNEL_BASE ) or ( starting_address > define.DATA_CHANNEL_BASE + define.SIZE_CHANNEL_DATA ):
			return [],0
		return libdb.readChannelDataBlock( theChannel, starting_address - define.DATA_CHANNEL_BASE, quantity_of_x )

#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		_read_registers														#
#	Parameters:	block_type			type of block to read							#
#				request_pdu			the request to process							#
#	Return:		the formated response												#
#	Raise:		ModbusError, ILLEGAL_DATA_VALUE										#
#####################################################################################

	def _read_registers(self, block_type, request_pdu):
	
		(starting_address, quantity_of_x) = struct.unpack(">HH", request_pdu[1:5])

		if (quantity_of_x <= 0) or (quantity_of_x > 125):
			# maximum allowed size is 125 registers in one reading
			LOGGER.debug("quantity_of_x is %d" % quantity_of_x)
			raise modbus.ModbusError(cst.ILLEGAL_DATA_VALUE)

		if ( self._id == define.GLOBAL_SLAVE):
			values = self._read_globals(starting_address, quantity_of_x)
		elif ( self._id >= define.CHANNEL_SLAVE_BASE):
			values, quantity_of_x  = self._read_one_channel( self._id - define.CHANNEL_SLAVE_BASE, starting_address, quantity_of_x)
		else:
			block, offset = self._get_block_and_offset(block_type, starting_address, quantity_of_x)
			values = block[offset:offset+quantity_of_x]

		#write the response header
		response = struct.pack(">B", 2 * quantity_of_x)
		#add the values of every register on 2 bytes
		for reg in values:
			response += struct.pack(">H", reg)
		return response
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def _read_holding_registers(self, request_pdu):
	
		return self._read_registers(cst.HOLDING_REGISTERS, request_pdu)
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def _read_input_registers(self, request_pdu):
	
		return self._read_registers(cst.ANALOG_INPUTS, request_pdu)

#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		_write_globals														#
#	Parameters:	starting_address	Address to start reading						#
#				quantity_of_x		size of data to read							#
#	Return:		the formated response												#
#	Raise:		None																#
#####################################################################################

	def _write_globals(self,starting_address, quantity_of_x, request_pdu):

		try:
			count = 0
			end_addr = starting_address + quantity_of_x
			while starting_address < end_addr:
				flag = 0
				for var in define.variables_list:
					if var[3] == starting_address:
#						theValue = var[1]( )
						if var[5] == define.TYPEDEF_INT:
							try:
								theValue = struct.unpack(">H", request_pdu[6+2*count:8+2*count])[0]
								count += 1
							except:
								theValue = 0
								print "Unexpected error (_write_globals, TYPEDEF_INT): %d, %d, %d "%( starting_address, end_addr, theValue ), sys.exc_info()
						elif var[5] == define.TYPEDEF_STRING:
							theValues = []
							for v in xrange( define.SIZE_ASCII ):
								if count >= quantity_of_x:
									v = define.SIZE_ASCII + 10
								else:
									print request_pdu[6+2*count:8+2*count], count, quantity_of_x
									theValues.append( struct.unpack(">H", request_pdu[6+2*count:8+2*count])[0] )
									count +=1
							theValue = int2string( theValues )
						elif var[5] == define.TYPEDEF_DOUBLE:
							try:
								theValue = long( 0 ) + struct.unpack(">H", request_pdu[6+2*count:8+2*count])[0]
								print theValue
								count += 1
								theValue = ( theValue ) + ( struct.unpack(">H", request_pdu[6+2*count:8+2*count])[0] << 16 )
								print theValue
								count += 1
							except:
								print "Unexpected error (_read_globals, TYPEDEF_DOUBLE): %d, %d, %s "%( starting_address, end_addr, theValue ), sys.exc_info()
								theValue = 0x00000000
						var[2]( theValue, var )
						starting_address += var[4]
						flag = 1
						if starting_address >= end_addr:
							return count
				if flag == 0:
						starting_address += 1
		except:
				print "Unexpected error (_write_globals): %d, %d "%( starting_address, end_addr ), sys.exc_info()
		return count
		
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def _write_multiple_registers(self, request_pdu):
		(starting_address, quantity_of_x, byte_count) = struct.unpack(">HHB", request_pdu[1:6])

		if (quantity_of_x <= 0) or (quantity_of_x > 123) or (byte_count != (quantity_of_x * 2)):
			# maximum allowed size is 123 registers in one reading
			raise modbus.ModbusError(cst.ILLEGAL_DATA_VALUE)

		if self._id == define.GLOBAL_SLAVE :
			count = self._write_globals( starting_address, quantity_of_x, request_pdu )
		elif self._id == define.CHANNEL_SLAVE_BASE :
			count = 0
			raise modbus.ModbusError(cst.ILLEGAL_DATA_ADDRESS)
		else :
			# look for the block corresponding to the request
			block, offset = self._get_block_and_offset(cst.HOLDING_REGISTERS, starting_address, quantity_of_x)
			count = 0
			for i in xrange(quantity_of_x):
				count += 1
				block[offset+i] = struct.unpack(">H", request_pdu[6+2*i:8+2*i])[0]

		return struct.pack(">HH", starting_address, count)
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def _write_multiple_coils(self, request_pdu):
		
		(starting_address, quantity_of_x, byte_count) = struct.unpack(">HHB", request_pdu[1:6])

		expected_byte_count = quantity_of_x / 8
		if (quantity_of_x % 8) > 0:
			expected_byte_count += 1

		if (quantity_of_x <= 0) or (quantity_of_x > 1968) or (byte_count != expected_byte_count):
			# maximum allowed size is 1968 coils
			raise modbus.ModbusError(cst.ILLEGAL_DATA_VALUE)

		# look for the block corresponding to the request
		block, offset = self._get_block_and_offset(cst.COILS, starting_address, quantity_of_x)

		count = 0
		for i in xrange(byte_count):
			if count >= quantity_of_x:
				break
			(byte_value, ) = struct.unpack(">B", request_pdu[6+i])
			for j in xrange(8):
				if byte_value & (1 << j):
					block[offset+i*8+j] = 1
				else:
					block[offset+i*8+j] = 0
				if count >= quantity_of_x:
					break
				count += 1
		return struct.pack(">HH", starting_address, count)
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def _write_single_register(self, request_pdu):
	
		(data_address, value) = struct.unpack(">HH", request_pdu[1:5])
		if self._id == define.GLOBAL_SLAVE:
			for var in define.variables_list:
				if var[3] == data_address:
					if var[5] == define.TYPEDEF_INT:
						try:
							var[2]( theValue )
							return request_pdu[1:] #returns echo of the command
						except:
							theValue = 0
							print "Unexpected error (_write_globals, TYPEDEF_INT): %d, %d, %d "%( starting_address, end_addr, theValue ), sys.exc_info()
							return request_pdu[1:1] #returns echo of the command
			return request_pdu[1:1] #returns echo of the command

		else:
			block, offset = self._get_block_and_offset(cst.HOLDING_REGISTERS, data_address, 1)
			block[offset] = value
			return request_pdu[1:] #returns echo of the command
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def _write_single_coil(self, request_pdu):
	
		(data_address, value) = struct.unpack(">HH", request_pdu[1:5])
		block, offset = self._get_block_and_offset(cst.COILS, data_address, 1)
		if value == 0:
			block[offset] = 0
		elif value == 0xff00:
			block[offset] = 1
		else:
			raise modbus.ModbusError(cst.ILLEGAL_DATA_VALUE)
		return request_pdu[1:] #returns echo of the command
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def handle_request(self, request_pdu, broadcast=False):

		with self._data_lock: #thread-safe
			try:
				retval = call_hooks("modbus.Slave.handle_request", (self, request_pdu))
				if retval <> None:
					return retval

				# get the function code
				(function_code, ) = struct.unpack(">B", request_pdu[0])

				# check if the function code is valid. If not returns error response
				if not self._fn_code_map.has_key(function_code):
					raise modbus.ModbusError(cst.ILLEGAL_FUNCTION)

				# if read query is broadcasted raises an error
				cant_be_broadcasted = (cst.READ_COILS, cst.READ_DISCRETE_INPUTS,
									  cst.READ_INPUT_REGISTERS, cst.READ_HOLDING_REGISTERS)
				if broadcast and (function_code in cant_be_broadcasted):
					raise modbus.ModbusInvalidRequestError("Function %d can not be broadcasted" % function_code)

				#execute the corresponding function
				response_pdu = self._fn_code_map[function_code](request_pdu)
				if response_pdu:
					if broadcast:
						call_hooks("modbus.Slave.on_handle_broadcast", (self, response_pdu))
						LOGGER.debug("broadcast: %s" % (utils.get_log_buffer("!!", response_pdu)))
						return ""
					else:
						return struct.pack(">B", function_code) + response_pdu
				raise Exception("No response for function %d" % function_code)

			except modbus.ModbusError, excpt:
				LOGGER.debug(str(excpt))
				call_hooks("modbus.Slave.on_exception", (self, function_code, excpt))
				return struct.pack(">BB", function_code+128, excpt.get_exception_code())
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def add_block(self, block_name, block_type, starting_address, size):

		with self._data_lock: #thread-safe
			if size <= 0:
				raise modbus.InvalidArgumentError, "size must be a positive number"
			if starting_address < 0:
				raise modbus.InvalidArgumentError, "starting address must be zero or positive number"
			if self._blocks.has_key(block_name):
				raise modbus.DuplicatedKeyError, "Block %s already exists. " % (block_name)

			if not self._memory.has_key(block_type):
				raise modbus.InvalidModbusBlockError, "Invalid block type %d" % (block_type)

			# check that the new block doesn't overlap an existing block
			# it means that only 1 block per type must correspond to a given address
			# for example: it must not have 2 holding registers at address 100
			index = 0
			for i in xrange(len(self._memory[block_type])):
				block = self._memory[block_type][i]
				if block.is_in(starting_address, size):
					raise modbus.OverlapModbusBlockError, "Overlap block at %d size %d" % (block.starting_address, block.size)
				if block.starting_address > starting_address:
					index = i
					break

			# if the block is ok: register it
			self._blocks[block_name] = (block_type, starting_address)
			# add it in the 'per type' shortcut
			self._memory[block_type].insert(index, modbus.ModbusBlock(starting_address, size))
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def remove_block(self, block_name):

		with self._data_lock: #thread safe
			block = self._get_block(block_name)

			# the block has been found: remove it from the shortcut
			block_type = self._blocks.pop(block_name)[0]
			self._memory[block_type].remove(block)
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def remove_all_blocks(self):
		
		with self._data_lock: #thread safe
			self._blocks.clear()
			for key in self._memory:
				self._memory[key] = []
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def _get_block(self, block_name):
	
		if not self._blocks.has_key(block_name):
			raise modbus.MissingKeyError, "block %s not found" % (block_name)
		(block_type, starting_address) = self._blocks[block_name]
		for block in self._memory[block_type]:
			if block.starting_address == starting_address:
				return block
		raise Exception, "Bug?: the block %s is not registered properly in memory" % (block_name)
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def set_values(self, block_name, address, values):

		with self._data_lock: #thread safe
			block = self._get_block(block_name)

			# the block has been found
			# check that it doesn't write out of the block
			offset = address-block.starting_address

			size = 1
			if (type(values) is list) or (type(values) is tuple):
				size = len(values)

			if (offset < 0) or ((offset + size) > block.size):
				raise modbus.OutOfModbusBlockError, "address %s size %d is out of block %s" % (address, size, block_name)

			#if Ok: write the values
			if (type(values) is list) or (type(values) is tuple):
				block[offset:offset+len(values)] = values
			else:
				block[offset] = values
#####################################################################################
#	Class: 		ArduinoSlave from modbus.Slave										#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def get_values(self, block_name, address, size=1):

		with self._data_lock: #thread safe
			block = self._get_block(block_name)

			# the block has been found
			# check that it doesn't write out of the block
			offset = address - block.starting_address

			if (offset<0) or ((offset + size) > block.size):
				raise modbus.OutOfModbusBlockError, "address %s size %d is out of block %s" % (address, size, block_name)

			#returns the values
			if size == 1:
				return (block[offset],)
			else:
				return tuple(block[offset:offset+size])

# Local databank class

class ArduinoData( modbus.Databank ):
#####################################################################################
#	Class: 		ArduinoData from modbus.Databank									#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def __init__(self):
		"""Constructor"""
		self._slaves = {} # the map of slaves by ids
		self._lock = threading.Lock() # protect access to the map of slaves
#####################################################################################
#	Class: 		ArduinoData from modbus.Databank									#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def add_slave(self, slave_id):
		"""Add a new slave with the given id"""
		with self._lock: #thread-safe
			if (slave_id <= 0) or (slave_id > 255):
				raise Exception, "Invalid slave id %d" % (slave_id)
			if not self._slaves.has_key(slave_id):
				self._slaves[slave_id] = ArduinoSlave(slave_id)
				return self._slaves[slave_id]
			else:
				raise modbus.DuplicatedKeyError, "Slave %d already exists" % (slave_id)
			
#####################################################################################
#	Class: 		ArduinoData from modbus.Databank									#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def get_slave(self, slave_id):
		
		with self._lock: #thread-safe
			if self._slaves.has_key(slave_id):
				return self._slaves[slave_id]
			else:
				raise modbus.MissingKeyError, "Slave %d doesn't exist" % (slave_id)
			
#####################################################################################
#	Class: 		ArduinoData from modbus.Databank									#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#	Raise:		ModbusError, ILLEGAL_DATA_ADDRESS									#
#####################################################################################

	def remove_slave(self, slave_id):
		
		with self._lock: #thread-safe
			if self._slaves.has_key(slave_id):
				self._slaves.pop(slave_id)
			else:
				raise modbus.MissingKeyError, "Slave %d already exists" % (slave_id)
			
#####################################################################################
#	Class: 		ArduinoData from modbus.Databank									#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def remove_all_slaves(self):

		with self._lock: #thread-safe
			self._slaves.clear()
			
#####################################################################################
#	Class: 		ArduinoData from modbus.Databank									#
#	Member:		__init_																#
#	Parameters:	id					id of the slave, 1=global, 100.. = channel data #
#	Return:		None																#
#####################################################################################

	def handle_request(self, query, request):
		
		request_pdu = ""
		try:
			#extract the pdu and the slave id
			(slave_id, request_pdu) = query.parse_request(request)

			#get the slave and let him executes the action
			if slave_id == 0:
				#broadcast
				for key in self._slaves:
					self._slaves[key].handle_request(request_pdu, broadcast=True)
				return
			else:
				slave = self.get_slave(slave_id)
				response_pdu = slave.handle_request(request_pdu)
				#make the full response
				response = query.build_response(response_pdu)
				return response
		except Exception, excpt:
			call_hooks("modbus.Databank.on_error", (self, excpt, request_pdu))
			LOGGER.error("handle request failed: " + str(excpt))
		except:
			LOGGER.error("handle request failed: unknown error")

		#If the request was not handled correctly, return a server error response
		func_code = 1
		if len(request_pdu) > 0:
			(func_code, ) = struct.unpack(">B", request_pdu[0])
		return struct.pack(">BB", func_code+0x80, cst.SLAVE_DEVICE_FAILURE)

#-------------------------------------------------------------------------------

logger = modbus_tk.utils.create_logger(name="console", record_format="%(message)s")
LOGGER = logging.getLogger("modbus_tk")
LOGGER.setLevel( 10 )
logger.setLevel( 10 )


if __name__ == "__main__":

	try:
		libdb.openDB()
		slaves = []
		#Create the server
		try:								#-- we start the thead
			save2Db = saveIndb.saveData( "Database Task")
			logger.info("trying to start db saving task")
			save2Db.start()
		except excpt:
			logger.error("Error during creating the db saving task : " + str( excpt ))
			exit()
		try:								#-- we start the thead
			hardMonitor = hardware.monitor( "Hardware Monitor",save2Db, slaves)
			logger.info("trying to connect hardware")
		except excpt:
			logger.error("Error during creating the hardware montitor : " + str( excpt ))
			save2Db.loop.set()
			save2Db.evt.set()
			exit()
			
		logger.info("Creating S7 server")
		fondarex.initS7();
		logger.info("Starting S7 server")
		fondarex.startS7();
		
		server = modbus_tcp.TcpServer(address="0.0.0.0", databank=ArduinoData())
		logger.info("running...")
		logger.info("enter 'quit' for closing the server")

		server.start()


		# Add global slave
		slaves.append( server.add_slave( define.GLOBAL_SLAVE ) )

		slave = server.add_slave( define.X_AXE_SLAVE )
		slave.add_block(define.GLOBAL_BLOCK_NAME, cst.HOLDING_REGISTERS, define.VARIABLES_BASE, 100)
		slave.add_block(define.CHANNEL_BLOCK_NAME, cst.HOLDING_REGISTERS, define.DATA_CHANNEL_BASE, define.SIZE_CHANNEL_DATA)
		for v in range( define.DATA_CHANNEL_BASE, define.DATA_CHANNEL_BASE + define.SIZE_CHANNEL_DATA ):
			slave.set_values( define.CHANNEL_BLOCK_NAME , v, v )
		slaves.append( slave )
		test_slave = slave
	
		for v in range(0,3 ):
			slave = server.add_slave( define.CHANNEL_SLAVE_BASE + v )
			slaves.append( slave )


		try:
			logger.info("trying to start hardware")
			hardMonitor.start()
		except excpt:
			logger.error("Error during creating the hardware montitor : " + str( excpt ))
			fondarex.deleteS7();
			server.stop()
			save2Db.loop.set()
			save2Db.evt.set()
			exit()
			
		while True:
			try:
				cmd = sys.stdin.readline()
				args = cmd.split(' ')
				if cmd.find('quit')==0:
					sys.stdout.write('bye-bye\r\n')
					break
				else:
					fondarex.clearS7Event()
					sys.stdout.write("unknown command %s\r\n" % (args[0]))
			except:
				pass
			
	finally:
		pass
	logger.info("Stopping S7 server")
	fondarex.deleteS7()
	server.stop()
	logger.info("Stopping Hardware server")
	hardMonitor.loop.set()
	logger.info("Stopping database server")
	save2Db.loop.set()
	save2Db.evt.set()

			