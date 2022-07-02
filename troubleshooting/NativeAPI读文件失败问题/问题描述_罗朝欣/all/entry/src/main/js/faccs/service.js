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
        try {
            fileio.accessSync('/yolo/test.txt');
        } catch(e) {
            //不符合条件则进入
            console.error('[HIT] accessSync error.')
        }
        console.info('[HIT]access end')
    }
};