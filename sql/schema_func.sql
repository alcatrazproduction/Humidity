/********************************************************************************************************
 * Schema for the database on the linux part of the arduino Yun
 * $Author	: Yves Huguenin
 * $Created	: 07.10.2013
 * $Version	: 1.0
 *
 *******************************************************************************************************/

use HumidityV2;


/********************* Some procedure for creating Demo datas ******************************************************************/

DELIMITER $$

DROP FUNCTION IF EXISTS ReadChannel $$
CREATE FUNCTION ReadChannel( theInjection unsigned bigint, theChannel char, thePage int  )
RETURNS varchar( 255 )
BEGIN 

DECLARE theInt 		int;
DECLARE theText  	varchar( 255 ) DEFAULT '';
DECLARE done 		int DEFAULT FALSE;
DECLARE i           int DEFAULT 0;

DECLARE csra CURSOR FOR
	SELECT channel_a FROM T_Humidity_Value WHERE injection_id = theInjection;
DECLARE csrb CURSOR FOR
	SELECT channel_b FROM T_Humidity_Value WHERE injection_id = theInjection;
DECLARE csrc CURSOR FOR
	SELECT channel_c FROM T_Humidity_Value WHERE injection_id = theInjection;
DECLARE csrd CURSOR FOR
	SELECT channel_d FROM T_Humidity_Value WHERE injection_id = theInjection;

DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

	SET thePage = thePage * 126;
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
		IF thePage = 0 THEN
			SET theText = CONCAT( theText, CHAR( 128 | (( theInt >> 7) & 127 )), CHAR( 128 | (( theInt ) & 127 )));
			IF i >= 126 THEN
				LEAVE theLoop;
			ELSE
				SET i = i +1;
			END  IF;
		ELSE
			SET thePage = thePage - 1;
		END IF;
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
	RETURN theText;
END;
$$

DROP FUNCTION IF EXISTS CreateCsv $$
CREATE FUNCTION CreateCsv( theInjection unsigned bigint, theSep char)
RETURNS TEXT

BEGIN

DECLARE theInt 					int;
DECLARE theText 				TEXT 	DEFAULT '';
DECLARE done 					int 	DEFAULT FALSE;
DECLARE i 						int 	DEFAULT 0;
DECLARE samples 				int 	DEFAULT 0;

DECLARE ci_id 					unsigned bigint(20);
DECLARE ci_part_name 			varchar(80);
DECLARE ci_prod_num 			int(11);
DECLARE ci_injection 			int(11);
DECLARE ci_timecode 			datetime;
DECLARE ci_t1 					int(11);
DECLARE ci_t2 					int(11);
DECLARE ci_t3 					int(11);
DECLARE ci_cal_a 				int(11);
DECLARE ci_cal_b 				int(11);
DECLARE ci_cal_c 				int(11);
DECLARE ci_cal_d 				int(11);
DECLARE ci_hv_injection 		unsigned bigint(20);

DECLARE cc_id 					unsigned bigint(20);
DECLARE cc_injection_id 		unsigned bigint(20);
DECLARE cc_channel 				int(11);
DECLARE cc_sample_count 		int(11);
DECLARE cc_sample_rate 			int(11);
DECLARE cc_humidity_max 		int(11);
DECLARE cc_humidity_min 		int(11);
DECLARE cc_humidity_flags 		int(11);
DECLARE cc_humidity_cal 		int(11);
DECLARE cc_humidity_cal_res 	int(11);
DECLARE cc_temperature_max 		int(11);
DECLARE cc_temperature_min 		int(11);
DECLARE cc_temperature_flags 	int(11);
DECLARE cc_temperature_cal 		int(11);
DECLARE cc_temperature_cal_res 	int(11);
DECLARE cc_vaccum_max 			int(11);
DECLARE cc_vaccum_min 			int(11);
DECLARE cc_vaccum_flags 		int(11);
DECLARE cc_vaccum_cal 			int(11);
DECLARE cc_vaccum_cal_res 		int(11);


DECLARE cv_channel_a 			int(11);
DECLARE cv_channel_b 			int(11);
DECLARE cv_channel_c 			int(11);
DECLARE cv_channel_d 			int(11);
DECLARE cv_temp_a 				int(11);
DECLARE cv_temp_b 				int(11);
DECLARE cv_temp_c 				int(11);
DECLARE cv_temp_d 				int(11);
DECLARE cv_vaccum_a 			int(11);
DECLARE cv_vaccum_b 			int(11);
DECLARE cv_vaccum_c 			int(11);
DECLARE cv_vaccum_d 			int(11);
DECLARE cv_hv_vaccum_a 			int(11);
DECLARE cv_hv_vaccum_b 			int(11);
DECLARE cv_hv_vaccum_c 			int(11);
DECLARE cv_hv_vaccum_d 			int(11);

DECLARE csample CURSOR FOR
	SELECT count(*) FROM T_Humidity_Value WHERE injection_id = theInjection;
DECLARE cinfo CURSOR FOR
	SELECT * FROM T_Injection WHERE id = theInjection;
DECLARE ccinfo CURSOR FOR
	SELECT * FROM T_Channel_Info WHERE injection_id = theInjection ORDER BY channel;
DECLARE cvalue CURSOR FOR
	SELECT channel_a, channel_b, channel_c, channel_d,
		temp_a, temp_b, temp_c, temp_d,
		vaccum_a/10 as vaccum_a, vaccum_b/10 as vaccum_b, vaccum_c/10 as vaccum_c, vaccum_d/10 as vaccum_d,
		hv_vaccum_a, hv_vaccum_b, hv_vaccum_c, hv_vaccum_d FROM T_Humidity_Value WHERE injection_id = theInjection ORDER BY id;

DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

	OPEN csample;
	FETCH csample INTO samples;
	CLOSE csample;
	OPEN cinfo;
	FETCH cinfo INTO ci_id, ci_part_name, ci_prod_num, ci_injection, ci_timecode, ci_t1, ci_t2, ci_t3, 
					 ci_cal_a, ci_cal_b, ci_cal_c, ci_cal_d, ci_hv_injection;
	CLOSE cinfo;

	SET theText = CONCAT( "Parts Name:",theSep,ci_part_name,"\nProduction Number:",theSep,ci_prod_num,
				"\nInjection Number:",theSep,ci_injection,"\nHighVac Injection:",theSep,ci_hv_injection,
				"\nDate & Time:",theSep,ci_timecode,"\nNumber of values:",theSep,samples,
				"\nT1 T2 T3:",theSep,ci_t1,theSep,ci_t2,theSep,ci_t3 );

	OPEN ccinfo;

	SET theText = CONCAT( theText,"\nChannel setting and values\n",
		"Channel Number(Phy)",theSep,"Channel Number(Log)",theSep,"Sample Rate",theSep,
		"Humidity: Max",theSep,"Min",theSep,"Flags",theSep,"Calibration Offset",theSep,"Calibration Value",theSep,
		"Temperature: Max",theSep,"Min",theSep,"Flags",theSep,"Calibration Offset",theSep,"Calibration Value",theSep,
		"Vaccum: Max",theSep,"Min",theSep,"Flags",theSep,"Calibration Offset",theSep,"Calibration Value\n" );
		
	theLoop: LOOP
		FETCH ccinfo INTO cc_id, cc_injection_id, cc_channel, cc_sample_count, cc_sample_rate, cc_humidity_max,
						cc_humidity_min, cc_humidity_flags, cc_humidity_cal, cc_humidity_cal_res, cc_temperature_max,
						cc_temperature_min, cc_temperature_flags, cc_temperature_cal, cc_temperature_cal_res,
						cc_vaccum_max, cc_vaccum_min, cc_vaccum_flags, cc_vaccum_cal, cc_vaccum_cal_res;
		IF done THEN
			LEAVE theLoop;
		END IF;
		SET theText = CONCAT( theText, (cc_channel+1),theSep,-1,theSep, cc_sample_rate,theSep,
				cc_humidity_max,theSep, cc_humidity_min,theSep, cc_humidity_flags,theSep, cc_humidity_cal,theSep, cc_humidity_cal_res,theSep,
				cc_temperature_max,theSep, cc_temperature_min,theSep, cc_temperature_flags,theSep, cc_temperature_cal,theSep, cc_temperature_cal_res,theSep,
				cc_vaccum_max,theSep, cc_vaccum_min,theSep, cc_vaccum_flags,theSep, cc_vaccum_cal,theSep, cc_vaccum_cal_res,"\n");

	END LOOP;

	CLOSE ccinfo;
	
	SET done = FALSE;

	SET theText = CONCAT( theText,"\nSamples Values\n",
		"Humidity: CH1",theSep,"CH2",theSep,"CH3",theSep,"CH4",theSep,
		"Temperature: CH1",theSep,"CH2",theSep,"CH3",theSep,"CH4",theSep,
		"Vaccum: CH1",theSep,"CH2",theSep,"CH3",theSep,"CH4", theSep,
		"HigVac Vaccum: CH1",theSep,"CH2",theSep,"CH3",theSep,"CH4\n" );

	OPEN cvalue;
	
	theLoop: LOOP
		FETCH cvalue INTO cv_channel_a, cv_channel_b, cv_channel_c, cv_channel_d, cv_temp_a,
						  cv_temp_b, cv_temp_c, cv_temp_d, cv_vaccum_a, cv_vaccum_b, cv_vaccum_c, cv_vaccum_d,
						  cv_hv_vaccum_a, cv_hv_vaccum_b, cv_hv_vaccum_c, cv_hv_vaccum_d;
 
		IF done THEN
			LEAVE theLoop;
		END IF;
		SET theText = CONCAT( theText, cv_channel_a,theSep, cv_channel_b,theSep, cv_channel_c,theSep, cv_channel_d,theSep,
						  cv_temp_a,theSep, cv_temp_b,theSep, cv_temp_c,theSep, cv_temp_d,theSep,
						  cv_vaccum_a,theSep, cv_vaccum_b,theSep, cv_vaccum_c,theSep, cv_vaccum_d,theSep,
						  cv_hv_vaccum_a,theSep, cv_hv_vaccum_b,theSep, cv_hv_vaccum_c,theSep, cv_hv_vaccum_d,"\n");

	END LOOP;

	CLOSE cvalue;
	
	return theText;
END;
$$
DELIMITER ;
