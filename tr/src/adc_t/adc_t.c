#include "adc_t.h"
#include<zephyr/zbus/zbus.h>


extern struct k_work workq_coap;
extern struct k_work wq_brief_b1;


LOG_MODULE_REGISTER(adc_t, LOG_LEVEL_DBG);
ZBUS_OBS_DECLARE(coap_data_subscriber);
// ADC verisi için bir zbus kanalı tanımlıyoruz
ZBUS_CHAN_DEFINE(adc_data_chan,                                                 // Kanal adı
                 adc_data_t,                                                   // Kanalda taşınacak veri türü
                 NULL,                                                        // Publisher (Yayıncı)
                 NULL,                                                       // Subscriber (Abone)
                 ZBUS_OBSERVERS(coap_data_subscriber), 
                 ZBUS_MSG_INIT(0)                                            // Başlangıç durumu
);

const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));
int16_t buf;
struct adc_sequence sequence = {
    .buffer = &buf,
    .buffer_size = sizeof(buf),
};

int val_mv ;
void adc_init(void)
{
    int err;
    if (!device_is_ready(adc_channel.dev)) {  // Burada adc_is_ready_dt yerine device_is_ready kullanılıyor
        LOG_ERR("Adc cihazi %s okunamadi ! \n", adc_channel.dev->name);
        return;
    }
    err = adc_channel_setup_dt(&adc_channel);
    if (err < 0) {
        LOG_ERR("Could not setup channel #%d (%d)", 0, err);
        return;
    }
    err = adc_sequence_init_dt(&adc_channel, &sequence);
    if (err < 0) {
        LOG_ERR("Baslatilamadi ! \n");
        return;
    }
    LOG_DBG("Adc islemleri basarili ! \n");
}

static int adc_init_func(const struct device *dev)
{
    adc_init();  // Burada adc_init_func yerine adc_init fonksiyonu çağırılmalı
    LOG_DBG("Func : %s \n", __func__);
    return 0;
}
SYS_INIT(adc_init_func, APPLICATION, 80);

void workH_adc(struct k_work *item)
{
    int err;
    adc_data_t data = {0};
    static unsigned int count = 0; 


    err = adc_read(adc_channel.dev, &sequence);
    if (err < 0) {
        LOG_ERR("Okunamadi , hata no : %d ", err);
        return;
    }
    val_mv = (int)buf;
    LOG_INF("ADC reading[%u]: %s, channel %d: Raw: %d", count++, adc_channel.dev->name, adc_channel.channel_id, val_mv);
    data.adc_value=val_mv;

    zbus_chan_pub(&adc_data_chan,&data,K_NO_WAIT);
    LOG_DBG("adc_data_chan publish\n");

    err = adc_raw_to_millivolts_dt(&adc_channel, &val_mv);
    if (err < 0) {
        LOG_WRN(" (value in mV not available)\n");
    } else {
        LOG_INF(" = %d mV", val_mv);
    }


}

K_WORK_DEFINE(workq_adc, workH_adc);
