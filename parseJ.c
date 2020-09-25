/* 
 Copyright © Dougie Lawson, 2020, All rights reserved 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "parseJ.h"
#include <math.h>

char* current_key;
int minute;
int hour;
char* day_of_week;
int day_of_month;
int month;
int year;
int import;
int exported;
int genplus;
int genminus;
int zappiimp;
int zappidiv;
int voltage;
int frequency;
int pect_1;
int nect_1;
int pect_2;
int nect_2;
int pect_3;
int nect_3;
char* serno;
char* ectt_1;
char* ectt_2;
char* ectt_3;
char* ectt_4;
char* ectt_5;
char* ectt_6;
int cts[6];

enum { unknown, min, hr, dow, dom, mon, yr, imp, expd, gen, gep, h1d, h1b,  v1, frq, pect1, nect1, pect2, nect2, pect3, nect3, sno, ectt1, ectt2, ectt3, ectt4, ectt5, ectt6, };
char* none_str = "None";

float watts(int zValue)
{
	float volts;
	float valueZ;
	float watts;
	volts = (float)voltage / 10.0;
	valueZ = (float)zValue / 1.0;
	watts = roundf((valueZ / volts) * 400.0);
	return watts/100.0;
}

float ctSums(int ip, int ep, int p1, int p2, int p3, int n1, int n2, int n3)
{
	int sum;
	if ((ip > 0 && n1 > 0) && (ip == n1)) n1 = 0;
	if ((ep > 0 && p1 > 0) && (ep == p1)) p1 = 0;
	sum = p1 + p2 - n1 - n2;
	if (cts[3] != FALSE) sum = sum + p3 - n3;
	return watts(sum);
}

int lexer(const char *s)
{
	static struct entry_s
	{
		const char *key;
		int token;
	
	}
	token_table[] =
	{
		{ "min", min },
		{ "hr", hr },
		{ "dow", dow },
		{ "dom", dom },
		{ "mon", mon },
		{ "yr", yr },
		{ "imp", imp },
		{ "exp", expd },
		{ "gen", gen },
		{ "gep", gep },
		{ "h1d", h1d },
		{ "h1b", h1b },
		{ "v1", v1 },
		{ "frq", frq },
		{ "pect1", pect1 },
		{ "nect1", nect1 },
		{ "pect2", pect2 },
		{ "nect2", nect2 },
		{ "pect3", pect3 },
		{ "nect3", nect3 },
		{ "sno", sno },
		{ "ectt1", ectt1 },
		{ "ectt2", ectt2 },
		{ "ectt3", ectt3 },
		{ "ectt4", ectt4 },
		{ "ectt5", ectt5 },
		{ "ectt6", ectt6 },
	};
	struct entry_s *p = token_table;
	for(; p->key != NULL && strcmp(p->key, s) != 0; ++p);
	return p->token;
}

json_object* decode_json(json_object* jObj)
{
	json_object* analyse_result;
	enum json_type type;
	type = json_object_get_type(jObj);
	switch (type)
	{
		case json_type_object:
		case json_type_array:
			//printf("json_type_object\n");
			analyse_result = json_parse(jObj);
			break;
	}
	switch(lexer(current_key)) {
		case min:
			minute = json_object_get_int(jObj);
			break;
		case hr:
			hour = json_object_get_int(jObj);
			break;
		case dow:
			day_of_week = strdup(json_object_get_string(jObj));
			break;
		case dom:
			day_of_month = json_object_get_int(jObj);
			break;
		case mon:
			month = json_object_get_int(jObj);
			break;
		case yr:
			year = json_object_get_int(jObj);
			break;
		case imp:
			import = json_object_get_int(jObj);
			break;
		case expd:
			exported = json_object_get_int(jObj);
			break;
		case gen:
			genminus = json_object_get_int(jObj);
			break;
		case gep:
			genplus = json_object_get_int(jObj);
			break;
		case h1d:
			zappidiv = json_object_get_int(jObj);
			break;
		case h1b:
			zappiimp = json_object_get_int(jObj);
			break;
		case v1:
			voltage = json_object_get_int(jObj);
			break;
		case frq:
			frequency = json_object_get_int(jObj);
			break;
		case pect1:
			pect_1 = json_object_get_int(jObj);
			break;
		case nect1:
			nect_1 = json_object_get_int(jObj);
			break;
		case pect2:
			pect_2 = json_object_get_int(jObj);
			break;
		case nect2:
			nect_2 = json_object_get_int(jObj);
			break;
		case pect3:
			pect_3 = json_object_get_int(jObj);
			break;
		case nect3:
			nect_3 = json_object_get_int(jObj);
			break;
		case sno:
			serno = strdup(json_object_get_string(jObj));
			//printf("SNO: %s\n", serno);
			break;
		case ectt1:
			ectt_1 = strdup(json_object_get_string(jObj));
			cts[1] = strcmp(ectt_1, none_str);
			//printf("ectt_1 %s cts[1] %d ", ectt_1, cts[1]);
			break;
	
		case ectt2:
			ectt_2 = strdup(json_object_get_string(jObj));
			cts[2] = strcmp(ectt_2, none_str);
			//printf("ectt_2 %s cts[2] %d ", ectt_2, cts[2]);
			break;
	
		case ectt3:
			ectt_3 = strdup(json_object_get_string(jObj));
			cts[3] = strcmp(ectt_3, none_str);
			//printf("ectt_3 %s cts[3] %d ", ectt_3, cts[3]);
			break;
	
		case ectt4:
			ectt_4 = strdup(json_object_get_string(jObj));
			cts[4] = strcmp(ectt_4, none_str);
			//printf("ectt_4 %s cts[4] %d ", ectt_4, cts[4]);
			break;
	
		case ectt5:
			ectt_5 = strdup(json_object_get_string(jObj));
			cts[5] = strcmp(ectt_5, none_str);
			//printf("ectt_5 %s cts[5] %d ", ectt_5, cts[5]);
			break;
	
		case ectt6:
			ectt_6 = strdup(json_object_get_string(jObj));
			cts[6] = strcmp(ectt_6, none_str);
			//printf("ectt_6 %s cts[6] %d\n", ectt_6, cts[6]);
			break;

	}
	return analyse_result;
}

json_object* json_parse_array(json_object* jObj, int key)
{
	enum json_type type;
	int i, arraylen, idx;
	json_object* jarray = jObj;
	json_object* jvalue;
	if (key)
	{
		jarray = json_object_array_get_idx(jObj, key);
	}
	type = json_object_get_type(jarray);
	if (type == json_type_array && jarray != NULL)
	{

		arraylen = json_object_array_length(jarray);
	
		for (i=0; i< arraylen; i++)
		{
			jvalue = json_object_array_get_idx(jarray, i);
			type = json_object_get_type(jvalue);
			if (type != json_type_object)
			{
				decode_json(jvalue);
				json_object_put(jvalue);
			}
			else
			{
				idx = 0;
				json_object_parse(jvalue);
				json_object_put(jvalue);
			}
		}
	}
	return jarray;	
}

json_object* json_object_parse(json_object* jObj)
{
	//printf("\n-----------------\n");
	
	minute = 0;
	hour = 0;
	day_of_month = 0;
	month = 0;
	year = 0;
	import = 0;
	exported = 0;
	genplus = 0;
	genminus = 0;
	zappiimp = 0;
	zappidiv = 0;
	voltage = 0;
	frequency = 0;
	pect_1 = 0;
	nect_1 = 0;
	pect_2 = 0;
	nect_2 = 0;
	pect_3 = 0;
	nect_3 = 0;

	enum json_type type;
	json_object* parse_result;
	json_object* newObj;
	json_object_object_foreach(jObj, key, val)
	{
		current_key = strdup(key);
		type = json_object_get_type(val);
		switch (type)
		{
			case json_type_boolean: 
			case json_type_double: 
			case json_type_int: 
			case json_type_string:
				decode_json(val);
				break; 
			case json_type_object:
				newObj = json_tokener_parse(json_object_to_json_string(val));
				parse_result = json_parse(newObj);
				json_object_put(newObj);
				break;
			case json_type_array:
				newObj = json_tokener_parse(json_object_to_json_string(val));
				parse_result = json_parse(newObj);
				break;

		}
	}
//	printf("%04d-%02d-%02d %02d:%02d ", year, month, day_of_month, hour, minute); 
//	printf("%s imp: %4.1f exp: %4.1f gen+: %4.1f gen-: %4.1f zappi imp: %4.1f zappi div: %4.1f voltage: %3.1f freq: %2.2f pect1: %4.1f nect1: %4.1f pect2: %4.1f nect2: %4.1f pect3: %4.1f nect3: %4.1f ctSums: %4.1f\n", day_of_week, watts(import), watts(exported), watts(genplus), watts(genminus), watts(zappiimp), watts(zappidiv), (float)voltage/10.0, (float)frequency/100.0, watts(pect_1), watts(nect_1), watts(pect_2), watts(nect_2), watts(pect_3), watts(nect_3), ctSums(import, exported, pect_1, pect_2, pect_3, nect_1, nect_2, nect_3)); 

	sql_insert();
	if (parse_result == NULL) return parse_result;
}

json_object* json_parse(json_object* jObj)
{
	enum json_type type;
	json_object* parse_result;
	int idx =0;
	parse_result = jObj;
	type = json_object_get_type(jObj);
	do 
	{
		if (parse_result != NULL) 
		{
			switch (type)
			{
				case json_type_boolean: 
				case json_type_double: 
				case json_type_int: 
				case json_type_string:
					decode_json(jObj);
					break; 
				case json_type_object:
					parse_result = json_object_parse(jObj); 
		  			break;
				case json_type_array:
		  			parse_result = json_parse_array(jObj, idx);
					break;
			}
		}
		idx++;
	}
	while(parse_result != NULL);
	return parse_result;
}