#include <stdint.h>
#include "sqlite3ext.h"
#include "re2/re2.h"

extern "C" {

SQLITE_EXTENSION_INIT1

static void free_re(void *x) {
    re2::RE2 *re = (re2::RE2 *)x;
    delete re;
}

static void
regexp(
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
) {
    re2::RE2 *re = (re2::RE2 *)sqlite3_get_auxdata(context, 0);
    if (re == NULL) {
        const unsigned char *c_pattern = sqlite3_value_text(argv[0]);
        int c_pattern_len = sqlite3_value_bytes(argv[0]);
        re2::StringPiece pattern(
            reinterpret_cast<const char *>(c_pattern),
            c_pattern_len
        );

        re = new (std::nothrow) re2::RE2(pattern, re2::RE2::CannedOptions::Quiet);
        if (re == NULL) {
            sqlite3_result_error_nomem(context);
            return;
        }
        if (!re->ok()) {
            sqlite3_result_error(context, re->error().c_str(), -1);
            delete re;
            return;
        }
        sqlite3_set_auxdata(context, 0, re, free_re);
    }

    const unsigned char *c_data = sqlite3_value_text(argv[1]);
    int c_data_len = sqlite3_value_bytes(argv[1]);
    re2::StringPiece data(reinterpret_cast<const char *>(c_data), c_data_len);

    bool res = re->Match(data, 0, data.size(), re2::RE2::UNANCHORED, NULL, 0);
    sqlite3_result_int(context, res);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
int
sqlite3_xsqlitestrings_init(
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

    FUNCTION("regexp", 2, 0, regexp);

#undef FUNCTION

    return SQLITE_OK;
}

}
