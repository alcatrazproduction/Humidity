/********************************************************************************************************
 * Schema for the database on the linux part of the arduino Yun
 * $Author	: 	Yves Huguenin
 * $Created	: 	07.10.2013
 * $Version	:	1.1.1
 * $Revision: 	26.05.2014, added vaccum value to channel
 * $Revision:	10.2.2015, added sample_time, delay from T2 in msec
 *
 *******************************************************************************************************/

use fondarexV2;

DROP TABLE IF EXISTS T_Injection;
CREATE TABLE T_Injection
(
	id				bigint			PRIMARY KEY NOT NULL AUTO_INCREMENT,
	part_name		varchar(80),
	prod_num		int,
	injection		int,
	timecode		datetime,
	t1				int,
	t2				int,
	t3				int,
	cal_a			int,
	cal_b			int,
	cal_c			int,
	cal_d			int,
	hv_injection	bigint
);

DROP TABLE IF EXISTS T_Humidity_Value;
CREATE TABLE T_Humidity_Value
(
	id				bigint			PRIMARY KEY NOT NULL AUTO_INCREMENT,
	injection_id	bigint			NOT NULL,
	channel_a		int,
	channel_b		int,
	channel_c		int,
	channel_d		int,
	temp_a			int,
	temp_b			int,
	temp_c			int,
	temp_d			int,
	vaccum_a		int,
	vaccum_b		int,
	vaccum_c		int,
	vaccum_d		int,
	hv_vaccum_a		int,
	hv_vaccum_b		int,
	hv_vaccum_c		int,
	hv_vaccum_d		int,
	sample_time		int
);

DROP TABLE IF EXISTS T_Channel_Info;
CREATE TABLE T_Channel_Info
(
	id 					bigint 			PRIMARY KEY NOT NULL AUTO_INCREMENT,
	injection_id 		bigint 			NOT NULL,
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

/********************* Some procedure for creating Demo datas ******************************************************************/

DROP PROCEDURE IF EXISTS DemoInjection;
DROP PROCEDURE IF EXISTS DemoData;
