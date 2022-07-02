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
    onCommand(want, restart, startId) {
        console.info('ServiceAbility onCommand');

        console.info('[HIT] access /yolo/test.txt')
        fileio.access('/yolo/test.txt')
            .then(function(err) {
                console.info('[HIT] access file success.')
                // 文件存在,do something
            }).catch(function(e) {
            //若不符合条件则进入
            console.info('[HIT] access file failed.')
        });
        console.info('[HIT] access end')
    }
};