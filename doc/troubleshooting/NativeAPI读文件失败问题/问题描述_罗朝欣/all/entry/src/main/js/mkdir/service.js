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

        console.info('[HIT]mkdir /yolo/testdir callback')
        await fileio.mkdir('/yolo/testdir', function(err) {
            if (!err) {
                // do something
                console.info('[HIT]create dir success')
                return;
            }
            console.error('[HIT]create dir failed. err:' + err.message)
        });
    }
};