# Zappi_API H1
## Zappi_API H2
### This is not ready yet H3

This is built from reading 
[Ashley Pittman's python code](https://github.com/ashleypittman/mec)
for the Myenergi API (complete with all of it's quirks)

The detail for the JSON returned by the Myenergi API is documented in some detail in [Andy's JSON decoding](https://github.com/twonk/MyEnergi-App-Api)

Needs installation instructions.

Needs Zappi_test changed to a permanent table name.

Needs #define for where /home/pi/.zappi.cfg goes.

Needs code in cgi program to read config file.
Needs #define in cgi program for DB config in /etc.

Needs details of running the getup configuration program
./getup -u 10008888 -p password -h 192.168.3.14 -d EV -s DB_username -q DB_password

./getup --username 10008888 --password XXX --sqlhost raspberrypi.local --sqldbase XXX --sqluser XXX --sqlpwd XXX
mv ~/.zappi.cfg to /usr/local/etc/zappi.cfg for the CGI program and chown www-data /usr/local/etc/zappi.cfg 

crontab example needed
    # min hour dom month dow command
    59 * * * * sleep 30;/home/pi_d/c/Zappi_API/zparse

 ./zparse --dmy 04/10/2020

 --iso yyyy/mm/dd or -i yyyy/mm/dd
 --eur dd/mm/yyyy or -e dd/mm/yyyy
 --usa mm/dd/yyyy or -u mm/dd/yyyy
 --ymd (same as --iso) yyyy/mm/dd
 --dmy (same as --eur) dd/mm/yyyy
 --mdy (same as --usa) mm/dd/yyyy

Copyright © Dougie Lawson, 2020, All rights reserved
