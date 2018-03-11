
#include "humidity.h"

#define MAX_CAN_MSG_LEN		8

#if DEBUG > 0

static const char	*command_name[] =
{
	"00: ModuleInfo",	"01: ModuleStatus",	"02:",				"03:",
	"04:",				"05:",				"06:",				"07:",
	"08:",				"09:",				"0A:",				"0B:",
	"0C:",				"0D:",				"0E:",				"0F:",
	"10: IrTxStatus",	"11: IrTxStatusA",	"12: IrTxStatusB",	"13:",
	"14:",				"15:",				"16:",				"17:",
	"18:",				"19:",				"1A:",				"1B:",
	"1C:",				"1D:",				"1E:",				"1F:",
	"20: IrRxStatus",	"21: IrRxStatusA",	"22: IrRxStatusB",	"23:",
	"24:",				"25:",				"26:",				"27:",
	"28:",				"29:",				"2A:",				"2B:",
	"2C:",				"2D:",				"2E:",				"2F:",
	"30:",				"21:",				"32:",				"33:",
	"34:",				"35:",				"36:",				"37:",
	"38:",				"39:",				"3A:",				"3B:",
	"3C:",				"3D:",				"3E:",				"3F:",
	"40:",				"41:",				"42:",				"43:",
	"44:",				"45:",				"46:",				"47:",
	"48:",				"49:",				"4A:",				"4B:",
	"4C:",				"4D:",				"4E:",				"4F:",
	"50: Mesurement",	"51:",				"52:",				"53:",
	"54:",				"55:",				"56:",				"57:",
	"58:",				"59:",				"5A:",				"5B:",
	"5C:",				"5D:",				"5E:",				"5F:",
	"60:",				"61:",				"62:",				"63:",
	"64:",				"65:",				"66:",				"67:",
	"68:",				"69:",				"6A:",				"6B:",
	"6C:",				"6D:",				"6E:",				"6F:",
	"70:",				"71:",				"72:",				"73:",
	"74:",				"75:",				"76:",				"77:",
	"78:",				"79:",				"7A:",				"7B:",
	"7C:",				"7D:",				"7E:",				"7F:",
	"80:",				"81:",				"82:",				"83:",
	"84:",				"85:",				"86:",				"87:",
	"88:",				"89:",				"8A:",				"8B:",
	"8C:",				"8D:",				"8E:",				"8F:",
	"90:",				"91:",				"92:",				"93:",
	"94:",				"95:",				"96:",				"97:",
	"98:",				"99:",				"9A:",				"9B:",
	"9C:",				"9D:",				"9E:",				"9F:",
	"A0:",				"A1:",				"A2:",				"A3:",
	"A4:",				"A5:",				"A6:",				"A7:",
	"A8:",				"A9:",				"AA:",				"AB:",
	"AC:",				"AD:",				"AE:",				"AF:",
	"B0:",				"B1:",				"B2:",				"B3:",
	"B4:",				"B5:",				"B6:",				"B7:",
	"B8:",				"B9:",				"BA:",				"BB:",
	"BC:",				"BD:",				"BE:",				"BF:",
	"C0:",				"C1:",				"C2:",				"C3:",
	"C4:",				"C5:",				"C6:",				"C7:",
	"C8:",				"C9:",				"CA:",				"CB:",
	"CC:",				"CD:",				"CE:",				"CF:",
	"D0:",				"D1:",				"D2:",				"D3:",
	"D4:",				"D5:",				"D6:",				"D7:",
	"D8:",				"D9:",				"DA:",				"DB:",
	"DC:",				"DD:",				"DE:",				"DF:",
	"E0:",				"E1:",				"E2:",				"E3:",
	"E4:",				"E5:",				"E6:",				"E7:",
	"E8:",				"E9:",				"EA:",				"EB:",
	"EC:",				"ED:",				"EE:",				"EF:",
	"F0:",				"F1:",				"F2:",				"F3:",
	"F4:",				"F5:",				"F6:",				"F7:",
	"F8:",				"F9:",				"FA:",				"FB:",
	"FC:",				"FD:",				"FE:",				"FF: Reset",
	"  :",				"  :",				"  :",				"  :",
	"  :",				"  :",				"  :",				"  :",
	"  :",				"  :",				"  :",				"  :",
	"  :",				"  :",				"  :",				"  :"
};

#endif

// ******** Utility ****************************************
// wait for response with timeout, return false on error   *
//**********************************************************

int wait4Response( int bytes )
{
int		time2wait	 = (bytes + CAN_BYTES_HEADER) * CAN_BYTE_TIME;
uint8_t	msg;

	for( int i=0; i < RETRY; i++ )
	{
		delayMicroseconds( time2wait );
		msg	=  GETCAN->available();
		if( msg == 0x03 )
		{
			PRINT_DEBUG("int wait4Response( int bytes ): we have 2 messages waiting!!!\n");
			return msg;
		}
		else
			if( msg )
				return msg;
	}
	return msg;
}
// Function for CAN protocole
// Command definition

c_solutec::c_solutec( void )
{
		sizeNoParam 		= sizeParam = sizeReply = 0xFF;		// set to -1, bad size
		theId				= CAN_CHANNEL_BASE;					// first channel....
		saveCmd 			= 0;								// NULL CMD
}

c_solutec::~c_solutec( void )
{

}

c_solutec	*c_solutec::setSizeNoParam( uint8_t size )
{
	sizeNoParam = size <= MAX_CAN_MSG_LEN ? size : MAX_CAN_MSG_LEN;
	return this;
}
uint8_t		c_solutec::getSizeNoParam( void )
{
	return sizeNoParam;
}
c_solutec	*c_solutec::setSizeParam( uint8_t size )
{
	sizeParam = size <= MAX_CAN_MSG_LEN ? size : MAX_CAN_MSG_LEN;
	return this;
}
uint8_t		c_solutec::getSizeParam( void )
{
	return sizeParam;
}
c_solutec	*c_solutec::setSizeReply( uint8_t size )
{
	sizeReply = size <= MAX_CAN_MSG_LEN ? size : MAX_CAN_MSG_LEN;
	return this;
}
uint8_t		c_solutec::getSizeReply( void )
{
	return sizeReply;
}

c_solutec	*c_solutec::setCmd( uint32_t cmd )
{
	saveCmd = cmd & 0x000000FF;
	return this;
}
uint8_t		c_solutec::getCmd( void )
{
	return saveCmd;
}
c_solutec	*c_solutec::setId( uint32_t id )
{
	theId = id + CAN_CHANNEL_BASE;
	return this;
}
uint32_t	c_solutec::getId( void )
{
	return theId - CAN_CHANNEL_BASE;
}
uint8_t		c_solutec::sendCmd( void )
{
	return sizeNoParam == 0xFF ? 0xFE : sendCmd( sizeNoParam );
}
uint8_t		c_solutec::sendParam( void )
{
	return sizeParam == 0xFF ? 0xFD : sendCmd( sizeParam );
}
uint8_t		c_solutec::isResponseOK( void )
{
		return sizeReply == 0xFF ? 0xFC : isResponseOK( sizeReply );
}
uint8_t		c_solutec::isAckOK( void )
{
	
CAN_Frame	*theMessage = new CAN_Frame();

	if( theMessage == NULL )
		return 0x99;
	
	memset( theMessage, 0, sizeof( CAN_Frame ) );
	
	if( wait4Response ( 1 ) )
		*theMessage = GETCAN->read();
	else
	{
		PRINT_DEBUG("c_solutec::isAckOK:(%s): No Response !!!\n",command_name[saveCmd & 0xFF])
		delete theMessage;
		return 0x99;
	}
	
	if( GETCAN->available() )
	{
//		PRINT_DEBUG(" *isAckOK*Trying to reread to see what a fuck (%s)",command_name[saveCmd & 0xFF]);
		isAckOK();
//		PRINT_DEBUG(" ******MSG available after read is: 0x%x\n", GETCAN->available() );
	}
	if( theMessage->length == 1 )
	{
		PRINT_DEBUG("c_solutec::isAckOK:(%s) ## Size is %d, Got a Nack!!\n",
					command_name[saveCmd & 0xFF],(int) theMessage->length )
		delete theMessage;
		return 0x99;
	}
	
	if( theMessage->length != 0 )
	{
//		PRINT_DEBUG("c_solutec::isAckOK:(%s) ## Size is %d, expected %d\n",command_name[saveCmd & 0xFF],(int) theMessage->length, (int) 1 )
//		PRINT_DEBUG("(isAckOK) -> ")
//		for( int i = 0; i < theMessage->length; i++)
//			PRINT_DEBUG(" %2.2x ",(int)theMessage->data[i])
//		PRINT_DEBUG("\n")
		delete theMessage;
		return 0x99;
	}
	delete theMessage;
	return 0;
}

uint8_t		c_solutec::sendCmd( uint8_t size )
{
uint8_t 	ret;
CAN_Frame	*theFrame	= new CAN_Frame();

	if( theFrame == NULL )
		return 0;
	
	theCmd = saveCmd;// restore to be sure
	
	memset( theFrame,0,sizeof(  CAN_Frame ));
	
	if( size > MAX_CAN_MSG_LEN )
		size = MAX_CAN_MSG_LEN ;
	
	for( int i=0; i < size; i++) 
		theFrame->data[i] = ((char *)&theCmd)[i];

	theFrame->id		= theId;
	theFrame->valid 	= true;
	theFrame->length	= size;
	theFrame->extended	= CAN_STANDARD_FRAME;
	theFrame->rtr		= 0;


	ret = GETCAN->write( *theFrame );					// Send the Command

	if( ret != size )
			PRINT_DEBUG("c_solutec::sendCmd:(%s) ## Size is %d, sent %d\n",command_name[saveCmd & 0xFF],(int) size, (int) ret )
			
	delayMicroseconds( (size + CAN_BYTES_HEADER) * CAN_BYTE_TIME );
	
	delete theFrame;
	return ret;
}

uint8_t		c_solutec::isResponseOK( uint8_t size )
{
	CAN_Frame	*theMessage = new CAN_Frame();

	if( theMessage == NULL )
		return 0x83;
	
	if( size > MAX_CAN_MSG_LEN )
		size = MAX_CAN_MSG_LEN ;

	memset( theMessage, 0, sizeof( CAN_Frame ) );
	
	if( wait4Response ( size ) )
		*theMessage = GETCAN->read();
	else
	{
		PRINT_DEBUG("c_solutec::isResponseOK::(%s) No Response !!!\n",command_name[saveCmd & 0xFF])
		memset( &theMessage->data, 0, sizeof( theMessage->data ) );
		delete theMessage;
		return 0x82;
	}

	if( GETCAN->available() )
	{
	int ret;
	
//		PRINT_DEBUG(" *isResponseOK*Trying to reread to see what a fuck(%s)",command_name[saveCmd & 0xFF]);
		ret=isResponseOK( size );
//		PRINT_DEBUG(" ******MSG available after read is: 0x%x (sup: %d, real %d) [c,m]\n",
//					GETCAN->available(), size,  theMessage->length);
		if( ( ret == 0 ) && ( lastSizeRead == size ) && ( theMessage->length != size ))
		{
//			for( int i = 0; i< 8; i++)
//				PRINT_DEBUG(" [%2.2x;%2.2x]",(int)((char *)&theCmd)[i],(int)theMessage->data[i])
//			PRINT_DEBUG("\n")
			delete theMessage;
			return ret;
		}
	}

	lastSizeRead = theMessage->length;
	
	memset( &theCmd, 0, size );
	
	if( theMessage->length > 0 )
		memcpy( &theCmd, &theMessage->data, size  );
	
	if( theMessage->length != size )
	{
		PRINT_DEBUG("c_solutec::isResponseOK ##(%s) Size is %d, expected %d\n\t\t",
					command_name[saveCmd & 0xFF],(int) theMessage->length, (int) size )
		for( int i = 0; i<theMessage->length; i++)
			PRINT_DEBUG(" %2.2x ",(int)theMessage->data[i])
		PRINT_DEBUG("\n")
		delete theMessage;
		return 0x81;
	}
	if( theMessage->valid )
	{
			delete theMessage;
			return 0;
	}
	delete theMessage;
	return 0x82;
}

//*******************************************************************************************************************************

#define CND_ACK				0
#define	CMD_RESET			0xFF
#define CMD_MODULEINFO		0x00

c_moduleinfo::c_moduleinfo( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_moduleinfo ) -sizeof( c_solutec ));
	setCmd( CMD_MODULEINFO );
	setSizeNoParam( getCmdRLength() );
	setSizeReply( getRspLength() );
}
uint8_t	c_moduleinfo::getDataSize( void )
{
	return sizeof( *this );
}

uint8_t	c_moduleinfo::getCmdRLength( void )
{
	return 1;
}
uint8_t	c_moduleinfo::getRspLength( void )
{
	return 8;
}

int 	c_moduleinfo::getSerial()
{
	return ( serial[0]<<8 ) + serial[1];
}
int 	c_moduleinfo::getHardWareVersion()
{
	return ( hardware[0]<<8 ) + hardware[1];
}
int 	c_moduleinfo::getSoftWareVersion()
{
	return ( software[0]<<8 ) + software[1];
}
uint8_t c_moduleinfo::getStatus()
{
	return status;
}

#define CMD_MODULESTATUS	0x01

c_modulestatus::c_modulestatus( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_modulestatus ) -sizeof( c_solutec ));
	setCmd( CMD_MODULESTATUS );
	setSizeNoParam( getCmdRLength() );
	setSizeReply( getRspLength() );

}
uint8_t	c_modulestatus::getCmdRLength( void )
{
	return 1;
}
uint8_t	c_modulestatus::getRspLength( void )
{
	return 2;
}

uint8_t c_modulestatus::getStatus()
{
	return status;
}
#define CMD_IRTXSTATUS	0x10
#define CMD_IRTXSTATUSA	0x11
#define CMD_IRTXSTATUSB	0x12
c_IrTxStatus::c_IrTxStatus( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_IrTxStatus ) -sizeof( c_solutec ));
	setCmd( CMD_IRTXSTATUS );
	setSizeNoParam( getCmdRLength() );
	setSizeParam( 3 );
	setSizeReply( getRspLength() );

}
uint8_t	c_IrTxStatus::getCmdRLength( void )
{
	return 1;
}
uint8_t	c_IrTxStatus::getRspLength( void )
{
	return 3;
}

int 	c_IrTxStatus::getChannelA()
{
	return channel_a;
}
int 	c_IrTxStatus::getChannelB()
{
	return channel_b;
}
c_IrTxStatus *c_IrTxStatus::setChannelA( int a)
{
	channel_a = (uint8_t) a;
	return this;
}
c_IrTxStatus *c_IrTxStatus::setChannelB(int b)
{
	channel_b = (uint8_t) b;
	return this;
}
#define CMD_IRRXSTATUS		0x20
#define CMD_IRRXSTATUSA		0x21
#define CMD_IRRXSTATUSB		0x22
c_IrRxStatus::c_IrRxStatus( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_IrRxStatus ) -sizeof( c_solutec ));
	setCmd( CMD_IRRXSTATUS );
	setSizeNoParam( getCmdRLength() );
	setSizeReply( getRspLength() );
}
uint8_t	c_IrRxStatus::getCmdRLength( void )
{
	return 1;
}
uint8_t	c_IrRxStatus::getRspLength( void )
{
	return 5;
}
int 	c_IrRxStatus::getChannelA()
{
	return ( channel_a[0]<<8 ) + channel_a[1];
}
int 	c_IrRxStatus::getChannelB()
{
	return ( channel_b[0]<<8 ) + channel_b[1];
}
// List & Nodes

c_List	channelList;

bool	c_List::IsListEmpty()
{
//	if( lh_Head == NULL )
//		return TRUE;
//	if( lh_Tail == NULL )
//		return TRUE;
//	if( lh_TailPred == NULL )
//		return TRUE;
	return ( (lh_TailPred) == (c_Node *)(this) );
}

c_List 	*c_List::AddHead( c_Node *n)
{
c_Node	*tmp = lh_Head;

	lh_Head = n;
	n->setSucc( tmp );
	n->setPred( (c_Node *) this );
	tmp->setPred( n );
	return this;
}

c_List	*c_List::AddTail( c_Node *n)
{
c_Node	*tmp = ((c_Node *)&lh_Tail)->getPred();

	((c_Node *)&lh_Tail)->setPred( n );
	n->setSucc( ((c_Node *)&lh_Tail) );
	n->setPred( tmp );
	tmp->setSucc( n );
	return this;
}

c_List	*c_List::Enqueue( c_Node *n)
{
	PRINT_WARNING("\n\t no implemented Enqueue, c_List::Enqueue \007\n")
	return this;
}

c_List	*c_List::Insert( c_Node *n, c_Node *after )
{
	PRINT_WARNING("\n\t no implemented Insert, c_List::Insert \007\n")
	return this;
}

c_Node	*c_List::RemHead( void )
{
c_Node	*ret = lh_Head;
c_Node	*tmp = ret->getSucc();

	if( tmp != NULL )
	{
		lh_Head = tmp;
		tmp->setPred( (c_Node *)this);
	}
	return ret;
}

c_Node	*c_List::RemTail( void )
{
c_Node	*tailPred	= lh_TailPred;
c_Node	*ret 		= tailPred->getPred();

	if( ret != NULL )
	{
		lh_TailPred = ret;
		ret->setSucc( (c_Node *) &lh_Tail );
		ret = tailPred;
	}
	return ret;
}

c_List	*c_List::Remove( c_Node *n )
{
c_Node	*tmp = n->getSucc();

	n->getPred()->setSucc( tmp );
	tmp->setPred( n->getPred());
	return this;
}


#define	RETRIES_BASE		1
#define	FUNC_TIME			delayMicroseconds( 10000 );

int  DB_AxeData::doCalibration(  )
{
c_IrTxStatus		*txStatus;
c_IrRxStatus		*rxStatus;
c_GetMeasurement	*mdStatus;

short			newCal;
short			retry		= RETRIES_BASE;
short 			oldCal		= HumidityCalibration();


		mdStatus = new c_GetMeasurement();
		if( mdStatus )
		{
			mdStatus->setId( ID ) ->
					sendCmd();
			FUNC_TIME
			if( mdStatus->isResponseOK() )
			{
				PRINT_ERROR( "Error in doCalibration, no sensor at pos %d\n", ID)
				HumidityChannelConnected( FALSE ) ->
				HumidityChannelUseA(		 FALSE ) ->
				HumidityChannelUseB(		 FALSE );
				delete mdStatus;
				return oldCal;
			}
			else
				HumidityChannelConnected( TRUE  ) ->
				HumidityChannelUseA(		 FALSE ) ->
				HumidityChannelUseB(		 FALSE );
				delete mdStatus;
		}
		else
		{// mdStatus == NULL don't delete it
				PRINT_ERROR( "Error in doCalibration, mdStatus is NULL at pos %d\n", ID)
				HumidityChannelConnected( FALSE ) ->
				HumidityChannelUseA(		 FALSE ) ->
				HumidityChannelUseB(		 FALSE );
				return oldCal;
		}

		
		if ( ( oldCal < 1 ) || ( oldCal > 256 ) )
			oldCal = 1;
		
		newCal = oldCal;

		txStatus = new c_IrTxStatus();
		rxStatus = new c_IrRxStatus();

		while ( retry > 0 )
		{
			rxStatus->setId( ID )
					->sendCmd();
			FUNC_TIME
			if( rxStatus->isResponseOK() != 0x81 )
			{
				retry = -1;
				break;
			}
			else
			{
				retry -= 1;
				FUNC_TIME
			}
		}
		retry = RETRIES_BASE;
		do
		{
			newCal -= 1;
			retry = RETRIES_BASE;
			while ( retry > 0 )
			{			
				txStatus->setChannelA( newCal )
						->setChannelB( newCal )
						->setId( ID )
						->sendParam();
				FUNC_TIME
				if( txStatus->isAckOK() )
					retry -=1;
				else
					retry = -1;
			}
			
			FUNC_TIME

			retry = RETRIES_BASE;
			while ( retry > 0 )
			{
				rxStatus->setId( ID )
						->sendCmd();
				FUNC_TIME
				if( rxStatus->isResponseOK() != 0x81 )
				{
					retry = -1;
					break;
				}
				else
				{
					retry -= 1;
					FUNC_TIME
				}
			}
			if( retry == 0 )
			{
				PRINT_ERROR( "Error in doCalibration low, can't read level TX error\n")
				break;
			}
			else
				retry = RETRIES_BASE;
		} while (
			(
				( rxStatus->getChannelA() > CAL_LIMIT ) |
				( rxStatus->getChannelB() > CAL_LIMIT )
			)  &
				( newCal > 2)
			  );
		retry = RETRIES_BASE;
		//** check for the 2 channel... this is dipending right or left sensor....
		do
		{	
			newCal += 1;
			retry = RETRIES_BASE;
			while ( retry > 0 )
			{
				txStatus->setChannelA( newCal )
						->setChannelB( newCal )
						->setId( ID )
						->sendParam();
				FUNC_TIME
				if( txStatus->isAckOK() )
					retry -=1;
				else
					retry = -1;
			}
			retry = RETRIES_BASE;
			while ( retry > 0 )
			{
			int err;

				rxStatus->setId(ID)
						->sendCmd();
						
				FUNC_TIME
				
				err = rxStatus->isResponseOK();
				if(  err != 0x81 )
				{
					retry = -2;
					break;
				}
				else
				{
					retry -= 1;
					FUNC_TIME
				}
			}
			if( retry == 0 )
			{
				PRINT_ERROR( "Error in doCalibration high, can't read level TX error, restoring old value %d \n",oldCal )
				newCal = oldCal;
				txStatus->setChannelA( newCal )
						->setChannelB( newCal )
						->setId( ID )
						->sendParam();
				FUNC_TIME
				txStatus->isAckOK();
				break;
			}
			else
				retry = RETRIES_BASE;
//			PRINT_DEBUG(" cal %d level A( %d ), B( %d )\n ", newCal,rxStatus->getChannelA(),rxStatus->getChannelB() )
		} while (
			(
				( rxStatus->getChannelA() < CAL_LIMIT ) &
				( rxStatus->getChannelB() < CAL_LIMIT )
			) &
				( newCal < 255 )
			  );
		if( newCal >= GETCALIBRATIONLIMIT )
		{
			HumidityChannelCalibrationError( TRUE );
			if( GETHIGHVAC )
				if( GETHIGHVAC->openConnection() )
				{
					GETHIGHVAC->setNeed2Clean( TRUE );
					GETHIGHVAC->closeConnection();
				}
			PRINT_ERROR("****** ERROR ******: Calibration level to high, please check glass ***********\n")
		}
		else
			HumidityChannelCalibrationError( FALSE );
		{
		unsigned short		flags = HumidityChannelFlags() & (~(CH_FLAG_USEA | CH_FLAG_USEB ));
		c_GetMeasurement	*cmd = new c_GetMeasurement();
		
			HumidityCalibration( newCal & 0x00ff );

			flags |=  rxStatus->getChannelA() >= CAL_LIMIT ? CH_FLAG_USEA : 0;
			flags |=  rxStatus->getChannelB() >= CAL_LIMIT ? CH_FLAG_USEB : 0;

			HumidityChannelFlags( flags );

			cmd->resetError(  )
				->setChannelUse( rxStatus->getChannelA() >= CAL_LIMIT ? TRUE : FALSE, rxStatus->getChannelB() >= CAL_LIMIT ? TRUE : FALSE )
				->setTxLevel( newCal & 0x00ff )
				->setId( ID );
			cmd->sendParam();
			FUNC_TIME
			if( cmd->isAckOK())
				PRINT_DEBUG("****** ERROR ******: can't set cal with new cmd ***********\n")

			
		}
		
		HumidityCalibrationResult( rxStatus->getChannelA() > rxStatus->getChannelB() ? rxStatus->getChannelA() : rxStatus->getChannelB() );
		ProcessCalibrationResult( rxStatus->getChannelA() > rxStatus->getChannelB() ? rxStatus->getChannelA() : rxStatus->getChannelB() );
		
		delete rxStatus;
		delete txStatus;
		return newCal;
}

#define CMD_GETMESUREMENT			0x50

#define MASK_GETMESUREMENT_CHANNEL	0b00000011
#define MASK_GETMESUREMENT_CHANNELB	0b00000001
#define MASK_GETMESUREMENT_CHANNELA	0b00000010
#define MASK_GETMESUREMENT_AUTOSET	0b00000100
#define MASK_GETMESUREMENT_AACTIVE	0b00001000
#define MASK_GETMESUREMENT_rsrv1	0b00010000
#define MASK_GETMESUREMENT_CONFERR	0b00100000
#define MASK_GETMESUREMENT_ASETERR	0b01000000
#define MASK_GETMESUREMENT_TEMPERR	0b10000000
#define MASK_GETMESUREMENT_RESETERR	0b10000000

//#define SETMESUREMENT_BASE			(&(((unsigned char *)this)[sizeof( c_solutec )]))
//#define SETMESUREMENT_FLAGS			(SETMESUREMENT_BASE[0])
//#define SETMESUREMENT_POWERLEVEL	(SETMESUREMENT_BASE[1])

c_GetMeasurement::c_GetMeasurement( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_GetMeasurement ) -sizeof( c_solutec ));
	setCmd( CMD_GETMESUREMENT );
	setSizeNoParam( getCmdRLength() );
	setSizeParam( 3 );
	setSizeReply( getRspLength() );
}
/*uint8_t c_GetMeasurement::getDataSize( void )
{
	return sizeof( *this );
}*/
uint8_t c_GetMeasurement::getCmdRLength( void )
{
	return 1;
}
uint8_t c_GetMeasurement::getRspLength( void )
{
	return 8;
}

int c_GetMeasurement::getHumidity()
{
	return ( humidity[0] << 8 ) + humidity[1];
}
int c_GetMeasurement::getPressure()
{
	return ( ( pressure[0] << 8 ) + pressure[1] ) * ( 4000/500 );
}
int c_GetMeasurement::getTemperature()
{
	return temperature;
}
int c_GetMeasurement::getTxLevel( void )
{
	return power_level;
}
bool c_GetMeasurement::getAutoSetPower( void )
{
	return flags & MASK_GETMESUREMENT_AUTOSET;
}
bool				c_GetMeasurement::getAutoSetActive( void )
{
	return flags & MASK_GETMESUREMENT_AACTIVE;
}
bool				c_GetMeasurement::getConfigurationError( void )
{
	return flags & MASK_GETMESUREMENT_CONFERR;
}
bool				c_GetMeasurement::getAutoSetError( void )
{
	return flags & MASK_GETMESUREMENT_ASETERR;
}
bool				c_GetMeasurement::getTemperatureError( void )
{
	return flags & MASK_GETMESUREMENT_TEMPERR;
}
int					c_GetMeasurement::getChannelUse( void )
{
	return flags & MASK_GETMESUREMENT_CHANNEL;
}

c_GetMeasurement	*c_GetMeasurement::setChannelUse( bool TxA, bool TxB )
{
	flags = ( flags & ~MASK_GETMESUREMENT_CHANNEL ) 	|
			( TxA ? MASK_GETMESUREMENT_CHANNELA : 0 )				|
			( TxB ? MASK_GETMESUREMENT_CHANNELB : 0 );
	return this;
}

c_GetMeasurement	*c_GetMeasurement::setChannelUse( int channel )
{
	flags = ( flags & ~MASK_GETMESUREMENT_CHANNEL ) 	|
							( channel & MASK_GETMESUREMENT_CHANNEL );
	return this;
}

c_GetMeasurement	*c_GetMeasurement::setAutoSetPower( bool status )
{
	flags = ( flags & ~MASK_GETMESUREMENT_AUTOSET ) 	|
							( status ? MASK_GETMESUREMENT_AUTOSET : 0 );
	return this;
}

c_GetMeasurement	*c_GetMeasurement::resetError( void )
{
	flags = flags | MASK_GETMESUREMENT_RESETERR ;
	return this;
}

c_GetMeasurement	*c_GetMeasurement::resetFlags( void )
{
	flags	= 0;
	return this;
}

c_GetMeasurement	*c_GetMeasurement::setTxLevel( int level )
{
	power_level = level;
	return this;
}

c_GetMeasurement	*c_GetMeasurement::clearResponse( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_GetMeasurement ) -sizeof( c_solutec ));
	return this;
}


/*
#define CMD_SETMESUREMENT	0x50
c_SetMeasurement::c_SetMeasurement( void )
{
	memset( &(((char *)this)[sizeof( c_solutec )]), 0, sizeof( c_SetMeasurement ) -sizeof( c_solutec ));
	setCmd( CMD_IRTXSTATUS );
	setSizeNoParam( getCmdRLength() );
	setSizeParam( 3 );
	setSizeReply( getRspLength() );
}
uint8_t			c_SetMeasurement::getCmdRLength( void );
uint8_t			c_SetMeasurement::getRspLength( void );
*/
