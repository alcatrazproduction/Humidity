/*****************************************************************************
 *
 *
 *
 *
 *
 *
 * **************************************************************************/

#include "fondarex.h"

unsigned char	reserved_cpu_id[]	= "ID:165166878048517249484848058283c6";

static const unsigned char 	copyright[] 	= "Yves Huguenin\nCase postale 276\nCH-1814 La Tour-de-Peilz\nSwitzerland\nmailto:yves.huguenin@dreampitbikes.ch\nhttp://www.tricala.org/";
static unsigned char 		tocrypt[] 		= "Yves Huguenin\nCase postale 276\nCH-1814 La Tour-de-Peilz\nSwitzerland\nmailto:yves.huguenin@dreampitbikes.ch\nhttp://www.tricala.org/";

static char					conf[]			= "/etc/humidity.conf";


void	encode( unsigned long a, unsigned long b, unsigned long c, unsigned long d )
{
short i=0;

	memcpy( &tocrypt, &copyright, sizeof( tocrypt ) );

	PRINT_DEBUG("Crypting CPU ID: %8.8x %8.8x %8.8x %8.8x ", a, b, c, d)
	
	for( int j = 0; j < 32; j ++ )
	{
	unsigned char e = tocrypt[i] | (unsigned char)( ( a & 0x80000000 ) >> 24 );

		a = a << 1;
		tocrypt[i++] = e;
	}
	
	for( int j = 0; j < 32; j ++ )
	{
	unsigned char e = tocrypt[i] | (unsigned char)( ( b & 0xC0000000 ) >> 24 );

		b = b << 1;
		tocrypt[i++] = e;
	}
	
	for( int j = 0; j < 32; j ++ )
	{
	unsigned char e = tocrypt[i] | (unsigned char)( ( c & 0xE0000000 ) >> 24 );

		c = c << 1;
		tocrypt[i++] = e;
	}
	
	for( int j = 0; j < 32; j ++ )
	{
	unsigned char e = tocrypt[i] | (unsigned char)( ( d & 0xF0000000 ) >> 24 );

		d = d << 1;
		tocrypt[i++] = e;
	}
	PRINT_DEBUG( " END \n")
}

bool	checkConfig( void )
{
unsigned char	theString[] = "Yves Huguenin\nCase postale 276\nCH-1814 La Tour-de-Peilz\nSwitzerland\nmailto:yves.huguenin@dreampitbikes.ch\nhttp://www.tricala.org/";
FILE			*theFile	= NULL;
unsigned char	buffer[1024];

#define			TOLKIEN 	" =\"'\n\r"

	try
	{
		theFile = fopen( conf, "r");
		if( theFile == 0 )
			return false;
		memset( &buffer, 0, sizeof( buffer ));
		PRINT_DEBUG(" FEOF state %d\n", feof( theFile ) );
		while( fgets( (char *)&buffer, sizeof( buffer ) -20, theFile ))
		{
		char *str = strtok( (char *)&buffer,TOLKIEN);
		
			while( str )
			{
printf(" Token: %s\n",str );

				if( !strcasecmp("LIC",(char *)&buffer ) )
				{
				unsigned short	i=0;
				
					str = strtok( NULL, TOLKIEN);

printf("\nLic found: %s\n", str );

					while( ( i < sizeof( theString ) ) && ( *str != 0 ) )
					{
						unsigned char	c = 0;

						sscanf( str, "%2hhx",&c );
						theString[i++] = c;
						str +=2;
					}

					if( !memcmp( theString, tocrypt, sizeof( tocrypt ) ) )
					{
						fclose( theFile );
						return true;
					}
					else
					{
						str = NULL;
					}
				}
				else
				{
					str = strtok( NULL, TOLKIEN);
				}
			}
		}
		fclose( theFile );
		return false;
	}
	catch( std::exception &e )
	{
		return false;
	}
#undef TOLKIEN
}

#ifdef WITH_MAIN
int	main( int argc, char **argv)
{
FILE			*theFile	= NULL;
unsigned long	cpuid[4];

	if( argc <= 2 )
		if( argv[1][1]!='c' )
		{
			printf("Generating Licence for CPU ID: ");

			{
			unsigned char	buffer[256];
			FILE			*in;

				in = fopen("/proc/cpuid","r");

				if( in )
				{
					while( fgets( (char *)&buffer, sizeof( buffer ) -10, in ) )
						if( ( buffer[0] == 'I' ) && ( buffer[1] =='D' ))
						{
						char	*str = (char *)&buffer[3];

							sscanf( str, "%8lx%8lx%8lx%8lx", &cpuid[0],&cpuid[1],&cpuid[2],&cpuid[3] );
							printf("%s",str);
							break;
						}
					fclose( in );
				}
				else
				{
					printf("CANNOT GET CPUID !!!! \n");
					return -1;
				}
			}
			printf("\n");
			encode( cpuid[0],cpuid[1],cpuid[2],cpuid[3] );
			theFile = fopen( conf, "a");
			if( !theFile )
			{
				return -2;
			}
			fprintf( theFile,"LIC=");
			printf( "LIC=" );
			unsigned short i = 0;

			while( i < sizeof( tocrypt ) )
			{
				fprintf( theFile,"%2.2hhX",tocrypt[i] );
				printf( "%2.2hhX",tocrypt[i] );
				i++;
			}
			fprintf( theFile,"\n" );
			printf( "\n" );
			fclose( theFile );
		}
	printf("Checking licence file:");
	if( checkConfig() )
		printf(" ALL GOOD\n");
	else
		printf("ERROR BAD LICENCE!!!!\n");
	return 0;
}
#endif