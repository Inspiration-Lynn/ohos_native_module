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
        console.info('[HIT] get file dir callback');
        var context = ability_featureAbility.getContext();
        context.getFilesDir((error, data)=>{
            if (error) {
                console.error('[HIT]Failed to obtain the file directory. Cause: ' + JSON.stringify(error));
                return;
            }
            console.info('[HIT]File directory obtained. Data: ' + JSON.stringify(data))
        })
    }
};