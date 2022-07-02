import file from '@system.file';

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

        console.info('[HIT] access to app.')
        file.access({
            uri: 'internal://app/',
            success: function() {
                console.log('[HIT]call access success.');
            },
            fail: function(data, code) {
                console.error('[HIT]call fail callback fail, code: ' + code + ', data: ' + data);
            },
        });
    }
};