#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "yolo.h"

// 2 parameters: input image path & output image path
static napi_value YoloDetect(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");

    // check parameter's type, expect 2 string
    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));

    napi_valuetype valuetype1;
    NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype1));

    NAPI_ASSERT(env, valuetype0 == napi_string && valuetype1 == napi_string, "Wrong argument type. Strings expected.");

    // convert parameters from N-API to C types
    char buffer1[128];
    size_t copied1;
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], buffer1, sizeof(buffer1), &copied1));
    std::string imgPath = buffer1;

    char buffer2[128];
    size_t copied2;
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[1], buffer2, sizeof(buffer2), &copied2));
    std::string outPath = buffer2;

    HILOG_INFO("[HIT] [yolo] 1");   // 1

    cv::Mat result = detect(imgPath);

    HILOG_INFO("[HIT] [yolo] 8");   // 8
    cv::imwrite(outPath, result);
    HILOG_INFO("[HIT] [yolo] 9");   // 9

    return 0;
}

static napi_value YoloHelloworld(napi_env env, napi_callback_info info)
{
	HILOG_INFO("[HIT] hello world !");
	helloworld();
	HILOG_INFO("[HIT] if success will tell");
	
	return 0;
}


static napi_value YoloExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("yoloDetect", YoloDetect),
	DECLARE_NAPI_FUNCTION("helloworld", YoloHelloworld),
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
