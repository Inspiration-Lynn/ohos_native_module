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
        console.info('[HIT]fopen promise start')
        let fd = await fileio.open('/yolo/test.txt', 0o1, 0o0200);
        console.info('[HIT] fopen promise result is fd: ' + fd.toString())
    }
};