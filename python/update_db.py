#!/usr/bin/python
#
#  (c) FONDAREX SA
#  $Author:       Yves Huguenin
#  $Title:        update_db.py
#  $Create Date:  20.12.2013
#  $Vers:         1.0a1
#
#  1.0a1:
#	Manage all the interface between Siemens and the mysql database
#	actualy with the httpSiemens protocole
#


# import all the modules we need
import _mysql
import sys, getopt, os


# global definition

global connectionList, con
global theInjection


def openDB():
# Open a set of database connection, 10 will be enouth 
	global connectionList
	try:
# we do all in a try block to catch the exception...
		connectionList = [
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarexV2'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarexV2'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarexV2'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarexV2'), False ],
			]
# TODO: we must change the constant to a variable in the future...
	except:
# if an error, show it on the console and exit.
		print "Unexpected error (openDB):", sys.exc_info()
		sys.exit(1)

# ***********************************************************************************************************
# get a free connection from the array, and set it to be used
#
# ***********************************************************************************************************		
def getDb():

# (YHU 03.12.2013)added test of connection lost, if yes, attempt to reconnect
	global connectionList
	try:
		while True:
# loop until we found a free connection and parse the array
			for oneConnection in connectionList:
				if oneConnection[1] == False :
# ok, we found a free connection, set it to be used
					oneConnection[1] = True
					try:
# try if is alive and does not die
						oneConnection[ 0 ].ping()
# ok, so return the connection to the caller
						return oneConnection[ 0 ]
					except:
# try to reopen it
						oneConnection[1] = False
# TODO: must check if ok to set it unsed, maybe this should give as a problem
						oneConnection[ 0 ].close()
# Close the connection and try to reconnect
						oneConnection[ 0 ] = _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex')
# OK, so set it to be used and return it to the caller
						oneConnection[1] = True
						return oneConnection[ 0 ] 
	except:
		print "Unexpected error (getDb):", sys.exc_info()
# in case of error, return a null connection after printing the error to the console
		return null
# ***********************************************************************************************************
# Release a connection, so it will be avaible for an other thread
# ***********************************************************************************************************
def releaseDb( theLink ):
	global connectionList
	try:
		while True:
# TODO: must not be in a permanent loop
			for oneConnection in connectionList:
# parse the array of connection to find the good one 
				if oneConnection[0] == theLink :
# when found, set it to be unused
					oneConnection[1] = False
					return
	except:
		print "Unexpected error (releaseDb):", sys.exc_info()
		
# ******************* Some routine to access Arduino **************************************************************************************


def createError( errnum ):
	return "E%8d#%f"%(errnum, random.random() )
#def readChannelDataBlock( theChannel, start, quantity ):

def doUpdateDb():
	global con,theInjection,con1,con2
	try:
		con 	= getDb()
		con1 	= getDb()
		con2	= getDb()
#		return																							# TODO: remove
		print "    Starting transaction"
		try:
			con2.query("START TRANSACTION")
		except:
			print "Unexpected error Starting transaction:", sys.exc_info()
			return
		try:
			con2.query("DROP TABLE IF EXISTS T_Channel_Info")
			con2.query("CREATE TABLE T_Channel_Info ("
				"id                  bigint PRIMARY KEY NOT NULL AUTO_INCREMENT, "
				"injection_id        bigint NOT NULL, "
				"channel             int    NOT NULL, "
				"sample_count        int, "
				"sample_rate         int    DEFAULT 20, "
				"humidity_max        int    DEFAULT 4095, "
				"humidity_min        int    DEFAULT 0, "
				"humidity_flags      int, "
				"humidity_cal        int, "
				"humidity_cal_res    int, "
				"temperature_max     int    DEFAULT 4095, "
				"temperature_min     int    DEFAULT 0, "
				"temperature_flags   int, "
				"temperature_cal     int, "
				"temperature_cal_res int, "
				"vaccum_max          int    DEFAULT 4095, "
				"vaccum_min          int    DEFAULT 0, "
				"vaccum_flags        int, "
				"vaccum_cal          int, "
				"vaccum_cal_res      int, "
				"humidity_lavg       int    DEFAULT 90, "
				"humidity_lstart     int    DEFAULT 100, "
				"humidity_lstop      int    DEFAULT 3000 "
				")")
		except:
			print "Unexpected error Creating TABLE T_Channel_Info:", sys.exc_info()
			con2.rollback()
			return

		try:
			print "      Quering list of injection"
			con.query("SELECT  injection_id,max(channel_a),max(channel_b),max(channel_c),max(channel_d),min(channel_a),min(channel_b),min(channel_c),min(channel_d),count(*) FROM T_Humidity_Value GROUP BY injection_id")
#			print "       Fetcing result"
			list_ing = con.use_result()
			if( list_ing == 0 ):
				print "no resultset !!"
				return
#			print "       Fetcing first row"
			row_ing = list_ing.fetch_row()
			print "       Entering loop"
			while( row_ing ):
#				print "        Reading injection num %s " % row_ing
				theInjection 	= row_ing[0][0]
#				print "        Reading Sample count %s" % row_ing[0][9]
				sampleCount		= row_ing[0][9]
				print "               processing injection %s samples %s :" % (theInjection, sampleCount) 
				try:
					con1.query("SELECT cal_a,cal_b,cal_c,cal_d FROM T_Injection WHERE id=%s" % ( theInjection ) )
					res_ing = con1.use_result()
					if( res_ing  ):
						row = res_ing.fetch_row ()
						if( row ):
#							print row
							con2.query("INSERT INTO T_Channel_Info VALUES ( NULL,%s,0,%s,NULL,%s,%s,0,%s,0,0,0,0,0,0,0,0,0,0,0,0,0,0)" %
								( theInjection, sampleCount, row_ing[0][1],row_ing[0][5], row[0][0] ))
							con2.query("INSERT INTO T_Channel_Info VALUES ( NULL,%s,1,%s,NULL,%s,%s,0,%s,0,0,0,0,0,0,0,0,0,0,0,0,0,0)" %
								( theInjection, sampleCount, row_ing[0][2],row_ing[0][6], row[0][1] ))
							con2.query("INSERT INTO T_Channel_Info VALUES ( NULL,%s,2,%s,NULL,%s,%s,0,%s,0,0,0,0,0,0,0,0,0,0,0,0,0,0)" %
								( theInjection, sampleCount, row_ing[0][3],row_ing[0][7], row[0][2] ))
							con2.query("INSERT INTO T_Channel_Info VALUES ( NULL,%s,3,%s,NULL,%s,%s,0,%s,0,0,0,0,0,0,0,0,0,0,0,0,0,0)" %
								( theInjection, sampleCount, row_ing[0][4],row_ing[0][8], row[0][3] ))
							del res_ing
					row_ing = list_ing.fetch_row()
				except:
					print "Unexpected error main loop:", sys.exc_info()
					con2.rollback()
					return
			del list_ing
			con2.commit()		# TODO: Change to commit
		except:
			print "Unexpected error get list of injection:", sys.exc_info()
			con2.rollback()
	except:
		print "Unexpected error main init:", sys.exc_info()
	return

def updateFunction():
	global con,theInjection,con1,con2
	
	try:
		con.query("START TRANSACTION")
	except:
		print "Unexpected error Starting transaction:", sys.exc_info()
		return
	try:
		q = "ALTER TABLE T_Globals ADD   quand timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP()"
		con.query( q )
	except:
		print "Unexpected error Altering table T_Globals: %s " % q, sys.exc_info()

#	con.query("DELIMITER $$")
	try:
		q = "DROP FUNCTION IF EXISTS ReadChannel"
		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
	try:
		q ="CREATE FUNCTION ReadChannel( theInjection bigint, theChannel char, thePage int  )"\
		"RETURNS varchar( 255 ) "\
		"BEGIN "\
		"DECLARE theInt     int;"\
		"DECLARE theText    varchar( 255 ) DEFAULT '';"\
		"DECLARE done       int DEFAULT FALSE;"\
		"DECLARE i          int DEFAULT 0;"\
		"DECLARE csra CURSOR FOR "\
		"SELECT channel_a FROM T_Humidity_Value WHERE injection_id = theInjection;"\
		"DECLARE csrb CURSOR FOR "\
		"SELECT channel_b FROM T_Humidity_Value WHERE injection_id = theInjection;"\
		"DECLARE csrc CURSOR FOR "\
		"SELECT channel_c FROM T_Humidity_Value WHERE injection_id = theInjection;"\
		"DECLARE csrd CURSOR FOR "\
		"SELECT channel_d FROM T_Humidity_Value WHERE injection_id = theInjection;"\
		"DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;"\
		"SET thePage = thePage * 126;"\
		"IF theChannel = 'A' THEN "\
		"OPEN csra;"\
		"ELSEIF theChannel = 'B' THEN "\
		"OPEN csrb;"\
		"ELSEIF theChannel = 'C' THEN "\
		"OPEN csrc;"\
		"ELSEIF theChannel = 'D' THEN "\
		"OPEN csrd;"\
		"ELSE "\
		"RETURN NULL;"\
		"END IF;"\
		"theLoop: LOOP "\
		"IF theChannel = 'A' THEN "\
		"FETCH csra INTO theInt;"\
		"ELSEIF theChannel = 'B' THEN "\
		"FETCH csrb INTO theInt;"\
		"ELSEIF theChannel = 'C' THEN "\
		"FETCH csrc INTO theInt;"\
		"ELSEIF theChannel = 'D' THEN "\
		"FETCH csrd INTO theInt;"\
		"END IF;"\
		"IF done THEN "\
		"LEAVE theLoop;"\
		"END IF;"\
		"IF thePage = 0 THEN "\
		"SET theText = CONCAT( theText, CHAR( 128 | (( theInt >> 7) & 127 )), CHAR( 128 | (( theInt ) & 127 )));"\
		"IF i >= 126 THEN "\
		"LEAVE theLoop;"\
		"ELSE "\
		"SET i = i +1;"\
		"END  IF;"\
		"ELSE "\
		"SET thePage = thePage - 1;"\
		"END IF;"\
		"END LOOP;"\
		"IF theChannel = 'A' THEN "\
		"CLOSE csra;"\
		"ELSEIF theChannel = 'B' THEN "\
		"CLOSE csrb;"\
		"ELSEIF theChannel = 'C' THEN "\
		"CLOSE csrc;"\
		"ELSEIF theChannel = 'D' THEN "\
		"CLOSE csrd;"\
		"END IF;"\
		"RETURN theText;"\
		"END;"
#		"$$"

		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="DROP FUNCTION IF EXISTS CreateCsv"
		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="CREATE FUNCTION CreateCsv( theInjection bigint, theSep char) "\
		"RETURNS TEXT "\
		"BEGIN "\
		"DECLARE theInt   int;"\
		"DECLARE theText  TEXT DEFAULT '';"\
		"DECLARE done     int  DEFAULT FALSE;"\
		"DECLARE i        int  DEFAULT 0;"\
		"DECLARE samples  int  DEFAULT 0;"\
		"DECLARE ci_id           bigint(20);"\
		"DECLARE ci_part_name    varchar(80);"\
		"DECLARE ci_prod_num     int(11);"\
		"DECLARE ci_injection    int(11);"\
		"DECLARE ci_timecode     datetime;"\
		"DECLARE ci_t1           int(11);"\
		"DECLARE ci_t2           int(11);"\
		"DECLARE ci_t3           int(11);"\
		"DECLARE ci_cal_a        int(11);"\
		"DECLARE ci_cal_b        int(11);"\
		"DECLARE ci_cal_c        int(11);"\
		"DECLARE ci_cal_d        int(11);"\
		"DECLARE ci_hv_injection bigint(20);"\
		"DECLARE cc_id                  bigint(20);"\
		"DECLARE cc_injection_id        bigint(20);"\
		"DECLARE cc_channel             int(11);"\
		"DECLARE cc_sample_count        int(11);"\
		"DECLARE cc_sample_rate         int(11);"\
		"DECLARE cc_humidity_max        int(11);"\
		"DECLARE cc_humidity_min        int(11);"\
		"DECLARE cc_humidity_flags      int(11);"\
		"DECLARE cc_humidity_cal        int(11);"\
		"DECLARE cc_humidity_cal_res    int(11);"\
		"DECLARE cc_temperature_max     int(11);"\
		"DECLARE cc_temperature_min     int(11);"\
		"DECLARE cc_temperature_flags   int(11);"\
		"DECLARE cc_temperature_cal     int(11);"\
		"DECLARE cc_temperature_cal_res int(11);"\
		"DECLARE cc_vaccum_max          int(11);"\
		"DECLARE cc_vaccum_min          int(11);"\
		"DECLARE cc_vaccum_flags        int(11);"\
		"DECLARE cc_vaccum_cal          int(11);"\
		"DECLARE cc_vaccum_cal_res      int(11);"\
		"DECLARE cv_channel_a           int(11);"\
		"DECLARE cv_channel_b           int(11);"\
		"DECLARE cv_channel_c           int(11);"\
		"DECLARE cv_channel_d           int(11);"\
		"DECLARE cv_temp_a              int(11);"\
		"DECLARE cv_temp_b              int(11);"\
		"DECLARE cv_temp_c              int(11);"\
		"DECLARE cv_temp_d              int(11);"\
		"DECLARE cv_vaccum_a            int(11);"\
		"DECLARE cv_vaccum_b            int(11);"\
		"DECLARE cv_vaccum_c            int(11);"\
		"DECLARE cv_vaccum_d            int(11);"\
		"DECLARE cv_hv_vaccum_a         int(11);"\
		"DECLARE cv_hv_vaccum_b         int(11);"\
		"DECLARE cv_hv_vaccum_c         int(11);"\
		"DECLARE cv_hv_vaccum_d         int(11);"\
		"DECLARE csample CURSOR FOR "\
		"SELECT count(*) FROM T_Humidity_Value WHERE injection_id = theInjection;"\
		"DECLARE cinfo CURSOR FOR "\
		"SELECT * FROM T_Injection WHERE id = theInjection;"\
		"DECLARE ccinfo CURSOR FOR "\
		"SELECT * FROM T_Channel_Info WHERE injection_id = theInjection ORDER BY channel;"\
		"DECLARE cvalue CURSOR FOR "\
		"SELECT channel_a, channel_b, channel_c, channel_d,"\
		"temp_a, temp_b, temp_c, temp_d,"\
		"vaccum_a/10 as vaccum_a, vaccum_b/10 as vaccum_b, vaccum_c/10 as vaccum_c, vaccum_d/10 as vaccum_d,"\
		"hv_vaccum_a, hv_vaccum_b, hv_vaccum_c, hv_vaccum_d FROM T_Humidity_Value WHERE injection_id = theInjection ORDER BY id;"\
		"DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;"\
		"OPEN csample;"\
		"FETCH csample INTO samples;"\
		"CLOSE csample;"\
		"OPEN cinfo;"\
		"FETCH cinfo INTO ci_id, ci_part_name, ci_prod_num, ci_injection, ci_timecode, ci_t1, ci_t2, ci_t3,"\
		"ci_cal_a, ci_cal_b, ci_cal_c, ci_cal_d, ci_hv_injection;"\
		"CLOSE cinfo;"\
		"SET theText = CONCAT( \"Parts Name:\",theSep,ci_part_name,\"\\nProduction Number:\",theSep,ci_prod_num,"\
		"\"\\nInjection Number:\",theSep,ci_injection,\"\\nHighVac Injection:\",theSep,ci_hv_injection,"\
		"\"\\nDate & Time:\",theSep,ci_timecode,\"\\nNumber of values:\",theSep,samples,"\
		"\"\\nT1 T2 T3:\",theSep,ci_t1,theSep,ci_t2,theSep,ci_t3 );"\
		"OPEN ccinfo;"\
		"SET theText = CONCAT( theText,\"\\nChannel setting and values\\n\","\
		"\"Channel Number(Phy)\",theSep,\"Channel Number(Log)\",theSep,\"Sample Rate\",theSep,"\
		"\"Humidity: Max\",theSep,\"Min\",theSep,\"Flags\",theSep,\"Calibration Offset\",theSep,\"Calibration Value\",theSep,"\
		"\"Temperature: Max\",theSep,\"Min\",theSep,\"Flags\",theSep,\"Calibration Offset\",theSep,\"Calibration Value\",theSep,"\
		"\"Vaccum: Max\",theSep,\"Min\",theSep,\"Flags\",theSep,\"Calibration Offset\",theSep,\"Calibration Value\\n\" );"\
		"theLoop: LOOP "\
		"FETCH ccinfo INTO cc_id, cc_injection_id, cc_channel, cc_sample_count, cc_sample_rate, cc_humidity_max,"\
		"cc_humidity_min, cc_humidity_flags, cc_humidity_cal, cc_humidity_cal_res, cc_temperature_max,"\
		"cc_temperature_min, cc_temperature_flags, cc_temperature_cal, cc_temperature_cal_res,"\
		"cc_vaccum_max, cc_vaccum_min, cc_vaccum_flags, cc_vaccum_cal, cc_vaccum_cal_res;"\
		"IF done THEN "\
		"LEAVE theLoop;"\
		"END IF;"\
		"SET theText = CONCAT( theText, (cc_channel+1),theSep,-1,theSep, cc_sample_rate,theSep,"\
		"cc_humidity_max,theSep, cc_humidity_min,theSep, cc_humidity_flags,theSep, cc_humidity_cal,theSep, cc_humidity_cal_res,theSep,"\
		"cc_temperature_max,theSep, cc_temperature_min,theSep, cc_temperature_flags,theSep, cc_temperature_cal,theSep, cc_temperature_cal_res,theSep,"\
		"cc_vaccum_max,theSep, cc_vaccum_min,theSep, cc_vaccum_flags,theSep, cc_vaccum_cal,theSep, cc_vaccum_cal_res,\"\\n\");"\
		"END LOOP;"\
		"CLOSE ccinfo;"\
		"SET done = FALSE;"\
		"SET theText = CONCAT( theText,\"\\nSamples Values\\n\","\
		"\"Humidity: CH1\",theSep,\"CH2\",theSep,\"CH3\",theSep,\"CH4\",theSep,"\
		"\"Temperature: CH1\",theSep,\"CH2\",theSep,\"CH3\",theSep,\"CH4\",theSep,"\
		"\"Vaccum: CH1\",theSep,\"CH2\",theSep,\"CH3\",theSep,\"CH4\", theSep,"\
		"\"HigVac Vaccum: CH1\",theSep,\"CH2\",theSep,\"CH3\",theSep,\"CH4\\n\" );"\
		"OPEN cvalue;"\
		"theLoop: LOOP "\
		"FETCH cvalue INTO cv_channel_a, cv_channel_b, cv_channel_c, cv_channel_d, cv_temp_a,"\
		"cv_temp_b, cv_temp_c, cv_temp_d, cv_vaccum_a, cv_vaccum_b, cv_vaccum_c, cv_vaccum_d,"\
		"cv_hv_vaccum_a, cv_hv_vaccum_b, cv_hv_vaccum_c, cv_hv_vaccum_d;"\
		"IF done THEN "\
		"LEAVE theLoop;"\
		"END IF;"\
		"SET theText = CONCAT( theText, cv_channel_a,theSep, cv_channel_b,theSep, cv_channel_c,theSep, cv_channel_d,theSep,"\
		"cv_temp_a,theSep, cv_temp_b,theSep, cv_temp_c,theSep, cv_temp_d,theSep,"\
		"cv_vaccum_a,theSep, cv_vaccum_b,theSep, cv_vaccum_c,theSep, cv_vaccum_d,theSep,"\
		"cv_hv_vaccum_a,theSep, cv_hv_vaccum_b,theSep, cv_hv_vaccum_c,theSep, cv_hv_vaccum_d,\"\\n\");"\
		"END LOOP;"\
		"CLOSE cvalue;"\
		"return theText;"\
		"END;"
#		"$$"

		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="DROP FUNCTION IF EXISTS ReadGlobal"
		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="CREATE FUNCTION ReadGlobal( theName varchar(30) )"\
		"RETURNS text "\
		"BEGIN "\
		"DECLARE theInt       int;"\
		"DECLARE ok           int;"\
		"DECLARE theFloat     float;"\
		"DECLARE theText      text;"\
		"DECLARE theType      enum('int','text','float');"\
		"SELECT v_int, v_text, v_float, v_type, count(*) INTO theInt, theText, theFloat, theType, ok FROM T_Globals WHERE name = theName;"\
		"IF ok = 0 THEN "\
		"RETURN NULL;"\
		"END IF;"\
		"IF theType = 'int' THEN "\
		"RETURN theInt;"\
		"ELSEIF theType = 'float' THEN "\
		"RETURN theFloat;"\
		"END IF;"\
		"RETURN theText;"\
		"END;"\

#		"$$"

		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="DROP PROCEDURE IF EXISTS UpdateGlobal"
		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="CREATE PROCEDURE UpdateGlobal( IN theName varchar(30), IN theValue text ) "\
		"BEGIN "\
\
		"DECLARE ok       int;"\
		"DECLARE theType  enum('int','text','float');"\
\
		"SELECT v_type, count(*) INTO theType, ok FROM T_Globals WHERE name = theName;"\
		"IF ok = 0 THEN "\
		"CALL CreateGlobal( theName, theValue, 'text' );"\
		"ELSEIF theType = 'int' THEN "\
		"UPDATE T_Globals SET v_int = theValue WHERE name=theName;"\
		"ELSEIF theType = 'float' THEN "\
		"UPDATE T_Globals SET v_float = theValue WHERE name=theName;"\
		"ELSE "\
		"UPDATE T_Globals SET v_text = theValue WHERE name=theName;"\
		"END IF;"\
		"END;"\
#		"$$"

		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="DROP PROCEDURE IF EXISTS CreateGlobal"
		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="CREATE PROCEDURE CreateGlobal( IN theName varchar(30), IN theValue text, IN theType  enum('int','text','float'))"\
		"BEGIN "\
\
		"DECLARE ok   int;"\
\
		"SELECT  count(*) INTO ok FROM T_Globals WHERE name = theName;"\
		"IF ok = 1 THEN "\
		"CALL UpdateGlobal( theName, theValue );"\
		"ELSEIF theType = 'int' THEN "\
		"INSERT INTO T_Globals VALUES ( theName, theValue, NULL, NULL, theType );"\
		"ELSEIF theType = 'float' THEN "\
		"INSERT INTO T_Globals VALUES ( theName, NULL, NULL, theValue, theType );"\
		"ELSE "\
		"INSERT INTO T_Globals VALUES ( theName, NULL, theValue, NULL, theType );"\
		"END IF;"\
\
		"END;"\

#		"$$"

		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	try:
		q ="DELIMITER ;"
#		con.query( q )
	except:
		print "Unexpected error on function : %s " % q, sys.exc_info()
		con.rollback()
		return
		
	con.commit()

def main(argv):

	global con,con1,con2

	print "Trying to update the database ....."

	doUpdateDb()
	updateFunction()
	
	print "All done, Have a nice day ....."

	if con:
		con.close()
	if con1:
		con1.close()
	if con2:
		con2.close()

# open the db, so all is ok and make some init part....		
openDB()
if __name__ == "__main__":
   main(sys.argv[1:])   