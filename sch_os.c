
/******************************************************************************
**+-------------------------------------------------------------------------+**
**|                                 *******                                 |**
**|                                *********                                |**
**|                               ****   ****                               |**
**|                              **** *** ****                              |**
**|                             **** ***** ****                             |**
**|                            ****         ****                            |**
**|                           ****           ****                           |**
**|                                                                         |**
**|     Copyright (c) 2017 - 2018 Aswin Chadran                             |**
**|     Author: Aswin Chandran                                              |**
**|     Author details: https://github.com/aswinchandran                    |**
**|     Contact details: https://www.linkedin.com/in/aswin-chandran/        |**
**|                                                                         |**
**| This program is free software: you can redistribute it and/or modify    |**
**| it under the terms of the GNU General Public License as published by    |**
**| the Free Software Foundation, either version 3 of the License, or       |**
**| (at your option) any later version.                                     |**
**|                                                                         |**
**| This program is distributed in the hope that it will be useful,         |**
**| but WITHOUT ANY WARRANTY; without even the implied warranty of          |**
**| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |**
**| GNU General Public License for more details.                            |**
**|                                                                         |**
**| You should have received a copy of the GNU General Public License       |**
**| along with this program.  If not, see <https://www.gnu.org/licenses/>   |**
**|                                                                         |**
**+-------------------------------------------------------------------------+**
******************************************************************************/
/**===========================================================================

	\file	: sch_os.c
	\Author	: Aswin chandran
	\Date	: 18/03/2018
	\brief	: 
	\see	: 
  ==========================================================================*/


#include "sch_os.h"

SCH_HANDLE os_memoryAlloc (SCH_UINT32 Length)
{
	return malloc (Length);
}

void os_memoryZero ( void* pMemPtr, SCH_UINT32 Length)
{
	memset (pMemPtr,0,Length);
}

void os_memoryFree ( void* pMemPtr, SCH_UINT32 Length)
{
	free (pMemPtr);
}

void os_memoryCopy (void* pDstPtr, void* pSrcPtr, SCH_UINT32 Size)
{
   memcpy (pDstPtr,pSrcPtr,Size);
}


SCH_STATUS os_CreateTask (TASK_FUNCTION fcb, SCH_HANDLE hSchContext, pthread_t* threadh, pthread_cond_t* cv)
{
	pthread_cond_init (cv, NULL);
	return pthread_create (threadh, NULL, fcb, hSchContext);
}

void os_DestroyTask (pthread_t* threadh, pthread_cond_t* cv)
{
	pthread_cond_destroy (cv);
}

SCH_STATUS OsProtectLock (pthread_mutex_t * lock)
{
    return pthread_mutex_lock (lock);
}

SCH_STATUS OsProtectUnLock (pthread_mutex_t * lock)
{
    return pthread_mutex_unlock (lock);
}

SCH_STATUS OsProtectWait (pthread_mutex_t* lock, pthread_cond_t* cv)
{
    return pthread_cond_wait (cv, lock);
}

void os_RequestSchedule (pthread_cond_t* cv)
{
    pthread_cond_signal (cv);
}

#if 0
Current implementation is for Linux, you can use below changes for Windows

CreateEvent -> pthread_cond_init
CloseHandle -> pthread_cond_destroy
WaitForSingleObject -> pthread_cond_wait or pthread_cond_timedwait
SetEvent -> pthread_cond_signal or pthread_cond_broadcast
#endif