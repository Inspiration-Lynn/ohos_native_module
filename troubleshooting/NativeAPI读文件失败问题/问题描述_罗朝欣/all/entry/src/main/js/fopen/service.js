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
        console.info('[HIT]fopen callback start')
        fileio.open('/yolo/test.txt', 0, function(err, fd) {
            if(err){
                console.error('[HIT] open test.txt error. err:' + err.message)
                return;
            }
            console.info('[HIT] open test.txt success. fd is' + fd.toString())
        });
    }
};