This is not ready yet.


Needs installation instructions.



Needs Zappi_test changed to a permanent table name.

Needs #define for where /home/pi/.zappi.cfg goes.

Needds code in cgi program to read config file.
Needs #define in cgi program for DB config in /etc.


./getup -u 10008888 -p password -h 192.168.3.14 -d EV -s DB_username -q DB_password

./getup --username 10008888 --password XXX --sqlhost raspberrypi.local --sqldbase XXX --sqluser XXX --sqlpwd XXX

# min hour dom month dow command
59 * * * * sleep 30; /home/pi_d/c/Zappi_API/zparse 


 
Copyright © Dougie Lawson, 2020, All rights reserved 
