/* 
 Copyright © Dougie Lawson, 2020-2024, All rights reserved 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "parseJ.h"
#include <math.h>

char* current_key;
char* curTime;
char* unk_string;
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
int statusCode;
char* statusText;

enum { unknown, asn, batteryDischargeEnabled, bsm, bss, bst, beingTamperedWith, che, cmt, dat, deviceClass, divi, dom, dow, dst, ectp1, ectp2, ectp3, ectp4, ectp5, ectp6, ectt1, ectt2, ectt3, ectt4, ectt5, ectt6, eddi, expd, frq, fwv, gen, gep, grd, g100LockoutState, h1b, h1d, harvi, hr, imp, isVHubEnabled, isZappi2, lck, libbi, mgl, min, mon, nect1, nect2, nect3, newAppAvailable, newBootloaderAvailable, phaseSetting, pect1, pect2, pect3, pha, productCode, pri, pst, pwm, rac, rdc, rrac, sbh, sbk, sbm, sno, sta, status, statustext, tbk, tim, tz, v1, vhub, vol, yr, zappi, zmo, zs, zsh, zsl };

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
		{ "asn", asn },
		{ "batteryDischargeEnabled", batteryDischargeEnabled },
		{ "bsm", bsm },
		{ "bss", bss },
		{ "bst", bst },
		{ "beingTamperedWith", beingTamperedWith },
		{ "che", che },
		{ "cmt", cmt },
		{ "dat", dat },
		{ "deviceClass", deviceClass },
		{ "div", divi },
		{ "dom", dom },
		{ "dow", dow },
		{ "dst", dst },
		{ "ectp1", ectp1 },
		{ "ectp2", ectp2 },
		{ "ectp3", ectp3 },
		{ "ectp4", ectp4 },
		{ "ectp5", ectp6 },
		{ "ectp6", ectp6 },
		{ "ectt1", ectt1 },
		{ "ectt2", ectt2 },
		{ "ectt3", ectt3 },
		{ "ectt4", ectt4 },
		{ "ectt5", ectt5 },
		{ "ectt6", ectt6 },
		{ "eddi", eddi },
		{ "exp", expd },
		{ "frq", frq },
		{ "fwv", fwv },
		{ "gen", gen },
		{ "gep", gep },
		{ "grd", grd },
		{ "g100LockoutState", g100LockoutState },
		{ "harvi", harvi },
		{ "h1b", h1b },
		{ "h1d", h1d },
		{ "hr", hr },
		{ "imp", imp },
		{ "isVHubEnabled", isVHubEnabled },
		{ "isZappi2", isZappi2 },
		{ "lck", lck },
		{ "libbi", libbi },
		{ "mgl", mgl },
		{ "min", min },
		{ "mon", mon },
		{ "newAppAvailable", newAppAvailable},
		{ "newBootloaderAvailable", newBootloaderAvailable},
		{ "nect1", nect1 },
		{ "nect2", nect2 },
		{ "nect3", nect3 },
		{ "pect1", pect1 },
		{ "pect2", pect2 },
		{ "pect3", pect3 },
		{ "phaseSetting", phaseSetting },
		{ "pha", pha },
		{ "pri", pri },
		{ "productCode", productCode },
		{ "pst", pst },
		{ "pwm", pwm },
		{ "rac", rac },
		{ "rdc", rdc },
		{ "rrac", rrac },
		{ "sbh", sbh },
		{ "sbk", sbk },
		{ "sbm", sbm },
		{ "sno", sno },
		{ "sta", sta },
		{ "status", status },
		{ "statustext", statustext },
		{ "tbk", tbk },
		{ "tim", tim },
		{ "tz", tz },
		{ "v1", v1 },
		{ "vhub", vhub },
		{ "vol", vol },
		{ "yr", yr },
		{ "zappi", zappi },
		{ "zmo", zmo },
		{ "zs", zs },
		{ "zsh", zsh },
		{ "zsl", zsl },
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
			analyse_result = json_parse(jObj);
			break;
	}
	switch(lexer(current_key)) {
		case dom:
			day_of_month = json_object_get_int(jObj);
			break;
		case dow:
			day_of_week = strdup(json_object_get_string(jObj));
			break;
		case ectt1:
			ectt_1 = strdup(json_object_get_string(jObj));
			cts[1] = strcmp(ectt_1, none_str);
			break;
		case ectt2:
			ectt_2 = strdup(json_object_get_string(jObj));
			cts[2] = strcmp(ectt_2, none_str);
			break;
		case ectt3:
			ectt_3 = strdup(json_object_get_string(jObj));
			cts[3] = strcmp(ectt_3, none_str);
			break;
		case ectt4:
			ectt_4 = strdup(json_object_get_string(jObj));
			cts[4] = strcmp(ectt_4, none_str);
			break;
		case ectt5:
			ectt_5 = strdup(json_object_get_string(jObj));
			cts[5] = strcmp(ectt_5, none_str);
			break;
		case ectt6:
			ectt_6 = strdup(json_object_get_string(jObj));
			cts[6] = strcmp(ectt_6, none_str);
			break;
		case expd:
			exported = json_object_get_int(jObj);
			break;
		case frq:
			frequency = json_object_get_int(jObj);
			break;
		case gen:
			genminus = json_object_get_int(jObj);
			break;
		case gep:
			genplus = json_object_get_int(jObj);
			break;
		case h1b:
			zappiimp = json_object_get_int(jObj);
			break;
		case h1d:
			zappidiv = json_object_get_int(jObj);
			break;
		case hr:
			hour = json_object_get_int(jObj);
			break;
		case imp:
			import = json_object_get_int(jObj);
			break;
		case min:
			minute = json_object_get_int(jObj);
			break;
		case mon:
			month = json_object_get_int(jObj);
			break;
		case nect1:
			nect_1 = json_object_get_int(jObj);
			break;
		case nect2:
			nect_2 = json_object_get_int(jObj);
			break;
		case nect3:
			nect_3 = json_object_get_int(jObj);
			break;
		case pect1:
			pect_1 = json_object_get_int(jObj);
			break;
		case pect2:
			pect_2 = json_object_get_int(jObj);
			break;
		case pect3:
			pect_3 = json_object_get_int(jObj);
			break;
		case sno:
			serno = strdup(json_object_get_string(jObj));
			curl2();
			break;
		case status:
			statusCode = json_object_get_int(jObj);
			break;
		case statustext:
			statusText = strdup(json_object_get_string(jObj));
			fprintf(stderr, "Error: %d %s\n", statusCode, statusText);
			exit(statusCode);
			break;
		case tim:
			curTime = strdup(json_object_get_string(jObj));
			break;
		case v1:
			voltage = json_object_get_int(jObj);
			break;
		case yr:
			year = json_object_get_int(jObj);
			break;
		case asn:
			break;
		case batteryDischargeEnabled:
			break;
		case bsm:
			break;
		case bss:
			break;
		case bst:
			break;
		case beingTamperedWith:
			break;
		case che:
			break;
		case cmt:
			break;
		case dat:
			break;
		case deviceClass:
			break;
		case divi:
			break;
		case dst:
			break;
		case eddi:
			break;
		case ectp1:
			break;
		case ectp2:
			break;
		case ectp3:
			break;
		case ectp4:
			break;
		case ectp5:
			break;
		case ectp6:
			break;
		case fwv:
			break;
		case grd:
			break;
		case g100LockoutState:
			break;
		case harvi:
			break;
		case isVHubEnabled:
			break;
		case isZappi2:
			break;
		case lck:
			break;
		case libbi:
			break;
		case newAppAvailable:
			break;
		case newBootloaderAvailable:
			break;
		case mgl:
			break;
		case phaseSetting:
			break;
		case pha:
			break;
		case pri:
			break;
		case productCode:
			break;
		case pwm:
			break;
		case pst:
			break;
		case rac:
			break;
		case rdc:
			break;
		case rrac:
			break;
		case sbh:
			break;
		case sbk:
			break;
		case sbm:
			break;
		case sta:
			break;
		case tbk:
			break;
		case tz:
			break;
		case vhub:
			break;
		case vol:
			break;
		case zmo:
			break;
		case zs:
			break;
		case zsh:
			break;
		case zsl:
			break;
		case unknown:
		default:
			unk_string = strdup(json_object_get_string(jObj));

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
			}
			else
			{
				idx = 0;
				json_object_parse(jvalue);
				if (jvalue != NULL) 
				{
					json_object_put(jvalue);
				}
			}
		}
	}
	return jarray;	
}

json_object* json_object_parse(json_object* jObj)
{
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
