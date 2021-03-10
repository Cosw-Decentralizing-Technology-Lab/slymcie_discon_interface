/*
 * BaramInterf.c
 *
 *  Created on: 2021. 2. 9.
 *      Author: Inwoo Chung (gutomitai@cosw.space)
 */

//#include <Python.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
//#define HOST_COMMAND_URL "http://localhost:5000/command"
#define HOST_COMMAND_URL "http://172.30.1.34:5000/command"
#define COMMAND_CAL_MISALIGNMENT "cal_misalignment"

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

// Using embedded python.
/*
int InitBaramInterfaceP() {
	if (initFlag == 0) {
		Py_Initialize();

		if (Py_IsInitialized()) {
			pName = PyUnicode_DecodeFSDefault(moduleName);
			pModule = PyImport_Import(pName);
			Py_DECREF(pName);

			if (pModule != NULL) {
				pFunc = PyObject_GetAttrString(pModule, funcName);

				if (pFunc && PyCallable_Check(pFunc)) {
					initFlag = 1;
				}
				else {
					if (PyErr_Occurred())
						PyErr_Print();

					Py_XDECREF(pFunc);
					Py_DECREF(pModule);
				}
			}
			else {
				PyErr_Print();
			}
		}
	}

	return initFlag;
}

int UninitBaramInterfaceP() {
	if (initFlag == 1) {
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);

	    if (Py_FinalizeEx() < 0) {
	    	initFlag = -1;
	    } else {
	    	initFlag = 0;
	    }
	}

	return initFlag;
}

int CalculateYawMisalignmentP(double rdRaw
			, double fwsRaw
			, double rsRaw
			, double powRaw
			, long tRaw
			, double *ym) {
	PyObject *pComDict, *pComArgsDict, *pYM, *pArgs, *pKwargs, *pResponse, *pJsonFunc, *pJsonDict, *pStatus;
	char *status;
	char *description;

	if (initFlag == 0)
		return 0;

	pComDict = PyDict_New();
	PyDict_SetItemString(pComDict, "command", PyUnicode_DecodeFSDefault(COMMAND_CAL_MISALIGNMENT));

	pComArgsDict = PyDict_New();
	PyDict_SetItemString(pComArgsDict, "rd_raw", PyFloat_FromDouble(rdRaw));
	PyDict_SetItemString(pComArgsDict, "fws_raw", PyFloat_FromDouble(fwsRaw));
	PyDict_SetItemString(pComArgsDict, "rs_raw", PyFloat_FromDouble(rsRaw));
	PyDict_SetItemString(pComArgsDict, "pow_raw", PyFloat_FromDouble(powRaw));
	PyDict_SetItemString(pComArgsDict, "t_raw", PyLong_FromLong(tRaw));

	PyDict_SetItemString(pComDict, "args", pComArgsDict);

	pArgs = PyTuple_New(1);
	PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(HOST_COMMAND_URL));

	pKwargs = NULL; //PyDict_New();
	//PyDict_SetItemString(pKwargs, "json", pComDict);

	pResponse = PyObject_Call(pFunc, pArgs, pKwargs);
	//Py_DECREF(pKwargs);
	Py_DECREF(pArgs);
	Py_DECREF(pComArgsDict);
	Py_DECREF(pComDict);

	// Check exception.
	if (pResponse != NULL) {
		pJsonFunc = PyObject_GetAttrString(pResponse, jsonFuncName);

		if (pJsonFunc && PyCallable_Check(pJsonFunc)) {
			pJsonDict = PyObject_CallObject(pJsonFunc, NULL);

			if (pJsonDict == NULL) {
				if (PyErr_Occurred())
					PyErr_Print();

				Py_DECREF(pJsonFunc);
				Py_DECREF(pResponse);

				return 0;
			} else {
				// Get status.
				pName = PyUnicode_DecodeFSDefault(statusName);
				pStatus = PyDict_GetItem(pJsonDict, pName);
				Py_DECREF(pName);
				status = PyUnicode_AS_DATA(pStatus);
				Py_DECREF(pStatus);

				if (strcmp(status, "success") == 0) {
					// Get yaw misalignment.
					pName = PyUnicode_DecodeFSDefault(ymName);
					pYM = PyDict_GetItem(pJsonDict, pName);
					Py_DECREF(pName);

					*ym = PyFloat_AsDouble(pYM);
					Py_DECREF(pYM);
				} else {
					Py_DECREF(pJsonDict);
					Py_DECREF(pJsonFunc);
					Py_DECREF(pResponse);

					return 0;
				}

				Py_DECREF(pJsonDict);
				Py_DECREF(pJsonFunc);
				Py_DECREF(pResponse);
			}
		} else {
			if (PyErr_Occurred())
				PyErr_Print();

			Py_XDECREF(pJsonFunc);
			Py_DECREF(pResponse);
			return 0;
		}
	} else {
		if (PyErr_Occurred())
			PyErr_Print();

		return 0;
	}

	return 1;
}
*/

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
			pRes->status = 1;

			// Get yaw misalignment.
			pym = cJSON_GetObjectItemCaseSensitive(pResJson, ymName);
			if (cJSON_IsNumber(pym)) {
				pRes->ym = pym->valuedouble;
			} else {
				cJSON_Delete(pResJson);
				fprintf(stderr, "Failed to get yaw misalignment value.\n");
			}
		} else {
			pRes->status = 0;
			fprintf(stderr, "Error occured: %s\n", pStatus->valuestring);
			cJSON_Delete(pRes);
		}
	} else {
		pRes->status = -1;
		fprintf(stderr, "Failed to get status.\n");
		cJSON_Delete(pRes);
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
	JsonResponse pRes;
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
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pRes);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		free(jsonString);
		curl_slist_free_all(headers);
		fprintf(stderr, "%s\n", curl_easy_strerror(res));
		return 0;
	}

	if (pRes.status == 1) {
		*ym = pRes.ym;
	}

	return 1;
}
