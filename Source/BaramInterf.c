/*
 * BaramInterf.c
 *
 *  Created on: 2021. 2. 9.
 *      Author: Inwoo Chung (gutomitai@cosw.space)
 */

#include "Python.h"
#define HOST_COMMAND_URL "http:///localhost:5000/command"


int CalculateYawMisalignment(double rdRaw
			, double fwsRaw
			, double rsRaw
			, double powRaw
			, long tRaw
			, double *ym)
{
	PyObject *pName, *pModule, *pFunc, *pComDict, *pYM, *pArgs;
	const char *moduleName = "requests";
	const char *funcName = "post";

	Py_Initialize();
	pName = PyUnicode_DecodeFSDefault(moduleName);
	pModule = PyImport_import(pName);
	Py_DECREF(pName);

	if (pModule != NULL) {
		pFunc = PyObject_GetAttrString(pModule, funcName);

		if (pFunc && PyCallable_Check(pFunc)) {
			pComDict = PyDict_New();
			PyDict_SetItemString(pComDict, "rd_raw", PyFloat_FromDouble(rdRaw));
			PyDict_SetItemString(pComDict, "fws_raw", PyFloat_FromDouble(fwsRaw));
			PyDict_SetItemString(pComDict, "rs_raw", PyFloat_FromDouble(rsRaw));
			PyDict_SetItemString(pComDict, "pow_raw", PyFloat_FromDouble(powRaw));
			PyDict_SetItemString(pComDict, "t_raw", PyFloat_FromDouble(tRaw));

			pArgs = PyTuple_New(2);
			PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(HOST_COMMAND_URL));
			PyTuple_SetItem(pArgs, 1, pComDict);

			pYM = PyObject_CallObject(pFunc, pArgs);

			Py_DECREF(pArgs);
			Py_DECREF(pComDict); //?
			Py_DECREF(pFunc);
			Py_DECREF(pModule);

			if(pYM != NULL) {
				*ym = PyLong_AsLong(pYM);
			}
			else {
				Py_DECREF(pYM);
				PyErr_Print();
				return 1;
			}

			Py_DECREF(pYM);
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();

			Py_XDECREF(pFunc); //?
			Py_DECREF(pModule);
			return 1;
		}

		Py_XDECREF(pFunc); //?
		Py_DECREF(pModule);
	}
	else {
		PyErr_Print();
		return 1;
	}

    if (Py_FinalizeEx() < 0) { //?
        return 120;
    }

	return 0;
}
