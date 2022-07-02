# JS-yoloDetect接口文档

```javascript
yoloDetect(percentage): detectObj;
```

yoloDetect()方法调用摄像头进行拍照，并对照片进行Yolo目标检测，筛掉置信度小于参数percentage的目标，得到每个检测目标的标签与中心位置坐标，拼接为特定格式的字符串返回。

## 参数

`percentage`

​	number类型，取值范围为[0,1]。当一个目标的检测结果置信度小于阈值percentage时被筛掉。

## 返回值

`detectObj`

​	string类型。由所有检测结果的标签与中心位置坐标拼接成的特定格式字符串，如"person:241,375|person:301,366|"。

## 检测结果字符串解析

检测结果字符串（yoloDetect方法的返回值）的解析工作在JS代码中实现，构建为对象数组。

### 每个检测到的目标对象

```javascript
{
	label: person,
    position: {
        x: center_x,
        y: center_y
    }
}
```

### JS侧字符串解析代码

```javascript
// parse string "person:241,375|person:301,366|"
let objString = detectObj.slice(0, -1);  // "person:241,375|person:301,366"
const objArray = objString.split("|");   // [ 'person:241,375', 'person:301,366' ]

// 构建对象数组
let parseArray = [];
for (let i=0; i < objArray.length; i++) {
    let items = objArray[i].split(":");
    let centerPosition = items[1].split(",");
    parseArray.push({
        label: items[0],
        position: {
            x: parseInt(centerPosition[0]),
            y: parseInt(centerPosition[1])
        }
    })
}
```

### 遍历打印每个检测目标的标签和位置坐标

```javascript
for (let i=0; i < objArray.length; i++) {
    console.info('[HIT] label: ' + parseArray[i].label);           
    console.info('[HIT] center_x: ' + parseArray[i].position.x);   
    console.info('[HIT] center_y: ' + parseArray[i].position.y);   
}
```

## 完整示例代码

HAP-Service Ability service.js示例代码：

```javascript
import yolo from '@ohos.yolo'   // 导入yolo模块

export default {
    async onStart(want)
    {
        console.info('[HIT]ServiceAbility onStart');
        console.info('[HIT]ServiceAbility onStart end');
    },
    onStop()
    {
        console.info('[HIT]ServiceAbility onStop');
    },
    onConnect(want)
    {
        console.info('[HIT]ServiceAbility onConnect');
        return {};
    },
    onReconnect(want)
    {
        console.info('[HIT]ServiceAbility onReconnect');
    },
    onDisconnect()
    {
        console.info('[HIT]ServiceAbility onDisconnect');
    },
    onCommand(want, restart, startId)
    {
        console.info('[HIT]ServiceAbility onCommand');

        // yolo 
        let detectObj = yolo.yoloDetect(0.5);    // detectObj = "person:241,375|person:301,366|"
        console.info('[HIT] detectObj string： ' + detectObj);   

        // parse string
        let objString = detectObj.slice(0, -1);  // "person:241,375|person:301,366"
        const objArray = objString.split("|");   // [ 'person:241,375', 'person:301,366' ]

        // 构建对象数组
        let parseArray = [];
        for (let i=0; i < objArray.length; i++) {
            let items = objArray[i].split(":");
            let centerPosition = items[1].split(",");
            parseArray.push({
                label: items[0],
                position: {
                    x: parseInt(centerPosition[0]),
                    y: parseInt(centerPosition[1])
                }
            })
        }

        // 打印验证
        console.info('[HIT] parseArray[0] label: ' + parseArray[0].label);           // person
        console.info('[HIT] parseArray[0] center_x: ' + parseArray[0].position.x);   // 241
        console.info('[HIT] parseArray[0] center_y: ' + parseArray[0].position.y);   // 375

        console.info('[HIT]ServiceAbility onCommand end');
    }
};
```

