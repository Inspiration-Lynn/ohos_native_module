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

        console.info('[HIT] get or create local dir callback');
        var context = ability_featureAbility.getContext();
        context.getOrCreateLocalDir((error, data)=>{
            if (error) {
                console.error('[HIT]Operation failed. Cause: ' + JSON.stringify(error));
                return;
            }
            console.info('[HIT]Operation successful. Data:' + JSON.stringify(data))
        })

    }
};