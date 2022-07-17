#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "artag.h"

static napi_value GetBackgroundImage(napi_env env, napi_callback_info info)
{
    HILOG_INFO("[HIT] [artag] GetBackgroundImage start");   
    int res = getBackground(); 
    napi_value result;
    NAPI_CALL(env, napi_create_int64(env, res, &result));
    HILOG_INFO("[HIT] [artag] GetBackgroundImage end");   

    return result;
}

static napi_value GetBackgroundImageFromFile(napi_env env, napi_callback_info info)
{
    HILOG_INFO("[HIT] [artag] GetBackgroundImageFromFile start");   
    // size_t requireArgc = 1;
    // size_t argc = 1;
    // napi_value argv[1] = { nullptr };
    // NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");

    // // check parameter's type, expect 1 string
    // napi_valuetype valuetype0;
    // NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));
    // NAPI_ASSERT(env, valuetype0 == napi_string, "Wrong argument type. A Number expected.");

    // napi_get_value_string_utf8(env, argv[0], filePath, KEY_BUFFER_SIZE, NULL);

    int res = getBackgroundFromFile(); 
    napi_value result;
    NAPI_CALL(env, napi_create_int64(env, res, &result));
    HILOG_INFO("[HIT] [artag] GetBackgroundImageFromFile end");   

    return result;
}

static napi_value ArtagLocating(napi_env env, napi_callback_info info)
{
    HILOG_INFO("[HIT] [artag] ArtagLocating start");   

    // recieve frame image from udp and decode to mat
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
        DECLARE_NAPI_FUNCTION("getBackgroundImage", GetBackgroundImage),
        DECLARE_NAPI_FUNCTION("getBackgroundImageFromFile", GetBackgroundImageFromFile),
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