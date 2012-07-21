#include <stdlib.h>
#include "Python.h"
#include "frameobject.h"
#include "serial.h"
#include "dump.h"
#include "defs.h"

#define MODULE_DOC PyDoc_STR("C extension for fast function tracing.")

static int
trace_func(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
  if (!should_trace_module(frame)) {
    return NULL;
  }

  switch (what) {
  case PyTrace_CALL:
    handle_call(frame);
    break;
  case PyTrace_RETURN:
    handle_return(frame, arg);
    break;
  case PyTrace_EXCEPTION: // setprofile translates exceptions to calls
    handle_exception(frame, arg);
  }
  return NULL;
}

static PyListObject *filter_modules = NULL;

int should_trace_module(PyFrameObject *frame) {
  int i;
  char *filter, *module;

  if (NULL == filter_modules) {
    return TRUE;
  }
  module = PyString_AsString(frame->f_code->co_filename);
  for (i = 0; i < PyList_Size(filter_modules); i++) {
    filter = PyString_AsString(PyList_GetItem(filter_modules, i));
    if (0 == strncmp(module, filter, strlen(filter))) {
      return TRUE;
    };
  }
  return FALSE;
}

static PyObject*
set_filter_modules(PyObject *self, PyObject *args) {
  if (NULL != filter_modules) {
    Py_DECREF(filter_modules);
  }
  if (!PyArg_ParseTuple(args, "|O!", &PyList_Type, &filter_modules)){
    return;
  }
  if (NULL != filter_modules) {
    Py_INCREF(filter_modules);
  }
  return Py_BuildValue("");
}

static PyObject*
install(PyObject *self, PyObject *args) {
  PyEval_SetTrace((Py_tracefunc) trace_func, (PyObject*) self);
  return Py_BuildValue("");
}

static PyObject*
start_dumper(PyObject *self, PyObject *args) {
  dump_main_in_thread();
}

static PyObject*
stop_dumper(PyObject *self, PyObject *args) {
  dump_stop();
}

static PyObject*
uninstall(PyObject *self, PyObject *args)
{
  PyEval_SetTrace(NULL, NULL);
  return Py_BuildValue("");
}

static PyMethodDef
methods[] = {
  {"start_dumper", (PyCFunction) start_dumper, METH_VARARGS, PyDoc_STR("Start the dumper")},
  {"stop_dumper", (PyCFunction) stop_dumper, METH_VARARGS, PyDoc_STR("Stop the dumper")},
  {"install", (PyCFunction) install, METH_VARARGS, PyDoc_STR("The trace function")},
  {"uninstall", (PyCFunction) uninstall, METH_VARARGS, PyDoc_STR("The trace function")},
  {"set_filter_modules", (PyCFunction) set_filter_modules, METH_VARARGS, PyDoc_STR("Set the package names to trace")},
  {NULL}
};

void
inittracer(void)
{
  init_serialize();
  Py_InitModule3("pytrace.tracer", methods, MODULE_DOC);
}