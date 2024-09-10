#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h> 
#include "wq.h"

LOG_MODULE_REGISTER(workq_q, LOG_LEVEL_DBG);

K_THREAD_STACK_DEFINE(stack_area_brief, STACK_SIZE_brief);
struct k_work_q wq_brief;


/**
 * @brief İş kuyruğunu başlatan fonksiyon.
 *
 * Bu fonksiyon, `wq_brief` iş kuyruğunu başlatır ve tanımlı işlerin bu
 * kuyruğa eklenmesini sağlar. İş kuyruğu, belirlenen bir thread stack alanı
 * ve öncelik ile başlatılır.
 */
static int workq_init_func(const struct device *dev)
{
    k_work_queue_init(&wq_brief);
    LOG_DBG("Workq 'wq_brief' init...\n");
    k_work_queue_start(&wq_brief, stack_area_brief,
        K_THREAD_STACK_SIZEOF(stack_area_brief), PRIORITY_brief, NULL);
    LOG_DBG("Workq 'wq_brief'start...\n");
    return 0;
}

SYS_INIT(workq_init_func, APPLICATION, 10);
