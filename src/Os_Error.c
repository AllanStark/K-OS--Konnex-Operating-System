/*
 * k_os (Konnex Operating-System based on the OSEK/VDX-Standard).
 *
 * (C) 2007-2009 by Christoph Schueler <chris@konnex-tools.de>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "Os_Error.h"

#if defined(OS_USE_GETSERVICEID) || defined(OS_USE_PARAMETERACCESS)
Os_ServiceContextType Os_ServiceContext;
#endif

#if defined(USE_ORTI)
StatusType OsLastError=E_OK;
#define OS_SAVE_LAST_ERROR(Error)   OsLastError=Error
#else
#define OS_SAVE_LAST_ERROR(Error)
#endif


void OsErrorCallErrorHook(StatusType Error)
{
    if (((OsFlags & OS_SYS_FLAG_IN_OS_ERROR_HOOK)!=OS_SYS_FLAG_IN_OS_ERROR_HOOK)) {
        DISABLE_ALL_OS_INTERRUPTS();
        OsFlags|=OS_SYS_FLAG_IN_OS_ERROR_HOOK;
        OS_SAVE_CALLEVEL(); /* s. 'os_alm' !!! */
        OS_SET_CALLEVEL(OS_CL_ERROR_HOOK);
        OS_SAVE_LAST_ERROR(Error);
        ErrorHook(Error);
        OS_RESTORE_CALLEVEL();
        OsFlags&=~OS_SYS_FLAG_IN_OS_ERROR_HOOK;
        ENABLE_ALL_OS_INTERRUPTS();
    }    
}


void COMErrorCallErrorHook(StatusType Error)
{
    if (((OsFlags & OS_SYS_FLAG_IN_COM_ERROR_HOOK)!=OS_SYS_FLAG_IN_COM_ERROR_HOOK)) {
        DISABLE_ALL_OS_INTERRUPTS();
        OsFlags|=OS_SYS_FLAG_IN_COM_ERROR_HOOK;
        OS_SAVE_CALLEVEL();
        OS_SET_CALLEVEL(OS_CL_ERROR_HOOK);
        OS_SAVE_LAST_ERROR(Error);
        COMErrorHook(Error);
        OS_RESTORE_CALLEVEL();
        OsFlags&=~OS_SYS_FLAG_IN_COM_ERROR_HOOK;
        ENABLE_ALL_OS_INTERRUPTS();
    }    
}


#if defined(OS_USE_GETSERVICEID) && defined(OS_USE_PARAMETERACCESS)
void OSSaveServiceContext(Os_ServiceIdType id,void *param1,void *param2,void *param3)
{
        Os_ServiceContext.id=id;
        Os_ServiceContext.param1=param1;
        Os_ServiceContext.param2=param2;
        Os_ServiceContext.param3=param3;                       
}
#endif


#if defined(OS_USE_GETSERVICEID) && !defined(OS_USE_PARAMETERACCESS)
void OSSaveServiceContext(Os_ServiceIdType id)
{
        Os_ServiceContext.id=id;                       
}
#endif


#if !defined(OS_USE_GETSERVICEID) && defined(OS_USE_PARAMETERACCESS)
void OSSaveServiceContext(void *param1,void *param2,void *param3)
{
        Os_ServiceContext.param1=param1;
        Os_ServiceContext.param2=param2;
        Os_ServiceContext.param3=param3;                       
}
#endif
