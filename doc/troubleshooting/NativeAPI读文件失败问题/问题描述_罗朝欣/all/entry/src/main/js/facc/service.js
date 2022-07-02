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
        console.info('[HIT]file access start')
        fileio.access('/yolo/test.txt', function (err) {
            if (err){
                console.error('[HIT] access failed.')
                return;
            }
            console.info("[HIT] access success")
        });
        console.info('[HIT]file access end')
    }
};