//import yolo from '@ohos.yolo'
import artag from '@ohos.artag'

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

//        artag定位
        let coor = artag.artagLocating();
        console.info('[HIT] coordinate is: ' + coor);

//         yolo
//        let detectObj = yolo.yoloDetect(0.5);
//        console.info('[HIT] detectObj string： ' + detectObj);
//
//        // parse string
//        console.info('[HIT] parse object string');
//        let objString = detectObj.slice(0, -1);
//
//        const objArray = objString.split("|");   // [ 'person:241,375', 'person:301,366' ]
//        // console.log(objArray);
//
//        let parseArray = [];
//        for (let i=0; i < objArray.length; i++) {
//            // console.log(objArray[i]);
//            let items = objArray[i].split(":");
//            let centerPosition = items[1].split(",");
//            parseArray.push({
//                label: items[0],
//                position: {
//                    x: parseInt(centerPosition[0]),
//                    y: parseInt(centerPosition[1])
//                }
//            })
//        }
//
//        console.info('[HIT] parseArray： ' + parseArray);
//        console.info('[HIT] parseArray[0] label： ' + parseArray[0].label);
//        console.info('[HIT] parseArray[0] center_x： ' + parseArray[0].position.x);
//        console.info('[HIT] parseArray[0] center_y： ' + parseArray[0].position.y);

        console.info('[HIT]ServiceAbility onCommand end');
    }
};