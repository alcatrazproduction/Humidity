##
## LINUX barebone makefile for c++ examples : good for all platforms
##
## Simply run make or make clean
##
## Intend this makefile only as a "batch examples updater" after library modification.
##
##	sources 	= ['src/spi.cpp','src/mcp_can.cpp','src/solutec.cpp','src/global.cpp','src/interrupt.cpp','src/s7_server.cpp'],
##	include_dirs 	= ['Arduino/hardware/arduino/cores/arduino/','Arduino/hardware/arduino/variants/sunxi/','Arduino/libraries/SPI/','include/'],
##	libraries	= ['arduino'],
##	library_dirs	= ['/usr/lib/'],

headers		:= 	include/solutec.h 	include/humidity.h 	include/globals.h 	include/mcp_can.h 	\
			include/mcp_can_dfs.h	include/s7_server.h 	include/snap7.h 	include/libdb.h 	\
			include/CAN_MCP2515.h 	include/CAN.h		include/s7_client.h 	include/st7036.h
		   
headers_small	:= 	include/humidity.h 	include/st7036.h

Libs		:= 	-lsnap7  		-lmysqlcppconn 		-lmysqlpp
Wrapper		:= 	snap7.cpp
Include_Dirs	:= 	-IArduino/hardware/arduino/pcduino/cores/arduino/ \
			-IArduino/hardware/arduino/pcduino/variants/sunxi/ \
			-IArduino/hardware/arduino/pcduino/libraries/SPI/ \
			-IArduino/hardware/arduino/pcduino/libraries/Wire/ \
			-Iinclude/ \
			-I/usr/include/python2.7/ 


CXX		:= 	g++
CC		:= 	gcc
## -DDEBUG=2
CXXGFLAGS	:=	-O3 -pthread -fno-strict-aliasing -DPCDUINO -g -fwrapv -Wall -fPIC $(Include_Dirs) \
			-DPRINTF=fprintf
CXXFLAGS 	:= 	-DDEBUG=0 $(CXXGFLAGS) 
CXXFLAGSDEBUG 	:= 	-DDEBUG=2 $(CXXGFLAGS)
CXXLINK		:= 	-pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro
LIBLINKDIR	:= 	-L/usr/lib/ 		-L/usr/lib/mysql
CFLAGS   	:=
PCDUINO_DIR	:= 	Arduino/hardware/arduino/pcduino/cores/arduino/
PCDUINO_LDIR	:= 	Arduino/hardware/arduino/pcduino/libraries/
SRC_DIR		:= 	src/
PYTHON_DIR	:= 	python/
BUILD_DIR_LIB	:= 	build/lib.linux-armv7l-2.7/
BUILD_DIR_OBJ	:= 	build/object/
BUILD_DIR_DOBJ	:= 	build/object/debug/
PCDUINO_OBJ	:= 	build/object/pcduino/
BUILD_DIR_BIN	:= 	build/bin/

Sources_PCDuino	:=	$(PCDUINO_DIR)Print.cpp 	$(PCDUINO_DIR)Serial.cpp 	$(PCDUINO_DIR)Stream.cpp	\
			$(PCDUINO_DIR)Tone.cpp 		$(PCDUINO_DIR)WInterrupts.c 	$(PCDUINO_DIR)WMath.cpp 	\
			$(PCDUINO_DIR)WString.cpp 	$(PCDUINO_DIR)platform.c	$(PCDUINO_DIR)wiring.c 		\
			$(PCDUINO_DIR)wiring_analog.c 	$(PCDUINO_DIR)wiring_digital.c	$(PCDUINO_DIR)wiring_pulse.c 	\
			$(PCDUINO_DIR)wiring_shift.c	$(PCDUINO_LDIR)SPI/SPI.cpp	$(PCDUINO_LDIR)Wire/Wire.cpp
			
Sources_Lib	:=	$(SRC_DIR)spi.cpp 		$(SRC_DIR)mcp_can.cpp 		$(SRC_DIR)solutec.cpp 		\
			$(SRC_DIR)interrupt.cpp 	$(SRC_DIR)global.cpp 		$(SRC_DIR)s7_server.cpp 	\
			$(SRC_DIR)libdb.cpp 		$(SRC_DIR)CAN_MCP2515.cpp	$(SRC_DIR)CAN.cpp 		\
			$(SRC_DIR)s7_client.cpp 	$(SRC_DIR)st7036.cpp		$(SRC_DIR)profinet.cpp

Old_Objects	:= 	$(BUILD_DIR_OBJ)mcp_can.o 	$(BUILD_DIR_OBJ)solutec.o 	$(BUILD_DIR_OBJ)CAN_MCP2515.o	\
			$(BUILD_DIR_OBJ)interrupt.o 	$(BUILD_DIR_OBJ)global.o 	$(BUILD_DIR_OBJ)s7_server.o 	\
			$(BUILD_DIR_OBJ)main.o 		$(BUILD_DIR_OBJ)libdb.o 	$(BUILD_DIR_OBJ)snap7.o
			
			
Objects		:= 	$(BUILD_DIR_OBJ)solutec.o 	$(BUILD_DIR_OBJ)CAN.o		$(BUILD_DIR_OBJ)interrupt.o 	\
			$(BUILD_DIR_OBJ)global.o 	$(BUILD_DIR_OBJ)s7_server.o 	$(BUILD_DIR_OBJ)main.o 		\
			$(BUILD_DIR_OBJ)libdb.o 	$(BUILD_DIR_OBJ)snap7.o 	$(BUILD_DIR_OBJ)CAN_MCP2515.o	\
			$(BUILD_DIR_OBJ)s7_client.o 	$(BUILD_DIR_OBJ)st7036.o	$(BUILD_DIR_OBJ)profinet.o

ObjectsDebug	:= 	$(BUILD_DIR_DOBJ)solutec.o 	$(BUILD_DIR_DOBJ)CAN.o		$(BUILD_DIR_DOBJ)interrupt.o 	\
			$(BUILD_DIR_DOBJ)global.o 	$(BUILD_DIR_DOBJ)s7_server.o 	$(BUILD_DIR_DOBJ)main.o 	\
			$(BUILD_DIR_DOBJ)libdb.o 	$(BUILD_DIR_DOBJ)snap7.o 	$(BUILD_DIR_DOBJ)CAN_MCP2515.o	\
			$(BUILD_DIR_DOBJ)s7_client.o 	$(BUILD_DIR_DOBJ)st7036.o	$(BUILD_DIR_DOBJ)profinet.o

Objects_PCDuino	:=	$(PCDUINO_OBJ)Print.o	 	$(PCDUINO_OBJ)Serial.o	 	$(PCDUINO_OBJ)Stream.o		\
			$(PCDUINO_OBJ)Tone.o 		$(PCDUINO_OBJ)WInterrupts.o 	$(PCDUINO_OBJ)WMath.o 		\
			$(PCDUINO_OBJ)WString.o 	$(PCDUINO_OBJ)platform.o	$(PCDUINO_OBJ)wiring.o 		\
			$(PCDUINO_OBJ)wiring_analog.o 	$(PCDUINO_OBJ)wiring_digital.o	$(PCDUINO_OBJ)wiring_pulse.o 	\
			$(PCDUINO_OBJ)wiring_shift.o	$(PCDUINO_OBJ)SPI.o		$(PCDUINO_OBJ)Wire.o

Objects_small	:=	$(BUILD_DIR_OBJ)lcd_main.o 	$(BUILD_DIR_OBJ)st7036.o
			
.PHONY: all clean lib Humidity Humidity_debug lcd_display prepareOS

$(BUILD_DIR_BIN)Humidity:	$(Objects)		$(headers)	 $(Objects_PCDuino)
	$(CXX) -pthread  -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro \
		$(Objects) $(Objects_PCDuino) $(LIBLINKDIR) $(Libs) -o $(BUILD_DIR_BIN)Humidity

$(BUILD_DIR_BIN)Humidity_debug:	$(ObjectsDebug)		$(headers)	 $(Objects_PCDuino)
	$(CXX) -pthread  -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro \
		$(ObjectsDebug) $(Objects_PCDuino) $(LIBLINKDIR) $(Libs) -o $(BUILD_DIR_BIN)Humidity_debug


$(BUILD_DIR_LIB)fondarex.so:	$(Sources_Lib)		$(headers)	 $(Objects_PCDuino)
	python $(PYTHON_DIR)setup.py install

$(BUILD_DIR_BIN)lcd_display:	$(Objects_small)	$(headers_small) $(Objects_PCDuino)
	$(CXX) -pthread  -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro \
	$(Objects_small) $(Objects_PCDuino) $(LIBLINKDIR) $(Libs) -o $(BUILD_DIR_BIN)lcd_display

## Object to build application and libraries

$(BUILD_DIR_OBJ)CAN_MCP2515.o: 	$(SRC_DIR)CAN_MCP2515.cpp 	$(headers)
	$(CXX) -c $(SRC_DIR)CAN_MCP2515.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)CAN_MCP2515.o

$(BUILD_DIR_OBJ)CAN.o: 	 	$(SRC_DIR)CAN.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)CAN.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)CAN.o
	
$(BUILD_DIR_OBJ)spi.o: 	 	$(SRC_DIR)spi.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)spi.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)spi.o
	
$(BUILD_DIR_OBJ)mcp_can.o: 	$(SRC_DIR)mcp_can.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)mcp_can.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)mcp_can.o
	
$(BUILD_DIR_OBJ)solutec.o: 	$(SRC_DIR)solutec.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)solutec.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)solutec.o
	
$(BUILD_DIR_OBJ)interrupt.o: 	$(SRC_DIR)interrupt.cpp		$(headers)
	$(CXX) -c $(SRC_DIR)interrupt.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)interrupt.o
	
$(BUILD_DIR_OBJ)global.o: 	$(SRC_DIR)global.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)global.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)global.o
	
$(BUILD_DIR_OBJ)s7_server.o: 	$(SRC_DIR)s7_server.cpp		$(headers)
	$(CXX) -c $(SRC_DIR)s7_server.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)s7_server.o

$(BUILD_DIR_OBJ)s7_client.o: 	$(SRC_DIR)s7_client.cpp		$(headers)
	$(CXX) -c $(SRC_DIR)s7_client.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)s7_client.o

$(BUILD_DIR_OBJ)libdb.o: 	$(SRC_DIR)libdb.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)libdb.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)libdb.o

$(BUILD_DIR_OBJ)main.o: 	$(SRC_DIR)main.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)main.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)main.o

$(BUILD_DIR_OBJ)snap7.o: 	$(SRC_DIR)$(Wrapper) 		$(headers)
	$(CXX) -c $(SRC_DIR)$(Wrapper) 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)snap7.o

$(BUILD_DIR_OBJ)st7036.o: 	$(SRC_DIR)st7036.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)st7036.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)st7036.o
	
$(BUILD_DIR_OBJ)lcd_main.o: 	$(SRC_DIR)lcd_main.cpp 		$(headers_small)
	$(CXX) -c $(SRC_DIR)lcd_main.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)lcd_main.o
	
$(BUILD_DIR_OBJ)profinet.o: 	$(SRC_DIR)profinet.cpp 		$(headers_small)
	$(CXX) -c $(SRC_DIR)profinet.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)profinet.o
	
## Object to build application and libraries (DEBUG Version )

$(BUILD_DIR_DOBJ)CAN_MCP2515.o: 	$(SRC_DIR)CAN_MCP2515.cpp 	$(headers)
	$(CXX) -c $(SRC_DIR)CAN_MCP2515.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)CAN_MCP2515.o

$(BUILD_DIR_DOBJ)CAN.o: 	 	$(SRC_DIR)CAN.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)CAN.cpp 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)CAN.o

$(BUILD_DIR_DOBJ)spi.o: 	 	$(SRC_DIR)spi.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)spi.cpp 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)spi.o

$(BUILD_DIR_DOBJ)mcp_can.o: 	$(SRC_DIR)mcp_can.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)mcp_can.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)mcp_can.o

$(BUILD_DIR_DOBJ)solutec.o: 	$(SRC_DIR)solutec.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)solutec.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)solutec.o

$(BUILD_DIR_DOBJ)interrupt.o: 	$(SRC_DIR)interrupt.cpp		$(headers)
	$(CXX) -c $(SRC_DIR)interrupt.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)interrupt.o

$(BUILD_DIR_DOBJ)global.o: 	$(SRC_DIR)global.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)global.cpp 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)global.o

$(BUILD_DIR_DOBJ)s7_server.o: 	$(SRC_DIR)s7_server.cpp		$(headers)
	$(CXX) -c $(SRC_DIR)s7_server.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)s7_server.o

$(BUILD_DIR_DOBJ)s7_client.o: 	$(SRC_DIR)s7_client.cpp		$(headers)
	$(CXX) -c $(SRC_DIR)s7_client.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)s7_client.o

$(BUILD_DIR_DOBJ)libdb.o: 	$(SRC_DIR)libdb.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)libdb.cpp 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)libdb.o

$(BUILD_DIR_DOBJ)main.o: 	$(SRC_DIR)main.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)main.cpp 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)main.o

$(BUILD_DIR_DOBJ)snap7.o: 	$(SRC_DIR)$(Wrapper) 		$(headers)
	$(CXX) -c $(SRC_DIR)$(Wrapper) 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)snap7.o

$(BUILD_DIR_DOBJ)st7036.o: 	$(SRC_DIR)st7036.cpp 		$(headers)
	$(CXX) -c $(SRC_DIR)st7036.cpp 			$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)st7036.o

$(BUILD_DIR_DOBJ)lcd_main.o: 	$(SRC_DIR)lcd_main.cpp 		$(headers_small)
	$(CXX) -c $(SRC_DIR)lcd_main.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)lcd_main.o

$(BUILD_DIR_DOBJ)profinet.o: 	$(SRC_DIR)profinet.cpp 		$(headers_small)
	$(CXX) -c $(SRC_DIR)profinet.cpp 		$(CXXFLAGSDEBUG) -o $(BUILD_DIR_DOBJ)profinet.o

## .o For pcduino libs

$(PCDUINO_OBJ)Print.o:		$(PCDUINO_DIR)Print.cpp
	$(CXX) -c $(PCDUINO_DIR)Print.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)Print.o
	
$(PCDUINO_OBJ)Serial.o:		$(PCDUINO_DIR)Serial.cpp
	$(CXX) -c $(PCDUINO_DIR)Serial.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)Serial.o
	
$(PCDUINO_OBJ)Stream.o:		$(PCDUINO_DIR)Stream.cpp
	$(CXX) -c $(PCDUINO_DIR)Stream.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)Stream.o
	
$(PCDUINO_OBJ)Tone.o:		$(PCDUINO_DIR)Tone.cpp
	$(CXX) -c $(PCDUINO_DIR)Tone.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)Tone.o
	
$(PCDUINO_OBJ)WInterrupts.o:	$(PCDUINO_DIR)WInterrupts.c
	$(CXX) -c $(PCDUINO_DIR)WInterrupts.c 		$(CXXFLAGS) -o $(PCDUINO_OBJ)WInterrupts.o
	
$(PCDUINO_OBJ)WMath.o:		$(PCDUINO_DIR)WMath.cpp 
	$(CXX) -c $(PCDUINO_DIR)WMath.cpp  		$(CXXFLAGS) -o $(PCDUINO_OBJ)WMath.o
	
$(PCDUINO_OBJ)WString.o:	$(PCDUINO_DIR)WString.cpp
	$(CXX) -c $(PCDUINO_DIR)WString.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)WString.o
	
$(PCDUINO_OBJ)platform.o:	$(PCDUINO_DIR)platform.c
	$(CXX) -c $(PCDUINO_DIR)platform.c 		$(CXXFLAGS) -o $(PCDUINO_OBJ)platform.o
	
$(PCDUINO_OBJ)wiring.o:		$(PCDUINO_DIR)wiring.c 		
	$(CXX) -c $(PCDUINO_DIR)wiring.c 		$(CXXFLAGS) -o $(PCDUINO_OBJ)wiring.o
	
$(PCDUINO_OBJ)wiring_analog.o:	$(PCDUINO_DIR)wiring_analog.c
	$(CXX) -c $(PCDUINO_DIR)wiring_analog.c 	$(CXXFLAGS) -o $(PCDUINO_OBJ)wiring_analog.o
	
$(PCDUINO_OBJ)wiring_digital.o:	$(PCDUINO_DIR)wiring_digital.c
	$(CXX) -c $(PCDUINO_DIR)wiring_digital.c	$(CXXFLAGS) -o $(PCDUINO_OBJ)wiring_digital.o
	
$(PCDUINO_OBJ)wiring_pulse.o:	$(PCDUINO_DIR)wiring_pulse.c 	
	$(CXX) -c $(PCDUINO_DIR)wiring_pulse.c 		$(CXXFLAGS) -o $(PCDUINO_OBJ)wiring_pulse.o
	
$(PCDUINO_OBJ)wiring_shift.o:	$(PCDUINO_DIR)wiring_shift.c
	$(CXX) -c $(PCDUINO_DIR)wiring_shift.c 		$(CXXFLAGS) -o $(PCDUINO_OBJ)wiring_shift.o

$(PCDUINO_OBJ)SPI.o:	$(PCDUINO_LDIR)SPI/SPI.cpp
	$(CXX) -c $(PCDUINO_LDIR)SPI/SPI.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)SPI.o

$(PCDUINO_OBJ)Wire.o:	$(PCDUINO_LDIR)Wire/Wire.cpp
	$(CXX) -c $(PCDUINO_LDIR)Wire/Wire.cpp 		$(CXXFLAGS) -o $(PCDUINO_OBJ)Wire.o

## .o file definition, to be build individualy

CAN_MCP2515.o: 	$(SRC_DIR)CAN_MCP2515.cpp 			$(headers)
	$(CXX) -c $(SRC_DIR)CAN_MCP2515.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)CAN_MCP2515.o

CAN.o: 	 	$(SRC_DIR)CAN.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)CAN.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)CAN.o

spi.o: 	 	$(SRC_DIR)spi.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)spi.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)spi.o

mcp_can.o: 	$(SRC_DIR)mcp_can.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)mcp_can.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)mcp_can.o

solutec.o: 	$(SRC_DIR)solutec.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)solutec.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)solutec.o

interrupt.o: 	$(SRC_DIR)interrupt.cpp				$(headers)
	$(CXX) -c $(SRC_DIR)interrupt.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)interrupt.o

global.o: 	$(SRC_DIR)global.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)global.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)global.o

s7_server.o: 	$(SRC_DIR)s7_server.cpp				$(headers)
	$(CXX) -c $(SRC_DIR)s7_server.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)s7_server.o

libdb.o: 	$(SRC_DIR)libdb.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)libdb.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)libdb.o

main.o: 	$(SRC_DIR)main.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)main.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)main.o

snap7.o: 	$(SRC_DIR)$(Wrapper) 				$(headers)
	$(CXX) -c $(SRC_DIR)$(Wrapper) 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)snap7.o

st7036.o: 	$(SRC_DIR)st7036.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)st7036.cpp 			$(CXXFLAGS) -o $(BUILD_DIR_OBJ)st7036.o

lcd_main.o: 	$(SRC_DIR)lcd_main.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)lcd_main.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)lcd_main.o
	
profinet.o: 	$(SRC_DIR)profinet.cpp 				$(headers)
	$(CXX) -c $(SRC_DIR)profinet.cpp 		$(CXXFLAGS) -o $(BUILD_DIR_OBJ)profinet.o 
	
all:		$(Objects)  Humidity Humidity_debug lcd_display

clean:
		rm  $(BUILD_DIR_BIN)Humidity $(BUILD_DIR_BIN)Humidity_debug $(BUILD_DIR_BIN)lcd_display

lib: 		$(BUILD_DIR_LIB)fondarex.so 	$(headers) 		$(Sources_Lib) 	 $(Objects_PCDuino)

Humidity: 	$(BUILD_DIR_BIN)Humidity 	$(headers) 		$(Objects) 	 $(Objects_PCDuino)

Humidity_debug: $(BUILD_DIR_BIN)Humidity_debug 	$(headers) 		$(ObjectsDebug)	 $(Objects_PCDuino)

lcd_display: 	$(BUILD_DIR_BIN)lcd_display 	$(headers_small) 	$(Objects_small) $(Objects_PCDuino)

license:	$(SRC_DIR)profinet.cpp 				$(headers)
	$(CXX)  $(SRC_DIR)profinet.cpp $(CXXFLAGS) -DWITH_MAIN -pthread  -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro \
		 $(LIBLINKDIR) $(Libs) -o license
		 
rtc:		$(SRC_DIR)rtc.cpp 				$(headers)
		$(CXX)  $(SRC_DIR)rtc.cpp $(CXXFLAGS)  -pthread  -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro \
		 $(LIBLINKDIR) -o rtc

install:
		cp $(BUILD_DIR_BIN)Humidity /usr/bin/
		
prepareOS:
	apt-get update
	apt-get remove vin-tiny xorg* network-manager x11-*
	apt-get autoremove
	apt-get install vim nfs-kernel-server mysql-server mysql-client libmysql++-dev php5-mysqlnd php5-gd \
			libapache2-mod-php5filter php-auth
