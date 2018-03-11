/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

#ifdef DEBUG
#	if DEBUG > 1
#		define	PRINT_DEBUG( ... )	PRINTF ( stderr, __VA_ARGS__ );
#	else
#		define	PRINT_DEBUG( ... )
#	endif
#	define	PRINT_WARNING( ... )	PRINTF ( stderr, __VA_ARGS__ );
#else
#	define	PRINT_DEBUG( ... )
#endif
#	define	PRINT_WARNING( ... )	PRINTF ( stderr, __VA_ARGS__ );
#define		PRINT_ERROR( ... )		PRINTF ( stderr, __VA_ARGS__ );

//#include <Python.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <endian.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <argp.h>

#include "Endian.h"
#include "arduino.h"

#include "st7036.h"

const char					*argp_program_version		= "$VER: 0.1 alpha";
const char					*argp_program_bug_address	= "<yves.huguenin@tricala.org>";

static char					doc[]						= "All the stuff to pilot the DOG LCD Module EA DOGM162x-A";
static char					args_docs[]					= "the args";

static struct argp_option	options[]					= {
										{ "verbose",		'v',	0,			0,						"Produce verbose output" 					, 0 },
										{ "clear",			'c',	0,			0,						"Clear display" 							, 0 },
										{ "home",			'h',	0,			0,						"Go to Home position" 						, 0 },
										{ "first",			'1',	"TEXT",		OPTION_ARG_OPTIONAL,	"Go to start of first line and write TEXT" 	, 0 },
										{ "second",			'2',	"TEXT",		OPTION_ARG_OPTIONAL,	"Go to start ofsecond line and write TEXT" 	, 0 },
										{ "init",			'i',	0,			0,						"Initialise the lcd processor with default"	, 0 },
										{ "device",			'd',	"NUM",		0,						"Set the device,-100 = hard"				, 0 },
										{ "line",			'l',	"NUM",		0,						"Set the number of lines [1,2]"				, 0 },

														{ 0 }
};

void setup( void )
{
}

void loop( void )
{
}

static error_t parse_opt( int key, char *arg, struct argp_state *state )
{
	switch ( key )
	{
		case 'v':
			break;
		case 'c':
			ST_7036_BEGIN( state->input )
			ST_7036_CLEAR
			ST_7036_DELAY( 30 )
			ST_7036_END
			break;
		case 'h':
			ST_7036_BEGIN( state->input )
			ST_7036_HOME
			ST_7036_DELAY( 30 )
			ST_7036_END
			break;
		case '1':
			ST_7036_BEGIN( state->input )
			ST_7036_DRAMADDR( 0 )
			ST_7036_DELAY( 30 )
			ST_7036_WRITE( arg )
			ST_7036_DELAY( 30 )
			ST_7036_END
			break;
		case '2':
			ST_7036_BEGIN( state->input )
			ST_7036_DRAMADDR( 40 )
			ST_7036_DELAY( 30 )
			ST_7036_WRITE( arg )
			ST_7036_DELAY( 30 )
			ST_7036_END
			break;
		case 'i':
			ST_7036_BEGIN( state->input )
			ST_7036_FUNCTIONSET( ST_7036_TRUE,	ST_7036_TRUE,	ST_7036_FALSE,	ST_7036_FALSE,	ST_7036_FALSE 	)
			ST_7036_DELAY( 30 )
			ST_7036_BIAS( ST_7036_FALSE )
			ST_7036_DELAY( 30 )
			ST_7036_DISPLAY( ST_7036_TRUE, ST_7036_FALSE, ST_7036_FALSE )
			ST_7036_DELAY( 30 )
			ST_7036_FOLLOWER( ST_7036_TRUE, 2 )
			ST_7036_DELAY( 200000 )
			ST_7036_CLEAR
			ST_7036_DELAY( 2000 )
			ST_7036_ENTRYMODE( ST_7036_TRUE, ST_7036_FALSE )
			ST_7036_DELAY( 30 )
			ST_7036_DRAMADDR( 0 )
			ST_7036_DELAY( 30 )
			ST_7036_HOME
			ST_7036_DELAY( 30 )
			ST_7036_END
			break;
		case 'd':
			break;
		case 'l':
			break;

		case ARGP_KEY_ARG:
			argp_usage( state );
			break;
		case ARGP_KEY_END:
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int main( int argc, char **argv)
{
st7036		*lcd;
struct argp	argp 	= { options,	parse_opt,	args_docs,	doc };
//	if( argc == 1 )
//	{
//		return 0;
//	}
	init();
	pinMode( GPIO11, 	OUTPUT);
	digitalWrite( GPIO11, HIGH );
	lcd = new st7036( -100, GPIO12, GPIO13, GPIO0, GPIO1 );
	if( lcd == NULL )
	{
		PRINTF( stderr, "Error initialising library \n");
		return -1;
	}

	argp_parse( &argp, argc, argv, 0, 0, lcd );
	
	return 0;
}