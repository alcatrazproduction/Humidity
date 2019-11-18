#!/usr/bin/python
#
#  
#  $Author:       Yves Huguenin
#  $Title:        humidity_cron.py
#  $Create Date:  23.09.2014
#  $Vers:         1.0a1
#
#  1.0a1:
#	Manage all the interface between Siemens and the mysql database
#	actualy with the httpSiemens protocole
#


# import all the modules we need
import socket, time, threading, signal
import sys, getopt, os
from time import sleep

TCP_IP = 'fondsav.dnsd.info'
TCP_PORT = 20000
BUFFER_SIZE = 1024
MESSAGE = "Hello, World!"


if __name__ == "__main__":

	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((TCP_IP, TCP_PORT))
		s.send(MESSAGE)
		data = s.recv(BUFFER_SIZE)
		sleep( 10*60 )			# wait 10minutes
		s.close()

		print "received data:", data
	finally:
		pass

