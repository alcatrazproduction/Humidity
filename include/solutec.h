#ifndef __SOLUTEC__
#define __SOLUTEC__

#define		INT_DIEOPEN		1
#define		INT_VACUSTART	1
#define		INT_MCP			2

#define CAL_LIMIT	(4096 / 4)
#define NT_CHANNEL	253
#define	CAN_CHANNEL_BASE	0x20

#define __NEWHARDWARE__

// Prototype
void startTimer( void );
void stopTimer( void );
void dieOpenInterrupt( void);
void vacuStartInterrut( void );
void dispatchInterrupt( void );
void MCPInterrupt( void );
void timeInterruptAct( int, siginfo_t *, void *);
void raiseInterruptAct( int signum, siginfo_t *sinfo, void *pad);
void sendSIGUSR1( void );
//int  doCalibrationProcess(int id, int oldCal );
int wait4Response( int bytes );

class c_solutec
{
protected:
	uint8_t			sizeNoParam, sizeParam,sizeReply,saveCmd;
	uint32_t		theId;
	uint8_t			lastSizeRead;
	uint8_t			theCmd;
public:
	c_solutec( void );
	~c_solutec( void );
	uint8_t		sendCmd();
	uint8_t		sendParam();
	uint8_t 	isResponseOK();
	uint8_t 	isAckOK();
	c_solutec	*setId( uint32_t id );
	uint32_t	getId( void );
protected:
	uint8_t		sendCmd( uint8_t );
	uint8_t 	isResponseOK( uint8_t );
	c_solutec	*setCmd( uint32_t cmd );
	uint8_t		getCmd( void );
	c_solutec	*setSizeNoParam( uint8_t size );
	uint8_t		getSizeNoParam( void );
	c_solutec	*setSizeParam( uint8_t size );
	uint8_t		getSizeParam( void );
	c_solutec	*setSizeReply( uint8_t size );
	uint8_t		getSizeReply( void );
};

class c_moduleinfo : public c_solutec
{
private:
//	uint8_t			cmd;
	uint8_t			serial[2];
	uint8_t			hardware[2];
	uint8_t			software[2];
	uint8_t			status;
public:
	c_moduleinfo( void );
	uint8_t			getCmdRLength( void );
	uint8_t			getRspLength( void );
	uint8_t			getDataSize( void );
public:
	int 			getSerial();
	int 			getHardWareVersion();
	int 			getSoftWareVersion();
	uint8_t 		getStatus();
	c_moduleinfo	*setSerial( int );
	c_moduleinfo	*setHardWareVersion( int );
	c_moduleinfo	*setSoftWareVersion( int );
};

class c_modulestatus : public c_solutec
{
private:
//	uint8_t			cmd;
	uint8_t			status;
public:
	c_modulestatus( void );
	uint8_t	getCmdRLength( void );
	uint8_t	getRspLength( void );
public:
	uint8_t getStatus();
};

class c_IrTxStatus : public c_solutec
{
private:
//	uint8_t			cmd;
	uint8_t			channel_a;
	uint8_t			channel_b;
public:
	c_IrTxStatus( void );
	uint8_t			getCmdRLength( void );
	uint8_t			getRspLength( void );
public:
	int 			getChannelA();
	int 			getChannelB();
	c_IrTxStatus	*setChannelA( int );
	c_IrTxStatus	*setChannelB( int );
};

class c_IrRxStatus : public c_solutec
{
private:
//	uint8_t			cmd;
	uint8_t			channel_a[2];
	uint8_t			channel_b[2];
public:
	c_IrRxStatus( void );
	uint8_t			getCmdRLength( void );
	uint8_t			getRspLength( void );
public:
	int 			getChannelA();
	int 			getChannelB();
};

class c_GetMeasurement : public c_solutec
{
private:
//	uint8_t			cmd;
	uint8_t			flags;
	uint8_t			power_level;
	uint8_t			humidity[2];
	uint8_t			pressure[2];
	uint8_t			temperature;
public:
	c_GetMeasurement( void );
	uint8_t				getCmdRLength( void );
	uint8_t				getRspLength( void );
	uint8_t 			isAckOK() { return 0;};
public:
	int 				getHumidity( void );
	int 				getPressure( void );
	int					getTemperature( void );
	int					getTxLevel( void );
	bool				getAutoSetPower( void );
	bool				getAutoSetActive( void );
	bool				getConfigurationError( void );
	bool				getAutoSetError( void );
	bool				getTemperatureError( void );
	int					getChannelUse( void );

	c_GetMeasurement	*setChannelUse( bool TxA, bool TxB );
	c_GetMeasurement	*setChannelUse( int );
	c_GetMeasurement	*setAutoSetPower( bool status );
	c_GetMeasurement	*resetError( void );
	c_GetMeasurement	*resetFlags( void );
	c_GetMeasurement	*setTxLevel( int level );
	c_GetMeasurement	*clearResponse( void );
};
/*
class c_SetMeasurement : public c_solutec
{
private:
//	uint8_t			cmd;
	uint8_t			flags;
	uint8_t			txlevel;
public:
	c_SetMeasurement( void );
	uint8_t			getCmdRLength( void );
	uint8_t			getRspLength( void );
public:
};
*/

// Class for saving data in cycle
class c_Node		// See Amiga Autodocs 44.1, file exec/node.h for info
{
protected:
	c_Node		*ln_Succ;
	c_Node		*ln_Pred;
	uint8_t		ln_Type;
	char		ln_Pri;
	char		*ln_Name;
public:
	c_Node( c_Node *s = NULL, c_Node *p = NULL, uint8_t t = 255, char pr = 0, char *n = NULL )
								{ ln_Succ = s; ln_Pred = p; ln_Type = t; ln_Pri = pr; ln_Name = n; };
	c_Node	*setSucc( c_Node *n) { ln_Succ = n; return this;};
	c_Node	*setPred( c_Node *n) { ln_Pred = n; return this;};
	c_Node	*setType( uint8_t t) { ln_Type = t; return this;};
	c_Node	*setPri( char p) 	{ ln_Pri = p; return this; };
	c_Node	*setName( char *n) 	{ ln_Name = n; return this;};
	c_Node	*setNode( c_Node *s = NULL, c_Node *p = NULL, uint8_t t = 255, char pr = 0, char *n = NULL )
								{ ln_Succ = s; ln_Pred = p; ln_Type = t; ln_Pri = pr; ln_Name = n; return this;};
	c_Node	*getSucc( void ) 	{ return ln_Succ; };
	c_Node	*getPred( void ) 	{ return ln_Pred; };
	uint8_t	getType( void ) 	{ return ln_Type; };
	char	getPri( void ) 		{ return ln_Pri; };
	char	*getName( void) 	{ return ln_Name; };
};

class c_List
{
protected:
   c_Node 	*lh_Head;
   c_Node 	*lh_Tail;
   c_Node 	*lh_TailPred;
   uint8_t   lh_Type;
   uint8_t   l_pad;
public:
		c_List( void )
			{ lh_Head = (c_Node *)&lh_Tail; lh_Tail = NULL; lh_TailPred = (c_Node *) &lh_Head; lh_Type = 0;};
		c_List( uint8_t t )
			{ c_List();  lh_Type = t; };
public:
	bool	IsListEmpty( void );
	c_List	*AddHead( c_Node *);
	c_List	*AddTail( c_Node *);
	c_List	*Enqueue( c_Node *);
	c_List	*Insert( c_Node *n, c_Node *after );
	c_Node	*RemHead( void );
	c_Node	*RemTail( void );
	c_List	*Remove( c_Node * );
	c_Node	*getHead() 		{ return lh_Head;};
	c_Node	*getTail() 		{ return lh_Tail;};
	c_Node	*getTailPred() 	{ return lh_TailPred;};
	bool	isAtEnd( c_Node *n) {return n == (c_Node *) & lh_Tail;};

};	/* word aligned */

/*----- Node Types for LN_TYPE -----*/



class c_Channel : public c_Node
{
private:
	c_List			values;
	uint8_t			number;
	c_moduleinfo	info;
	int				maxHumidityValue;
	int				minHumidityValue;
	int				maxTemperatureValue;
	int				minTemperatureValue;
	int				maxVaccumValue;
	int				minVaccumValue;
	bool			active;
	bool			channelA;
	bool			channelB;
	bool			asTemperature;
	bool			asVaccum;
public:
					c_Channel( uint8_t num ) { number = num; active = channelA = channelB = FALSE; resetMaxValue(); resetMinValue(); };
	c_Channel		*addValue( unsigned short v1, unsigned short v2, uint32_t tc );
	c_Channel		*addValue( c_Node *n ) { values.AddTail( n ); return this; };
	uint32_t		getValues( void );
	c_Node 			*remNextValue( void ) { return values.IsListEmpty()? NULL : values.RemHead(); } ;
	c_moduleinfo	*getInfo( void ) { return &info; };
	uint8_t			getChannelNum( void ) { return number; };
	c_Channel		*setHumidityMinMaxValue( int v2, int v1 )
									{
										if( v1 > v2 ) v2 = v1;
										if( v2 > maxHumidityValue ) maxHumidityValue = v2;
										if( v2 < minHumidityValue ) minHumidityValue = v2;
										return this;
									};
	c_Channel		*setTemperatureMinMaxValue( int v2, int v1 )
									{
										if( v1 > v2 ) v2 = v1;
										if( v2 > maxTemperatureValue ) maxTemperatureValue = v2;
										if( v2 < minTemperatureValue ) minTemperatureValue = v2;
										return this;
									};
	c_Channel		*setVaccumMinMaxValue( int v2, int v1 )
									{
										if( v1 > v2 ) v2 = v1;
										if( v2 > maxVaccumValue ) maxVaccumValue = v2;
										if( v2 < minVaccumValue ) minVaccumValue = v2;
										return this;
									};
	c_Channel		*setMinMaxValue( int h, int t, int v )
									{
										if( h > maxHumidityValue 	) maxHumidityValue = h;
										if( h < minHumidityValue 	) minHumidityValue = h;
										if( t > maxTemperatureValue	) maxTemperatureValue = t;
										if( t < minTemperatureValue	) minTemperatureValue = t;
										if( v > maxVaccumValue 		) maxVaccumValue = v;
										if( v < minVaccumValue 		) minVaccumValue = v;
										return this;
									};
													
	c_Channel		*setChannelConnected( bool b )		{ active	= b; return this; };
	c_Channel		*setChannelRxA( bool b )			{ channelA	= b; return this; };
	c_Channel		*setChannelRxB( bool b )			{ asVaccum	= b; return this; };
													
	int				getHumidityMaxValue( void )		{ return maxHumidityValue; };
	int				getHumidityMinValue( void )		{ return minHumidityValue; };
	int				getTemperatureMaxValue( void )	{ return maxTemperatureValue; };
	int				getTemperatureMinValue( void )	{ return minTemperatureValue; };
	int				getVaccumMaxValue( void )		{ return maxVaccumValue; };
	int				getVaccumMinValue( void )		{ return minVaccumValue; };
	bool			getChannelConnected( void )		{ return active; };
	bool			getChannelRxA( void )			{ return channelA; };
	bool			getChannelRxB( void )			{ return asVaccum; };
	
	c_Channel		*resetMaxValue( void ) 	{ maxHumidityValue = maxTemperatureValue = maxVaccumValue = 0; return this;};
	c_Channel		*resetMinValue( void )	{ minHumidityValue = minTemperatureValue = minVaccumValue = 4096; return this;};
};

class c_Value : public c_Node
{
private:
	unsigned short		values[4];
	uint32_t			timecode;
public:
	c_Value( unsigned short v1, unsigned short v2, uint32_t tc )
											{ values[0] = v1; values[1] = v2; values[2] = 0; values[3] = 0;timecode = tc;};
	c_Value( unsigned short v1, unsigned short v2, unsigned short v3, uint32_t tc )
											{ values[0] = v1; values[1] = v2; values[2] = v3; values[3] = 0;timecode = tc;};
	c_Value( unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4,uint32_t tc )
											{ values[0] = v1; values[1] = v2; values[2] = v3; values[3] = v4;timecode = tc;};
	uint64_t	getValues( void ) { return 	( ((uint64_t) values[0] & 0xffff) << 0 ) +
											( ((uint64_t) values[1] & 0xffff) << 16  ) +
											( ((uint64_t) values[2] & 0xffff) << 32 ) +
											( ((uint64_t) values[3] & 0xffff) << 48 );};
	uint64_t	getTimeCode( void ) { return timecode; };
};


#endif
