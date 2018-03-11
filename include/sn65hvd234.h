/*
  Copyright (c) 2013 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* Copyright (C) 2014

   Contributors:  Pedro Cevallos & Neil McNeight

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Acknowledgements:
Fabian Greif for the initial MCP2515 library
  http://www.kreatives-chaos.com/artikel/universelle-can-bibliothek
  as well as his updates at https://github.com/dergraaf/avr-can-lib
David Harding for his version of the MCP2515 library
  http://forum.arduino.cc/index.php/topic,8730.0.html
Kyle Crockett CANduino library with 16Mhz oscillator
  http://code.google.com/p/canduino/
Nuno Alves for the help on Extended ID messaging
Stevenh for his work on library and all of the MCP research/work
  http://modelrail.otenko.com/arduino/arduino-controller-area-network-can
Collin Kidder (collin80) for his work on the Arduino Due CAN interface
  https://github.com/collin80/due_can
Daniel Kasamis (togglebit) both for his code at
  https://github.com/togglebit/ArduinoDUE_OBD_FreeRunningCAN as well as his
  DUE CANshield http://togglebit.net/product/arduino-due-can-shield/
Cory Fowler (coryjfowler) for 16 MHz bitrate timing information
  https://github.com/coryjfowler/MCP2515_lib
teachop for the FlexCAN library for the Teensy 3.1
  https://github.com/teachop/FlexCAN_Library

-------------------------------------------------------------------------------------------------------------
Change Log

DATE		VER		WHO			WHAT
07/07/13	0.1		PC		Modified and merge all MCP2515 libraries found. Stripped away most unused functions and corrected MCP2515 defs
09/12/13	0.2		PC		Added selectable CS SPI for CAN controller to use 1 IC to control several mcp2515
02/05/14	0.3		PC		Added filter and mask controls
05/01/14	0.4		PC		Cleaned up functions, variables and added message structures for J1939, CANopen and CAN.
05/07/14	1.0		PC		Released Library to the public through GitHub
06/18/14  1.9   NEM   Preparing a unified CAN library across three different CAN controllers
-------------------------------------------------------------------------------------------------------------

*/

#if defined(ARDUINO_ARCH_SAM)

/**
  *  \file
  *
  *  Include Defines & macros for the SN65HVD234.
  */

#ifndef _CAN_SN65HVD234_
#define _CAN_SN65HVD234_

#include "variant.h"

class SSN65HVD234
{
  private:
    /** Rs Pin on PIO */
    uint32_t dwPin_Rs ;

    /** EN Pin on PIO */
    uint32_t dwPin_EN ;
  public:
    uint32_t Init() ;
    uint32_t SetRs(uint32_t Rs ) ;
    uint32_t SetEN(uint32_t EN ) ;

    uint32_t Enable();
    uint32_t Disable();

    uint32_t EnableLowPower();
    uint32_t DisableLowPower();
    SSN65HVD234(uint32_t Rs, uint32_t En);
};

#endif // _CAN_SN65HVD234_

#endif // defined(ARDUINO_ARCH_SAM)
