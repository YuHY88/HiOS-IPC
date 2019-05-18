/* define the semaphore operation  */



#ifndef HIOS_SEMAPHORE_H
#define HIOS_SEMAPHORE_H
#include <stdint.h>

#define SEM_WAIT_FOREVER       (-1)
#define SEM_DO_NOT_WAIT         (0)



enum SEM_TYPE 
{
   SEM_BINARY,
   SEM_COUNTING,
   SEM_MUTEX
};




int sem_new(char *sem_name, enum SEM_TYPE type, uint32_t num_of_tokens, void *sem_flags, void *psem_id);
int sem_delete(enum SEM_TYPE type, void *psem_id);
int sem_lock(enum SEM_TYPE type, void *psem_id, int timeout);
int sem_unlock(enum SEM_TYPE type, void *psem_id); 




#endif
