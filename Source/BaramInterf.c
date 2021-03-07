/*
 * BaramInterf.c
 *
 *  Created on: 2021. 2. 9.
 *      Author: Inwoo Chung (gutomitai@cosw.space)
 */

#include <Python.h>
#define HOST_COMMAND_URL "http://localhost:5000/command"
#define COMMAND_CAL_MISALIGNMENT "cal_misalignment"

int initFlag = 0;
PyObject *pName, *pModule, *pFunc;
const char *moduleName = "requests";
const char *funcName = "post";
const char *jsonFuncName = "json";
const char *statusName = "status";
const char *ymName = "ym";
const char *descriptionName = "description";

int InitBaramInterface() {
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

int UninitBaramInterface() {
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

int CalculateYawMisalignment(double rdRaw
			, double fwsRaw
			, double rsRaw
			, double powRaw
			, long tRaw
			, double *ym)
{
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

	pKwargs = PyDict_New();
	PyDict_SetItemString(pKwargs, "json", pComDict);

	pResponse = PyObject_Call(pFunc, pArgs, pKwargs);

	// Check exception.
	if (pResponse != NULL) {
		pJsonFunc = PyObject_GetAttrString(pResponse, jsonFuncName);

		if (pJsonFunc && PyCallable_Check(pJsonFunc)) {
			pJsonDict = PyObject_CallObject(pJsonFunc, NULL);

			if (pJsonDict == NULL) {
				if (PyErr_Occurred())
					PyErr_Print();

				Py_XDECREF(pJsonFunc);
				Py_DECREF(pResponse);
				Py_DECREF(pArgs);
				Py_DECREF(pKwargs);
				Py_DECREF(pComArgsDict);
				Py_DECREF(pComDict);
				return 0;
			} else {
				// Get status.
				pName = PyUnicode_DecodeFSDefault(statusName);
				pStatus = PyDict_GetItem(pJsonDict, pName);
				Py_DECREF(pName);
				status = PyUnicode_AS_DATA(pStatus);

				if (strcmp(status, "error") != 0) {
					// Get yaw misalignment.
					pName = PyUnicode_DecodeFSDefault(ymName);
					pYM = PyDict_GetItem(pJsonDict, pName);
					Py_DECREF(pName);

					*ym = PyFloat_AsDouble(pYM);

					Py_DECREF(pYM);
				} else {
					Py_DECREF(pStatus);
					Py_DECREF(pJsonDict);
					Py_XDECREF(pJsonFunc);
					Py_DECREF(pResponse);
					Py_DECREF(pArgs);
					Py_DECREF(pKwargs);
					Py_DECREF(pComArgsDict);
					Py_DECREF(pComDict);
					return 0;
				}

				Py_DECREF(pStatus);
				Py_DECREF(pJsonDict);
				Py_XDECREF(pJsonFunc);
				Py_DECREF(pResponse);
				Py_DECREF(pArgs);
				Py_DECREF(pKwargs);
				Py_DECREF(pComArgsDict);
				Py_DECREF(pComDict);
			}
		} else {
			if (PyErr_Occurred())
				PyErr_Print();

			Py_DECREF(pArgs);
			Py_DECREF(pKwargs);
			Py_DECREF(pComArgsDict);
			Py_DECREF(pComDict);
			return 0;
		}
	} else {
		if (PyErr_Occurred())
			PyErr_Print();

		Py_DECREF(pArgs);
		Py_DECREF(pKwargs);
		Py_DECREF(pComArgsDict);
		Py_DECREF(pComDict);
		return 0;
	}

	return 1;
}
