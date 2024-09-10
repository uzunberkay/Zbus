#ifndef __BERKAY_H
#define __BERKAY_H

#define STACK_SIZE_brief 1024
#define PRIORITY_brief 7

// Sadece bildirimler (declaration)
extern struct k_work_q wq_brief;
extern K_THREAD_STACK_DEFINE(stack_area_brief, STACK_SIZE_brief);

//static int workq_init_func(const struct device *dev);

#endif // __BERKAY_H
