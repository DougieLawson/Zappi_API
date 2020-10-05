# Zappi_API


This is built from reading 
[Ashley Pittman's python code](https://github.com/ashleypittman/mec)
for the Myenergi API (complete with all of it's quirks)

The detail for the JSON returned by the Myenergi API is documented in some detail in [Andy's JSON decoding](https://github.com/twonk/MyEnergi-App-Api)

### Todos
Needs Zappi_test changed to a permanent table name.

/home/pi_d/c needs to be /home/pi. 

## Installation

You'll need the `json-c`, `config` and `cmake` packages along with the usual build-essentials.

Build the code with

    git clone https://github.com/DougieLawson/Zappi_API
    cd Zappi_API
    cmake .
    make

Build a configuration file with the `getup` program 

`./getup -u 10008888 -p password -h 192.168.3.14 -d EV -s DB_username -q DB_password`

Each option for getup also has a long name like `./getup --username 10008888 --password XXX --sqlhost raspberrypi.local --sqldbase XXX --sqluser XXX --sqlpwd XXX`

- `-u` or `--username` is your Hub serial number
- `-p` or `--password` is the password for your hub
- `-h` or `--sqlhost` is the server where your MariaDB system runs (use `-h 127.0.0.1` if it runs on your local system)
- `-d` or `--sqldbase` is your MariaDB database name
- `-s` or `--sqluser` is your MariaDB userid
- `-q` or `--sqlpwd` is your MariaDB password

Once the program has created `~/pi/.zappi.cfg` you need to make that available for your web server CGI program. Use `sudo mv ~/.zappi.cfg /usr/local/etc/zappi.cfg` and `sudo chown www-data /usr/local/etc/zappi.cfg` to make it available. 

The data collection program can be run from a crontab line. The example will run the program every hour from 00:59 until 23:59. The zparse program waits for three minutes after it's started to ensure it has the data from the whole hour. 

    # min hour dom month dow command
    59 * * * * sleep 30;/home/pi/Zappi_API/zparse

To collect data for a specific date run `zparse` with an extra parm. 

 `./zparse --dmy 04/10/2020`

- `--iso yyyy/mm/dd` or `-i yyyy/mm/dd` for ISO/Japanese date formats
- `--eur dd/mm/yyyy` or `-e dd/mm/yyyy` for European dates
- `--usa mm/dd/yyyy` or `-u mm/dd/yyyy` for USA dates
- `--ymd` (same as `--iso yyyy/mm/dd`)
- `--dmy` (same as `--eur dd/mm/yyyy`) 
- `--mdy` (same as `--usa mm/dd/yyyy`)

Copyright © Dougie Lawson, 2020, All rights reserved
