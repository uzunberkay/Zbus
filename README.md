# **Zbus**

Bu proje, **Zephyr RTOS** üzerinde Zbus kullanılarak ADC sensör verileri ve çalışma zamanı bilgilerinin yönetilmesini sağlar. Toplanan veriler, **CoAP** protokolü üzerinden uzak bir sunucuya asenkron olarak iletilir. Proje, modüler yapısı sayesinde kolayca genişletilebilir ve farklı veri kaynakları, sensörler veya protokoller ekleyerek sistemin işlevselliği artırılabilir.

## **Özellikler:**
- **Zbus Kanalı**: ADC verilerini ve zaman bilgilerini yöneten asenkron veri iletişim kanalları.
- **Timer Tetikleme**: ADC sensör verileri, belirli zaman aralıklarıyla timer kullanılarak toplanır.
- **CoAP İletimi**: Zbus üzerinden işlenen veriler, CoAP protokolü ile uzak bir sunucuya gönderilir.
- **Kolay Genişletilebilirlik**: Zbus yapısı sayesinde projeye farklı sensörler, yeni iletişim protokolleri veya ek işlevler kolayca entegre edilebilir.

Proje sadece mevcut fonksiyonları değil, aynı zamanda modüler yapısı sayesinde gelecekteki genişletmelere de açıktır. Sisteme kolayca yeni sensörler veya modüller ekleyebilir ve başka iletişim protokolleriyle entegre edebilirsiniz.

## **Kurulum ve Kullanım:**

Bu projede **nRF Connect SDK v2.3.0** kullanılmıştır. Kart seçimi olarak **nRF52840 DK** (nrf52840dk_nrf52840) kullanılmış ve **nrf52840dk_nrf52840.overlay** dosyası eklenmiştir. Başka bir geliştirme kartı kullanmak isterseniz, lütfen kartınıza uygun bir **overlay** dosyası ekleyin ve build yapılandırmasını buna göre ayarlayın.

### **Proje Dosya Yapısı:**

```plaintext
├── src/
│   ├── adc_t/         # ADC sensör okuma kodları
│   ├── coap/          # CoAP protokolüyle veri gönderimi
│   ├── tim/           # Zaman tetikleyici ve zaman hesaplama
│   └── workqueue/     # İş kuyrukları ile işleme yapma
├── prj.conf           # Zephyr RTOS proje ayarları
├── nrf52840dk_nrf52840.overlay  # Donanım ayarları (devicetree overlay)
└── CMakeLists.txt     # Proje yapılandırma dosyası
