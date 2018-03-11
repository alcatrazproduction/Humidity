#!/usr/bin/python
#
#  (c) FONDAREX SA
#  $Author:       Yves Huguenin
#  $Title:        humidity_srv.py
#  $Create Date:  23.09.2014
#  $Vers:         1.0a1
#
#  1.0a1:
#	Manage all the interface between Siemens and the mysql database
#	actualy with the httpSiemens protocole
#


# import all the modules we need
import socket
import sys, getopt, os

TCP_IP = '0.0.0.0'
TCP_PORT = 20000
BUFFER_SIZE = 1024
MESSAGE = "Hello, World!"


if __name__ == "__main__":

	try:

		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.bind((TCP_IP, TCP_PORT))
		while 1:
			s.listen(1)

			conn, addr = s.accept()
			print 'Connection address:', addr
			while 1:
				data = conn.recv(BUFFER_SIZE)
				if not data: break
					print "received data:", data
				conn.send(data)  # echo
	finally:
		pass

