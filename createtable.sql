CREATE TABLE IF NOT EXISTS `Zappi_test` (
  `datetime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `imported` float NOT NULL,
  `exported` float NOT NULL,
  `genplus` float NOT NULL,
  `genminus` float NOT NULL,
  `zappiimp` float NOT NULL,
  `zappidiv` float NOT NULL,
  `voltage` float NOT NULL,
  `frequency` float NOT NULL,
  `pect1` float NOT NULL,
  `nect1` float NOT NULL,
  `pect2` float NOT NULL,
  `nect2` float NOT NULL,
  `pect3` float DEFAULT NULL,
  `nect3` float DEFAULT NULL,
  `ctsums` float NOT NULL,
  `house` float NOT NULL,
  PRIMARY KEY (`datetime`)
) ENGINE=Aria ;

ALTER TABLE Zappi_test ADD UNIQUE(datetime);
