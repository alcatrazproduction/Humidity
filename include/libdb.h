/*****************************************************************************
 *  
 *  $Author:       Yves Huguenin
 *  $Title:        libdb.py
 *  $Create Date:  09.05.2014
 * $Vers:         1.0a1
 *
 *  1.0a1:
 *
 * **************************************************************************/
#ifndef __ALCATRAZ_LIBDB__
#define __ALCATRAZ_LIBDB__

#include <pthread.h>

typedef enum { INT,TEXT,FLOAT,BYTE,WORD,LONG }	E_GlobalTypes;

#define		GB_TYPE_INT				"int"
#define		GB_TYPE_TEXT			"text"
#define		GB_TYPE_FLOAT			"float"

#define		GB_VN_INGNUM			"CURRENTINJECTION#DB001.DBD.160"
#define		GB_VT_INGNUM			((E_GlobalTypes)INT)

#define		GB_VN_PARTNAME			"PARTNAME#DB001.DBX.164..80"
#define		GB_VT_PARTNAME			((E_GlobalTypes)TEXT)

#define		GB_VN_PRODNUM			"PRODNUM#DB001.DBW.244"
#define		GB_VT_PRODNUM			((E_GlobalTypes)INT)

#define		GB_VN_NUMCHANNEL		"NUMCHAN#DB001.DBW.332"
#define		GB_VT_NUMCHANNEL		((E_GlobalTypes)INT)

#define		GB_VN_SAMPLERATE		"SAMPLERATE#DB001.DBW.334"
#define		GB_VT_SAMPLERATE		((E_GlobalTypes)INT)

#define		GB_VN_T1				"T1#DB001.DBW.246"
#define		GB_VT_T1				((E_GlobalTypes)INT)

#define		GB_VN_T2				"T2#DB001.DBW.248"
#define		GB_VT_T2				((E_GlobalTypes)INT)

#define		GB_VN_T3				"T3#DB001.DBW.250"
#define		GB_VT_T3				((E_GlobalTypes)INT)

#define		GB_GET_VALUE_I( nnme )	"SELECT v_int   FROM T_Globals WHERE name='nnme'"
#define		GB_GET_VALUE_T( nnme )	"SELECT v_text  FROM T_Globals WHERE name='nnme'"
#define		GB_GET_VALUE_F( nnme )	"SELECT v_float FROM T_Globals WHERE name='nnme'"

#define		GB_SET_VALUE_I( nnme )	"UPDATE T_Globals SET v_int  = ? WHERE name='nnme'"
#define		GB_SET_VALUE_T( nnme )	"UPDATE T_Globals SET v_text = ? WHERE name='nnme'"
#define		GB_SET_VALUE_F( nnme )	"UPDATE T_Globals SET v_float= ? WHERE name='nnme'"

// def getGraphData():
#define		GB_READCHANNEL		"SELECT ReadChannel( %s,'%s',%s   )" % (theInjection , theChannel , thePage )  )
// def readChannelDataBlock( theChannel, start, quantity ):
#define		READCHANNELFULL		"SELECT channel_%c, temp_%c, vaccum_%c, hv_vaccum_%c FROM T_Humidity_Value WHERE injection_id = %ld"
#define		READCHANNELBLOCK	"SELECT channel_%c, temp_%c, vaccum_%c, hv_vaccum_%c FROM T_Humidity_Value WHERE injection_id = (SELECT v_int FROM T_Globals WHERE name='ARDUINO_CURRENT_INJECTION')  LIMIT %s,%s" % (theChannel + ord('a') , start, quantity )  )
#define 	READCCINFO			"SELECT * FROM T_Channel_Info WHERE injection_id = %ld AND channel=%d"
// def getPartName( var ):
#define		GB_GET_PARTNAME		GB_GET_VALUE_T( GB_VN_PARTNAME )
// def getProdNum( var ):
#define		GB_GET_PRODNUM		GB_GET_VALUE_I( GB_VN_PRODNUM )
// def getIngNum( var ):
#define		GB_GET_INGNUM		GB_GET_VALUE_I( GB_VN_INGNUM )
// def getT1( var ):
#define		GB_GET_T1			GB_GET_VALUE_I( GB_VN_T1 )
// def getT2( var ):
#define		GB_GET_T2			GB_GET_VALUE_I( GB_VN_T2 )
// def getNumChannel( var ):
#define		GB_GET_NUMCHANNEL	GB_GET_VALUE_I( GB_VN_NUMCHANNEL )
// def getT3( var ):
#define		GB_GET_T3			GB_GET_VALUE_I( GB_VN_T3 )
// def setPartName( arg,  var ):
#define		GB_SET_PARTNAME		GB_GET_VALUE_T( GB_VN_PARTNAME )
// def setProdNum( arg,  var ):
#define		GB_SET_PRODNUM		GB_GET_VALUE_I( GB_VN_PRODNUM )
// def setIngNum( arg,  var ):
#define		GB_SET_INGNUM		GB_GET_VALUE_I( GB_VN_INGNUM )
// def setT1( arg,  var ):
#define		GB_SET_T1			GB_GET_VALUE_I( GB_VN_T1 )
// def setT2( arg,  var ):
#define		GB_SET_T2			GB_GET_VALUE_I( GB_VN_T2 )
// def setNumChannel( arg,  var ):
#define		GB_SET_NUMCHANNEL	GB_GET_VALUE_I( GB_VN_NUMCHANNEL )
// def setT3( arg,  var ):
#define		GB_SET_T3			GB_GET_VALUE_I( GB_VN_T3 )

#define	STMT_SAVEDATA4			"INSERT INTO T_Humidity_Value VALUE ( NULL, ( SELECT id FROM T_Injection WHERE part_name LIKE ? AND prod_num LIKE ? AND injection LIKE ? ),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
//			( part_name, prod_num, ing_num, channel_a, channel_b, channel_c, channel_d, temp_a, temp_b, temp_c, temp_d, vaccum_a, vaccum_b, vaccum_c, vaccum_d, hv_vaccum_a, hv_vaccum_b, hv_vaccum_c, hv_vaccum_d )
#define STMT_CREATEINJECTION	"INSERT INTO T_Injection VALUE ( NULL,?,?,?, NOW(),?,?,?,?,?,?,?,?)"

#define STMT_READGLOBALS_		"SELECT v_int, v_text, v_float, v_type, count(*) INTO theInt, theText, theFloat, theType, ok FROM T_Globals WHERE name = ?"
#define STMT_TESTGLOBALS_		"SELECT v_type, count(*) INTO theType, ok FROM T_Globals WHERE name =  ?"
#define STMT_UPDATEGLOBALS_I	"UPDATE T_Globals SET v_int = ? WHERE name= ?"
#define STMT_UPDATEGLOBALS_F	"UPDATE T_Globals SET v_float = ? WHERE name= ?"
#define STMT_UPDATEGLOBALS_T	"UPDATE T_Globals SET v_text = ? WHERE name= ?"
#define STMT_CREATEGLOBALS_		"SELECT v_int, v_text, v_float, v_type, count(*) INTO theInt, theText, theFloat, theType, ok FROM T_Globals WHERE name = ?"

#define STMT_READGLOBALS		"SELECT ReadGlobal( ? )"			// theName
#define STMT_UPDATEGLOBALS		"CALL UpdateGlobal( ?, ? )"		// theName, theValue
#define STMT_CREATEGLOBALS		"CALL CreateGlobal( ?, ?, ?)"		// theName, theValue, theType

#define STMT_CREATECINFO		"INSERT INTO T_Channel_Info VALUE ( NULL, ( SELECT id FROM T_Injection WHERE part_name LIKE ? AND prod_num LIKE ? AND injection LIKE ? ),?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
// id,injection_id,channel,sample_count,sample_rate,humidity_max,humidity_min,humidity_flags,humidity_cal,humidity_cal_res,
//		temperature_max,temperature_min,temperature_flags,temperature_cal,temperature_cal_res,vaccum_max,vaccum_min,vaccum_flags,vaccum_cal,vaccum_cal_res,humidity_lavg,humidity_lstart,humidity_lstop



class C_Connection
{
protected:
	sql::Connection *			link;
	sql::Statement *			stmt;
	sql::PreparedStatement *	stmt_rglobals;
	sql::PreparedStatement *	stmt_cglobals;
	sql::PreparedStatement *	stmt_uglobals;
	sql::PreparedStatement *	stmt_cinjection;
	sql::PreparedStatement *	stmt_savedata4;
	sql::PreparedStatement *	stmt_ccinfo;
	bool						used;
	bool						online;
public:
	~C_Connection( );
	C_Connection( sql::Driver *driver);
	bool				checkOpen( void );
	bool				reConnect( sql::Driver *driver );
	C_Connection * 		Obtain()	{ if( used ) return NULL; used = TRUE; return this;};
	void				Release()	{ used = link ? FALSE : TRUE;};
	
	int					ReadGlobal( const char *theName, void *theValue);		// no type check is done, you must pass the good type !!!!
	int					ReadGlobal( const char *theName, void *theValue, E_GlobalTypes theType );
	int					UpdateGlobal( const char *theName, void *theValue);		// no type check is done, you must pass the good type !!!!
	int					UpdateGlobal( const char *theName, void *theValue, E_GlobalTypes theType );
	int					CreateGlobal( const char *theName, void *theValue, E_GlobalTypes theType );
	int 				LoadChannelCurve(DB_Global *theGlobal,DB_AxeData *theChannel);
	int 				SaveChannelCurve(DB_Global *theGlobal,DB_AxeData *theChannel);
	int 				SaveChannelSample(DB_Global *theGlobal, short sample, DB_AxeData *cha, DB_AxeData *chb, DB_AxeData *chc, DB_AxeData *chd );
	unsigned long		CreateNewInjection( DB_Global *theGlobal );
	
	void				getPartName( unsigned char *buffer, unsigned short &length);
	unsigned short		getProdNum( );
	unsigned long		getIngNum( );
	unsigned short		getT1(  );
	unsigned short		getT2(  );
	unsigned short		getT3(  );
	unsigned short		getNumChannel( );
	unsigned short		getSampleRate( );
	int					setPartName( unsigned char * );
	int					setProdNum( unsigned short );
	int					setIngNum( unsigned long );
	int					setT1( unsigned short );
	int					setT2( unsigned short );
	int					setT3( unsigned short );
	int 				setNumChannel( unsigned short );
	int 				setSampleRate( unsigned short );
	
};

class	C_LibDb
{
protected:
// thread stuff...

	pthread_t	thread_id;
	void		*mysqlThread(void *);
// mysql stuff....
	sql::Driver *driver;

	C_Connection	*connectionList[10];
	
public:
	C_LibDb( void );
	~C_LibDb( void );
	C_Connection *	getDb();

	void 				releaseDb( C_Connection *theLink );

};

#endif
