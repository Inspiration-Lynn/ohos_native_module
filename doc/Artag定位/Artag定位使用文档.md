# Artag定位使用文档

## **环境**

- 开发板：Rock 3a 
- OS：Openharmony3.1 Release
- HAP开发
  - IDE：DevEco Studio 3.0 Beta2（3.0.0.800）
  - SDK：OpenHarmony API 7
  - 开发语言：JavaScript
  - Ability类型：Service Ability
- 视觉动态库开发：Native API（C++）

## 步骤

### 1- NAPI动态库编译

服务器路径：/data/muyang/OHOS_3.1_release/OHOS/foundation/ace/napi/sample/native_module_artag

可调参数设置见artag.h，包括：

- OBJECT_MIN_AREA

  待检测目标最小面积；需要根据待检测的物体 & 分辨率进行调整

- DEST_RESOLUTION_X、DEST_RESOLUTION_Y

  裁切后图片宽度与高度

- IS_WRITE_IMG

  是否写出检测后图片，若写出图片，写出到系统的/data/storage/el2/base/haps/artag路径下，文件名为out.png（需要提前新建文件夹artag并修改用户与用户组）

NAPI动态库编译：

在/data/muyang/OHOS_3.1_release/OHOS/目录下，编译napi模块，产品名为rk3568：

```bash
sudo ./build.sh --product-name rk3568 --build-target "napi_packages_test"
```

编译出来的动态库名为libartag.z.so，位置为/data/muyang/OHOS_3.1_release/OHOS/out/rk3568/ace/napi/

将其拷贝到系统/system/lib64/module/路径下

### 2- HAP包构建

> 提供示例工程文件MyApplication，bundle name为napi.test，ability name为napi.test.ServiceAbility

选择编译类型为release，首先Clean Project，然后Build Haps。

构建完成的HAP包路径为\\\build\outputs\hap\release\phone\entry-release-standard-ark-signed.hap，将hap拷贝到ohos系统中

### 4- 测试

Rock 3a上电开机

```bash
bm install -p artag.hap
hilog -r
aa start -a napi.test.ServiceAbility -b napi.test   // aa start -a <ability-name> -b <bundle-name>
hilog | grep HIT
```

