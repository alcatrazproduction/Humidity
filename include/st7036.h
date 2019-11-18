/*****************************************************************************
 * 
 *  $Author:       Yves Huguenin
 *  $Title:        st7036.h
 *  $Create Date:  12.08.2014
 *  $Vers:         1.0a1
 *
 *  1.0a1:
 *
 * **************************************************************************/

#ifndef __ST7036__
#define __ST7036__

#include "SPI.h"

#define		ST_7036_FALSE								0
#define		ST_7036_TRUE								1
#define		ST_7036_OLD									-1

#define		ST_7036_BEGIN( a )							( ( st7036 * ) a )
#define		ST_7036_CLEAR								->Clear()
#define		ST_7036_HOME								->Home()
#define		ST_7036_ENTRYMODE( a,b )					->EntryMode( a,b)
#define		ST_7036_DISPLAY( a,b,c)						->DisplayStatus( a,b,c )
#define		ST_7036_SHIFT( a,b)							->Shift( a,b )
#define		ST_7036_FUNCTIONSET( a,b,c,d,e)				->FunctionSet( a,b,c,d,e )
#define		ST_7036_CGRAMADDR(a)						->SetCGRAMAddr( a )
#define		ST_7036_DRAMADDR( a )						->SetDRAMAddr( a )
#define		ST_7036_ICONADDR( a )						->SetIconAddr( s )
#define		ST_7036_WRITE( a )							->WriteData( a )
#define		ST_7036_BIAS( a )							->SetBias( a )
#define		ST_7036_CONTRAST( a )						->SetContrast( ST_7036_OLD,ST_7036_OLD,a)
#define		ST_7036_FOLLOWER( a,b)						->SetFollower( a,b)
#define		ST_7036_DELAY( a )							->Wait( a )

#define		ST_7036_END									;

#define		ST_7036_SET_INSTRUCTION_MODE				digitalWrite( RS_Pin, 	LOW);
#define		ST_7036_SET_DATA_MODE						digitalWrite( RS_Pin, 	HIGH);

#define 	ST_7036_BIT_ID								( 0b00000010 )
#define 	ST_7036_BIT_S								( 0b00000001 )
#define 	ST_7036_BIT_D								( 0b00000100 )
#define 	ST_7036_BIT_C								( 0b00000010 )
#define 	ST_7036_BIT_B								( 0b00000001 )
#define 	ST_7036_BIT_SC								( 0b00001000 )
#define 	ST_7036_BIT_RL								( 0b00000100 )
#define 	ST_7036_BIT_DL								( 0b00010000 )
#define 	ST_7036_BIT_N								( 0b00001000 )
#define 	ST_7036_BIT_DH								( 0b00000100 )
#define 	ST_7036_BIT_IS2								( 0b00000010 )
#define 	ST_7036_BIT_IS1								( 0b00000001 )
#define 	ST_7036_BIT_UD								( 0b00001000 )
#define 	ST_7036_BIT_BS								( 0b00001000 )
#define 	ST_7036_BIT_FX								( 0b00000001 )
#define 	ST_7036_BIT_ION								( 0b00001000 )
#define 	ST_7036_BIT_BON								( 0b00000100 )
#define 	ST_7036_BIT_FON								( 0b00001000 )



#define		ST_7036_INST_CLEAR							( 0b00000001 )
#define		ST_7036_INST_HOME							( 0b00000010 )
#define		ST_7036_INST_MODE_SET( id,s )				(( 0b00000100 ) | ( id 	? ST_7036_BIT_ID 	: 0 ) | ( s 	? ST_7036_BIT_S 	: 0 ))
#define		ST_7036_INST_DISPLAY( d,c,b )				(( 0b00001000 ) | ( d	? ST_7036_BIT_D		: 0 ) | ( c		? ST_7036_BIT_C		: 0 )\
																	   | ( b	? ST_7036_BIT_B 	: 0 ))
#define		ST_7036_INST_SHIFT( sc,rl )					(( 0b00010000 ) | ( sc	? ST_7036_BIT_SC	: 0 ) | ( rl	? ST_7036_BIT_RL	: 0 ))
#define		ST_7036_INST_FUNCTION( dl,n,dh,is2,is1 )	(( 0b00100000 ) | ( dl	? ST_7036_BIT_DL	: 0 ) | ( n		? ST_7036_BIT_N		: 0 )\
																	   | ( dh	? ST_7036_BIT_DH	: 0 ) | ( is2	? ST_7036_BIT_IS2	: 0 )\
																	   | ( is1	? ST_7036_BIT_IS1	: 0 ))
#define		ST_7036_INST_CGRAM( addr )					(( 0b01000000 ) | ( addr & 0b00111111 		))
#define		ST_7036_INST_DRAM( addr )					(( 0b10000000 ) | ( addr & 0b01111111			))
#define		ST_7036_INST_BIAS( bs,fx )					(( 0b00010100 ) | ( bs 	? ST_7036_BIT_BS	: 0 ) | ( fx	? ST_7036_BIT_FX	: 0 ))
#define		ST_7036_INST_ICON( addr )					(( 0b01000000 ) | ( addr & 0b00001111			))
#define		ST_7036_INST_POWER( ion,bon,contrast )		(( 0b01010000 ) | ( ion	? ST_7036_BIT_ION	: 0 ) | ( bon	? ST_7036_BIT_BON	: 0 )\
																	   | ( ( contrast & 0b00110000 ) >> 4 ))
#define		ST_7036_INST_FOLLOWER( fon,rab )			(( 0b01100000 ) | ( fon	? ST_7036_BIT_FON	: 0 ) | ( rab & 0b00000111 ))
#define		ST_7036_INST_CONTRAST_LOW( contrast )		(( 0b01110000 ) | ( contrast & 0b00001111 ))
#define		ST_7036_INST_DOUBLE( ud )					(( 0b00010000 ) | ( ud 	? ST_7036_BIT_UD	: 0 ))

#define		ST_7036_START_WRITE( num )					->send( num 
#define		ST_7036_DOWRITE( data )						, ((char *)data)
#define		ST_7036_END_WRITE							)
/*
 * st7036 Class definition
 */

class st7036
{
// All field are defined private
private:
	bool		mode_increment;				//	I/D
	bool		mode_shift;					//	S
	bool		display_on;					//	D
	bool		cursor_on;					//	C
	bool		cursor_blink;				//	B
	bool		shift_screen;				//	S/C
	bool		shift_right;				//	R/L
	bool		interface_size;				//	DL
	bool		two_lines;					//	N
	bool		double_height;				//	DH
	bool		instruction_height;			//	IS2
	bool		instruction_low;			//	IS1
	bool		dbl_height_set;				//	AC
	bool		bias_selection;				//	BS
	bool		icon_on;					//	Ion
	bool		booster_on;					//	Bon
	bool		follower_on;				//	Fon
	
	
	char		double_height_pos;
	char		CGRam_address;
	char		DDRam_address;
	char		Icon_address;
	char		contrast;

	char		RS_Pin;
	char		CSB_Pin;
	char		SI_Pin;
	char		SCL_Pin;
	int			Device;

	SPIClass	*port;
	
	
private:
	
	st7036		*send( int num,...);

public:
// constructor

	st7036( int theDevice );
	st7036( int theDevice , char si, char scl, char rs, char csb );

	st7036		*Clear( void );
	st7036		*Home( void );
	st7036		*EntryMode( char I_D, char S);
	st7036		*DisplayStatus( char Display, char Cursor, char Blink );
	st7036		*Shift( char S_C, char R_L );
	st7036		*FunctionSet( char DL, char N, char DH, char IS2, char IS1 );
	st7036		*SetCGRAMAddr( int addr );
	st7036		*SetDRAMAddr( int addr );
	st7036		*WriteData( int data );
	st7036		*WriteData( char *data );
	st7036		*SetBias( char BS );
	st7036		*SetIconAddr( int addr );
	st7036		*SetContrast( char theIcon, char theBouster, int theContrast );
	st7036		*SetFollower( char theFollower, int theRatio );
	st7036		*Wait( long theDelay );


	int			 ReadBusyAddr( void );
	int			 ReadData( void );
};

#endif
