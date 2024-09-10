#include"coap_t.h"
#include<stdio.h>
#include<zephyr/init.h>
#include<zephyr/zbus/zbus.h>
#include"adc_t.h"
#include"tim_t.h"
LOG_MODULE_REGISTER(coap_settings, LOG_LEVEL_DBG);

ZBUS_CHAN_DECLARE(adc_data_chan);
ZBUS_CHAN_DECLARE(time_date);
ZBUS_SUBSCRIBER_DEFINE(coap_data_subscriber, 5);



/**
 * @brief CoAP üzerinden sensör verisi gönderme fonksiyonu.
 *
 * Bu fonksiyon, verilen sensör adı ve değerini JSON formatına dönüştürerek CoAP protokolü 
 * üzerinden belirlenen bir URI'ye veri gönderir. Mesaj, non-confirmable (onaysız) bir CoAP 
 * isteği olarak `PUT` metodunu kullanarak iletilir.
 */
void coap_send_data_request(const char *sensor_name, char *sensor_value,char* uri_path)
{
    otError error = OT_ERROR_NONE;
    otMessage *myMessage;
    otMessageInfo myMessageInfo;
    otInstance *myInstance = openthread_get_default_instance();  // OpenThread instansını al
	
	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);             
    uint8_t serverInterFaceID[8] = { 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01};   

    char payload[BUFFERSIZE];  // JSON verisini tutacak tampon
    snprintf(payload, sizeof(payload), "{\"%s\": %s}", sensor_name, sensor_value);  // JSON formatında veriyi oluştur

    do
    {
        myMessage = otCoapNewMessage(myInstance , NULL);  // Yeni bir CoAP mesajı oluştur
        if (myMessage == NULL)
        {
            printk("Failed to allocate message for CoAP Request\n");
            return;
        }

        otCoapMessageInit(myMessage , OT_COAP_TYPE_NON_CONFIRMABLE , OT_COAP_CODE_PUT);  // Mesajı başlat
        error = otCoapMessageAppendUriPathOptions(myMessage , uri_path);  // URI yolunu ekle
        if (error != OT_ERROR_NONE) { break; }

        error = otCoapMessageAppendContentFormatOption(myMessage , OT_COAP_OPTION_CONTENT_FORMAT_JSON);  // İçerik formatını ekle
        if (error != OT_ERROR_NONE) { break; }

        error = otCoapMessageSetPayloadMarker(myMessage);  // Yük işaretleyiciyi ayarla
        if (error != OT_ERROR_NONE) { break; }

        error = otMessageAppend(myMessage , payload , strlen(payload));  // JSON formatındaki veriyi yük olarak ekle
        if (error != OT_ERROR_NONE) { break; }

        // Mesaj bilgilerini ayarla
        memset(&myMessageInfo , 0 , sizeof(myMessageInfo));

        if (error != OT_ERROR_NONE) { 
            printk("Failed to set IP address: %d\n", error);
            break; 
        }
		
        memset(&myMessageInfo , 0 , sizeof(myMessageInfo));
        memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0] , ml_prefix , 8);
        memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8] , serverInterFaceID,8);		

        myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;  // CoAP portunu ayarla

        // CoAP isteğini gönder
        error = otCoapSendRequest(myInstance , myMessage , &myMessageInfo ,  coap_send_data_response_cb , NULL);

    } while (false);

    if (error != OT_ERROR_NONE) {
        printk("Failed to send CoAP Request : %d \n", error);
    } else {
        printk("Coap Send.\n");
    }
}

/**
 * @brief CoAP veri iletimi için geri çağırma fonksiyonu.
 *
 * Bu fonksiyon, bir CoAP mesajının iletimi sonrasında çağrılan geri çağırma (callback) fonksiyonudur.
 * Mesaj iletiminin başarıyla tamamlanıp tamamlanmadığını kontrol eder ve buna göre çıktı verir.
 * Eğer iletim başarılı ise "Delivery confirmed." mesajı konsola yazdırılır, aksi durumda hata kodu ile birlikte
 * "Delivery not confirmed" mesajı gösterilir.
 */
void coap_send_data_response_cb(void *p_context , otMessage *p_message , const otMessageInfo *p_message_info , otError result)
{
    if(result == OT_ERROR_NONE)
    {
        LOG_DBG("Delivery confirmed. \n");

    }else{
        LOG_DBG("Delivery not confirmed : %d \n",result);
    }
}


void coap_init_transmit(void)
{
    otInstance *p_instance=openthread_get_default_instance();
    otError error = otCoapStart(p_instance , OT_DEFAULT_COAP_PORT);         // CoAP sunucusunu başlatır.
    if(error != OT_ERROR_NONE)
    {
        LOG_ERR("Failed to start coap : %d \n", error);
    }
}


/**
 * @brief CoAP iletimini başlatan init fonksiyonu.
 *
 * Bu fonksiyon, sistem başlatılırken CoAP iletim fonksiyonlarını başlatmak için kullanılır.
 * `coap_init_transmit()` fonksiyonunu çağırarak CoAP iletişim altyapısını hazırlar.
 * Bu fonksiyon, Zephyr'in `SYS_INIT()` makrosu aracılığıyla uygulamanın bir parçası
 * olarak, sistemin belirli bir aşamasında otomatik olarak çalıştırılır.
 *
 * @param dev Sistemin başlatma sırasında kullandığı cihaz yapısı (genellikle init fonksiyonlarında kullanılır, ancak burada kullanılmamıştır).
 * 
 * @return Her zaman 0 döndürür. (Başlatma başarılı)
 */
static int coap_init_func(const struct device *dev)
{
    coap_init_transmit();
    LOG_DBG("Coap init...\n");


    return 0;
}
SYS_INIT(coap_init_func, APPLICATION, 70);



/**
 * @brief ZBus subscriber thread'i. Gelen publish mesajlarına göre işlem yapar.
 *
 * Bu fonksiyon, ZBus kanallarında yayınlanan mesajları dinlemek için bir subscriber (abone)
 * thread'idir. İlgili kanaldan gelen mesaj tespit edilir ve buna göre ADC veya zaman
 * bilgisi verisi CoAP üzerinden iletilir. İki ana ZBus kanalı izlenir: `adc_data_chan` ve 
 * `time_date`. Bu kanalların her birinden gelen veriler CoAP mesajı olarak gönderilir.
 *
 * @details
 * - `adc_data_chan` üzerinden gelen veriler CoAP isteği ile "adc_value" olarak gönderilir.
 * - `time_date` kanalından gelen zaman verisi ise "time" olarak CoAP sunucusuna iletilir.
 * - Bu bir subscriber thread'dir ve sürekli olarak `zbus_sub_wait()` ile mesajları dinler.
 */
void coap_zbus (void)
{
    const struct zbus_channel *chan;
    
    while(!zbus_sub_wait(&coap_data_subscriber,&chan,K_FOREVER))
    {
        char message[BUFFERSIZE] = {0};
        if(&adc_data_chan  == chan)
        {
            adc_data_t msg = {0};
            zbus_chan_read(&adc_data_chan,&msg,K_SECONDS(1));
            sprintf(message, "%d",msg.adc_value);
            coap_send_data_request("adc_value", message,"sensor");           
        }
        else if (chan == &time_date)
        {
            time_date_t msg = {0};
            int ret = zbus_chan_read(&time_date, &msg, K_SECONDS(5));
            
            if (ret == 0) {  // Eğer okuma başarılıysa
               

                sprintf(message, "{\"Year\": %d, \"Month\": %d, \"Day\": %d, \"Hours\": %d, \"Minute\": %d, \"Seconds\": %d}}", 
                    msg.year, msg.month, msg.day, msg.hours, msg.minute, msg.seconds);
                coap_send_data_request("time", message,"runtime");
            } else {
                LOG_ERR("Zbus channel read failed with error: %d", ret);
            }
        }


    }


}

K_THREAD_DEFINE(coap_zbus_id,1024,coap_zbus,NULL,NULL,NULL,5,0,0);
