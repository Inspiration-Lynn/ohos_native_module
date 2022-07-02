#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "yolo.h"

// 1 parameter: percentage
static napi_value YoloDetect(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 1;
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");

    // check parameter's type, expect 1 number
    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));

    NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. A Number expected.");

    // convert parameters from N-API to C types
    double percentage;
    NAPI_CALL(env, napi_get_value_double(env, argv[0], &percentage));

    // 1: read image from file
    // char buffer1[128];
    // size_t copied1;
    // NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], buffer1, sizeof(buffer1), &copied1));
    // std::string imgPath = buffer1;

    HILOG_INFO("[HIT] [yolo] 1");   // 1
    std::string obj = detect(percentage);
    HILOG_INFO("[HIT] In NAPI: obj is %{public}s", obj.c_str());
    HILOG_INFO("[HIT] [yolo] detect finished");   

    napi_value detectObj = nullptr;
    napi_create_string_utf8(env, obj.c_str(), obj.length(), &detectObj);   

    return detectObj;
}

static napi_value YoloExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("yoloDetect", YoloDetect),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// yolo module description
static napi_module yoloModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = YoloExport,
    .nm_modname = "yolo",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

// yolo module register
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&yoloModule);
}