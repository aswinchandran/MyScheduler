
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

	\file	: scheduler.c
	\Author	: Aswin chandran
	\Date	: 18/03/2018
	\brief	: 
	\see	: 
  ==========================================================================*/


#include "sch_os.h"

int bSeverityLevel=LOG_LEVEL_ERROR;

#define SCH_PRINT(level, args ...) \
    do {if ((int)level >= bSeverityLevel) printf(args);} while(0)


static SCH_HANDLE _aswScheduler_DriverTask(SCH_HANDLE hSchContext);
static void *_schedulerTask(SCH_HANDLE hSchContext);


SCH_HANDLE aswScheduler_Create ()
{
	SchContext *hSchContext;
	pthread_t threadh;

	/* allocate module object */
	hSchContext = (SchContext *)os_memoryAlloc (sizeof(SchContext));
	if (!hSchContext)
	{
		SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: aswScheduler_Create():  Allocation failed!!\n");
		return NULL;
	}
    os_memoryZero (hSchContext, (sizeof(SchContext)));

    hSchContext->bIsRunning=1;
    hSchContext->bSeverityLevel=bSeverityLevel;
	if (os_CreateTask(_schedulerTask, hSchContext, &(hSchContext->threadh), &(hSchContext->cv))!=0)
	{
		SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched:aswScheduler_Create():  os_CreateTask failed!!\n");
		return NULL;
	}
	return (hSchContext);
}


SCH_STATUS aswScheduler_Destroy (SCH_HANDLE hSchContext)
{
    SCH_PRINT(LOG_LEVEL_WARNING, "AswSched: Calling aswScheduler_Destroy()\n");
    SchContext *pSchContext = (SchContext *)hSchContext;
    pSchContext->bIsRunning=0;
    os_DestroyTask(&(pSchContext->threadh), &(pSchContext->cv));
    /* free module object */
	os_memoryFree (pSchContext, sizeof(SchContext));
	
    return SCH_TRUE;
}


SCH_STATUS aswScheduler_LogLevel (SCH_HANDLE hSchContext, SCH_BOOL level)
{
    SchContext *pSchContext = (SchContext *)hSchContext;

    if (pSchContext && (level < LOG_LEVEL_MAX) && (level >= LOG_LEVEL_NONE))
    {
        pSchContext->bSeverityLevel = level;
    }
    
    bSeverityLevel=pSchContext->bSeverityLevel;
    SCH_PRINT(LOG_LEVEL_WARNING, "AswSched: Log level changed to %d\n", bSeverityLevel);
    return SCH_TRUE;
}


int aswScheduler_RegisterClient (SCH_HANDLE       hSchContext,
                                  SchContextCbFunc  fCbFunc,
                                  SCH_HANDLE       hCbHndl,
                                  SCH_BOOL         bEnable,
                                  char           *sName,
                                  SCH_UINT32       uNameSize)
{
	SchContext *pSchContext = (SchContext *)hSchContext;
    SCH_UINT32 uClientId = pSchContext->uNumClients;

    /* If max number of clients is exceeded, report error and exit. */
    if (uClientId == MAX_CLIENTS) 
    {
        SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: aswScheduler_RegisterClient() MAX_CLIENTS limit exceeded!!\n");
        return 0;
    }

    /* Save the new client's parameters. */
    pSchContext->aClientCbFunc[uClientId]  = fCbFunc;
    pSchContext->aClientCbHndl[uClientId]  = hCbHndl;
    pSchContext->aClientEnabled[uClientId] = bEnable;
    pSchContext->aClientPending[uClientId] = 0;

    if ((uNameSize <= MAX_NAME_SIZE)&&(sName != NULL))
    {
        os_memoryCopy((void *)(pSchContext->sClientName[uClientId]), (void *)sName, uNameSize);
    }
    else
    {
        os_memoryCopy((void *)(pSchContext->sClientName[uClientId]), (void *)"UNKNOWN", 7);
    }
	SCH_PRINT(LOG_LEVEL_INFORMATION, "AswSched: aswScheduler_RegisterClient(): Client=%s, ID=%d, enabled=%d\n", pSchContext->sClientName[uClientId], uClientId, bEnable);

    pSchContext->uNumClients++;
    sleep(1);
    return uClientId;
}


void aswScheduler_RequestRun (SCH_HANDLE hSchContext, SCH_UINT32 uClientId)
{
	SchContext *pSchContext = (SchContext *)hSchContext;

    pSchContext->aRequestCount[uClientId]++; 
    SCH_PRINT(LOG_LEVEL_INFORMATION, "AswSched: aswScheduler_RequestRun(): Client ID=%d, enabled=%d, pending=%d\n", uClientId, pSchContext->aClientEnabled[uClientId], pSchContext->aClientPending[uClientId]);

    /* Set client's Pending flag and request driver task scheduling */
    pSchContext->aClientPending[uClientId] = 1;
    os_RequestSchedule(&(pSchContext->cv));
}


void aswScheduler_RequestSchedule (SCH_HANDLE hSchContext, SCH_UINT32 uClientId)
{
	SchContext *pSchContext = (SchContext *)hSchContext;

    pSchContext->aRequestCount[uClientId]++; 
    SCH_PRINT(LOG_LEVEL_INFORMATION, "AswSched: aswScheduler_RequestSchedule(): Client ID=%d, enabled=%d, pending=%d\n", uClientId, pSchContext->aClientEnabled[uClientId], pSchContext->aClientPending[uClientId]);

    /* Set client's Pending flag and request driver task scheduling */
    pSchContext->aClientPending[uClientId] = 1;
}


static void *_schedulerTask (SCH_HANDLE hSchContext)
{
	SchContext *pSchContext = (SchContext *)hSchContext;

    SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: Entering schedulerTask()\n");
    while (1)
    {
        if(OsProtectLock(&(pSchContext->lock))!=0)
        {
            OsProtectUnLock(&(pSchContext->lock));
            continue;
        }
		if (OsProtectWait(&(pSchContext->lock), &(pSchContext->cv))) 
		{
            continue;
        }
        if (!pSchContext->bIsRunning)
        {
            SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: Scheduler task is going to exit!\n");
            break;
        }
        _aswScheduler_DriverTask(pSchContext);
        OsProtectUnLock(&(pSchContext->lock));
    }
    
    SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: Exiting schedulerTask()\n");
    return NULL;
}

static SCH_HANDLE _aswScheduler_DriverTask (SCH_HANDLE hSchContext)
{
	SchContext       *pSchContext = (SchContext *)hSchContext;
    SchContextCbFunc  fCbFunc;
    SCH_HANDLE       hCbHndl;
    SCH_UINT32       i;

    /* For all registered clients do: */
	for (i = 0; i < pSchContext->uNumClients; i++)
    {
        /* If client is pending and enabled */
        if (pSchContext->aClientPending[i]  &&  pSchContext->aClientEnabled[i])
        {
                pSchContext->aInvokeCount[i]++; 
                SCH_PRINT(LOG_LEVEL_INFORMATION, "AswSched: Invoking - Client=%s ID=%d\n", pSchContext->sClientName[i], i);

            /* Clear client's pending flag */
            pSchContext->aClientPending[i] = 0;

            /* Call client's callback function */
            fCbFunc = pSchContext->aClientCbFunc[i];
            hCbHndl = pSchContext->aClientCbHndl[i];
            fCbFunc(hCbHndl);
        }
    }
}


void aswScheduler_EnableClient (SCH_HANDLE hSchContext, SCH_UINT32 uClientId)
{
	SchContext *pSchContext = (SchContext *)hSchContext;

    if (pSchContext->aClientEnabled[uClientId])
    {
        SCH_PRINT(LOG_LEVEL_WARNING, "AswSched: aswScheduler_EnableClient() Client  already enabled!!\n");
        return;
    }
    SCH_PRINT(LOG_LEVEL_INFORMATION, "AswSched: aswScheduler_EnableClient(): Client=, ID=%d, enabled=%d, pending=%d\n", uClientId, pSchContext->aClientEnabled[uClientId], pSchContext->aClientPending[uClientId]);

    /* Enable client */
    pSchContext->aClientEnabled[uClientId] = SCH_TRUE;

    /* If client is pending, schedule driver task */
    if (pSchContext->aClientPending[uClientId])
    {
        SCH_PRINT(LOG_LEVEL_INFORMATION, "aswScheduler_EnableClient()-scheduling\n");
        os_RequestSchedule(&(pSchContext->cv));
    }
}

void aswScheduler_DisableClient (SCH_HANDLE hSchContext, SCH_UINT32 uClientId)
{
	SchContext *pSchContext = (SchContext *)hSchContext;

    if (!pSchContext->aClientEnabled[uClientId])
    {
        SCH_PRINT(LOG_LEVEL_WARNING, "AswSched: aswScheduler_DisableClient() Client  already disabled!!\n");
        return;
    }
    SCH_PRINT(LOG_LEVEL_INFORMATION, "AswSched: aswScheduler_DisableClient(): Client=, ID=%d, enabled=%d, pending=%d\n", uClientId, pSchContext->aClientEnabled[uClientId], pSchContext->aClientPending[uClientId]);

    /* Disable client */
    pSchContext->aClientEnabled[uClientId] = SCH_FALSE;
}

void aswScheduler_Print(SCH_HANDLE hSchContext)
{
	SchContext *pSchContext = (SchContext *)hSchContext;
	SCH_UINT32 i;

	SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: aswScheduler_Print():  %d Clients Registered:\n", pSchContext->uNumClients);
	SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: =======================================\n");

	for (i = 0; i < pSchContext->uNumClients; i++)
	{
		SCH_PRINT(LOG_LEVEL_FATAL_ERROR, "AswSched: Client %d - %s: Enabled=%d, Pending=%d, Requests=%d, Invoked=%d\n",
			i,
			pSchContext->sClientName[i],
			pSchContext->aClientEnabled[i],
			pSchContext->aClientPending[i],
			pSchContext->aRequestCount[i],
			pSchContext->aInvokeCount[i]);
	}
}