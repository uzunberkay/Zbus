#include "tim_t.h"
#include "wq.h"
#include "adc_t.h"
#include "zephyr/logging/log.h"
#include "zephyr/init.h"
#include <zephyr/kernel.h>
#include<zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(tim_t, LOG_LEVEL_DBG);


time_date_t tim ={0};





ZBUS_OBS_DECLARE(coap_data_subscriber);
// Zaman verisi için bir zbus kanalı tanımlıyoruz
ZBUS_CHAN_DEFINE(time_date,                                     // Kanal adı
                 time_date_t,                                   // Kanalda taşınacak veri türü
                 NULL,                                          // Publisher (Yayıncı)
                 NULL,                                          // Subscriber (Abone)
                 ZBUS_OBSERVERS(coap_data_subscriber), 
                 ZBUS_MSG_INIT(0)                               // Başlangıç durumu
);

void workH_timer(struct k_work *item);
void timer_func_cb(struct k_timer *dummy);
void adc_tim_func(struct k_timer *dummy);

extern struct k_work workq_adc;
K_WORK_DEFINE(workq_timer_work, workH_timer);


K_TIMER_DEFINE(workq_time_timer, timer_func_cb, NULL);
K_TIMER_DEFINE(adc_tim, adc_tim_func, NULL);





static int tim_init_func(const struct device *dev)
{
    k_timer_start(&workq_time_timer, K_MSEC(1000), K_MSEC(1000));
    k_timer_start(&adc_tim, K_MSEC(5000), K_MSEC(5000));

    return 0;
}
SYS_INIT(tim_init_func, APPLICATION, 70);


/**
 * @brief Zamanlayıcı işlemi için işleme fonksiyonu.
 * 
 * Bu fonksiyon, zamanlayıcı tetiklendiğinde dakika, saat, gün, ay ve yıl gibi zaman
 * verilerini günceller. Zaman güncellemelerinden sonra zaman bilgisi ZBus üzerinden
 * diğer modüllere iletilir.
 * 
 * @param item Zamanlayıcı tetiklendiğinde iş kuyruğundan gelen iş öğesi.
 */
void workH_timer(struct k_work *item)
{
    
    if(++tim.minute  == 60 )
    {
        tim.minute = 0;
        ++tim.hours;
    }
    if(tim.hours > 24)
    {
        tim.hours = 0 ;
        ++tim.day;
    }
    if(tim.day > 30)
    {
        tim.day =0;
        ++tim.month;
    }
    if(tim.month > 12 )
    {
        tim.month = 0 ; 
        ++tim.year;
    }
    LOG_DBG("Year = %d, Month = %d, Day = %d, Hours = %d, Minute = %d, Seconds = %d\n", 
                        tim .year, tim.month, tim.day, tim.hours, tim.minute, tim.seconds);

    zbus_chan_pub(&time_date,&tim,K_NO_WAIT);

}



/**
 * @brief Zamanlayıcı callback fonksiyonu.
 * 
 * Bu fonksiyon, zamanlayıcı her tetiklendiğinde saniyeyi günceller ve
 * dakika değiştiğinde iş kuyruğuna `workq_timer_work` ekler.
 */
void timer_func_cb(struct k_timer *dummy)
{
    
   if(++tim.seconds == 60)
    {
        tim.seconds = 0 ;
        k_work_submit_to_queue(&wq_brief,&workq_timer_work);

    }

}


/**
 * @brief ADC zamanlayıcı callback fonksiyonu.
 * 
 * Bu fonksiyon, 5 saniyelik aralıklarla ADC işini kuyruğa ekler.
 */
void adc_tim_func(struct k_timer *dummy)
{
    k_work_submit_to_queue(&wq_brief,&workq_adc);

}
