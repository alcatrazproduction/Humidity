/* Old tables

T_Channel_Info:

+---------------------+------------+------+-----+---------+----------------+
| Field               | Type       | Null | Key | Default | Extra          |
+---------------------+------------+------+-----+---------+----------------+
| id                  | bigint(20) | NO   | PRI | NULL    | auto_increment |
| injection_id        | bigint(20) | NO   |     | NULL    |                |
| channel             | int(11)    | NO   |     | NULL    |                |
| sample_count        | int(11)    | YES  |     | NULL    |                |
| sample_rate         | int(11)    | YES  |     | 20      |                |
| humidity_max        | int(11)    | YES  |     | 4095    |                |
| humidity_min        | int(11)    | YES  |     | 0       |                |
| humidity_flags      | int(11)    | YES  |     | NULL    |                |
| humidity_cal        | int(11)    | YES  |     | NULL    |                |
| humidity_cal_res    | int(11)    | YES  |     | NULL    |                |
| temperature_max     | int(11)    | YES  |     | 4095    |                |
| temperature_min     | int(11)    | YES  |     | 0       |                |
| temperature_flags   | int(11)    | YES  |     | NULL    |                |
| temperature_cal     | int(11)    | YES  |     | NULL    |                |
| temperature_cal_res | int(11)    | YES  |     | NULL    |                |
| vaccum_max          | int(11)    | YES  |     | 4095    |                |
| vaccum_min          | int(11)    | YES  |     | 0       |                |
| vaccum_flags        | int(11)    | YES  |     | NULL    |                |
| vaccum_cal          | int(11)    | YES  |     | NULL    |                |
| vaccum_cal_res      | int(11)    | YES  |     | NULL    |                |
+---------------------+------------+------+-----+---------+----------------+

mysql> desc T_Globals;
+---------+----------------------------+------+-----+---------+-------+
| Field   | Type                       | Null | Key | Default | Extra |
+---------+----------------------------+------+-----+---------+-------+
| name    | varchar(30)                | NO   | PRI | NULL    |       |
| v_int   | bigint(20)                 | YES  |     | 0       |       |
| v_text  | text                       | YES  |     | NULL    |       |
| v_float | double                     | YES  |     | 0       |       |
| v_type  | enum('int','text','float') | NO   |     | text    |       |
+---------+----------------------------+------+-----+---------+-------+

mysql> desc T_Humidity_Value;
+--------------+------------+------+-----+---------+----------------+
| Field        | Type       | Null | Key | Default | Extra          |
+--------------+------------+------+-----+---------+----------------+
| id           | bigint(20) | NO   | PRI | NULL    | auto_increment |
| injection_id | bigint(20) | NO   |     | NULL    |                |
| channel_a    | int(11)    | YES  |     | NULL    |                |
| channel_b    | int(11)    | YES  |     | NULL    |                |
| channel_c    | int(11)    | YES  |     | NULL    |                |
| channel_d    | int(11)    | YES  |     | NULL    |                |
| temp_a       | int(11)    | YES  |     | NULL    |                |
| temp_b       | int(11)    | YES  |     | NULL    |                |
| temp_c       | int(11)    | YES  |     | NULL    |                |
| temp_d       | int(11)    | YES  |     | NULL    |                |
| vaccum_a     | int(11)    | YES  |     | NULL    |                |
| vaccum_b     | int(11)    | YES  |     | NULL    |                |
| vaccum_c     | int(11)    | YES  |     | NULL    |                |
| vaccum_d     | int(11)    | YES  |     | NULL    |                |
| hv_vaccum_a  | int(11)    | YES  |     | NULL    |                |
| hv_vaccum_b  | int(11)    | YES  |     | NULL    |                |
| hv_vaccum_c  | int(11)    | YES  |     | NULL    |                |
| hv_vaccum_d  | int(11)    | YES  |     | NULL    |                |
+--------------+------------+------+-----+---------+----------------+

mysql> desc T_Injection;
+--------------+-------------+------+-----+---------+----------------+
| Field        | Type        | Null | Key | Default | Extra          |
+--------------+-------------+------+-----+---------+----------------+
| id           | bigint(20)  | NO   | PRI | NULL    | auto_increment |
| part_name    | varchar(80) | YES  |     | NULL    |                |
| prod_num     | int(11)     | YES  |     | NULL    |                |
| injection    | int(11)     | YES  |     | NULL    |                |
| timecode     | datetime    | YES  |     | NULL    |                |
| t1           | int(11)     | YES  |     | NULL    |                |
| t2           | int(11)     | YES  |     | NULL    |                |
| t3           | int(11)     | YES  |     | NULL    |                |
| cal_a        | int(11)     | YES  |     | NULL    |                |
| cal_b        | int(11)     | YES  |     | NULL    |                |
| cal_c        | int(11)     | YES  |     | NULL    |                |
| cal_d        | int(11)     | YES  |     | NULL    |                |
| hv_injection | bigint(20)  | YES  |     | NULL    |                |
+--------------+-------------+------+-----+---------+----------------+
13 rows in set (0.01 sec)

desc T_Globals
+---------+----------------------------+------+-----+---------+-------+
| Field   | Type                       | Null | Key | Default | Extra |
+---------+----------------------------+------+-----+---------+-------+
| name    | varchar(30)                | NO   | PRI | NULL    |       |
| v_int   | bigint(20)                 | YES  |     | 0       |       |
| v_text  | text                       | YES  |     | NULL    |       |
| v_float | double                     | YES  |     | 0       |       |
| v_type  | enum('int','text','float') | NO   |     | text    |       |
+---------+----------------------------+------+-----+---------+-------+

*/

use HumidityV2;
/* Create the new table......
*/

DROP TABLE IF EXISTS T_Channel_Info;
CREATE TABLE T_Channel_Info
(
	id 					unsigned bigint 			PRIMARY KEY NOT NULL AUTO_INCREMENT,
	injection_id 		unsigned bigint 			NOT NULL,
	channel 			int 			NOT NULL,
	sample_count 		int,
	sample_rate 		int 			DEFAULT 20,
	humidity_max 		int 			DEFAULT 4095,
	humidity_min 		int 			DEFAULT 0,
	humidity_flags 		int,
	humidity_cal 		int,
	humidity_cal_res 	int,
	temperature_max 	int 			DEFAULT 4095,
	temperature_min 	int 			DEFAULT 0,
	temperature_flags 	int,
	temperature_cal 	int,
	temperature_cal_res int,
	vaccum_max 			int 			DEFAULT 4095,
	vaccum_min 			int 			DEFAULT 0,
	vaccum_flags 		int,
	vaccum_cal 			int,
	vaccum_cal_res 		int,
	humidity_lavg		int				DEFAULT 90,
	humidity_lstart		int				DEFAULT 100,
	humidity_lstop		int				DEFAULT 3000,

);

ALTER TABLE T_Globals ADD   quand timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP();
ALTER TABLE T_Humidity_Value ADD   sample_time int DEFAULT 0;

ALTER TABLE T_Channel_Info   ADD INDEX (injection_id);
ALTER TABLE T_Humidity_Value ADD INDEX (injection_id);
ALTER TABLE T_Injection      ADD INDEX (part_name);
ALTER TABLE T_Injection      ADD INDEX (prod_num);
ALTER TABLE T_Injection      ADD INDEX (injection);
ALTER TABLE T_Injection      ADD INDEX (timecode);
ALTER TABLE T_Injection      ADD INDEX (hv_injection);


DELIMITER $$

DROP FUNCTION IF EXISTS CreateSampleTime $$
CREATE FUNCTION CreateSampleTime( inj_id bigint )
RETURNS bool
BEGIN

DECLARE minid    unsigned bigint;
DECLARE rate     int;
DECLARE ok       int;

	SELECT sample_rate, count(*) INTO rate, ok FROM T_Channel_Info WHERE injection_id = inj_id;
	IF ok = 0 THEN
		SET rate = 20;
	END IF;
	SELECT MIN( id ), count(*) INTO minid, ok FROM T_Humidity_Value WHERE injection_id = inj_id;
	IF ok = 0 THEN
		RETURN FALSE;
	END IF;
	UPDATE T_Humidity_Value SET sample_time = ( id - minid ) * rate where  injection_id = inj_id;
	RETURN TRUE;
END;
$$

DROP FUNCTION IF EXISTS GetChannelSurface $$
CREATE FUNCTION GetChannelSurface( theInjection bigint, theChannel char, theStart int , theEnd int, thePad int   )
RETURNS int
BEGIN

DECLARE theInt 		int;
DECLARE theValue 	int DEFAULT 0;
DECLARE done 		int DEFAULT FALSE;
DECLARE i 			int DEFAULT 0;

DECLARE csra CURSOR FOR
	SELECT thePad - channel_a FROM T_Humidity_Value WHERE injection_id = theInjection AND sample_time BETWEEN theStart and theEnd;
DECLARE csrb CURSOR FOR
	SELECT thePad - channel_b FROM T_Humidity_Value WHERE injection_id = theInjection AND sample_time BETWEEN theStart and theEnd;
DECLARE csrc CURSOR FOR
	SELECT thePad - channel_c FROM T_Humidity_Value WHERE injection_id = theInjection AND sample_time BETWEEN theStart and theEnd;
DECLARE csrd CURSOR FOR
	SELECT thePad - channel_d FROM T_Humidity_Value WHERE injection_id = theInjection AND sample_time BETWEEN theStart and theEnd;

DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

	IF 	theChannel = 'A' THEN
		OPEN csra;
	ELSEIF 	theChannel = 'B' THEN
		OPEN csrb;
	ELSEIF 	theChannel = 'C' THEN
		OPEN csrc;
	ELSEIF 	theChannel = 'D' THEN
		OPEN csrd;
	ELSE
		RETURN NULL;
	END IF;

	theLoop: LOOP
		IF 	theChannel = 'A' THEN
			FETCH csra INTO theInt;
		ELSEIF 	theChannel = 'B' THEN
			FETCH csrb INTO theInt;
		ELSEIF 	theChannel = 'C' THEN
			FETCH csrc INTO theInt;
		ELSEIF 	theChannel = 'D' THEN
			FETCH csrd INTO theInt;
		END IF;
		IF done THEN
			LEAVE theLoop;
		END IF;
		SET theValue = theValue + theInt;
	END LOOP;
	IF 	theChannel = 'A' THEN
		CLOSE csra;
	ELSEIF 	theChannel = 'B' THEN
		CLOSE csrb;
	ELSEIF 	theChannel = 'C' THEN
		CLOSE csrc;
	ELSEIF 	theChannel = 'D' THEN
		CLOSE csrd;
	END IF;
	RETURN theValue;
END;
$$

DELIMITER ;


UPDATE T_Globals SET  T_Globals.name = "CURRENTINJECTION#DB001.DBD.160" WHERE T_Globals.name LIKE "ARDUINO_CURRENT_INJECTION";
UPDATE T_Globals SET  T_Globals.name = "PARTNAME#DB001.DBX.164..80" 	WHERE T_Globals.name LIKE "ARDUINO_PART_NAME";
UPDATE T_Globals SET  T_Globals.name = "PRODNUM#DB001.DBW.244" 			WHERE T_Globals.name LIKE "ARDUINO_PROD_NUM";
UPDATE T_Globals SET  T_Globals.name = "NUMCHAN#DB001.DBW.332" 			WHERE T_Globals.name LIKE "ARDUINO_NUMCHANNEL";
UPDATE T_Globals SET  T_Globals.name = "SAMPLERATE#DB001.DBW.334" 		WHERE T_Globals.name LIKE "ARDUINO_SAMPLERATE";
UPDATE T_Globals SET  T_Globals.name = "T1#DB001.DBW.246" 				WHERE T_Globals.name LIKE "ARDUINO_T1";
UPDATE T_Globals SET  T_Globals.name = "T2#DB001.DBW.248" 				WHERE T_Globals.name LIKE "ARDUINO_T2";
UPDATE T_Globals SET  T_Globals.name = "T3#DB001.DBW.250" 				WHERE T_Globals.name LIKE "ARDUINO_T3";
