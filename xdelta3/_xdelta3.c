#define NOT_MAIN 1

#include "xdelta3.h"
#include "xdelta3.c"
#include <Python.h>

static PyObject *NoDeltaFound;
static PyObject *XDeltaError;

static PyObject * xdelta3_execute(PyObject *self, PyObject *args)
{
  uint8_t *input_bytes = NULL, *source_bytes = NULL, *output_buf = NULL;
  int input_len, source_len, flags, action, result;
  size_t input_size, source_size, output_alloc, output_size;

  if (!PyArg_ParseTuple(args, "s#s#ii", &input_bytes, &input_len, &source_bytes, &source_len, &flags, &action))
    return NULL;

  source_size = (size_t)source_len;
  input_size = (size_t)input_len;

  if (action == 0) {
    // if the output would be longer than the input itself, there's no point using delta encoding
    output_alloc = input_size;
    output_buf = malloc(output_alloc);
    result = xd3_encode_memory(input_bytes, input_size, source_bytes, source_size,
        output_buf, &output_size, output_alloc, flags);
  } else {
    // output shouldn't be bigger than the original plus the delta, but give a little leeway
    output_alloc = input_size + source_size * 11 / 10;
    output_buf = malloc(output_alloc);
    result = xd3_decode_memory(input_bytes, input_size, source_bytes, source_size,
        output_buf, &output_size, output_alloc, flags);
  }

  if (result == 0) {
    PyObject *ret = Py_BuildValue("s#", output_buf, output_size);
    free(output_buf);
    return ret;
  }

  if(result == ENOSPC) {
    if (action == 0) {
      // all is well, just not efficient delta could be found
      PyErr_SetString(NoDeltaFound, "No delta found shorter than the input value");
    } else {
      PyErr_SetString(XDeltaError, "Output of decoding delta longer than expected");
    }
  } else {
    char exc_str[80];
    sprintf(exc_str, "Error occur executing xdelta3: %s", xd3_strerror(result));
    PyErr_SetString(XDeltaError, exc_str);

  }
  free(output_buf);
  return NULL;
}

static PyObject * xdelta3_version(PyObject *self, PyObject *args)
{
  int result = 3;
  PyObject *ret = Py_BuildValue("i", result);
  return ret;
}

static PyMethodDef xdelta3_methods[] = {
  {"execute",  xdelta3_execute, METH_VARARGS, "xdelta3 encode or decode"},
  {"version",  xdelta3_version, METH_VARARGS, "print xdelta3 version info"},
  {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef xdelta3_module = {
  PyModuleDef_HEAD_INIT,
  "_xdelta3",
  NULL,
  0,
  xdelta3_methods
};
#endif


PyMODINIT_FUNC init_xdelta3(void) {
  PyObject *m;

#if PY_MAJOR_VERSION >= 3
  m = PyModule_Create(&xdelta3_module);
  if (m == NULL)
    return NULL;
#else
  m = Py_InitModule3("_xdelta3", xdelta3_methods, "This is a xdelta3 module.");
#endif

  XDeltaError = PyErr_NewException("xdelta3.XDeltaError", NULL, NULL);
  Py_INCREF(XDeltaError);
  PyModule_AddObject(m, "XDeltaError", XDeltaError);

  NoDeltaFound = PyErr_NewException("xdelta3.NoDeltaFound", NULL, NULL);
  Py_INCREF(NoDeltaFound);
  PyModule_AddObject(m, "NoDeltaFound", NoDeltaFound);
#if PY_MAJOR_VERSION >= 3
  return m;
#else
  return;
#endif
}
