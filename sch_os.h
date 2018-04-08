
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

	\file	: sch_os.h
	\Author	: Aswin chandran
	\Date	: 18/03/2018
	\brief	: 
	\see	: 
  ==========================================================================*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS     8
#define MAX_NAME_SIZE   16


#define SCH_FALSE 1
#define SCH_TRUE 0

#define SCH_HANDLE void *
#define SCH_STATUS int

typedef char                SCH_CHAR;
typedef signed char         SCH_INT8;
typedef unsigned char       SCH_UINT8;
typedef signed short        SCH_INT16;
typedef unsigned short      SCH_UINT16;
typedef signed int          SCH_INT32;
typedef unsigned int        SCH_UINT32;
typedef signed long long    SCH_INT64;
typedef unsigned long long  SCH_UINT64;
typedef unsigned int 		SCH_BOOL;
typedef void (*SchContextCbFunc)(SCH_HANDLE hCbHndl);
typedef void* (*TASK_FUNCTION)(void *);


/* context module structure */
typedef struct 
{
	SCH_UINT32			uNumClients;
	SchContextCbFunc	aClientCbFunc [MAX_CLIENTS];
	SCH_HANDLE			aClientCbHndl [MAX_CLIENTS];
	SCH_BOOL			aClientEnabled[MAX_CLIENTS];
	SCH_BOOL			aClientPending[MAX_CLIENTS];
	SCH_UINT32          aInvokeCount[MAX_CLIENTS];
	SCH_UINT32          aRequestCount[MAX_CLIENTS];
	char				sClientName[MAX_CLIENTS][MAX_NAME_SIZE];
	SCH_BOOL			bIsRunning;
	SCH_BOOL            bSeverityLevel;

	pthread_t threadh;
    pthread_cond_t cv;
    pthread_mutex_t lock;
} SchContext;

typedef enum
{
    LOG_LEVEL_NONE           =  0,
	LOG_LEVEL_INIT           =  1,
	LOG_LEVEL_INFORMATION    =  2,
	LOG_LEVEL_WARNING        =  3,
	LOG_LEVEL_ERROR          =  4,
	LOG_LEVEL_FATAL_ERROR    =  5,
	LOG_LEVEL_MAX            =  6,
} ELogLevel;



SCH_HANDLE aswScheduler_Create ();
SCH_STATUS aswScheduler_Destroy (SCH_HANDLE hSchContext);
SCH_STATUS aswScheduler_RegisterClient (SCH_HANDLE       hSchContext,
                                  SchContextCbFunc  fCbFunc,
                                  SCH_HANDLE       hCbHndl,
                                  SCH_BOOL         bEnable,
                                  char           *sName,
                                  SCH_UINT32       uNameSize);
void aswScheduler_EnableClient (SCH_HANDLE hSchContext, SCH_UINT32 uClientId);
void aswScheduler_DisableClient (SCH_HANDLE hSchContext, SCH_UINT32 uClientId);
void aswScheduler_RequestRun (SCH_HANDLE hSchContext, SCH_UINT32 uClientId);
void aswScheduler_RequestSchedule (SCH_HANDLE hSchContext, SCH_UINT32 uClientId);
SCH_STATUS aswScheduler_LogLevel (SCH_HANDLE hSchContext, SCH_BOOL level);
void aswScheduler_Print (SCH_HANDLE hSchContext);

SCH_HANDLE os_memoryAlloc (SCH_UINT32 Length);
void os_memoryZero ( void* pMemPtr, SCH_UINT32 Length);
void os_memoryFree ( void* pMemPtr, SCH_UINT32 Length);
void os_memoryCopy (void* pDstPtr, void* pSrcPtr, SCH_UINT32 Size);

SCH_STATUS os_CreateTask (TASK_FUNCTION fcb, SCH_HANDLE hSchContext, pthread_t* threadh, pthread_cond_t* cv);
void os_DestroyTask (pthread_t* threadh, pthread_cond_t* cv);
SCH_STATUS OsProtectLock (pthread_mutex_t * lock);
SCH_STATUS OsProtectUnLock (pthread_mutex_t * lock);
SCH_STATUS OsProtectWait (pthread_mutex_t* lock, pthread_cond_t* cv);
void os_RequestSchedule (pthread_cond_t* cv);