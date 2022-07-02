#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "utils/log.h" // hilog调试用

namespace fs = std::filesystem;

using namespace cv;
using namespace std;

static napi_value FileTest(napi_env env, napi_callback_info info)
{
    // test 1: read image
    Mat image = imread("/data/bus.jpg"); 
    // Error Handling
    if (image.empty()) {
        HILOG_INFO("[HIT] Image not found");    // service ability输出这句
        return 0;
    }
    HILOG_INFO("[HIT] Image found");

    // create and write file
    // fstream my_file;
	// my_file.open("my_file", ios::out);
	// if (!my_file) {
    //     HILOG_INFO("[HIT] File not created!");   // 执行到这
	// }
	// else {
    //     HILOG_INFO("[HIT] File created successfully!");
    //     my_file << "write file";
	// 	my_file.close(); 
	// }

    // 无法执行
    // string image_path = samples::findFile("my_file");
    // HILOG_INFO("[HIT] my_file path is %{public}s", image_path.c_str());


    // test 2: read file
    fstream my_file1;
	my_file1.open("./my_file", ios::in);
	if (!my_file1) {
        HILOG_INFO("[HIT] No such file");
	}
	else {
        HILOG_INFO("[HIT] Open Successfully");
		string ch;

		while (1) {
			my_file1 >> ch;
			if (my_file1.eof())
				break;

            HILOG_INFO("[HIT] %{public}s", ch.c_str());
		}

	}
	my_file1.close();

    return 0;
}


static napi_value FileExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("file_test", FileTest),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// file module description
static napi_module fileModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = FileExport,
    .nm_modname = "file",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

// file module register
extern "C" __attribute__((constructor)) void FileRegisterModule(void)
{
    napi_module_register(&fileModule);
}