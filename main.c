
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
  
	\file	: main.c
	\Author	: Aswin chandran
	\Date	: 18/03/2018
	\brief	: 
	\see	: 
  ==========================================================================*/

#include <iostream>
#include "sch_os.h"


void Func1 (SCH_HANDLE hSchContext)
{
    printf("IN Func1\n");
}

void Func2 (SCH_HANDLE hSchContext)
{
    printf("IN Func2\n");
}

int main (void)
{
    SchContext *hSchContext;
    int id1, id2;
    char name[6]="ASWIN";
    char name2[7]="LINUX";
    
    hSchContext = (SchContext*)aswScheduler_Create();

    id1=aswScheduler_RegisterClient(hSchContext, Func1, hSchContext, 1, name,6);
    id2=aswScheduler_RegisterClient(hSchContext, Func2, hSchContext, 1, name2,700);

    aswScheduler_RequestSchedule(hSchContext, id2);
    aswScheduler_RequestRun (hSchContext, id1);
    sleep(5);
    aswScheduler_LogLevel(hSchContext, LOG_LEVEL_INIT);
    aswScheduler_RequestRun(hSchContext, id1);
    sleep(5);
	aswScheduler_Destroy(hSchContext);
    return 0;
}
