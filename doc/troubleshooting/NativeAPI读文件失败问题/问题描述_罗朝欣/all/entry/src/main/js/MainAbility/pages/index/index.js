import router from '@system.router'

export default {
    data: {
        title: ""
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
    onclick: function () {
        router.replace({
            uri: "pages/second/second"
        })
    }
}



