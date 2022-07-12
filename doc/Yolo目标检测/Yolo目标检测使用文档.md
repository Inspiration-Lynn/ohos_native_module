# Yolo目标检测使用文档

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

[Yolo NAPI源码](https://github.com/Inspiration-Lynn/ohos_native_module/tree/main/src/native_module_yolo)

服务器位置：/data/muyang/OHOS_3.1_release/OHOS/foundation/ace/napi/sample/native_module_yolo

NAPI动态库编译：

在/data/muyang/OHOS_3.1_release/OHOS/目录下，编译napi模块，产品名为rk3568：

```bash
sudo ./build.sh --product-name rk3568 --build-target "napi_packages_test"
```

编译出来的动态库名为libyolo.z.so，位置为/data/muyang/OHOS_3.1_release/OHOS/out/rk3568/ace/napi/

将其拷贝到系统/system/lib64/module/路径下

### 2- HAP包构建

> 提供示例工程文件MyApplication，bundle name为napi.test，ability name为napi.test.ServiceAbility

选择编译类型为release，首先Clean Project，然后Build Haps。

构建完成的HAP包路径为\\\build\outputs\hap\release\phone\entry-release-standard-ark-signed.hap，将hap拷贝到ohos系统中

### 3- 将Yolo所需文件放进HAP可访问路径

> 在ohos3.1中引入了HAP沙盒机制，HAP应用无法访问系统目录，无法读写文件
>
> 当前3.1 Release版本的沙箱代码还不完善，所以在退出程序之后并没有清理沙箱路径，所以可以通过手动放置文件的方式来加载所需要的文件
>
> 实际HAP包安装后的系统工作目录 /data/app/el2/100/base/napi.test/haps （物理路径）
>
> HAP中对应访问的沙箱路径 /data/storage/el2/base/haps/

将yolo文件夹拷贝进/data/app/el2/100/base/napi.test/haps/路径下

将yolo文件夹的用户和用户组修改为与HAP相同，确保其本身可以访问

```bash
chown -R 20010033:20010033 yolo
```

### 4- 测试

Rock 3a上电开机

```bash
bm install -p yolo-yes.hap
hilog -r
aa start -a napi.test.ServiceAbility -b napi.test   // aa start -a <ability-name> -b <bundle-name>
hilog | grep HIT
```

