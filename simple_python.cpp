#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>

using namespace std;

void simple_strings()
{
    // Run some strings
    PyRun_SimpleString("print(__name__)\n");
    PyRun_SimpleString("print('hello')\n");
}

void simple_file(const char * const file)
{
    FILE *script = fopen(file, "r");
    PyRun_SimpleFile(script, file);
}

template <int... Args>
void call_func(
        const char * const mod,
        const char * const func,
        int arg)
{
    PyObject *pName = PyUnicode_DecodeFSDefault(mod);

    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == nullptr) {
        cerr << "failed to load: " << mod << endl;
        PyErr_Print();
        return;
    }

    PyObject *pFunc = PyObject_GetAttrString(pModule, func);

    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject *pArgs = PyTuple_New(1);
        PyObject *pValue = PyLong_FromLong(arg);
        PyTuple_SetItem(pArgs, 0, pValue);

        // No need to DECREF pValue, reference stolen by PyTuple

        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);

        cout << mod << "::" << func << "(" << arg << ") -> ";
        if (pValue) {
            cout << PyLong_AsLong(pValue) << endl;
            Py_DECREF(pValue);
        } else {
            cout << "FAIL" << endl;
            PyErr_Print();
        }
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        if (PyErr_Occurred()) {
            PyErr_Print();
            cerr << "Cannot find function " << mod << "::" << func << endl;
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
}

int main(int argc, char **argv) {

    // convert program name to wchar_t*
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == nullptr) {
        cerr << "PyDecodeLocale(argv[0]) failed" << endl;
        return 1;
    }
    // Tell python the program name
    Py_SetProgramName(program);
    // Init
    Py_Initialize();
    // Add current directory to path so we can import our script
    PySys_SetPath(L".");

    simple_strings();

    // Load and run a file
    if (argc >= 2) {
        simple_file(argv[1]);
    }

    call_func("test", "square", 10);

    // Free up everything
    if (Py_FinalizeEx() < 0) {
        return 2;
    }
    // Free the wchar_* -> could this have been done after using it?
    PyMem_RawFree(program);
}
