#ifndef __TIM_T_H__
#define __TIM_T_H__

#include <zephyr/kernel.h>

/**
 * @brief Tarih ve saat bilgilerini tutan yapı.
 * 
 * Bu yapı, zamanla ilgili verileri içerir. Saat, dakika, saniye gibi zaman
 * verileri ve yıl, ay, gün gibi tarih bilgileri bu yapı içerisinde tutulur.
 */
typedef struct 
{
    uint8_t seconds;  /**< Saniye bilgisi (0-59) */
    uint8_t minute;   /**< Dakika bilgisi (0-59) */
    uint8_t hours;    /**< Saat bilgisi (0-23) */
    uint8_t day;      /**< Gün bilgisi (1-31) */
    uint8_t month;    /**< Ay bilgisi (1-12) */
    uint16_t year;    /**< Yıl bilgisi */
} time_date_t;

#endif // __TIM_T_H__
