#include <math.h>
#include <stdint.h>
#include "sqlite3ext.h"

SQLITE_EXTENSION_INIT1

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif
#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

#define INT_TO_PTR(X) ((void*)(intptr_t)(X))
#define PTR_TO_INT(X) ((int)(intptr_t)(X))


static void
get_pi(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  sqlite3_result_double(context, M_PI);
}


static double
radians(double x) {
    return x * (M_PI / 180.0);
}


static double
degrees(double x) {
    return x * (180.0 / M_PI);
}


static double
cot(double x) {
    return cos(x) / sin(x);
}


static void
wrap_floor_ceil(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  switch (sqlite3_value_numeric_type(argv[0])) {
    case SQLITE_INTEGER: {
       sqlite3_result_int64(context, sqlite3_value_int64(argv[0]));
       break;
    }
    case SQLITE_FLOAT: {
       double (*func)(double) = (double(*)(double))sqlite3_user_data(context);
       sqlite3_result_double(context, func(sqlite3_value_double(argv[0])));
       break;
    }
    default: {
       break;
    }
  }
}


static void wrap_log(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  double x, b, res;
  switch (sqlite3_value_numeric_type(argv[0])) {
    case SQLITE_INTEGER:
    case SQLITE_FLOAT:
      x = sqlite3_value_double(argv[0]);
      if (x <= 0.0) return;
      break;
    default:
      return;
  }
  if (argc == 2) {
    switch (sqlite3_value_numeric_type(argv[0])) {
      case SQLITE_INTEGER:
      case SQLITE_FLOAT:
        b = log(x);
        if (b <= 0.0) return;
        x = sqlite3_value_double(argv[1]);
        if (x <= 0.0) return;
        break;
     default:
        return;
    }
    res = log(x)/b;
  } else {
    res = log(x);
    switch (PTR_TO_INT(sqlite3_user_data(context))) {
      case 1:
        res /= M_LN10;
        break;
      case 2:
        res /= M_LN2;
        break;
      default:
        break;
    }
  }
  sqlite3_result_double(context, res);
}


static void
wrap_d_d(
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
) {
    int type;
    double val, res;
    double (*func)(double);

    type = sqlite3_value_numeric_type(argv[0]);
    if (type != SQLITE_INTEGER && type != SQLITE_FLOAT) return;

    val = sqlite3_value_double(argv[0]);
    func = (double(*)(double))sqlite3_user_data(context);
    res = func(val);
    sqlite3_result_double(context, res);
}


static void
wrap_dd_d(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  int type0, type1;
  double v0, v1, res;
  double (*func)(double, double);

  type0 = sqlite3_value_numeric_type(argv[0]);
  if (type0 != SQLITE_INTEGER && type0 != SQLITE_FLOAT) return;

  type1 = sqlite3_value_numeric_type(argv[1]);
  if (type1 != SQLITE_INTEGER && type1 != SQLITE_FLOAT) return;

  v0 = sqlite3_value_double(argv[0]);
  v1 = sqlite3_value_double(argv[1]);
  func = (double(*)(double, double))sqlite3_user_data(context);
  res = func(v0, v1);
  sqlite3_result_double(context, res);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
int
sqlite3_xsqlitemath_init(
    sqlite3* db,
    char** pzErrMsg,
    const sqlite3_api_routines *pApi
) {
    SQLITE_EXTENSION_INIT2(pApi);

    const int flags = SQLITE_UTF8 | SQLITE_INNOCUOUS | SQLITE_DETERMINISTIC;

#define FUNCTION(name, nargs, data, func) do { \
    int status = sqlite3_create_function_v2( \
        db, name, nargs, flags, data, func, 0, 0, 0 \
    ); \
    if (status != SQLITE_OK) return status; \
} while (0);

    FUNCTION("ceil", 1, ceil, wrap_floor_ceil);
    FUNCTION("ceiling", 1, ceil, wrap_floor_ceil);
    FUNCTION("floor", 1, floor, wrap_floor_ceil);
    FUNCTION("trunc", 1, trunc, wrap_floor_ceil);
    FUNCTION("ln", 1, 0, wrap_log);
    FUNCTION("log", 1, INT_TO_PTR(1), wrap_log);
    FUNCTION("log", 2, 0, wrap_log);
    FUNCTION("log10", 1, INT_TO_PTR(1), wrap_log);
    FUNCTION("log2", 1, INT_TO_PTR(2), wrap_log);
    FUNCTION("exp", 1, exp, wrap_d_d);
    FUNCTION("pow", 2, pow, wrap_dd_d);
    FUNCTION("power", 2, pow, wrap_dd_d);
    FUNCTION("mod", 2, fmod, wrap_dd_d);
    FUNCTION("acos", 1, acos, wrap_d_d);
    FUNCTION("asin", 1, asin, wrap_d_d);
    FUNCTION("atan", 1, atan, wrap_d_d);
    FUNCTION("atan2", 2, atan2, wrap_dd_d);
    FUNCTION("cos", 1, cos, wrap_d_d);
    FUNCTION("sin", 1, sin, wrap_d_d);
    FUNCTION("tan", 1, tan, wrap_d_d);
    FUNCTION("cot", 1, cot, wrap_d_d);
    FUNCTION("acosh", 1, acosh, wrap_d_d);
    FUNCTION("asinh", 1, asinh, wrap_d_d);
    FUNCTION("atanh", 1, atanh, wrap_d_d);
    FUNCTION("cosh", 1, cosh, wrap_d_d);
    FUNCTION("sinh", 1, sinh, wrap_d_d);
    FUNCTION("tanh", 1, tanh, wrap_d_d);
    FUNCTION("sqrt", 1, sqrt, wrap_d_d);
    FUNCTION("cbrt", 1, cbrt, wrap_d_d);
    FUNCTION("radians", 1, radians, wrap_d_d);
    FUNCTION("degrees", 1, degrees, wrap_d_d);
    FUNCTION("pi", 1, 0, get_pi);

#undef FUNCTION

    return SQLITE_OK;
}
