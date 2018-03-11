#!/usr/bin/python

import _mysql
import sys, getopt, os

# global definition

global con, part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d, t1, t2, t3

def openDB():
	global con
	try:
		con = _mysql.connect('db.arduino.local','arduinoyun','fo1806ch','fondarex')
	except _mysql.Error, e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(1)
# ******************* Getter and Setter ***************************************************************************************************
def getPartName():
	global part_name
	try:
		con.query("SELECT v_text FROM T_Globals WHERE name='ARDUINO_PART_NAME'" )
		result = con.use_result()
		part_name = result.fetch_row()[0][0]
	except:
		part_name = os.getenv("ARDUINO_PART_NAME")

def getProdNum():
	global prod_num
	try:
		con.query("SELECT v_int FROM T_Globals WHERE name='ARDUINO_PROD_NUM'" )
		result = con.use_result()
		prod_num = result.fetch_row()[0][0]
		if prod_num == None:
			prod_num = 0
	except:
		prod_num = os.getenv("ARDUINO_PROD_NUM")

def getIngNum():
	global ing_num
	try:
		con.query("SELECT v_int FROM T_Globals WHERE name='ARDUINO_CURRENT_INJECTION'" )
		result = con.use_result()
		ing_num = result.fetch_row()[0][0]
		if ing_num == None:
			ing_num = 0
	except:
		ing_num = os.getenv("ARDUINO_CURRENT_INJECTION")


def getT1():
	global t1
	try:
		con.query("SELECT v_int FROM T_Globals WHERE name='ARDUINO_T1'" )
		result = con.use_result()
		t1 = result.fetch_row()[0][0]
		if t1 == None:
			t1 = 0
	except:
		t1 = os.getenv("ARDUINO_T1")
		
def getT2():
	global t2
	try:
		con.query("SELECT v_int FROM T_Globals WHERE name='ARDUINO_T2'" )
		result = con.use_result()
		t2 = result.fetch_row()[0][0]
		if t2 == None:
			t2 = 0
	except:
		t2 = os.getenv("ARDUINO_T2")
		
def getT3():
	global t3
	try:
		con.query("SELECT v_int FROM T_Globals WHERE name='ARDUINO_T3'" )
		result = con.use_result()
		t3 = result.fetch_row()[0][0]
		if t3 == None:
			t3 = 0
	except:
		t3 = os.getenv("ARDUINO_T3")

def setPartName( arg ):
	global part_name
	try:
		con.query("UPDATE T_Globals SET v_text= '%s' WHERE name='ARDUINO_PART_NAME'" % arg )
		part_name = arg
	except _mysql.Error, e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(-1)

def setProdNum( arg ):
	global prod_num
	try:
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='ARDUINO_PROD_NUM'" % arg )
		prod_num = arg
	except _mysql.Error, e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(-2)

def setIngNum( arg ):
	global ing_num
	try:
		con.query("UPDATE T_Globals SET v_int= '%s' WHERE name='ARDUINO_CURRENT_INJECTION'" % arg )
		ing_num = arg
	except _mysql.Error, e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(-3)

def saveSample2Db():
	global con, part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d
	try:
		qry = "INSERT INTO T_Humidity_Value VALUE ( NULL, ( SELECT id FROM T_Injection WHERE part_name LIKE '%s' AND prod_num LIKE '%s' AND injection LIKE '%s' ),'%s','%s','%s','%s','%s','%s','%s','%s')" % \
			( part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d )
		con.query( qry )
	except _mysql.Error, e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(-3)

def createNewInjection():
	global con, part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d, t1, t2, t3
	try:
		ing_num = long( ing_num ) + 1
		setIngNum( ing_num )
		con.query(\
			"INSERT INTO T_Injection VALUE ( NULL,'%s','%s','%s', NOW(),'%s','%s','%s',9999,9999,9999,9999)" % \
			( part_name, prod_num, ing_num, t1, t2, t3 )\
			)
	except _mysql.Error, e:
		print "Error %d: %s" % (e.args[0], e.args[1])
		sys.exit(-3)

		
def main(argv):
	global con, part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, cal_a, cal_b, cal_c, cal_d, t1, t2, t3
	
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

	openDB()
	getPartName()
	getProdNum()
	getIngNum()
	
	print part_name
	print prod_num
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
   
if __name__ == "__main__":
   main(sys.argv[1:])


   
 
   