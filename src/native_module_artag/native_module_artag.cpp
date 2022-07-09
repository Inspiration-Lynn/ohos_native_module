#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "artag.h"

static napi_value ArtagLocating(napi_env env, napi_callback_info info)
{
    HILOG_INFO("[HIT] [artag] ArtagLocating start");   

    // read background and detect frame from "/data/storage/el2/base/haps/artag/"
    // write output to "/data/storage/el2/base/haps/artag/"
    std::string detectString = locating();

    napi_value detectObj = nullptr;
    napi_create_string_utf8(env, detectString.c_str(), detectString.length(), &detectObj);   

    HILOG_INFO("[HIT] [artag] ArtagLocating end");   

    return detectObj;
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