/**
 * @file EPICSPVAMessageI.cpp
 * @brief Source file for class EPICSPVAMessageI
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
 * the class EPICSPVAMessageI (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include "pv/pvData.h"

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "EPICSPVAMessageI.h"
#include "EPICSPVAStructureDataI.h"
#include "Message.h"
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {

EPICSPVAMessageI::EPICSPVAMessageI() :
        EPICSRPCService(), Object(), MessageI() {
    ReferenceContainer::AddBuildToken('_');
}

EPICSPVAMessageI::~EPICSPVAMessageI() {

}

epics::pvData::PVStructurePtr EPICSPVAMessageI::request(epics::pvData::PVStructure::shared_pointer const & args) {

    EPICSPVAStructureDataI config;
    config.InitStructure();
    config.SetStructure(args);
    ConfigurationDatabase cdbMsg;
    ReferenceT<Message> msg;
    bool ok = config.Copy(cdbMsg);
    if (ok) {
        msg = ReferenceT<Message>(GlobalObjectsDatabase::Instance()->GetStandardHeap());
        //In theory I could have passed the PVStructure config directly to the Initialise
        ok = msg->Initialise(cdbMsg);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The request is not a valid MARTe message");
        }
    }
    ErrorManagement::ErrorType err;
    bool expectsReply = false;
    if (ok) {
        expectsReply = msg->ExpectsReply();
        err = SendMessage(msg, this);

        ok = (err.ErrorsCleared());
        if (!ok) {
            REPORT_ERROR(err, "Could not send the message");
        }
    }

    epics::pvData::FieldBuilderPtr fieldBuilder = epics::pvData::getFieldCreate()->createFieldBuilder();
    epics::pvData::StructureConstPtr topStructure = fieldBuilder->createStructure();
    epics::pvData::PVStructurePtr reply(epics::pvData::getPVDataCreate()->createPVStructure(topStructure));
    if (expectsReply) {
        EPICSPVAStructureDataI replyStructuredDataI;
        replyStructuredDataI.InitStructure();
        ReferenceT<StructuredDataI> replyMsg = msg->Get(0u);
        if (replyMsg.IsValid()) {
            replyMsg->Copy(replyStructuredDataI);
        }
        if (replyStructuredDataI.FinaliseStructure()) {
            reply = replyStructuredDataI.GetRootStruct();
        }
    }
    return reply;
}

CLASS_REGISTER(EPICSPVAMessageI, "1.0")
}
