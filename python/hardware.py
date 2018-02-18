import logging
import threading
import time
import libdb
import database
import define
import signal

from time import sleep
import fondarex
from saveIndb import getT1_1, getT2_1, getT3_1

logger = logging.getLogger("modbus_tk")
logger.setLevel( 10 )
global saveDb,inInterrupt, slaves

def doCalibration():
	global  inInterrupt, slaves, saveDb
	try:
		oldCal = saveDb.cal_a
		if oldCal < 1:
			oldCal = 1
		newCal = fondarex.doCalibration( 0x00, oldCal )
		if( newCal >= 250 ):
			logger.error("****** ERROR ******: Calibration level to high, please check glass ***********")
		saveDb.cal_a = newCal
		logger.info("Calibration level set from %d to %d"%( oldCal,newCal) )
	except Exception, excpt:
		logger.error("Error in monitor.doCalibration: %s" % str(excpt) )
		
def _doCalibration():
	global  inInterrupt, slaves, saveDb
	try:
		retry = 5
		oldCal = saveDb.cal_a
		if oldCal < 1:
			oldCal = 1
		newCal = oldCal
		sleep( 0.02 )
		level = fondarex.getIrRxStatus( 0x00 )
		while ( level[1] > 3500 ) and ( newCal > 2) and ( retry == 5 ):
			sleep( 0.05 )
			newCal -= 1
			fondarex.setIrTxStatus( 0x00, newCal, newCal )
			sleep( 0.05 )
			do
			while (retry > 0 ):
				level = fondarex.getIrRxStatus( 0x00 )
				if( level[0] == 0 ):
					retry = -1
				else :
					retry -= 1
					sleep( 0.5 )
			if( retry == 0 ):
				logger.info( "Error in doCalibration low, can't read level TX error")
			else :
				retry = 5
			print(" cal ", newCal, " level ", level )
		retry = 5
		while ( level[1] < 3500 ) and ( newCal < 255 ) and ( retry == 5 ):
			sleep( 0.05 )
			newCal += 1
			fondarex.setIrTxStatus( 0x00, newCal, newCal )
			sleep( 0.05 )
			while (retry > 0 ):
				level = fondarex.getIrRxStatus( 0x00 )
				if( level[0] == 0 ):
					retry = -1
				else :
					retry -= 1
					sleep( 0.5 )
			if( retry == 0 ):
				logger.info( "Error in doCalibration high, can't read level TX error, restoring old value %d "% ( oldCal ))
				newCal = oldCal
				fondarex.setIrTxStatus( 0x00, newCal, newCal )
			else:
				retry = 5
			print(" cal ", newCal," level ", level )
		if( newCal >= 250 ):
			logger.error("****** ERROR ******: Calibration level to high, please check glass ***********")
		saveDb.cal_a = newCal
		logger.info("Calibration level set from %d to %d"%( oldCal,newCal) )
	except Exception, excpt:
		logger.error("Error in monitor._doCalibration: %s" % str(excpt) )
		
def SigUSR1Handler(signum, frame):
	global inInterrupt, slaves,saveDb

	if not inInterrupt:
		try:
			inInterrupt = (1 == 1 )
			why = fondarex.getInterrupt()
			logger.info("Sig usr...." + str( why )   )
			if ( why & (1 << 0)):				# DieOpen
				if not ( why & (1 << 4)):		# do only if not loosing die open
					doCalibration()
			if ( why & (1 << 1)):				# VacuStart
				pass
			if ( why & (1 << 2)):				# MCP interrupt
				pass
			if ( why & (1 << 3)):				# DataReady
			
				saveDb.evt.set()
				slaves[1].set_values( define.GLOBAL_BLOCK_NAME, define.REQCURVE_BASE,(0xFFFF,0xFFFF))
				slaves[1].set_values( define.GLOBAL_BLOCK_NAME, define.LOADCURVE_BASE,(0xFFFF,0xFFFF))
				fondarex.setReqCurve( 0xffff )
				fondarex.setLoadCurve( 0xffff )
				
			if ( why & (1 << 4)):				# DieOpen falling
				fondarex.setTimerValue( getT1_1(), getT2_1(), getT3_1() )
		except Exception, excpt:
			logger.error("Error in monitor.SigUSR1Handler: %s" % str(excpt) )
		inInterrupt = ( 0 == 1 )
	return
	
class monitor( threading.Thread ):
	

		
	def __init__( self, thName, theEvent, theSlaves):
		global saveDb, inInterrupt, slaves
		try:
			threading.Thread.__init__(self)
			self.name		= thName
			self.loop		= threading.Event()
			saveDb			= theEvent
			slaves			= theSlaves
			logger.info("Starting hardware monitor theard...")
			libdb.openDB()
			fondarex.MCPbegin( 11 )
			sleep( 0.1 )
			sleep( 0.1 )
			doCalibration()
			inInterrupt = ( 1 == 1 )
			fondarex.installInterrupt()
			signal.signal( signal.SIGUSR1, SigUSR1Handler )
			inInterrupt = ( 0 == 1 )
		except Exception, excpt:
			logger.error("Error in monitor.__init__: %s" % str(excpt))

	def run( self ):
		try:
			while not self.loop.isSet():
				try:
					self.loop.wait()
				except Exception, excpt:
					logger.error("unexcepted error in monitor ( main loop ) : %s" % str(excpt))
		except Exception, excpt:
					logger.error("unexcepted error in monitor (run ): %s" % str(excpt))
		logger.info("Hardware monitor task exiting on close")