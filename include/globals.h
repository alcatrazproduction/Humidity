/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/
#ifndef __FONDAREX_GLOBAL__
#define __FONDAREX_GLOBAL__


// global..... MCP

const		extern	char *			CAN_error[];

//********************************************* Some interrupt flags ****************************************************************************************

// globals for S7 Server
#define GLOBALSETSTART			globals
#define GLOBALSETEND			;

			extern	DB_Global 		*GLOBALSETSTART;			// global DB 				( DB1 )

#define	GETBASEADDRESS				GLOBALSETSTART->BaseAddress(  )
#define	GETBLOCKSIZE				GLOBALSETSTART->BlockSize(  )
#define	GETVERSION					GLOBALSETSTART->Version(  )
#define	GETCHANNEL2READ				GLOBALSETSTART->Channel2Read(  )
#define	GETCHANNEL_HUMIDITY			GLOBALSETSTART->ChannelHumidity(  )
#define	GETCHANNEL_TEMPERATURE		GLOBALSETSTART->ChannelTemperature(  )
#define	GETCHANNEL_VACCUM			GLOBALSETSTART->Channelvaccum(  )
#define	GETUSEAUTOFACTOR			GLOBALSETSTART->UseAutoFactor(  )
#define	GETAUTOFACTOR				GLOBALSETSTART->AutoFactor(  )
#define	GETDISPLAYZERO				GLOBALSETSTART->DisplayZero(  )
#define	GETDISPLAYFACTOR			GLOBALSETSTART->DisplayFactor(  )
#define	GETCURRENTINJECTION			GLOBALSETSTART->CurrentInjection(  )
#define	GETPARTNAME					GLOBALSETSTART->PartName(  )
#define	GETPRODUCTIONNUMBER			GLOBALSETSTART->ProdNum(  )
#define	GETT1						GLOBALSETSTART->T1(  )
#define	GETT2						GLOBALSETSTART->T2(  )
#define	GETT3						GLOBALSETSTART->T3(  )
#define	GETCOMMAND					GLOBALSETSTART->Cmd(  )
#define	GETNUMBEROFCHANNEL			GLOBALSETSTART->NumChan(  )
#define	GETSAMPLERATE				GLOBALSETSTART->SampleRate(  )
#define	GETLOADCURVE				GLOBALSETSTART->LoadCurve(  )
#define	GETREQUESTCURVE				GLOBALSETSTART->ReqCurve(  )
#define	GETREQUESTINJECTION			GLOBALSETSTART->RequestInjection(  )
#define	GETSPIMODE					GLOBALSETSTART->SPIMode(  )
#define	GETSPIBITS					GLOBALSETSTART->SPIBits(  )
#define	GETSPISPEED					GLOBALSETSTART->SPISpeed(  )
#define	GETSPIDELAY					GLOBALSETSTART->SPIDelay(  )
#define	GETDIEOPENRAISING			GLOBALSETSTART->getDieOpen0(  )
#define	GETDIEOPENFALLING			GLOBALSETSTART->getDieOpen1(  )
#define	GETVACUSTART				GLOBALSETSTART->getVacuStart(  )
#define	GETMCPINTERRUPT				GLOBALSETSTART->getInterruptMCP(  )
#define	GETDATAREADY				GLOBALSETSTART->getDataReady(  )
#define	GETINTERRUPTMASK			GLOBALSETSTART->InterruptMask(  )
#define	GETS7SERVER					GLOBALSETSTART->S7Server()
#define	GETHIGHVAC					GLOBALSETSTART->HighVac()
#define	GETAXEX						GLOBALSETSTART->AxeX()
#define	GETAXEDATA( ch )			GLOBALSETSTART->AxeData( ch )
#define	GETDATABASELIBRARY			GLOBALSETSTART->DatabaseLibrary(  )
#define	GETAXEARRAYINITED			GLOBALSETSTART->AxeArrayInited(  )
#define	GETSTANDALONE				GLOBALSETSTART->isStandAlone(  )
#define	GETCHANNELLIST				GLOBALSETSTART->ChannelList(  )
#define	GETCNT						GLOBALSETSTART->Cnt(  )
#define	GETCAN						GLOBALSETSTART->Can(  )
#define	GETSIG_DIEOPEN				GLOBALSETSTART->SIG_DieOpen()
#define	GETSIG__DIEOPEN				GLOBALSETSTART->SIG__DieOpen()
#define	GETSIG_VACUSTART			GLOBALSETSTART->SIG_Vacustart()
#define	GETSIG__VACUSTART			GLOBALSETSTART->SIG__Vacustart()
#define	GETSIG_DATAREADY			GLOBALSETSTART->SIG_DataReady()
#define	GETHVINJECTION				GLOBALSETSTART->HV_Injection()
#define	GETLIMITAVERAGE(a)			GLOBALSETSTART->LimitAverage(a)
#define	GETLIMITSTART(a)			GLOBALSETSTART->LimitStart(a)
#define	GETLIMITSTOP(a)				GLOBALSETSTART->LimitStop(a)
#define	GETHIGHVACIP				GLOBALSETSTART->HighVacIP(  )
#define	GETHVRACK					GLOBALSETSTART->hv_rack(  )
#define	GETHVSLOT					GLOBALSETSTART->hv_slot(  )
#define	GETHVVACUSTART				GLOBALSETSTART->hv_vacustart(  )
#define	GETHVVACUSTARTBIT			GLOBALSETSTART->hv_vacustart_bit(  )
#define	GETHVDIEOPEN				GLOBALSETSTART->hv_dieopen(  )
#define	GETHVDIEOPENBIT				GLOBALSETSTART->hv_dieopen_bit(  )
#define	GETHVT1						GLOBALSETSTART->hv_t1(  )
#define	GETHVT1BYTE					GLOBALSETSTART->hv_t1_byte(  )
#define	GETHVT2						GLOBALSETSTART->hv_t2(  )
#define	GETHVT2BYTE					GLOBALSETSTART->hv_t2_byte(  )
#define	GETHVT3						GLOBALSETSTART->hv_t3(  )
#define	GETHVT3BYTE					GLOBALSETSTART->hv_t3_byte(  )
#define	GETHVPRODNUM				GLOBALSETSTART->hv_prodnum(  )
#define	GETHVPRODNUMBYTE			GLOBALSETSTART->hv_prodnum_byte(  )
#define	GETHVPARTNUM				GLOBALSETSTART->hv_partnum(  )
#define	GETHVPARTNUMBYTE			GLOBALSETSTART->hv_partnum_byte(  )
#define	GETHVPARTNAME				GLOBALSETSTART->hv_partname(  )
#define	GETHVPARTNAMEBYTE			GLOBALSETSTART->hv_partname_byte(  )
#define	GETHVSHOTNUM				GLOBALSETSTART->hv_shotnum(  )
#define	GETHVSHOTNUMBYTE			GLOBALSETSTART->hv_shotnumbyte_byte(  )
#define	GETHVMAXLIMIT				GLOBALSETSTART->hv_max_limit(  )
#define	GETHVOUTTOL					GLOBALSETSTART->hv_outtol(  )
#define	GETHVOUTTOLBYTE				GLOBALSETSTART->hv_outtol_byte(  )
#define	GETHVOUTTOLBIT				GLOBALSETSTART->hv_outtol_bit(  )
#define	GETHVCLEAN					GLOBALSETSTART->hv_clean(  )
#define	GETHVCLEANBYTE				GLOBALSETSTART->hv_clean_byte(  )
#define	GETHVCLEANBIT				GLOBALSETSTART->hv_clean_bit(  )
#define	GETCALIBRATIONLIMIT			GLOBALSETSTART->CalibrationLimit()

#define	SETCHANNEL2READ(...)		->Channel2Read			(__VA_ARGS__)
#define	SETCHANNEL_HUMIDITY(...)	->ChannelHumidity		(__VA_ARGS__)
#define	SETHVMAXLIMIT(...)			->hv_max_limit			(__VA_ARGS__)
#define	SETCHANNEL_TEMPERATURE(...)	->ChannelTemperature	(__VA_ARGS__)
#define	SETCHANNEL_VACCUM(...)		->Channelvaccum			(__VA_ARGS__)
#define	SETUSEAUTOFACTOR(...)		->UseAutoFactor			(__VA_ARGS__)
#define	SETAUTOFACTOR(...)			->AutoFactor			(__VA_ARGS__)
#define	SETDISPLAYZERO(...)			->DisplayZero			(__VA_ARGS__)
#define	SETDISPLAYFACTOR(...)		->DisplayFactor			(__VA_ARGS__)
#define	SETCURRENTINJECTION(...)	->CurrentInjection		(__VA_ARGS__)
#define	SETPARTNAME(...)			->PartName				(__VA_ARGS__)
#define	SETPRODUCTIONNUMBER(...)	->ProdNum				(__VA_ARGS__)
#define	SETT1(...)					->T1					(__VA_ARGS__)
#define	SETT2(...)					->T2					(__VA_ARGS__)
#define	SETT3(...)					->T3					(__VA_ARGS__)
#define	SETCOMMAND(...)				->Cmd					(__VA_ARGS__)
#define	SETNUMBEROFCHANNEL(...)		->NumChan				(__VA_ARGS__)
#define	SETSAMPLERATE(...)			->SampleRate			(__VA_ARGS__)
#define	SETLOADCURVE(...)			->LoadCurve				(__VA_ARGS__)
#define	SETREQUESTCURVE(...)		->ReqCurve				(__VA_ARGS__)
#define	SETREQUESTINJECTION(...)	->RequestInjection		(__VA_ARGS__)
#define	SETSPIMODE(...)				->SPIMode				(__VA_ARGS__)
#define	SETSPIBITS(...)				->SPIBits				(__VA_ARGS__)
#define	SETSPISPEED(...)			->SPISpeed				(__VA_ARGS__)
#define	SETSPIDELAY(...)			->SPIDelay				(__VA_ARGS__)
#define	SETINTERRUPTMASK(...)		->InterruptMask			(__VA_ARGS__)
#define	SETS7SERVER(...)			->S7Server				(__VA_ARGS__)
#define	SETHIGHVAC(...)				->HighVac				(__VA_ARGS__)
#define	SETAXEX(...)				->AxeX					(__VA_ARGS__)
#define	SETAXEDATA(...)				->AxeData				(__VA_ARGS__)
#define	SETCNT(...)					->Cnt					(__VA_ARGS__)
#define	SETSIG_DIEOPEN(...)			->SIG_DieOpen			(__VA_ARGS__)
#define	SETSIG__DIEOPEN(...)		->SIG__DieOpen			(__VA_ARGS__)
#define	SETSIG_VACUSTART(...)		->SIG_Vacustart			(__VA_ARGS__)
#define	SETSIG__VACUSTART(...)		->SIG__Vacustart		(__VA_ARGS__)
#define	SETSIG_DATAREADY(...)		->SIG_DataReady			(__VA_ARGS__)
#define	SETHVINJECTION(...)			->HV_Injection			(__VA_ARGS__)
#define	SETLIMITAVERAGE(...)		->LimitAverage			(__VA_ARGS__)
#define	SETLIMITSTART(...)			->LimitStart			(__VA_ARGS__)
#define	SETLIMITSTOP(...)			->LimitStop				(__VA_ARGS__)

#define	SETDIEOPENRAISING			->setDieOpen0			()
#define	SETDIEOPENFALLING			->setDieOpen1			()
#define	SETVACUSTART				->setVacuStart			()
#define	SETMCPINTERRUPT				->setInterruptMCP		()
#define	SETDATAREADY				->setDataReady			()
#define	SETSTANDALONE				->setStandAlone			()

#define	CLEARDIEOPENRAISING			->clearDieOpen0			()
#define	CLEARDIEOPENFALLING			->clearDieOpen1			()
#define	CLEARVACUSTART				->clearVacuStart		()
#define	CLEARMCPINTERRUPT			->clearInterruptMCP		()
#define	CLEARDATAREADY				->clearDataReady		()
#define	CLEARSTANDALONE				->clearStandAlone		()
// Prototypes

int initGlobals( void );

#ifdef S7_STATIC
#define DO_INIT_GLOBAL \
class _s7_static_global {private: void *a; short b;char *c;unsigned long d;E_GlobalTypes e; public:																				\
_s7_static_global( void *a1, short b1, char *c1, unsigned long d1, E_GlobalTypes e1 ) { a = a1; b = b1; c = c1; d = d1; e = e1;}; void doit( C_Connection *con ) 				\
{unsigned char buffer[256];unsigned long t = con->ReadGlobal( c, &buffer, e) ? d : (*((unsigned long *)&buffer)); switch(b){case 16:*((unsigned short *)a) = htobe16( t);break;	\
case 32:*((unsigned long *)a) = htobe32( t);break;default:break;}}};typedef  _s7_static_global * _s7_static_global_ptr;_s7_static_global_ptr s7_static_global[] = {							   \
new _s7_static_global(&_UseAutoFactor	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, USEAUTOFACTOR  		) ? theName : NULL ,0				 		,theType), \
new _s7_static_global(&_AutoFactor		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, AUTOFACTOR  		) ? theName : NULL ,0 						,theType), \
new _s7_static_global(&_DisplayZero		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, DISPLAYZERO  		) ? theName : NULL ,1000 					,theType), \
new _s7_static_global(&_DisplayFactor	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, DISPLAYFACTOR  		) ? theName : NULL ,10 						,theType), \
new _s7_static_global(&_CalibrationLimit,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, CALIBRATIONLIMIT  	) ? theName : NULL ,200 					,theType), \
new _s7_static_global(&_hv_ip			,32,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVIP_BASE  			) ? theName : NULL ,MAKE_ID(192,168,1,10) 	,theType), \
new _s7_static_global(&_hv_rack			,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVRACK_BASE  		) ? theName : NULL ,0 						,theType), \
new _s7_static_global(&_hv_slot			,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVSLOT_BASE  		) ? theName : NULL ,1 						,theType), \
new _s7_static_global(&_hv_vacustart	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVVACUSTART_BASE  	) ? theName : NULL ,1 						,theType), \
new _s7_static_global(&_hv_vacustart_bit,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVVACUSTARTBIT_BASE	) ? theName : NULL ,0 						,theType), \
new _s7_static_global(&_hv_dieopen		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVDIEOPEN_BASE  	) ? theName : NULL ,0 						,theType), \
new _s7_static_global(&_hv_dieopen_bit	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVDIEOPENBIT_BASE 	) ? theName : NULL ,7 						,theType), \
new _s7_static_global(&_hv_t1			,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVT1_BASE			) ? theName : NULL ,114 					,theType), \
new _s7_static_global(&_hv_t1_byte		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVT1BYTE_BASE		) ? theName : NULL ,36 						,theType), \
new _s7_static_global(&_hv_t2			,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVT2_BASE			) ? theName : NULL ,114 					,theType), \
new _s7_static_global(&_hv_t2_byte		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVT2BYTE_BASE		) ? theName : NULL ,40 						,theType), \
new _s7_static_global(&_hv_t3			,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVT3_BASE			) ? theName : NULL ,114 					,theType), \
new _s7_static_global(&_hv_t3_byte		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVT3BYTE_BASE		) ? theName : NULL ,50 						,theType), \
new _s7_static_global(&_hv_prodnum		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVPRODNUM_BASE		) ? theName : NULL ,114 					,theType), \
new _s7_static_global(&_hv_prodnum_byte	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVPRODNUMBYTE_BASE	) ? theName : NULL ,344 					,theType), \
new _s7_static_global(&_hv_partnum		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVPARTNUM_BASE		) ? theName : NULL ,114 					,theType), \
new _s7_static_global(&_hv_partnum_byte	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVPARTNUMBYTE_BASE	) ? theName : NULL ,346 					,theType), \
new _s7_static_global(&_hv_partname		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVPARTNAME_BASE		) ? theName : NULL ,114 					,theType), \
new _s7_static_global(&_hv_partname_byte,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVPARTNAMEBYTE_BASE	) ? theName : NULL ,300 					,theType), \
new _s7_static_global(&_hv_shotnum		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVSHOTNUM_BASE 		) ? theName : NULL ,98 						,theType), \
new _s7_static_global(&_hv_shotnum_byte	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVSHOTNUMBYTE_BASE	) ? theName : NULL ,276 					,theType), \
new _s7_static_global(&_hv_outtol		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVOUTTOL_BASE  		) ? theName : NULL ,104 					,theType), \
new _s7_static_global(&_hv_outtol_byte	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVOUTTOLBYTE_BASE  	) ? theName : NULL ,52 						,theType), \
new _s7_static_global(&_hv_outtol_bit	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVOUTTOLBIT_BASE  	) ? theName : NULL ,3						,theType), \
new _s7_static_global(&_hv_clean		,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVCLEAN_BASE  		) ? theName : NULL ,104 					,theType), \
new _s7_static_global(&_hv_clean_byte	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVCLEANBYTE_BASE  	) ? theName : NULL ,52 						,theType), \
new _s7_static_global(&_hv_clean_bit	,16,decodeVarName( NULL, &theName, theType, theMode, theSize, GLOBAL_SLAVE, HVCLEANBIT_BASE  	) ? theName : NULL ,4 						,theType)  \
} ; for( unsigned short v=0; v < (sizeof(s7_static_global) / sizeof(_s7_static_global_ptr) ) ; v++) s7_static_global[v]->doit(con);

#endif
#endif