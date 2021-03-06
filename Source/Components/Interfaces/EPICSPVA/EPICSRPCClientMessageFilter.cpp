/**
 * @file EPICSRPCClientMessageFilter.cpp
 * @brief Source file for class EPICSRPCClientMessageFilter
 * @date 18/06/2018
 * @author Andre Neto
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing, 
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class EPICSRPCClientMessageFilter (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include "pv/pvAccess.h"
#include "pv/rpcClient.h"

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
/*lint -efile(766,EPICSRPCClientMessageFilter.cpp) EPICSRPCClientMessageFilter.h and Message.h are used in this file.*/
#include "AdvancedErrorManagement.h"
#include "EPICSPVAStructureDataI.h"
#include "EPICSRPCClientMessageFilter.h"
#include "Message.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {

EPICSRPCClientMessageFilter::EPICSRPCClientMessageFilter() :
        Object(), MessageFilter(true) {
    SetName("EPICSRPCClientMessageFilter");
    timeout = 10u;
}

EPICSRPCClientMessageFilter::~EPICSRPCClientMessageFilter() {

}

ErrorManagement::ErrorType EPICSRPCClientMessageFilter::ConsumeMessage(ReferenceT<Message> &messageToTest) {
    StreamString destination = messageToTest->GetName();
    ErrorManagement::ErrorType err;
    err.parametersError = (destination.Size() == 0u);
    if (!err.ErrorsCleared()) {
        REPORT_ERROR(err, "No message destination (name of the message) set");
    }
    ReferenceT<StructuredDataI> config = messageToTest->Get(0u);
    if (!config.IsValid()) {
        ReferenceT<ConfigurationDatabase> configRef(GlobalObjectsDatabase::Instance()->GetStandardHeap());
        config = configRef;
    }

    ReferenceT<EPICSPVAStructureDataI> pvaStructureDataI = config;
    if (err.ErrorsCleared()) {
        if (!pvaStructureDataI.IsValid()) {
            pvaStructureDataI = ReferenceT<EPICSPVAStructureDataI>(GlobalObjectsDatabase::Instance()->GetStandardHeap());
            pvaStructureDataI->InitStructure();
            err.parametersError = !config->Copy(*pvaStructureDataI.operator ->());
            if (err.ErrorsCleared()) {
                pvaStructureDataI->FinaliseStructure();
            }
            else {
                REPORT_ERROR(err, "Could not copy the received StructuredDataI");
            }
        }
    }

    if (err.ErrorsCleared()) {
        epics::pvData::FieldBuilderPtr fieldBuilder = epics::pvData::getFieldCreate()->createFieldBuilder();
        epics::pvData::PVStructurePtr structPtr = epics::pvData::getPVDataCreate()->createPVStructure(pvaStructureDataI->GetRootStruct());
        epics::pvAccess::RPCClient::shared_pointer client = epics::pvAccess::RPCClient::create(destination.Buffer());
        epics::pvData::PVStructurePtr response;
        if (client) {
            try {
                response = client->request(structPtr, timeout);
            }
            catch (epics::pvAccess::RPCRequestException &rpce) {
                REPORT_ERROR(ErrorManagement::Warning, "Exception while trying to access service @ %s", destination.Buffer());
            }
        }
        if (messageToTest->ExpectsReply()) {
            if (response) {
                ReferenceT<EPICSPVAStructureDataI> pvaStructureDataIReply(GlobalObjectsDatabase::Instance()->GetStandardHeap());
                pvaStructureDataIReply->SetStructure(response);
                /*ReferenceT<EPICSPVAStructureDataI> replyCopy(GlobalObjectsDatabase::Instance()->GetStandardHeap());
                err.parametersError = !pvaStructureDataIReply.Copy(*replyCopy.operator ->());*/
                if (err.ErrorsCleared()) {
                    err.parametersError = !messageToTest->Insert(pvaStructureDataIReply);
                }
            }
            messageToTest->SetAsReply(true);
        }
    }
    return err;
}

void EPICSRPCClientMessageFilter::SetTimeout(float64 timeoutIn) {
    timeout = timeoutIn;
}

uint32 EPICSRPCClientMessageFilter::GetTimeout() {
    return timeout;
}

CLASS_REGISTER(EPICSRPCClientMessageFilter, "1.0")
}

