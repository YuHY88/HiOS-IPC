#ifndef HIOS_SDHVCG_MAIN_H
#define HIOS_SDHVCG_MAIN_H

struct thread_master   *vcg_master;

extern int              g_vcg_config_finish;
extern uint32_t         g_vcg_config_timer;

void vcg_config_finish_func(void *);
void vcg_init(void);
void vcg_die(void);

#endif

