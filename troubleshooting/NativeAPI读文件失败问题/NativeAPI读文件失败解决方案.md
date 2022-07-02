# 读写文件问题解决

## ohos谭老师解答

问题1 ：NAPI无法正常访问系统路径下文件
在3.1release系统和3.2 Beta系统中，应用运行时只能访问应用沙箱内的文件，自定义的NAPI也无法访问沙箱外的路径。

问题2：JS API无法正常访问上下文及系统路径
在3.1release系统下，ServiceAbility中使用featureAbility.getContext获取应用上下文为null，因此导致获取路径失败，featureAbility.getContext可以在PageAbility中使用获取到应用的沙箱路径，验证可以正常使用。
然后使用fileio来操作文件和目录时，path入参均为沙箱路径。



可以在\entry\src\main\js\MainAbility\pages\index\index.js文件中通过@ohos.ability.featureAbility来获取应用沙箱路径，这个路径一般是默认固定的，代码如下：

```javascript
import featureAbility from '@ohos.ability.featureAbility';
var context = featureAbility.getContext();
context.getOrCreateLocalDir()
.then((data) => {
// 沙箱路径：/data/storage/el2/base/haps/entry
console.info('[HIT] promise getOrCreateLocalDir successful. Data: ' + JSON.stringify(data));
}).catch((error) => {
console.error('[HIT] promise getOrCreateLocalDir failed. Cause: ' + JSON.stringify(error));
})
```


日志如下：

```bash
08-05 17:01:35.400 22266 22275 I 03b00/JSApp: app Log: [HIT] promise getOrCreateLocalDir successful. Data: "/data/storage/el2/base/haps/entry"
```

然后在fileio进行文件操作时，需将沙箱路径作为前缀路径拼接，例如在\entry\src\main\js\mkdirp\service.js中使用fileio.mkdir创建文件夹：

```javascript
import fileio from '@ohos.fileio';
import fileio from '@ohos.fileio';

export default {
    onStart(want) {
    console.info('ServiceAbility onStart');
},
onStop() {
	console.info('ServiceAbility onStop');
},
onConnect(want) {
	console.info('ServiceAbility onConnect');
	return {};
},
onReconnect(want) {
	console.info('ServiceAbility onReconnect');
},
onDisconnect() {
	console.info('ServiceAbility onDisconnect');
},
async onCommand(want, restart, startId) {
    console.info('ServiceAbility onCommand');
    console.info('[HIT]mkdir /data/storage/el2/base/haps/entry/yolo promise')
    await fileio.mkdir('/data/storage/el2/base/haps/entry/yolo').then(function(err) {
        // 目录创建成功，do something
        console.info('[HIT] create dir success')
    }).catch(function (e){
        console.error('[HIT] create dir failed. err:' + e.message)
    	});
    }
};
```

日志如下：

```bash
08-05 17:29:41.825 23820 23968 I 03b00/JSApp: app Log: [HIT] create dir success
```

通过上述步骤就可以明白在ServiceAbility中使用fileio进行文件操作，应用沙箱路径一般是固定的值'/data/storage/el2/base/haps/entry'，沙箱路径映射的物理路径为 /data/app/el2/100/base/XXXXX/haps/entry，其中“XXXXX”代表的是应用的bundleName，示例：/data/app/el2/100/base/napi.test/haps/entry

# Native API读文件失败解决方案

## 环境
- OS：OpenHarmony3.1 release
- 设备：rk3568

## 问题原因

Release3.1,3.2bate 引入了应用沙箱机制，文件系统只能访问本应用目录下的文件夹。

## 解决方案

将napi需要访问的文件资源，存储到应用沙箱中，napi访问文件的接口，访问对应应用目录下的文件。

将需要解析的文件资源通过fileio接口，存储到应用沙箱中。napi解析资源文件的接口入参为文件的沙箱路径。

### fileio 使用：

#### api8 FA模式

- 通过沙盒路径获取文件：

```javascript
import fileio from '@ohos.fileio';
import featureAbility from "@ohos.ability.featureAbility"

private async openText() {
let str = await featureAbility.getContext().getFilesDir()
console.log('FILE-DEMO openText fd:' + str)
let fd = await fileio.open(`${str}/book.txt`)
console.log('FILE-DEMO fd:' + JSON.stringify(fd))
}
```

#### api9 Stage模式

- 通过沙盒路径获取文件：

```javascript
private async readSdFile() {
console.log("FILE-DEMO-S : " + globalThis.context.filesDir)
let fd = await fileio.open(globalThis.context.filesDir + "/book.txt")
console.log('FILE-DEMO-S fd:' + JSON.stringify(fd))
}
```
沙盒路径与实际路径映射：

沙盒路径：/data/storage/el2/base/haps/entry/files

实际路径 /data/app/el2/100/base/{应用bundleName}/haps/entry/files

例如：/data/app/el2/100/base/com.example.myapplication_zp/haps/entry/files/

通过hdc命令将文件发送到真实目录下，便可以通过沙盒路径获取。

```
hdc_std file send C:\Users\xxx\Desktop\image\book.txt /data/app/el2/100/base/com.example.myapplication_zp/haps/entry/files/book.txt
```

### napi示例

- napi OpenFile 方法

接口入参为文件的沙箱路径

```cpp
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

static napi_value OpenFile(napi_env env, napi_callback_info info)
{
POWER_HILOGD(MODULE_JS_NAPI, " %{public}s OpenFile star", __func__);
size_t argc = 1;
napi_value argv[1] = { 0 };
napi_value thisVar = nullptr;
void* data = nullptr;
napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

std::string path = GetStringFromNAPI(env, argv[0]);

POWER_HILOGD(MODULE_JS_NAPI, " %{public}s Open file path %{public}s", __func__,path.c_str());

ifstream ifs;
ifs.open(path, ios::in);
if (!ifs.is_open()) {
    POWER_HILOGD(MODULE_JS_NAPI, " %{public}s Open file failed", __func__);
    }else{
    	POWER_HILOGD(MODULE_JS_NAPI, " %{public}s Open file success", __func__);
    }
    POWER_HILOGD(MODULE_JS_NAPI, " %{public}s OpenFile end", __func__);
    ifs.close();
    return nullptr;
}
```



```cpp
static std::string GetStringFromNAPI(napi_env env, napi_value value)
{
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, 0, &size) != napi_ok) {
    	return "";
    }
    result.reserve(size + 1);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + 1), &size) != napi_ok) {
    	return "";
    }
    return result;
}
```

- Demo示例

```javascript
private openFile() {
// 沙盒路径也可以通过上文中示例获取
power.openFile("/data/storage/el2/base/haps/entry/files/book.txt")
}
```

## 朝欣师兄的解决方案

https://blog.luo980.com/tricks/play/ohos%e5%9b%be%e5%83%8f%e6%a3%80%e6%b5%8bhap/