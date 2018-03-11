/********************************************************************************************************
 * Sql Request for the database on the linux part of the arduino Yun
 * $Author	: Yves Huguenin
 * $Created	: 08.10.2013
 * $Version	: 1.0
 *
 *******************************************************************************************************/

 // Count injection number

SELECT COUNT( * ) FROM T_Injection;

//

SELECT * FROM  T_Injection AS I, T_Humidity_Value AS H WHERE I.id = H.injection_id;

// Get Autoincrement value for T_Injection

SELECT MAX(id) FROM T_Injection;

Call DemoData(10);

// get all max,min from channel and count of sample per injection

SELECT  injection_id,max(channel_a),max(channel_b),max(channel_c),max(channel_d),min(channel_a),min(channel_b),min(channel_c),min(channel_d),count(*) FROM T_Humidity_Value GROUP BY injection_id;