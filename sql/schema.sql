/********************************************************************************************************
 * Schema for the database on the linux part of the arduino Yun
 * $Author	: Yves Huguenin
 * $Created	: 07.10.2013
 * $Version	: 1.0
 *
 *******************************************************************************************************/

use fondarex;

DROP TABLE IF EXISTS T_Injection;
CREATE TABLE T_Injection
(
	id			bigint			PRIMARY KEY NOT NULL AUTO_INCREMENT,
	part_name		varchar(30),
	prod_num		int			NOT NULL DEFAULT 0,
	injection		int			NOT NULL DEFAULT 0,
	timecode		datetime		NOT NULL,
	t1			float			NOT NULL DEFAULT 0,
	t2			float			NOT NULL DEFAULT 0,
	t3			float			NOT NULL DEFAULT 0,
	vaccum_a		float			NOT NULL DEFAULT 9999,
	vaccum_b		float			NOT NULL DEFAULT 9999,
	vaccum_c		float			NOT NULL DEFAULT 9999,
	vaccum_d		float			NOT NULL DEFAULT 9999
);

DROP TABLE IF EXISTS T_Humidity_Value;
CREATE TABLE T_Humidity_Value
(
	id			bigint			PRIMARY KEY NOT NULL AUTO_INCREMENT,
	injection_id		bigint			NOT NULL,
	channel_a		int,
	channel_b		int,
	channel_c		int,
	channel_d		int,
	cal_a			int,
	cal_b			int,
	cal_c			int,
	cal_d			int
);

DROP TABLE IF EXISTS T_Globals;
CREATE TABLE T_Globals
(
	name			varchar(30)			PRIMARY KEY NOT NULL,
	v_int			bigint				DEFAULT 0,
	v_text			text				DEFAULT NULL,
	v_float			double				DEFAULT 0,
	v_type			enum('int','text','float')	NOT NULL DEFAULT 'text',
	quand			datetime			DEFAULT NOW()
);


/********************* Some procedure for creating Demo datas ******************************************************************/


DELIMITER $$

DROP PROCEDURE IF EXISTS DemoInjection $$

CREATE PROCEDURE DemoInjection( IN thetime float )
BEGIN
DECLARE ch_a int;
DECLARE ch_b int;
DECLARE ch_c int;
DECLARE ch_d int;

DECLARE ch_da int DEFAULT FLOOR( (RAND() * 8) -5 );
DECLARE ch_db int DEFAULT FLOOR( (RAND() * 8) -5 );
DECLARE ch_dc int DEFAULT FLOOR( (RAND() * 8) -5 );
DECLARE ch_dd int DEFAULT FLOOR( (RAND() * 8) -5 );


DECLARE ch_la int;
DECLARE ch_lb int;
DECLARE ch_lc int;
DECLARE ch_ld int;

DECLARE cal_a int;
DECLARE cal_b int;
DECLARE cal_c int;
DECLARE cal_d int;

DECLARE theid bigint;

	SET ch_a = 700 + FLOOR( RAND() * 200 - 100 );
	SET ch_b = 700 + FLOOR( RAND() * 200 - 100 );
	SET ch_c = 700 + FLOOR( RAND() * 200 - 100 );
	SET ch_d = 700 + FLOOR( RAND() * 200 - 100 );
        SET cal_b = FLOOR( RAND() * 100 );
        SET cal_b = FLOOR( RAND() * 100 );
        SET cal_c = FLOOR( RAND() * 100 );
        SET cal_d = FLOOR( RAND() * 100 );

	SELECT MAX(id) INTO theid FROM T_Injection;
	WHILE thetime > 0 DO
		INSERT INTO T_Humidity_Value VALUES ( NULL, theid  , ch_a, ch_b, ch_c, ch_d, cal_a, cal_b, cal_c, cal_d);
		SET ch_da = ch_da + FLOOR( (RAND() * 4) -2 );
		SET ch_db = ch_db + FLOOR( (RAND() * 4) -2 );
		SET ch_dc = ch_dc + FLOOR( (RAND() * 4) -2 );
		SET ch_dd = ch_dd + FLOOR( (RAND() * 4) -2 );

		SET ch_la = - FLOOR( ch_a / 20 );
		SET ch_lb = - FLOOR( ch_b / 20 );
		SET ch_lc = - FLOOR( ch_c / 20 );
		SET ch_ld = - FLOOR( ch_d / 20 );

		IF ch_da < ch_la THEN
			SET ch_da = ch_la;
			ELSEIF ch_da > (49 - ch_la ) / 2 THEN
				SET ch_da = (49 - ch_la ) / 2;
		END IF;
		IF ch_db < ch_lb THEN
			SET ch_db = ch_lb;
			ELSEIF ch_db > (49 - ch_lb ) / 2 THEN
				SET ch_db = (49 - ch_lb ) / 2;
		END IF;
		IF ch_dc < ch_lc THEN
			SET ch_dc = ch_lc;
			ELSEIF ch_dc > (49 - ch_lc ) / 2 THEN
				SET ch_dc = (49 - ch_lc ) / 2;
		END IF;
		IF ch_dd < ch_ld THEN
			SET ch_dd = ch_ld;
			ELSEIF ch_dd > (49 - ch_ld ) / 2 THEN
				SET ch_dd = (49 - ch_ld ) / 2;
		END IF;

		SET ch_a = ch_a + ch_da;
		SET ch_b = ch_b + ch_db;
		SET ch_c = ch_c + ch_dc;
		SET ch_d = ch_d + ch_dd;
		IF ch_a < 0 THEN
			SET ch_a = 0;
			ELSEIF ch_a > 1000 THEN
				SET ch_a = 1000;
		END IF;
		IF ch_b < 0 THEN
			SET ch_b = 0;
			ELSEIF ch_b > 1000 THEN
				SET ch_b = 1000;
		END IF;
		IF ch_c < 0 THEN
			SET ch_c = 0;
			ELSEIF ch_c > 1000 THEN
				SET ch_c = 1000;
		END IF;
		IF ch_d < 0 THEN
			SET ch_d = 0;
			ELSEIF ch_d > 1000 THEN
				SET ch_d = 1000;
		END IF;
		SET thetime = thetime - 0.02;
	END WHILE;
END;
$$

DROP PROCEDURE IF EXISTS DemoData $$

CREATE PROCEDURE DemoData( IN howMany int)
BEGIN

DECLARE theProd int;
DECLARE theInjection int DEFAULT 1;
DECLARE theT1 float DEFAULT 0.01;
DECLARE theT2 float DEFAULT 1.5;
DECLARE theT3 float DEFAULT 1;
DECLARE theTime datetime DEFAULT NOW();

	SELECT IFNULL( MAX(prod_num), 0 )  INTO theProd FROM T_Injection;
	SET theProd = theProd + 1;
	WHILE howMany > 0 DO
		INSERT INTO T_Injection VALUES ( NULL, "Demo Part", theProd, theInjection, theTime, theT1, theT2, theT3, FLOOR( RAND() * 100 ), FLOOR( RAND() * 100 ), FLOOR( RAND() * 100 ), FLOOR( RAND() * 100 ) );
		CALL DemoInjection( 1.5 + 1 );
		SET howMany = howMany - 1;
		SET theInjection = theInjection + 1;
		SET theTime =  ADDTIME( theTime,  theT1 + theT2 + theT3 );
	END WHILE;
END;
$$

DROP FUNCTION IF EXISTS ReadGlobal $$
CREATE FUNCTION ReadGlobal( theName varchar(30) )
RETURNS text
BEGIN

DECLARE theInt 		bigint unsigned;
DECLARE ok 		 	int;
DECLARE theFloat 	 float;
DECLARE theText 	 text;
DECLARE theType		 enum('int','text','float');

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

DECLARE ok          int;
DECLARE theType     enum('int','text','float');

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

DROP FUNCTION IF EXISTS ReadChannel $$
CREATE FUNCTION ReadChannel( theInjection bigint, theChannel char, thePage int  )
RETURNS varchar( 255 )
BEGIN

DECLARE theInt		int;
DECLARE theText 	varchar( 255 ) DEFAULT '';
DECLARE done		int DEFAULT FALSE;
DECLARE	i		int DEFAULT 0;

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

DELIMITER ;

select ReadChannel( 520,'a',0);