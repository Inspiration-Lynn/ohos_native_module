import ability_featureAbility from '@ohos.ability.featureAbility'

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
        console.info('[HIT] get cache dir callback');

        var context = ability_featureAbility.getContext();
        context.getCacheDir((error, data)=>{
            if (error) {
                console.error('[HIT]Failed to obtain the cache directory. Cause: ' + JSON.stringify(error));
                return;
            }
            console.info('[HIT]Cache directory obtained. Data: ' + JSON.stringify(data))
        })
    }
};