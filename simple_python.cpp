#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == nullptr) {
        cerr << "PyDecodeLocale(argv[0]) failed" << endl;
        return 1;
    }

    Py_SetProgramName(program);
    Py_Initialize();
    PyRun_SimpleString("print('hello')\n");

    if (argc >= 2) {
        FILE *script = fopen(argv[1], "r");
        PyRun_SimpleFile(script, argv[1]);
    }


    if (Py_FinalizeEx() < 0) {
        return 2;
    }
    PyMem_RawFree(program);
}
