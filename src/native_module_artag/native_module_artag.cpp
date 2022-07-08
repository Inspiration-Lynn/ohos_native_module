#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "artag.h"

static napi_value ArtagLocating(napi_env env, napi_callback_info info)
{
    HILOG_INFO("[HIT] [artag] 1");   

    // read t1.jpg and t2.jpg from "/data/storage/el2/base/haps/artag/"
    // write output to "/data/storage/el2/base/haps/artag"
    locating();

    HILOG_INFO("[HIT] [artag] 8");   

    return NULL;
}


static napi_value ArtagExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("artagLocating", ArtagLocating),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}


// artag module description
static napi_module artagModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = ArtagExport,
    .nm_modname = "artag",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

// artag module register
extern "C" __attribute__((constructor)) void ArtagRegisterModule(void)
{
    napi_module_register(&artagModule);
}