#!/usr/bin/python
#
#  (c) FONDAREX SA
#  $Author:       Yves Huguenin
#  $Title:        libdb.py
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

global connectionList, part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d
global cal_a, cal_b, cal_c, cal_d, t1, t2, t3, var_list,theInjection,theChannel,thePage,NumChannel


def openDB():
# Open a set of database connection, 10 will be enouth 
	global connectionList
	try:
# we do all in a try block to catch the exception...
		connectionList = [
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
			[ _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex'), False ],
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
def executeCmd( command, var ):
	global theCmd,theInjection,theChannel, thePage

	try:
		thePage = 0
		theCmd 	= ""
		opts, args = getopt.getopt( command.split(),"c:I:C:",["cmd=","injection=","channel="])
		for opt, arg in opts:
			print opt
			print arg
			if opt == '-h':
				print 'test.py -i <inputfile> -o <outputfile>'
			elif opt in ("-c","--cmd"):
				theCmd = arg
				print "   Command"+theCmd
			elif opt in ("-I","--injection"):
				theInjection = arg
				print "   Injection"+theInjection
			elif opt in ("-C","--channel"):
				theChannel = arg
				print "   Channel "+theChannel
	except:
		print "Unexpected error (executeCmd):", sys.exc_info()

def getLastCmd( var  ):
	global theCmd

	return theCmd

def arduinoWrite( arg, var ):			# execute a write command
	return
def createError( errnum ):
	return "E%8d#%f"%(errnum, random.random() )
def arduinoRead( var ):				# execute a read command
# ( YHU 03.12.2013) All return value MUST begin with the high bit of the first char returned set when all is good
#                   if not, folow with 8 char error code, separator '#' and a random num to check value update
	global theCmd
	try:
		theCmd = theCmd.strip()
		for cmdInfo in cmd_list:
			if cmdInfo[0].find( theCmd ) != -1:
				return cmdInfo[1]()
			else:
				print "Unknow command "+theCmd
				return createError( 1 )
	except:
		print "Unexpected error (arduinoRead):", sys.exc_info()

	return ""
# ******************* Command definition
def getGraphData():
	global theInjection, theChannel, thePage

	try:
		con = getDb()
		con.query("SELECT ReadChannel( %s,'%s',%s   )" % (theInjection , theChannel , thePage )  )
		result = con.use_result()
		theValue = result.fetch_row()[0][0]
		releaseDb( con )
	except:
		theValue = createError( 0 )
		print "Unexpected error (getGraphData):", sys.exc_info()
	thePage += 1
	return theValue
# ******************* Command definition
def readChannelDataBlock( theChannel, start, quantity ):


	try:
		con = getDb()
		con.query("SELECT channel_%c FROM T_Humidity_Value WHERE injection_id = (SELECT v_int FROM T_Globals WHERE name='ARDUINO_CURRENT_INJECTION')  LIMIT %s,%s" % (theChannel + ord('a') , start, quantity )  )
		result = con.use_result()
		theMysqlValues = result.fetch_row( quantity, 0 )
		theValues = []
		for reg in theMysqlValues:
			theValues.append( int( reg[0] ) )
		releaseDb( con )
		while len( theValues ) < quantity:
			theValues.append( 0 )				# do some padding
	except:
		theValues = []
		print "Unexpected error (readChannelDataBlock):", sys.exc_info()
	return theValues, len( theValues )
	
# ******************* Getter and Setter ***************************************************************************************************
def getPartName_1():
	getPartName( variables_list[3] )
def getPartName( var ):
	global part_name
	try:
		con = getDb()
		con.query("SELECT v_text FROM T_Globals WHERE name='%s'" % var[0] )
		result = con.use_result()
		part_name = result.fetch_row()[0][0]
		releaseDb( con )
	except:
		part_name = os.getenv( var[0] )
	return part_name

def getProdNum_1( ):
	getProdNum( variables_list[4] )
def getProdNum( var ):
	global prod_num
	try:
		con = getDb()
		con.query("SELECT v_int FROM T_Globals WHERE name='%s'" % var[0] )
		result = con.use_result()
		prod_num = result.fetch_row()[0][0]
		releaseDb( con )
		if prod_num == None:
			prod_num = 0
	except:
		prod_num = os.getenv( var[0] )
	return prod_num

def getIngNum_1(  ):
	getIngNum( variables_list[2] )
def getIngNum( var ):
	global ing_num
	try:
		con = getDb()
		con.query("SELECT v_int FROM T_Globals WHERE name='%s'" % var[0] )
		result = con.use_result()
		ing_num = result.fetch_row()[0][0]
		releaseDb( con )
		if ing_num == None:
			ing_num = 0
	except:
		ing_num = os.getenv( var[0] )
	return ing_num

def getT1_1(  ):
	getT1( variables_list[5] )
def getT1( var ):
	global t1
	try:
		con = getDb()
		con.query("SELECT v_int FROM T_Globals WHERE name='%s'" % var[0] )
		result = con.use_result()
		t1 = result.fetch_row()[0][0]
		releaseDb( con )
		if t1 == None:
			t1 = 0
	except:
		t1 = os.getenv( var[0] )
	return t1
		
def getT2_1(  ):
	getT2( variables_list[6] )
def getT2( var ):
	global t2
	try:
		con = getDb()
		con.query("SELECT v_int FROM T_Globals WHERE name='%s'" % var[0] )
		result = con.use_result()
		t2 = result.fetch_row()[0][0]
		releaseDb( con )
		if t2 == None:
			t2 = 0
	except:
		t2 = os.getenv( var[0] )
	return t2

def getNumChannel_1( var ):
	getNumChannel( variables_list[9] )
def getNumChannel( var ):
	global NumChannel
	try:
		con = getDb()
		con.query("SELECT v_int FROM T_Globals WHERE name='%s'"  % var[0] )
		result = con.use_result()
		NumChannel = result.fetch_row()[0][0]
		releaseDb( con )
		if NumChannel == None:
			NumChannel = 0
	except:
		NumChannel = os.getenv( var[0])
	return NumChannel
	
def getT3_1(  ):
	getT3( variables_list[7] )
def getT3( var ):
	global t3
	try:
		con = getDb()
		con.query("SELECT v_int FROM T_Globals WHERE name='%s'" % var[0] )
		result = con.use_result()
		t3 = result.fetch_row()[0][0]
		releaseDb( con )
		if t3 == None:
			t3 = 0
	except:
		t3 = os.getenv( var[0] )
	return t3

def setPartName( arg,  var ):
	global part_name
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_text= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		part_name = arg
	except:
		print "Unexpected error (setPartName):", sys.exc_info()
		releaseDb( con )
		sys.exit(-1)

def setProdNum( arg,  var ):
	global prod_num
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		prod_num = arg
	except:
		print "Unexpected error (setProdNum):", sys.exc_info()
		releaseDb( con )
		sys.exit(-2)

def setIngNum_1( arg ):
	setIngNum( arg,  variables_list[2] )
def setIngNum( arg,  var ):
	global ing_num
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		ing_num = arg
	except:
		print "Unexpected error (setIngNum):", sys.exc_info()
		releaseDb( con )
		sys.exit(-3)

def setT1( arg,  var ):
	global t1
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		t1 = arg
	except :
		print "Unexpected error (setT1):", sys.exc_info()
		releaseDb( con )
		sys.exit(-2)
		
def setT2( arg,  var ):
	global t2
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		t2 = arg
	except:
		print "Unexpected error(setT2):", sys.exc_info()
		releaseDb( con )
		sys.exit(-2)

def setNumChannel( arg,  var ):
	global NumChannel
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		NumChannel = arg
	except :
		print "Unexpected error (setNumChannel):", sys.exc_info()
		releaseDb( con )
		sys.exit(-2)

def setT3( arg,  var ):
	global t3
	try:
		con = getDb()
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='%s'" % ( arg, var[0] ))
		releaseDb( con )
		t3 = arg
	except:
		print "Unexpected error (setT3):", sys.exc_info()
		releaseDb( con )
		sys.exit(-2)

def saveSample2Db():
	global part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d
	try:
		con = getDb()
		qry = "INSERT INTO T_Humidity_Value VALUE ( NULL, ( SELECT id FROM T_Injection WHERE part_name LIKE '%s' AND prod_num LIKE '%s' AND injection LIKE '%s' ),'%s','%s','%s','%s','%s','%s','%s','%s')" % \
			( part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d )
		con.query( qry )
		releaseDb( con )
	except:
		print "Unexpected error (saveSample2Db):", sys.exc_info()
		releaseDb( con )
		sys.exit(-3)

def createNewInjection():
	global  part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d, t1, t2, t3
	try:
		ing_num = long( ing_num ) + 1
		setIngNum( ing_num )
		con = getDb()
		con.query(\
			"INSERT INTO T_Injection VALUE ( NULL,'%s','%s','%s', NOW(),'%s','%s','%s',9999,9999,9999,9999)" % \
			( part_name, prod_num, ing_num, t1, t2, t3 )\
			)
		releaseDb( con )
	except :
		print "Unexpected error (createNewInjection):", sys.exc_info()
		releaseDb( con )
		sys.exit(-3)

def dbGetVarList():
	theList = "<TABLE>"
	try:
		con = getDb()
		con.query("SELECT * FROM T_Globals" )
		result = con.use_result()
		while True:
			row = result.fetch_row()
			if not row:
				break
			theRow = "<TR><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD></TR>" %\
				( row[0][0], row[0][1], row[0][2], row[0][3], row[0][4] )
			theList = theList + theRow
		releaseDb( con )
	except:
		releaseDb( con )
		print "Unexpected error (dbGetVarList):", sys.exc_info()
	return theList + "</TABLE>"
	
def dbReadOneVariable( theVars ):
	global var_list
	try:
		theVars = theVars.strip()
		for varInfo in var_list:
			if varInfo[0].find( theVars ) != -1:
				print varInfo[1]
				return varInfo[1]( varInfo )
			else:
				try:
					print varInfo[0].find( theVars )
					print " ????" + theVars
					con = getDb()
					con.query("SELECT ReadGlobal( '%s')" % ( theVars ) )
					result = con.use_result()
					releaseDb( con )
					return result.fetch_row()[0][0]
				except:
					print "Unexpected error( dbReadOneVariable, ReadGlobal vars):", sys.exc_info()
		return os.getenv( theVars )
	except:
		print "Unexpected error (dbReadOneVariable):", sys.exc_info()

	return 0
	
def dbWriteOneVariable( theVars, theValue ):
	global var_list
	try:
		theVars = theVars.strip()
		for varInfo in var_list:
			if varInfo[0].find( theVars ) != -1:
				return varInfo[2]( theValue, varInfo )
			else:
				try:
					con = getDb()
					con.query("CALL UpdateGlobal( '%s','%s')" % ( theVars, theValue ) )
					releaseDb( con )
				except:
					print "Unexpected error( dbWriteOneVariable, UpdateGlobal vars):", sys.exc_info()
					releaseDb( con )

		return -1
	except:
		print "Unexpected error (dbWriteOneVariable):", sys.exc_info()

	return 0

	
def main(argv):
	global part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d, t1, t2, t3
	
	channel_a = 0
	channel_b = 0
	channel_c = 0
	channel_d = 0

	cal_a 	= 0
	cal_b 	= 0
	cal_c 	= 0
	cal_d 	= 0
	ing_num = 0
	mode	= 0
	t1	= 0
	t2	= 0
	t3	= 0

	getPartName()
	getProdNum()
	getIngNum()
	
	try:
		opts, args = getopt.getopt(argv,"hisa:b:c:d:A:B:C:D:p:n:",["acha=","bchb=","cchc=","dchd=","Acaa=","Bcab=","Ccac=","Dcad=","ppart=","nnum="])
	except getopt.GetoptError:
		print 'test.py -i <inputfile> -o <outputfile>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'test.py -i <inputfile> -o <outputfile>'
			sys.exit()
		elif opt in ("-a"):
			channel_a = arg
		elif opt in ("-b"):
			channel_b = arg
		elif opt in ("-c"):
			channel_c = arg
		elif opt in ("-d"):
			channel_d = arg
		elif opt in ("-A"):
			cal_A = arg
		elif opt in ("-B"):
			cal_B = arg
		elif opt in ("-C"):
			cal_C = arg
		elif opt in ("-D"):
			cal_D = arg
		elif opt in ("-p"):
			setPartName( arg )
		elif opt in ("-n"):
			prod_num = arg
			setProdNum( arg )
		elif opt in ("-i"):
			mode = mode | 1
		elif opt in ("-s"):
			mode = mode | 2

	if mode & 1 :
		createNewInjection()
	if mode & 2 :
		saveSample2Db();

	if con:
		con.close()

# open the db, so all is ok and make some init part....		
openDB()
import define
var_list = define.variables_list
#	[						# str: var mame, ptr read function, ptr write function
#	["ARDUINO_READWRITE",		arduinoRead,	arduinoWrite	],
#	["ARDUINO_CURRENT_INJECTION",	getIngNum,	setIngNum	],
#	["ARDUINO_PART_NAME",		getPartName,	setPartName	],
#	["ARDUINO_PROD_NUM",		getProdNum,	setProdNum	],
#	["ARDUINO_T1",			getT1,		setT1		],
#	["ARDUINO_T2",			getT2,		setT2		],
#	["ARDUINO_T3",			getT3,		setT3		],
#	["ARDUINO_CMD",			getLastCmd,	executeCmd	],
#	]
	
cmd_list = [						# str: var mame, ptr read function, ptr write function
	["READ_GRAPH_DATA",	getGraphData,	setIngNum	],
	["SELECT_DATA",		getPartName,	setPartName	],
	["DO_SELECT",		getProdNum,	setProdNum	],
	["",			getT1,		setT1		],
	["",			getT2,		setT2		],
	["",			getT3,		setT3		],
	["",			getLastCmd,	executeCmd	],
	["",			arduinoRead,	arduinoWrite	]
	]

if __name__ == "__main__":
   main(sys.argv[1:])   