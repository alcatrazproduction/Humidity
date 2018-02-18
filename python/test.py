#!/usr/bin/env python
from time import sleep
import fondarex

## The openSPI() function is where the SPI interface is configured. There are
##   three possible configuration options, and they all expect integer values:
##   speed - the clock speed in Hz
##   mode - the SPI mode (0, 1, 2, 3)
##   bits - the length of each word, in bits (defaults to 8, which is standard)
## It is also possible to pass a device name, but the default, spidev0.0, is
##   the only device currently supported by the pcDuino.

#status = fondarex.openSPI(speed=500000, mode=0)
status = fondarex.MCPbegin( 11 )

print "    "
##print "SPI configuration = ", status
##print "MCP Error         = ", fondarex.MCPcheckError()
##print "MCP CanId         = ", fondarex.MCPgetCanId()
## Data is sent as a tuple, so you can construct a tuple as long as you want 
##   and the result will come back as a tuple of the same length.
print "Module Info               = ", fondarex.getModuleInfo(0x20)
 
for x  in range(255):
##	print "SPI configuration = ",fondarex.MCPbegin( x )
##	sleep( 0.5 )
	print "Set Ir TX Level   = ", fondarex.setIrTxStatus(0x20,  x, x  )
	print "MCP Error         = ", fondarex.MCPcheckError()
	print "MCP CanId         = ", fondarex.MCPgetCanId()
	sleep( 0.1 )
	value = fondarex.getIrRxStatus(0x20 )
	sleep(0.05)
        print "Ir RX STATUS      = ", value, " ==> ",fondarex.getIrTxStatus(0x20) 
        print "MCP Error         = ", fondarex.MCPcheckError()
        print "MCP CanId         = ", fondarex.MCPgetCanId()
        if ( value[1] > 3000 ) & ( x > 0 ):
		print "found calibration at ", x
		fondarex.MCPReset()
		exit()
##	sleep( 1 )
#	fondarex.MCPbegin( 11 )
##	dat = spi.transfer((0x03,x,0))
##	print " reg 0x%X: %X" % ( x, dat[1])
## Finally, close the SPI connection. This is probably not necessary but it's
##   good practice.
##dat = fondarex.transfer((0xC0,0xC0,0xC0))
##      print " reg 0x%X: %X" % ( x, dat[1])
fondarex.MCPReset()
