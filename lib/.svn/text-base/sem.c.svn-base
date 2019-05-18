/* define the semaphore operation */



#include "config.h"
//#include "sal/core/sync.h"
//#include "types.h"
#include "sem.h"

#define sal_mutex_FOREVER	(-1)
#define sal_mutex_NOWAIT	0

#define sal_sem_FOREVER		(-1)
#define sal_sem_BINARY		1
#define sal_sem_COUNTING	0

/************************************************************************************
 * Function: oss_sem_new  - Service routine to create a new semaphore               *
 * Parameters:                                                                      *
 *   IN  sem_name        - semaphore descriptor.                                    *
 *   IN  sem_type        - type of semaphore (binary/counting/mutex).               *
 *   IN  num_of_tokens   - initial tokens count for counting semaphore.             *
 *   IN  sem_flags       - special flags - like SEM_DELETE_SAFE/SEM_INVERSION_SAFE  *
 *                                              SEM_Q_PRIORITY.                     *
 *   OUT sem_id_ptr      - pointer to newly created semaphore.                      *
 * Return value:                                                                    *
 *   0 - semaphore creation successful.                                             *
 *   otherwise - semaphore creation failed.                                         *
 ************************************************************************************/
int sem_new(char *sem_name, enum SEM_TYPE type, uint32_t num_of_tokens,
				void *sem_flags, void *psem_id)
{

   if(!sem_name || !psem_id)
   {
      return 1;
   }

   switch (type) 
   {
     case SEM_BINARY:
          *psem_id = sal_sem_create( sem_name, sal_sem_BINARY, num_of_tokens);
          break;
     case SEM_COUNTING:
          *psem_id = sal_sem_create( sem_name, sal_sem_COUNTING, num_of_tokens);
          break;
     case SEM_MUTEX:
          *psem_id = sal_mutex_create( sem_name );
          break; 
   }
   return 0;
}


/************************************************************************************
 * Function: oss_sem_delete  - Service routine to delete a semaphore                *
 * Parameters:                                                                      *
 *   IN  sem_type        - type of semaphore (binary/counting/mutex)                *
 *   IN  sem_id_ptr      - pointer to semaphore                                     *
 * Return value:                                                                    *
 *   0 - semaphore was successfully deleted.                                        *
 *   otherwise - semaphore deletion failed                                          *
 ************************************************************************************/
int sem_delete(enum SEM_TYPE type, void *psem_id)
{

   if(!psem_id)
   {
      return 1;
   }

   switch (type) 
   {
     case SEM_BINARY:
     case SEM_COUNTING:
          sal_sem_destroy((sal_sem_t)psem_id);
          break;
     case SEM_MUTEX:
          sal_mutex_destroy((sal_mutex_t)psem_id);
          break; 
   }
   return 0;
}


/************************************************************************************
 * Function: oss_sem_lock    - Service routine to lock a semaphore                  *
 * Parameters:                                                                      *
 *   IN  sem_type        - type of semaphore (binary/counting/mutex).               *
 *   IN  sem_id_ptr      - pointer to semaphore.                                    *
 *   IN  timeout         - miliseconds lock timeout value.                          *
 * Return value:                                                                    *
 *   0 - semaphore was successfully locked.                                         *
 *   otherwise - semaphore lock failed.                                             *
 ************************************************************************************/
int sem_lock(enum SEM_TYPE type, void *psem_id, int timeout)
{
   int status;
   int timeout_val;

   if (timeout == SEM_WAIT_FOREVER)
      timeout_val = sal_sem_FOREVER;
   else
      timeout_val = 1000 * timeout;

   if(!psem_id)
   {
      return 1;
   }

   switch (type) {
     case SEM_BINARY:
     case SEM_COUNTING:
          status = sal_sem_take((sal_sem_t)psem_id,timeout_val);
          break;
     case SEM_MUTEX:
          status = sal_mutex_take((sal_mutex_t)psem_id,timeout_val);
          break; 
     default: 
          status = 1;
   }
   return status;
}


/************************************************************************************
 * Function: oss_sem_unlock    - Service routine to unlock a semaphore              *
 * Parameters:                                                                      *
 *   IN  sem_type        - type of semaphore (binary/counting/mutex).               *
 *   IN  sem_id_ptr      - pointer to semaphore.                                    *
 * Return value:                                                                    *
 *   0 - semaphore was successfully unlocked.                                       *
 *   otherwise - semaphore unlock failed.                                           *
 ************************************************************************************/
int sem_unlock(enum SEM_TYPE type, void *psem_id)
{

   int status; 

   if(!psem_id)
   {
      return 1;
   }
   
   switch (type) {
     case SEM_BINARY:
     case SEM_COUNTING:
          status = sal_sem_give((sal_sem_t)psem_id);
          break;
     case SEM_MUTEX:
          status = sal_mutex_give((sal_mutex_t)psem_id);
          break; 
     default: 
          status = 1;
   }
   return status;
}



