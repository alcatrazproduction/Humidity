/********************************************************************************************************
 * Schema for the database on the linux part of the arduino Yun
 * $Author	: Yves Huguenin
 * $Created	: 16.05.2014
 * $Version	: 1.0
 *
 *******************************************************************************************************/

use fondarexV2;

/********************* Some procedure for accessing Globals ********************************************/


DELIMITER $$

DROP FUNCTION IF EXISTS ReadGlobal $$
CREATE FUNCTION ReadGlobal( theName varchar(30) )
RETURNS text
BEGIN

DECLARE theInt   int;
DECLARE ok       int;
DECLARE theFloat float;
DECLARE theText  text;
DECLARE theType  enum('int','text','float');

	SELECT v_int, v_text, v_float, v_type, count(*) INTO theInt, theText, theFloat, theType, ok FROM T_Globals WHERE name = theName;
	IF ok = 0 THEN
		RETURN NULL;
	END IF;
	IF theType = 'int' THEN
		RETURN theInt;
	ELSEIF theType = 'float' THEN
		RETURN theFloat;
	END IF;
	RETURN theText;
END;

$$

DROP PROCEDURE IF EXISTS UpdateGlobal $$
CREATE PROCEDURE UpdateGlobal( IN theName varchar(30), IN theValue text )
BEGIN

DECLARE ok      int;
DECLARE theType enum('int','text','float');

	SELECT v_type, count(*) INTO theType, ok FROM T_Globals WHERE name = theName;
	IF ok = 0 THEN
		CALL CreateGlobal( theName, theValue, 'text' );
	ELSEIF theType = 'int' THEN
		UPDATE T_Globals SET v_int = theValue, quand = NOW() WHERE name=theName;
	ELSEIF theType = 'float' THEN
		UPDATE T_Globals SET v_float = theValue, quand = NOW() WHERE name=theName;
	ELSE
		UPDATE T_Globals SET v_text = theValue, quand = NOW() WHERE name=theName;
	END IF;
END;
$$

DROP PROCEDURE IF EXISTS CreateGlobal $$
CREATE PROCEDURE CreateGlobal( IN theName varchar(30), IN theValue text, IN theType  enum('int','text','float'))
BEGIN

DECLARE ok int;

	SELECT  count(*) INTO ok FROM T_Globals WHERE name = theName;
	IF ok = 1 THEN
		CALL UpdateGlobal( theName, theValue );
	ELSEIF theType = 'int' THEN
		INSERT INTO T_Globals VALUES ( theName, theValue, NULL, NULL, theType, NULL );
	ELSEIF theType = 'float' THEN
		INSERT INTO T_Globals VALUES ( theName, NULL, NULL, theValue, theType, NULL );
	ELSE
		INSERT INTO T_Globals VALUES ( theName, NULL, theValue, NULL, theType, NULL );
	END IF;

END;

$$

DROP FUNCTION IF EXISTS CreateSampleTime $$
CREATE FUNCTION CreateSampleTime( inj_id bigint )
RETURNS bool
BEGIN

DECLARE minid    bigint;
DECLARE rate     int;
DECLARE ok       int;

	SELECT sample_rate, count(*) INTO rate, ok FROM T_Channel_Info WHERE id = inj_id;
	IF ok = 0 THEN
		RETURN FALSE;
	END IF;
	SELECT MIN( id ), count(*) INTO minid, ok FROM T_Humidity_Value WHERE id = inj_id;
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

