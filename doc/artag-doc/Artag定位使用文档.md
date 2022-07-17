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

1. 将native_module_artag放到//foundation/ace/napi/sample路径下

   可调参数设置见artag.h，包括：

   - OBJECT_MIN_AREA

     待检测目标最小面积；需要根据待检测的物体 & 分辨率进行调整

   - DEST_RESOLUTION_X、DEST_RESOLUTION_Y

     裁切后图片宽度与高度

   - IS_WRITE_IMG

     是否写出检测后图片，若写出图片，写出到系统的/data/storage/el2/base/haps/artag路径下，文件名为out.png（需要提前新建文件夹artag并修改用户与用户组）

2. 将OpenCV动态库与头文件拷贝到编译主机中（见目录//yolo/OpenCV-out）

   修改native_module_artag/BUILD.gn中OpenCV头文件路径以及动态库路径

3. 在//foundation/ace/napi/BUILD.gn中增加artag模块

```BUILD.gn
256+   "sample/native_module_artag:artag",
```

4. NAPI动态库编译

编译napi模块，产品名为rk3568：

```bash
./build.sh --product-name rk3568 --build-target "napi_packages_test"
```

编译出来的动态库名为libartag.z.so，位置为//out/rk3568/ace/napi/

5. 将libartag.z.so拷贝到系统/system/lib64/module/路径下
6. 将OpenCV动态库拷贝到系统/system/lib64/路径下

### 2- HAP包构建

> 提供示例工程文件MyApplication，bundle name为napi.test，ability name为napi.test.ServiceAbility

选择编译类型为release，首先Clean Project，然后Build Haps。

构建完成的HAP包路径为\\\build\outputs\hap\release\phone\entry-release-standard-ark-signed.hap，将hap拷贝到ohos系统中

### 3- 将artag所需文件放进HAP可访问路径

> 在ohos3.1中引入了HAP沙盒机制，HAP应用无法访问系统目录，无法读写文件
>
> 当前3.1 Release版本的沙箱代码还不完善，所以在退出程序之后并没有清理沙箱路径，所以可以通过手动放置文件的方式来加载所需要的文件
>
> 实际HAP包安装后的系统工作目录 /data/app/el2/100/base/napi.test/haps （物理路径）
>
> HAP中对应访问的沙箱路径 /data/storage/el2/base/haps/

将artag文件夹拷贝进/data/app/el2/100/base/napi.test/haps/路径下

- 已做过去畸变处理的场地背景图片background.jpg
- 用于去畸变的相机参数camera_undistortion.yaml

将artag文件夹的用户和用户组修改为与HAP相同，确保其本身可以访问

```bash
chown -R 20010033:20010033 artag     // 具体用户与用户组需查看相应hap
```

### 4- 测试

Rock 3a上电开机

```bash
bm install -p artag.hap
hilog -r
aa start -a napi.test.ServiceAbility -b napi.test   // aa start -a <ability-name> -b <bundle-name>
hilog | grep HIT
```

