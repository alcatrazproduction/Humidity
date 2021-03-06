/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

#include "humidity.h"
#include <stdarg.h>

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::send( int num, ...)
{
va_list valist;

    va_start(valist, num);
PRINT_DEBUG("Sending data( %d ): ",num);
    for (int i = 0; i < num; i++)
    {
	char 	*string = va_arg(valist, char *);
	int		data	= (( int )string ) & 0x000000ff;
	
		if( (( long )string ) & 0xff00 )
		{
		int		len 		= strlen( string );

			for( int j=0; j<len; j++ )
			{
				data = string[j];
				PRINT_DEBUG(" %2.2x:", data  );
				if( Device < 0 )
				{
					if( Device == -100 )
					{
						digitalWrite( SCL_Pin, 	HIGH );
						digitalWrite( CSB_Pin, 	LOW );
						for( char shift = 0; shift < 8; shift ++ )
						{
							digitalWrite( SI_Pin, data & 0b10000000 ? HIGH : LOW );
							digitalWrite( SCL_Pin, LOW );
putchar( data & 0b10000000 ? '1' : '0' );
							data = data << 1;
							digitalWrite( SCL_Pin, HIGH );
						}
						digitalWrite( CSB_Pin, 	HIGH );
						digitalWrite( SCL_Pin, 	HIGH );
						delayMicroseconds( 100 );
					}
					else
					{
						digitalWrite( SCL_Pin, 	LOW );
						digitalWrite( CSB_Pin, 	LOW );
						shiftOut( SI_Pin, SCL_Pin, MSBFIRST, data );
						digitalWrite( CSB_Pin, 	HIGH );
						delayMicroseconds( 100 );
						digitalWrite( SCL_Pin, 	LOW );
					}
				}
				else
					port->transfer( data, ( i < num ) & ( j < len) ? SPI_CONTINUE : SPI_LAST );

			}
		}
		else
		{	
			PRINT_DEBUG(" %2.2x:", data );
			if( Device < 0 )
			{
				if( Device == -100 )
				{
					digitalWrite( SCL_Pin, 	HIGH );
					digitalWrite( CSB_Pin, 	LOW );
					for( char shift = 0; shift < 8; shift ++ )
					{
						digitalWrite( SI_Pin, data & 0b10000000 ? HIGH : LOW );
						digitalWrite( SCL_Pin, LOW );
	putchar( data & 0b10000000 ? '1' : '0' );
						data = data << 1;
						digitalWrite( SCL_Pin, HIGH );
					}
					digitalWrite( CSB_Pin, 	HIGH );
					digitalWrite( SCL_Pin, 	HIGH );
					delayMicroseconds( 100 );
				}
				else
				{
					digitalWrite( SCL_Pin, 	LOW );
					digitalWrite( CSB_Pin, 	LOW );
					shiftOut( SI_Pin, SCL_Pin, MSBFIRST, data );
					digitalWrite( CSB_Pin, 	HIGH );
					delayMicroseconds( 100 );
					digitalWrite( SCL_Pin, 	LOW );
				}
			}
			else
				port->transfer( data, i < num ? SPI_CONTINUE : SPI_LAST );
		}
    }
    va_end(valist);
PRINT_DEBUG("\n");
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/
st7036::st7036( int theDevice )
{
	Device				= theDevice;
	if( theDevice < 0 )
		abort(  );

	if( theDevice == 0 )
	{
		port 	= &SPI;
		CSB_Pin	= SPI_CS;
	}
	else
	{
		port 	= &SPIEX;
		CSB_Pin	= SPIEX_CS;
	}
	interface_size		= TRUE;
	two_lines			= FALSE;
	double_height		= FALSE;
	instruction_height	= FALSE;
	instruction_low		= FALSE;
	display_on			= FALSE;
	cursor_on			= FALSE;
	cursor_blink		= FALSE;
	mode_increment		= TRUE;
	mode_shift			= FALSE;
	icon_on				= FALSE;
	bias_selection		= FALSE;
	booster_on			= FALSE;
	follower_on			= FALSE;
	contrast			= 0b00010000;
	dbl_height_set		= FALSE;

	pinMode( RS_Pin, 	OUTPUT);
	
	port->begin();
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/
st7036::st7036( int theDevice , char si, char scl, char rs, char csb )
{
	Device				= theDevice;
	if( theDevice > 0 )
	{
		if( theDevice == 0 )
		{
			port 	= &SPI;
			CSB_Pin	= SPI_CS;
		}
		else
		{
			port 	= &SPIEX;
			CSB_Pin	= SPIEX_CS;
		}
		interface_size		= TRUE;
		two_lines			= FALSE;
		double_height		= FALSE;
		instruction_height	= FALSE;
		instruction_low		= FALSE;
		display_on			= FALSE;
		cursor_on			= FALSE;
		cursor_blink		= FALSE;
		mode_increment		= TRUE;
		mode_shift			= FALSE;
		icon_on				= FALSE;
		bias_selection		= FALSE;
		booster_on			= FALSE;
		follower_on			= FALSE;
		contrast			= 0b00010000;
		dbl_height_set		= FALSE;

		pinMode( RS_Pin, 	OUTPUT);

		port->begin();
		return;
	}
	interface_size		= TRUE;
	two_lines			= FALSE;
	double_height		= FALSE;
	instruction_height	= FALSE;
	instruction_low		= FALSE;
	display_on			= FALSE;
	cursor_on			= FALSE;
	cursor_blink		= FALSE;
	mode_increment		= TRUE;
	mode_shift			= FALSE;
	icon_on				= FALSE;
	bias_selection		= FALSE;
	booster_on			= FALSE;
	follower_on			= FALSE;
	contrast			= 0b00010000;
	dbl_height_set		= FALSE;

	RS_Pin				= rs;
	CSB_Pin				= csb;
	SI_Pin				= si;
	SCL_Pin				= scl;
	
	pinMode( RS_Pin, 	OUTPUT);
	pinMode( CSB_Pin, 	OUTPUT);
	pinMode( SI_Pin, 	OUTPUT);
	pinMode( SCL_Pin, 	OUTPUT);
	
	port = NULL;
}
/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::Clear( void )
{
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_CLEAR )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::Home( void )
{
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_HOME )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::EntryMode( char I_D, char S)
{
	if( I_D < 0 )
		I_D = mode_increment;
	else
		mode_increment = I_D;
	if( S < 0 )
		S = mode_shift;
	else
		mode_shift = S;
	
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_MODE_SET( I_D, S ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::DisplayStatus( char Display, char Cursor, char Blink )
{
	if( Display < 0 )
		Display = display_on;
	else
		display_on = Display;
	if( Cursor < 0 )
		Cursor = cursor_on;
	else
		cursor_on = Cursor;
	if( Blink < 0 )
		Blink = cursor_blink;
	else
		cursor_blink = Blink;
	
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_DISPLAY( Display, Cursor, Blink ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::Shift( char S_C, char R_L )
{
	if( S_C < 0 )
		S_C = shift_screen;
	else
		shift_screen = S_C;
	if( R_L < 0 )
		R_L = shift_right;
	else
		shift_right = R_L;

	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_SHIFT( S_C, R_L ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::FunctionSet( char DL, char N, char DH, char IS2, char IS1 )
{
	if( DL < 0 )
		DL = interface_size;
	else
		interface_size = DL;
	if( N < 0 )
		N = two_lines;
	else
		two_lines = N;
	if( DH < 0 )
		DH = double_height;
	else
		double_height = DH;
	if( IS2 < 0 )
		IS2 = instruction_height;
	else
		instruction_height = IS2;
	if( IS1 < 0 )
		IS1 = instruction_low;
	else
		instruction_low = IS1;


	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( DL, N, DH, IS2, IS1 ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::SetCGRAMAddr( int addr )
{
	if( addr < 0 )
		addr = CGRam_address;
	else
		CGRam_address = addr;

	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_CGRAM( addr ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::SetDRAMAddr( int addr )
{
	if( addr < 0 )
		addr = DDRam_address;
	else
		DDRam_address = addr;

	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( ST_7036_INST_DRAM( addr ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::WriteData( int data )
{
	ST_7036_SET_DATA_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( data )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::WriteData( char *data )
{
	ST_7036_SET_DATA_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 1 )
	ST_7036_DOWRITE( data )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::SetBias( char BS )
{
	if( BS < 0 )
		BS = bias_selection;
	else
		bias_selection = BS;
	
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 3 )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, 0, 1 ) )
	ST_7036_DOWRITE( ST_7036_INST_BIAS( BS, 0 ) )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, instruction_height, instruction_low ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::SetIconAddr( int addr )
{
	if( addr < 0 )
		addr = Icon_address;
	else
		Icon_address = addr;
	
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 3 )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, 0, 1 ) )
	ST_7036_DOWRITE( ST_7036_INST_ICON( addr ) )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, instruction_height, instruction_low ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::SetContrast( char theIcon, char theBouster, int theContrast )
{
	if( theIcon <  0 )
		theIcon = icon_on;
	else
		icon_on = theIcon;
	if( theBouster < 0 )
		theBouster = booster_on;
	if( theContrast < 0 )
		theContrast = contrast;
	else
		contrast = theContrast;
	
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 4 )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, 0, 1 ) )
	ST_7036_DOWRITE( ST_7036_INST_POWER( theIcon, theBouster, theContrast ) )
	ST_7036_DOWRITE( ST_7036_INST_CONTRAST_LOW( theContrast ) )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, instruction_height, instruction_low ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::SetFollower( char theFollower, int theRatio )
{
	if( theFollower < 0 )
		theFollower = follower_on;
	else
		follower_on = theFollower;
	if( theRatio < 0 )
		;
	else
		;
	
	ST_7036_SET_INSTRUCTION_MODE
	ST_7036_BEGIN( this )
	ST_7036_START_WRITE( 3 )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, 0, 1 ) )
	ST_7036_DOWRITE( ST_7036_INST_FOLLOWER( theFollower, theRatio ) )
	ST_7036_DOWRITE( ST_7036_INST_FUNCTION( interface_size, two_lines, double_height, instruction_height, instruction_low ) )
	ST_7036_END_WRITE
	ST_7036_END
	return this;
}

/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

st7036 *st7036::Wait( long theDelay )
{
	delayMicroseconds( theDelay );
	return this;
}


/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

int st7036::ReadBusyAddr( void )
{
	return 0;
}
int st7036::ReadData( void )
{
	return 0;
}
