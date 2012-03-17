/*
   k_os (Konnex Operating-System based on the OSEK/VDX-Standard).

   (C) 2007-2012 by Christoph Schueler <github.com/Christoph2,
                                        cpu12.gems@googlemail.com>

   All Rights Reserved

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   s. FLOSS-EXCEPTION.txt
 */

/*
**
** Interface/Dispatcher (internal/external Comm.) for OSEK-COM-Functions.
**
*/
#include "Com_Int.h"
#include "Com_Ext.h"
#include "Os_Cfg.h"

#if OS_FEATURE_COM == STD_ON

extern const Com_MessageObjectType Com_MessageObjects[];

#if 0

#define Message_Send    ((uint8)0)
#define Message_Receive ((uint8)1)

static uint32 MsgDataReceive;

const ComReceiverType Message_Receive_Receiver[1] = {{1}};

const MessageSetEventType Evt_Message_Receive = {Task1, (EventMaskType)0x08};

const Com_MessageObjectType Com_MessageObjects[COM_NUMBER_OF_MESSAGES] = {
    {SEND_STATIC_INTERNAL,      COM_NOTIFY_NONE,      (void *)NULL,
     4,
     (const ApplicationDataRef *)
     NULL, (uint8)1, (ComReceiverType *)&Message_Receive_Receiver},
    {RECEIVE_UNQUEUED_INTERNAL, COM_SETEVENT,         (void *)&Evt_Message_Receive,
     4,
     (const ApplicationDataRef *)
     &MsgDataReceive, (uint8)0, (ComReceiverType *)NULL},
};
#endif

/*
**  Local variables.
*/
static Com_StatusType           Com_Status = COM_UNINIT;
static COMApplicationModeType   Com_AppMode;


#if KOS_MEMORY_MAPPING == STD_ON
    #define OSEK_COM_START_SEC_CODE
    #include "MemMap.h"
#endif /* KOS_MEMORY_MAPPING */

/*
**  Global functions.
*/
#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) StartCOM(COMApplicationModeType Mode)
#else
StatusType StartCOM(COMApplicationModeType Mode)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if the initialisation completed successfully.
        � This service returns an implementation-specific status code if the
          initialisation was not completed successfully.
    Extended:
        In addition to the standard status codes defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Mode> is out of range.
 */
#if defined(COM_START_COM_EXTENSION)
    StatusType Status;
#endif
    uint8_least             idx;
    Com_MessageObjectType * MessageObject;

    SAVE_SERVICE_CONTEXT(COMServiceId_StartCOM, Mode, NULL, NULL);

    Com_AppMode = Mode;

    for (idx = (uint8_least)0; idx < COM_NUMBER_OF_MESSAGES; ++idx) {
        MessageObject = (Com_MessageObjectType *)&OSEK_COM_GET_MESSAGE_OBJECT(idx);

        if (MessageObject->Property != SEND_STATIC_INTERNAL &&
            MessageObject->Property != SEND_ZERO_INTERNAL &&
            MessageObject->Property != SEND_ZERO_EXTERNAL &&
            MessageObject->Property != RECEIVE_ZERO_EXTERNAL)
        {
/* todo: use Initialisation-Value. */
#if 0
            DISABLE_ALL_OS_INTERRUPTS();
            OsUtilMemCpy((void *)MessageObject->Data, (void *)DataRef, MessageObject->Size);
            ENABLE_ALL_OS_INTERRUPTS();
#endif
        }
    }

#if defined(COM_START_COM_EXTENSION)
    Status = StartCOMExtension();
#endif
    Com_Status = COM_INIT;

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}


#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) StopCOM(COMShutdownModeType Mode)
#else
StatusType StopCOM(COMShutdownModeType Mode)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Parameter (in): Mode COM_SHUTDOWN_IMMEDIATE
        The shutdown occurs immediately without waiting for pending operations to complete.
 */
/*
    Standard:
        � This service returns E_OK if OSEK COM was shut down successfully.
        � This service returns an implementation-specific status code if the
          shutdown was not completed successfully.
    Extended:
        In addition to the standard status codes defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Mode> is out of range.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_StopCOM, Mode, NULL, NULL);

    Com_Status = COM_UNINIT;

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(COMApplicationModeType, OSEK_COM_CODE) GetCOMApplicationMode(void)
#else
COMApplicationModeType GetCOMApplicationMode(void)
#endif /* KOS_MEMORY_MAPPING */
{
/*  Return value: Current COM application mode. */
    return Com_AppMode;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) InitMessage(MessageIdentifier Message, ApplicationDataRef DataRef)
#else
StatusType InitMessage(MessageIdentifier Message, ApplicationDataRef DataRef)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if the initialisation of the message object
          completed successfully.
        � This service returns an implementation-specific status code if the
          initialisation did not complete successfully.
    Extended:
        In addition to the standard status code defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or refers to a zero-length message or to an internal transmit message.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_InitMessage, Message, DataRef, NULL);
    ASSERT_VALID_MESSAGE_ID(Message);
    ASSERT_CAN_INITIALIZE_MESSAGE(Message);

    DISABLE_ALL_OS_INTERRUPTS();
    Utl_MemCopy((void *)OSEK_COM_GET_MESSAGE_OBJECT(Message).Data, (void *)DataRef, (uint16)OSEK_COM_GET_MESSAGE_OBJECT(Message).Size);
    ENABLE_ALL_OS_INTERRUPTS();

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) StartPeriodic(void)
#else
StatusType StartPeriodic(void)
#endif /* KOS_MEMORY_MAPPING */
{
/*
   Standard and Extended:
    � This service returns E_OK if periodic transmission was started successfully.
    � This service returns an implementation-specific status code if starting of
      periodic transmission was not completed successfully.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_StartPeriodic, NULL, NULL, NULL);
    ASSERT_COM_IS_INITIALIZED();

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) StopPeriodic(void)
#else
StatusType StopPeriodic(void)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard and Extended:
        � This service returns E_OK if periodic transmission was stopped successfully.
        � This service returns an implementation-specific status code if
          stopping periodic transmission was not completed successfully.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_StopPeriodic, NULL, NULL, NULL);
    ASSERT_COM_IS_INITIALIZED();

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) SendMessage(MessageIdentifier Message, ApplicationDataRef DataRef)
#else
StatusType SendMessage(MessageIdentifier Message, ApplicationDataRef DataRef)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if the service operation completed successfully.
    Extended:
        In addition to the standard status code defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or if it refers to a message that is received or to a
          dynamic-length or zero-length message.
 */
    StatusType Status = E_OK;

    SAVE_SERVICE_CONTEXT(COMServiceId_SendMessage, Message, DataRef, NULL);
    ASSERT_COM_IS_INITIALIZED();
    ASSERT_VALID_MESSAGE_ID(Message);
    ASSERT_IS_STATIC_SENDING_MESSAGE(Message);

    switch (OSEK_COM_GET_MESSAGE_OBJECT(Message).Property) {
        case SEND_STATIC_INTERNAL:
            Status = ComInt_SendMessage(Message, DataRef);
            break;
        /* not supported yet. */
        case SEND_ZERO_INTERNAL:
            break;
        case SEND_STATIC_EXTERNAL:
            Status = ComExt_SendMessage(Message, DataRef);
            break;
        case SEND_DYNAMIC_EXTERNAL:
            break;
        case SEND_ZERO_EXTERNAL:
            break;
        default:
            ASSERT(FALSE);
    }

    CLEAR_SERVICE_CONTEXT();
    return Status;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) ReceiveMessage(MessageIdentifier Message, ApplicationDataRef DataRef)
#else
StatusType ReceiveMessage(MessageIdentifier Message, ApplicationDataRef DataRef)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if data in the queued or unqueued
          message identified by <Message> are available and returned to
          the application successfully.
        � This service returns E_COM_NOMSG if the queued message
          identified by <Message> is empty.
        � This service returns E_COM_LIMIT if an overflow of the message
          queue identified by <Message> occurred since the last call to
          ReceiveMessage for <Message>. E_COM_LIMIT indicates that
          at least one message has been discarded since the message
          queue filled. Nevertheless the service is performed and a
          message is returned. The service ReceiveMessage clears the
          overflow condition for <Message>.
    Extended:
        In addition to the standard status codes defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or if it refers to message that is sent or to a dynamiclength
          or zero-length message.
 */
    StatusType Status = E_OK;

    SAVE_SERVICE_CONTEXT(COMServiceId_ReceiveMessage, Message, DataRef, NULL);
    ASSERT_COM_IS_INITIALIZED();
    ASSERT_VALID_MESSAGE_ID(Message);
    ASSERT_IS_STATIC_RECEIVING_MESSAGE(Message);

    switch (OSEK_COM_GET_MESSAGE_OBJECT(Message).Property) {
        case RECEIVE_UNQUEUED_INTERNAL:
            Status = ComInt_ReceiveMessage(Message, DataRef);
            break;
        case RECEIVE_QUEUED_INTERNAL:
            break;
#if 0
/* not supported yet. */
        case RECEIVE_ZERO_INTERNAL:
        case RECEIVE_ZERO_EXTERNAL:
        case RECEIVE_UNQUEUED_EXTERNAL:
        case RECEIVE_QUEUED_EXTERNAL:
        case RECEIVE_DYNAMIC_EXTERNAL:
            break;
#endif
        default:
            ASSERT(FALSE);
    }

    CLEAR_SERVICE_CONTEXT();
    return Status;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) SendDynamicMessage(MessageIdentifier Message, ApplicationDataRef DataRef,
    LengthRef LengthRef
)
#else
StatusType SendDynamicMessage(MessageIdentifier Message, ApplicationDataRef DataRef, LengthRef LengthRef)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if the service operation completed successfully.
    Extended:
        In addition to the standard status code defined above, the following
        status codes are supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or if it refers to a received message, a static-length
          message or a zero-length message.
        � This service returns E_COM_LENGTH if the value to which
          <LengthRef> points is not within the range 0 to the maximum
          length defined for <Message>.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_SendDynamicMessage, Message, DataRef, LengthRef);
    ASSERT_COM_IS_INITIALIZED();
    ASSERT_VALID_MESSAGE_ID(Message);

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) ReceiveDynamicMessage(MessageIdentifier Message, ApplicationDataRef DataRef,
    LengthRef LengthRef
)
#else
StatusType ReceiveDynamicMessage(MessageIdentifier Message, ApplicationDataRef DataRef, LengthRef LengthRef)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if data in the unqueued message identified by <Message>
          is returned to the application succesfully.
    Extended:
        In addition to the standard status code defined above, the following
        status codes are supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or if it refers to a message that is sent, a queued message,
          a static-length message or a zero-length message.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_ReceiveDynamicMessage, Message, DataRef, LengthRef);
    ASSERT_COM_IS_INITIALIZED();
    ASSERT_VALID_MESSAGE_ID(Message);

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE)
#else
StatusType SendZeroMessage(MessageIdentifier Message)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_OK if the service operation completed successfully.
    Extended:
        In addition to the standard status code defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or if it refers to a non-zero-length message.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_SendZeroMessage, Message, NULL, NULL);
    ASSERT_COM_IS_INITIALIZED();
    ASSERT_VALID_MESSAGE_ID(Message);

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(StatusType, OSEK_COM_CODE) GetMessageStatus(MessageIdentifier Message)
#else
StatusType GetMessageStatus(MessageIdentifier Message)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Standard:
        � This service returns E_COM_NOMSG if the message queue
          identified by <Message> is empty.
        � This service returns E_COM_LIMIT if an overflow of the message
          queue identified by <Message> occurred since the last call to
          ReceiveMessage for <Message>.
        � This service returns E_OK if none of the conditions specified
          above is applicable or fulfilled and no error indication is present.
    Extended:
        In addition to the standard status codes defined above, the following
        status code is supported:
        � This service returns E_COM_ID if the parameter <Message> is
          out of range or if it does not refer to a queued message.
 */
    SAVE_SERVICE_CONTEXT(COMServiceId_GetMessageStatus, Message, NULL, NULL);
    ASSERT_COM_IS_INITIALIZED();
    ASSERT_VALID_MESSAGE_ID(Message);

    CLEAR_SERVICE_CONTEXT();
    return E_OK;
}

#if KOS_MEMORY_MAPPING == STD_ON
FUNC(COMServiceIdType, OSEK_COM_CODE) COMErrorGetServiceId(void)
#else
COMServiceIdType COMErrorGetServiceId(void)
#endif /* KOS_MEMORY_MAPPING */
{
/*
    Return value: Service Identifier.
 */
/*    return (COMServiceIdType)0;   */
}


#if KOS_MEMORY_MAPPING == STD_ON
FUNC(void, OSEK_COM_CODE) ComIfUpdateAndNotifyReceivers(
    P2VAR(Com_MessageObjectType, AUTOMATIC, OSEK_COM_APPL_DATA) MessageSendObject
    ApplicationDataRef DataRef
)
#else
void ComIfUpdateAndNotifyReceivers(Com_MessageObjectType * MessageSendObject, ApplicationDataRef DataRef)
#endif /* KOS_MEMORY_MAPPING */
{
    uint8_least             idx, count;
    Com_MessageObjectType * MessageObject;

#if defined(OS_EXTENDED_STATUS) && defined(OS_USE_CALLEVEL_CHECK)
    OsCallevelType CallevelSaved;
#endif

    ASSERT(MessageSendObject->Receiver != (Com_ReceiverType *)NULL);

    count = MessageSendObject->NumReceivers;

    for (idx = (uint8_least)0; idx < count; ++idx) {
        DISABLE_ALL_OS_INTERRUPTS();
        MessageObject = (Com_MessageObjectType *)&OSEK_COM_GET_MESSAGE_OBJECT(MessageSendObject->Receiver[idx].Message);
        ASSERT(MessageSendObject->Size == MessageObject->Size);
        ASSERT(MessageObject->Property == RECEIVE_UNQUEUED_INTERNAL);    /* todo: CCCB */
        ASSERT(MessageObject->Action.Dummy != (void *)NULL);
        Utl_MemCopy((void *)MessageObject->Data, (void *)DataRef, (uint16)MessageObject->Size);
        ENABLE_ALL_OS_INTERRUPTS();

        switch (MessageObject->Notification) {  /* NotificationType??? */
            case COM_ACTIVATETASK:
                (void)OsTask_Activate(MessageObject->Action.TaskID);
                break;
            case COM_SETEVENT:
                (void)OsEvtSetEvent(MessageObject->Action.Event->TaskID, MessageObject->Action.Event->Mask);
                break;
            case COM_COMCALLBACK:
                DISABLE_ALL_OS_INTERRUPTS();
                #if defined(OS_EXTENDED_STATUS) && defined(OS_USE_CALLEVEL_CHECK)
                CallevelSaved = OS_GET_CALLEVEL();
                #endif
                OS_SET_CALLEVEL(OS_CL_ALARM_CALLBACK);
                (MessageObject->Action.Callback)();
                #if defined(OS_EXTENDED_STATUS) && defined(OS_USE_CALLEVEL_CHECK)
                OS_SET_CALLEVEL(CallevelSaved);
                #endif
                ENABLE_ALL_OS_INTERRUPTS();
                break;
            case COM_FLAG:
                /* todo: Implementieren (nicht CCCA) !!! */
/*                MessageObject->Action.Flag; */
                break;
            case COM_NOTIFY_NONE:
                break;  /* No Action. */
            default:
                ASSERT(FALSE);
        }
    }
}

#if KOS_MEMORY_MAPPING == STD_ON
    #define OSEK_COM_STOP_SEC_CODE
    #include "MemMap.h"
#endif /* KOS_MEMORY_MAPPING */

/*
**
**  Routines provided by the application.
**
*/

/*  COMCallout(CalloutRoutineName) */

#endif /* OS_FEATURE_COM */
