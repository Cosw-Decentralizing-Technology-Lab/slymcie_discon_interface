/*
 * BaramInterf.c
 *
 *  Created on: 2021. 2. 9.
 *      Author: Inwoo Chung (gutomitai@cosw.space)
 */

//#include <Python.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#define HOST_COMMAND_URL "http://localhost:50000/command"
//#define HOST_COMMAND_URL "http://172.30.1.34:50000/command"
#define COMMAND_CAL_MISALIGNMENT "cal_misalignment"
#define PI 3.14159


static int initFlag = 0;

//static PyObject *pName, *pModule, *pFunc;
static const char *moduleName = "requests";
static const char *funcName = "post";
static const char *jsonFuncName = "json";
static const char *statusName = "status";
static const char *ymName = "ym";
static const char *descriptionName = "description";

static CURL *curl = NULL;

typedef struct _JsonResponse {
	int status;
	//char *description;
	double ym;
} JsonResponse;


// Using curl.
int InitBaramInterface() {
	if (initFlag == 0) {
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		if(curl) {
			initFlag = 1;
		} else {
			curl_global_cleanup();
		}
	}

	return initFlag;
}

int UninitBaramInterface() {
	if (initFlag == 1) {
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		initFlag = 0;
	}

	return initFlag;
}

cJSON * _MakeCommandJsonObj(double rdRaw
			, double fwsRaw
			, double rsRaw
			, double powRaw
			, long tRaw) {
	cJSON *pCom = cJSON_CreateObject();
	if (pCom == NULL) return NULL;
	cJSON_AddItemToObject(pCom, "command", cJSON_CreateString(COMMAND_CAL_MISALIGNMENT));

	cJSON *pArgs = cJSON_CreateObject();
	if (pArgs == NULL) return NULL;

	cJSON_AddItemToObject(pCom, "args", pArgs);

	cJSON_AddItemToObject(pArgs, "rd_raw", cJSON_CreateNumber(rdRaw)); // Exception handling?
	cJSON_AddItemToObject(pArgs, "fws_raw", cJSON_CreateNumber(fwsRaw));
	cJSON_AddItemToObject(pArgs, "rs_raw", cJSON_CreateNumber(rsRaw));
	cJSON_AddItemToObject(pArgs, "pow_raw", cJSON_CreateNumber(powRaw));
	cJSON_AddItemToObject(pArgs, "t_raw", cJSON_CreateNumber(tRaw));

	return pCom;
}

static size_t _GetResponse(void *buffer, size_t size, size_t nmemb, void *userp) {
	// Parse json string.
	cJSON *pResJson = NULL;
	cJSON *pStatus = NULL;
	cJSON *pym = NULL;
	JsonResponse *pRes = NULL;

	pResJson = cJSON_Parse((char *)buffer);
	pRes = (JsonResponse *)userp;

    // Get status.
	pStatus = cJSON_GetObjectItemCaseSensitive(pResJson, statusName);
	if (cJSON_IsString(pStatus) && (pStatus->valuestring != NULL)) {
		if (strcmp(pStatus->valuestring, "success") == 0) {
			// Get yaw misalignment.
			pym = cJSON_GetObjectItemCaseSensitive(pResJson, ymName);
			if (cJSON_IsNumber(pym)) {
				pRes->status = 1;
				pRes->ym = pym->valuedouble;
			} else {
				pRes->status = 0;
				cJSON_Delete(pResJson);
				fprintf(stderr, "Failed to get yaw misalignment value.\n");
			}
		} else if(strcmp(pStatus->valuestring, "ym_not_calculated") == 0) {
			pRes->status = 2;
		} else {
			pRes->status = 0;
			fprintf(stderr, "Error occured: %s\n", pStatus->valuestring);
			cJSON_Delete(pResJson);
		}
	} else {
		pRes->status = -1;
		fprintf(stderr, "Failed to get status.\n");
		cJSON_Delete(pResJson);
	}

	return (size_t)(size * nmemb);
}

int CalculateYawMisalignment(double rdRaw
			, double fwsRaw
			, double rsRaw
			, double powRaw
			, long tRaw
			, double *ym) {
	CURLcode res;
	cJSON *pCom = NULL;
	char *jsonString = NULL;
	JsonResponse jsonRes;
	cJSON *pStatus = NULL;
	cJSON *pym = NULL;
	struct curl_slist *headers=NULL;

	if (initFlag == 0)
		return 0;

	// Make command json string.
	pCom = _MakeCommandJsonObj(rdRaw, fwsRaw, rsRaw, powRaw, tRaw);
	if (pCom == NULL) {
		fprintf(stderr, "Failed to make json object.\n");
		return 0;
	}

	jsonString = cJSON_Print(pCom);
	if (jsonString == NULL) {
		cJSON_Delete(pCom);
		fprintf(stderr, "Failed to make json string.\n");
		return 0;
	}

	cJSON_Delete(pCom);

	// Send command.
	curl_easy_reset(curl);
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_URL, HOST_COMMAND_URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _GetResponse);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonRes);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		free(jsonString);
		curl_slist_free_all(headers);
		fprintf(stderr, "%s\n", curl_easy_strerror(res));
		return 0;
	}

	if (jsonRes.status == 1) {
		*ym = jsonRes.ym;
	} else {
		free(jsonString);
		curl_slist_free_all(headers);
		return 0;
	}

	free(jsonString);
	curl_slist_free_all(headers);

	return 1;
}
