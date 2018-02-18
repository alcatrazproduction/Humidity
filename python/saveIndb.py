import logging
import threading
import time
import libdb
import database
import define
import signal

from define import variables_list
from time import sleep
import fondarex
import _mysql
import sys

logger = logging.getLogger("modbus_tk")
logger.setLevel( 10 )

	
def getPartName_1():
	return libdb.getPartName( variables_list[3] )
def getProdNum_1( ):
	return (int)( libdb.getProdNum( variables_list[4] ) )
def getIngNum_1(  ):
	return (int)( libdb.getIngNum( variables_list[2] ) )
def getT1_1(  ):
	return (int)( libdb.getT1( variables_list[5] ) )
def getT2_1(  ):
	return (int)( libdb.getT2( variables_list[6] ) )
def getNumChannel_1( var ):
	return (int)( libdb.getNumChannel( variables_list[9] ) )
def getT3_1(  ):
	return (int)( libdb.getT3( variables_list[7] ) )
def setIngNum_1( arg ):
	libdb.setIngNum( arg,  variables_list[2] )

class saveData( threading.Thread ):
	
	def maxOf( self, first, second ):
		if first > second:
			if first < 0:
				return 0
			if first > 4095:
				return 4095
			return first
		if second < 0:
			return 0
		if second > 4095:
			return 4095
		return second


	def makeValue( self, value ):

		if value > self.maxValue:
			self.maxValue = value
		if value < self.minValue:
			self.minValue = value
		newValue = ( self.baseMax - value ) * ( 4096 / (self.baseMax - self.baseMin ))
		if newValue < 0:
			return 0
		return newValue

	def saveSample2Db(self):
		try:
			data 			= fondarex.getData()
			part_name		= fondarex.getPartName()
			prod_num		= fondarex.getProdNum()
			ing_num			= fondarex.getCurrentInjection()
			con = libdb.getDb()
			while data != None :
				try:
					qry = "INSERT INTO T_Humidity_Value VALUE ( NULL, ( SELECT id FROM T_Injection WHERE part_name LIKE '%s' AND prod_num LIKE '%s' AND injection LIKE '%s' ),'%s','%s','%s','%s','%s','%s','%s','%s')" % \
						( part_name, prod_num, ing_num, self.makeValue( self.maxOf( data[0], data[1] ) ), ( 8192 - (  data[0] + data[1] )) / 2, data[0], data[1], self.cal_a, self.cal_a, self.cal_a, self.cal_a )
					for i in range( 1,3 ):
							con.query( qry )
					data = fondarex.getData()
				except:
					print "Unexpected error (saveSample2Db):", sys.exc_info()
			libdb.releaseDb( con )
		finally:
			return


	def createNewInjection( self ):

		try:
			print( fondarex.getCurrentInjection() )
			fondarex.setCurrentInjection( fondarex.getCurrentInjection() + 1 )
			print( fondarex.getCurrentInjection() )
			setIngNum_1( fondarex.getCurrentInjection() )

			con = libdb.getDb()
			con.query(\
				"INSERT INTO T_Injection VALUE ( NULL,'%s','%s','%s', NOW(),'%s','%s','%s',9999,9999,9999,9999)" % \
				( fondarex.getPartName(), fondarex.getProdNum(), fondarex.getCurrentInjection(), fondarex.getT1(), fondarex.getT2(), fondarex.getT3() )\
				)
			libdb.releaseDb( con )
		except :
			print "Unexpected error (createNewInjection):", sys.exc_info()
			libdb.releaseDb( con )


	def __init__( self, thName ):
		try:
			threading.Thread.__init__(self)
			self.name		= thName
			logger.info("Save Theard theard...")
			libdb.openDB()
			self.cal_a		= 0
			self.loop		= threading.Event()
			self.evt		= threading.Event()
			self.maxValue	= 4096
			self.minValue	= 0
			self.baseMax	= 4096 - 500
			self.baseMin	= 500
			fondarex.setPartName( 			getPartName_1()	)
			fondarex.setProdNum( 			getProdNum_1()	)
			fondarex.setCurrentInjection(	getIngNum_1()	)
			fondarex.setT1( 				getT1_1()		)
			fondarex.setT2( 				getT2_1()		)
			fondarex.setT3( 				getT3_1()		)
			fondarex.setNumChan(			1				)		## TODO: change to variable in database
			fondarex.setSampleRate(			20				)		## TODO: changte to variable in database, actually 20ms as the siemens does
		except Exception, excpt:
			logger.error("Error in saveData.__init__: %s" % str(excpt))

	def run( self ):
		try:
			while not self.loop.isSet() :
				self.evt.wait(120)
				if ( self.evt.isSet() ) & ( not self.loop.isSet() ):
					minmax = fondarex.getMinMax()
					self.minValue = minmax[0]
					self.maxValue = minmax[1]
					self.baseMin = self.minValue - 50
					if self.baseMin < 0 :
						self.baseMin = 0
					self.baseMax = self.maxValue + 50
					if self.baseMax > 4096 :
						self.baseMax = 4096
					logger.info("--> baseMin is: %d, baseMax is: %d, minValue is : %d, maxValue is : %d, Factor is : %d" %
						( self.baseMin, self.baseMax, self.minValue, self.maxValue, ( 4096 / (self.baseMax - self.baseMin )) ))
					self.createNewInjection()
					self.minValue = self.baseMax
					self.maxValue = self.baseMin
					self.saveSample2Db()
					self.evt.clear()
		except Exception, excpt:
			logger.error("unexcepted error in saveData : %s" % str(excpt))
		except:
			logger.info("Interrult ?...")
		logger.info("Database  saving task exiting on close")
