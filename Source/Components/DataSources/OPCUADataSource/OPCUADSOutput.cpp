/**
 * @file OPCUADSOutput.cpp
 * @brief Source file for class OPCUADSOutput
 * @date 10 Jan 2019 TODO Verify the value and format of the date
 * @author lporzio TODO Verify the name and format of the author
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
 * the class OPCUADSOutput (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "OPCUADSOutput.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

OPCUADSOutput::OPCUADSOutput() :
        DataSourceI() {
    masterClient = NULL_PTR(OPCUAClientWrapper *);
    numberOfClients = 0u;
    numberOfNodes = 0u;
    paths = NULL_PTR(StreamString *);
    namespaceIndexes = NULL_PTR(uint32 *);
    tempPaths = NULL_PTR(StreamString *);
    tempNamespaceIndexes = NULL_PTR(uint32 *);
    serverAddress = "";
    numberOfBuffers = 0u;
    cpuMask = 0xffu;
    stackSize = THREADS_DEFAULT_STACKSIZE * 2u;
    nDimensions = NULL_PTR(uint8 *);
    nElements = NULL_PTR(uint32 *);
    types = NULL_PTR(TypeDescriptor *);
}

OPCUADSOutput::~OPCUADSOutput() {
    delete[] nDimensions;
    delete[] nElements;
}

bool OPCUADSOutput::Initialise(StructuredDataI & data) {
    bool ok = DataSourceI::Initialise(data);
    if (ok) {
        if (ok) {
            ok = data.Read("Address", serverAddress);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "Cannot read the Address attribute");
            }
        }
        if (ok) {
            ok = data.Read("NumberOfBuffers", numberOfBuffers);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "NumberOfBuffers shall be specified");
            }
        }
        if (ok) {
            if (!data.Read("CPUs", cpuMask)) {
                REPORT_ERROR(ErrorManagement::Information, "No CPUs defined. Using default = %d", cpuMask);
            }
            else {

            }
            if (!data.Read("StackSize", stackSize)) {
                REPORT_ERROR(ErrorManagement::Information, "No StackSize defined. Using default = %d", stackSize);
            }
        }
        ok = data.MoveRelative("Signals");
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Could not move to the Signals section");
        }
        if (ok) {
            ok = signalsDatabase.MoveRelative("Signals");
        }
        /* Do not allow to add signals in run-time */
        if (ok) {
            ok = signalsDatabase.Write("Locked", 1u);
            numberOfClients = (signalsDatabase.GetNumberOfChildren() - 1u);
            tempPaths = new StreamString[numberOfClients];
            tempNamespaceIndexes = new uint32[numberOfClients];
            for (uint32 i = 0u; (i < numberOfClients) && (ok); i++) {
                ok = signalsDatabase.MoveRelative(signalsDatabase.GetChildName(i));
                if (ok) {
                    ok = signalsDatabase.Read("Path", tempPaths[i]);
                    if (!ok) {
                        REPORT_ERROR(ErrorManagement::ParametersError, "Cannot read the Path attribute from signal %d", i);
                    }
                }
                if (ok) {
                    ok = signalsDatabase.Read("NamespaceIndex", tempNamespaceIndexes[i]);
                    if (!ok) {
                        REPORT_ERROR(ErrorManagement::ParametersError, "Cannot read the NamespaceIndex attribute from signal %d", i);
                    }
                }

                ok = signalsDatabase.MoveToAncestor(1u);
            }
        }
        if (ok) {
            ok = signalsDatabase.MoveToAncestor(1u);
        }
        if (ok) {
            ok = data.MoveToAncestor(1u);
        }
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Error during Initialise!");
    }
    return ok;
}

bool OPCUADSOutput::SetConfiguredDatabase(StructuredDataI & data) {
    bool ok = DataSourceI::SetConfiguredDatabase(data);
    uint32 nOfSignals = GetNumberOfSignals();
    numberOfNodes = nOfSignals;
    nDimensions = new uint8[numberOfNodes];
    nElements = new uint32[numberOfNodes];
    types = new TypeDescriptor[numberOfNodes];
    if (ok) {
        ok = (nOfSignals > 0u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "At least one signal shall be defined");
        }
    }
    StreamString signalName;
    for (uint32 k = 0u; k < numberOfNodes; k++) {
        ok = GetSignalNumberOfDimensions(k, nDimensions[k]);
        if (nDimensions[k] > 1 && ok) {
            GetSignalName(k, signalName);
            REPORT_ERROR(ErrorManagement::ParametersError, "Signal %s has Number Of Dimensions = %d. Multidimensional arrays not supported yet.",
                         signalName.Buffer(), nDimensions[k]);
            ok = false;
        }
        if (ok) {
            ok = GetSignalNumberOfElements(k, nElements[k]);
        }
        if (ok) {
            types[k] = GetSignalType(k);
        }
    }
    if (ok) {
        paths = new StreamString[numberOfNodes];
        namespaceIndexes = new uint32[numberOfNodes];
        StreamString sigName, pathToken, sigToken;
        char8 ignore;
        for (uint32 i = 0u; i < numberOfNodes; i++) {
            sigName = "";
            GetSignalName(i, sigName);

            /* Getting the first name from the signal path */
            sigName.Seek(0LLU);
            ok = sigName.GetToken(sigToken, ".", ignore);

            for (uint32 j = 0u; j < numberOfClients; j++) {
                /* This cycle will save the last token found */
                tempPaths[j].Seek(0LLU);
                do {
                    pathToken = "";
                    ok = tempPaths[j].GetToken(pathToken, ".", ignore);
                    if (pathToken == sigToken) {
                        paths[i] = tempPaths[j];
                        namespaceIndexes[i] = tempNamespaceIndexes[j];
                        ok = false; /* Exit from the cycle */
                    }
                }
                while (ok);
            }

            /* Then we add to the path the remaining node names */
            StreamString dotToken = ".";
            do {
                sigToken = "";
                ok = sigName.GetToken(sigToken, ".", ignore);
                if (ok) {
                    paths[i] += dotToken;
                    paths[i] += sigToken;
                }
            }
            while (ok);
            ok = true;
            /* Debug info */
            //REPORT_ERROR(ErrorManagement::Information, "%s", paths[i].Buffer());
        }
    }
//    if (ok) {
//        if (numberOfNodes > 0u) {
//            clients = new OPCUAClientWrapper[numberOfNodes];
//        }
//    }
//    if (ok) {
//        uint32 i;
//        /* Insert Address from configuration */
//        char* s = new char[strlen(reinterpret_cast<const char*>(serverAddress.Buffer()))];
//        strcpy(s, reinterpret_cast<const char*>(serverAddress.Buffer()));
//        for (i = 0u; i < numberOfNodes; i++) {
//            clients[i].SetServerAddress(s);
//            clients[i].Connect();
//        }
//    }
    if (ok) {
        /* Setting up the master Client who will perform the operations */
        char* s = new char[strlen(reinterpret_cast<const char*>(serverAddress.Buffer()))];
        strcpy(s, reinterpret_cast<const char*>(serverAddress.Buffer()));
        masterClient = new OPCUAClientWrapper("Write");
        masterClient->SetServerAddress(s);
        ok = masterClient->Connect();
        if (ok) {
            REPORT_ERROR(ErrorManagement::Information, "The connection with the OPCUA Server has been established successfully!");
        }
        ok = masterClient->SetTargetNodes(namespaceIndexes, paths, numberOfNodes);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Cannot find one or more signals in the Server.");
        }
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Error during configuration.");
    }
    return ok;
}

bool OPCUADSOutput::AllocateMemory() {

    return true;
}

uint32 OPCUADSOutput::GetNumberOfMemoryBuffers() {
    return 1u;
}

bool OPCUADSOutput::GetSignalMemoryBuffer(const uint32 signalIdx,
                                          const uint32 bufferIdx,
                                          void *&signalAddress) {
    StreamString opcDisplayName;
    bool ok = GetSignalName(signalIdx, opcDisplayName);
    /* Debug info */
    //REPORT_ERROR(ErrorManagement::Information, "Searching for signal [%s]", opcDisplayName.Buffer());
    if (ok) {
        if ((types[signalIdx].type == CArray) || (types[signalIdx].type == BT_CCString) || (types[signalIdx].type == PCString)
                || (types[signalIdx].type == SString)) {
            //uint32 numberOfChars = 0u;
            //If char8 the number of elements will be 1. Correct this!
            //GetSignalNumberOfElements(signalIdx, numberOfChars);
            //clients[signalIdx].SetMaxNumberOfChar(numberOfChars);
        }
        if (ok) {
            ok = masterClient->GetSignalMemory(signalAddress, signalIdx, types[signalIdx], nElements[signalIdx], nDimensions[signalIdx]);
        }
    }
    if (ok) {
        ok = (signalAddress != NULL_PTR(void *));
    }

    return ok;
}

const char8 * OPCUADSOutput::GetBrokerName(StructuredDataI &data,
                                           const SignalDirection direction) {
    const char8* brokerName = "";
    if (direction == OutputSignals) {
        brokerName = "MemoryMapSynchronisedOutputBroker";
    }
    return brokerName;
}

bool OPCUADSOutput::PrepareNextState(const char8 * const currentStateName,
                                     const char8 * const nextStateName) {
    return true;
}

bool OPCUADSOutput::Synchronise() {
    bool ok = true;
    ok = masterClient->Write(numberOfNodes);
    return ok;
}

CLASS_REGISTER(OPCUADSOutput, "1.0")

}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

