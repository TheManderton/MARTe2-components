/**
 * @file MDSWriter.h
 * @brief Header file for class MDSWriter
 * @date 10/02/2017
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

 * @details This header file contains the declaration of the class MDSWriter
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef DATASOURCES_MDSWRITER_MDSWRITER_H_
#define DATASOURCES_MDSWRITER_MDSWRITER_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "DataSourceI.h"
#include "ProcessorType.h"
#include "MDSWriterNode.h"
#include "MessageI.h"
#include "RegisteredMethodsMessageFilter.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
namespace MARTe {

/**
 * @brief A DataSourceI interface which allows to store signals in an MDSplus database.
 *
 * @details Data written into this data source is temporarily stored in a circular buffer and
 * asynchronously flushed to the MDSplus database in the context of a separate thread.
 * This circular buffer can either be continuously stored or stored only when a given event occurs (see StoreOnTriggerbelow).
 *
 * This DataSourceI has the functions FlushSegments and OpenTree registered as an RPC.
 *
 * The configuration syntax is (names are only given as an example):
 * +MDSWriter_0 = {
 *     Class = MDSWriter
 *     NumberOfBuffers = 10 //Compulsory. Number of buffers in the circular buffer defined above
 *     CPUMask = 15 //Compulsory. Affinity assigned to the threads responsible for asynchronously flush data into the MDSplus database.
 *     StackSize = 10000000 //Compulsory. Stack size of the thread above.
 *     TreeName = "mds_m2test" //Compulsory. Name of the MDSplus tree.
 *     PulseNumber = 1 //Optional. If -1 a new pulse will be created.
 *     StoreOnTrigger = 1 //Compulsory. If 0 all the data in the circular buffer is continuously stored. If 1 data is stored when the Trigger signal is 1 (see below).
 *     EventName = "updatejScope" //Compulsory. Event sent to jScope when TimeRefresh seconds have elapsed.
 *     TimeRefresh = 5 //Compulsory. Event sent to jScope when TimeRefresh seconds have elapsed.
 *     NumberOfPreTriggers = 2 //Compulsory, when StoreOnTrigger = 1.  Number of cycles to store before the trigger.
 *     NumberOfPostTriggers = 1 //Compulsory, when StoreOnTrigger = 1.  Number of cycles to store after the trigger.
 *
 *     Signals = {
 *         Trigger = { //Compulsory when StoreOnTrigger = 1. Must be store in index 0 of the Signals.
 *             Type = 'uint8" //Type must be uint8
 *         }
 *         Time = { //Compulsory when StoreOnTrigger = 1. Can be store in any index, but TimeSignal must be set = 1
 *             Type = "uint32" //Type must be uint32
 *             TimeSignal = 1 //When set this signal will be considered as the time source against which all signals will be stored.
 *         }
 *         SignalUInt16F = { //As many as required.
 *             NodeName = "SIGUINT16F" //MDSplus node name
 *             Period = 2 //Period between signal samples.
 *             MakeSegmentAfterNWrites = 4 //Force the creation of a segment after N MARTe cycles.
 *             DecimatedNodeName = "SIGUINT16D" //Optional the node where MDSplus stores the automatically computed decimated signal.
 *             MinMaxResampleFactor = 4 //Compulsory if DecimatedNodeName is set. Decimation factor that MDSplus applies to the decimated version of the signal.
 *             SamplePhase = 0 //Optional. Shift the time vector by SamplePhase * Period
 *         }
 *         ...
 *     }
 * }
 */
class MDSWriter: public DataSourceI, public MessageI {
public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief Default constructor.
     * @details Initialises all the optional parameters as described in the class description.
     * Registers the RPC FlushSegments and OpenTree callback functions.
     */
MDSWriter    ();

    /**
     * @brief Destructor.
     * @details Flushes any open segments and frees the circular buffer.
     */
    virtual ~MDSWriter();

    /**
     * @brief See DataSourceI::AllocateMemory. NOOP.
     * @return true.
     */
    virtual bool AllocateMemory();

    /**
     * @brief See DataSourceI::GetNumberOfMemoryBuffers.
     * @return 1.
     */
    virtual uint32 GetNumberOfMemoryBuffers();

    /**
     * @brief See DataSourceI::GetSignalMemoryBuffer.
     */
    virtual bool GetSignalMemoryBuffer(const uint32 signalIdx,
            const uint32 bufferIdx,
            void *&signalAddress);

    /**
     * @brief See DataSourceI::GetNumberOfMemoryBuffers.
     * @details Only OutputSignals are supported.
     * @return MemoryMapAsyncOutputBroker if storeOnTrigger == 0, MemoryMapAsyncTriggerOutputBroker otherwise.
     */
    virtual const char8 *GetBrokerName(StructuredDataI &data,
            const SignalDirection direction);

    /**
     * @brief See DataSourceI::GetInputBrokers.
     * @return false.
     */
    virtual bool GetInputBrokers(ReferenceContainer &inputBrokers,
            const char8* const functionName,
            void * const gamMemPtr);

    /**
     * @brief See DataSourceI::GetOutputBrokers.
     * @details If storeOnTrigger == 0 it adds a MemoryMapAsyncOutputBroker instance to
     *  the inputBrokers, otherwise it adds a MemoryMapAsyncTriggerOutputBroker instance to the outputBrokers.
     * @pre
     *   GetNumberOfFunctions() == 1u
     */
    virtual bool GetOutputBrokers(ReferenceContainer &outputBrokers,
            const char8* const functionName,
            void * const gamMemPtr);

    /**
     * @brief Calls Execute on all the MDSWriterNodes and if sufficient time has elapsed, issues an MDSplus::Event.
     * @return true if the MDSWriterNode::Execute returns true on all the nodes.
     */
    virtual bool Synchronise();

    /**
     * @brief See DataSourceI::PrepareNextState. NOOP.
     * @return true.
     */
    virtual bool PrepareNextState(const char8 * const currentStateName,
            const char8 * const nextStateName);

    /**
     * @brief Loads and verifies the configuration parameters detailed in the class description.
     * @return true if all the mandatory parameters are correctly specified and if the specified optional parameters have valid values.
     */
    virtual bool Initialise(StructuredDataI & data);

    /**
     * @brief Final verification of all the parameters and setup of the MDSTreeNode instances.
     * @details This method verifies that all the parameters (e.g. number of samples) requested by the GAMs interacting with this DataSource
     *  are valid and consistent with the parameters set during the initialisation phase.
     * In particular the following conditions shall be met:
     * - If relevant, the Trigger signal shall have type uint8
     * - If relevant, the Time signal shall have type uint32
     * - The number of samples of all the MDS signals is one.
     * - At least one MDS plus signal (apart from the eventual Trigger and Time signal) is set.
     * @return true if all the parameters are valid and, when PulseNumber is defined, if an MDSplus Tree can be successfully opened.
     */
    virtual bool SetConfiguredDatabase(StructuredDataI & data);

    /**
     * @brief Calls MDSTreeNode::Flush on all the MDSplus signals, constraining the creation of a new segment even if the number of
     *  cycles is less than MakeSegmentAfterNWrites
     * @return true if all the MDSTreeNode::Flush return no error.
     */
    ErrorManagement::ErrorType FlushSegments();

    /**
     * @brief Opens a new MDSplus tree.
     * @param[in] pulseNumber the MDSplus pulse number. If -1 a new pulse number will be created.
     * @return ErrorManagement::NoError if a tree against that pulse number can be sucessfully opened.
     */
    ErrorManagement::ErrorType OpenTree(int32 pulseNumber);

private:

    /**
     * CPU count at which the last MDS plus event was fired.
     */
    uint64 lastTimeRefreshCount;

    /**
     * Fires an MDS plus event warning about new data every refreshEveryCounts CPU counts.
     */
    uint64 refreshEveryCounts;

    /**
     * The tree pulse number.
     */
    int32 pulseNumber;

    /**
     * True if the data is only to be stored in MDS plus following a trigger.
     */
    bool storeOnTrigger;

    /**
     * Number of pre buffers when StoreOnTrigger == 1.
     */
    uint32 numberOfPreTriggers;

    /**
     * Number of post buffers when StoreOnTrigger == 1.
     */
    uint32 numberOfPostTriggers;

    /**
     * Number of buffers in the circular buffer.
     */
    uint32 numberOfBuffers;

    /**
     * The number of MDS signals declared (i.e. the ones that have the property Node= defined
     */
    uint32 numberOfMDSSignals;

    /**
     * Holds an MDSWriterNode for each Signal declared in the DataSourceI.
     */
    MDSWriterNode **nodes;

    /**
     * The index of the time signal in the signal list.
     */
    int32 timeSignalIdx;

    /**
     * Offset of each signal in the dataSourceMemory
     */
    uint32 *offsets;

    /**
     * Memory holding all the signals that are to be stored, for each cycle, in MDSplus
     */
    char8 *dataSourceMemory;

    /**
     * The affinity of the thread that asynchronously flushes data into MDSplus.
     */
    ProcessorType cpuMask;

    /**
     * The size of the stack of the thread that asynchronously flushes data into MDSplus.
     */
    uint32 stackSize;

    /**
     * The name of the MDSplus tree.
     */
    StreamString treeName;

    /**
     * The name of the jScope event to be fired every refreshTime seconds.
     */
    StreamString eventName;

    /**
     * The MDSplus tree.
     */
    MDSplus::Tree *tree;

    /**
     * Stores the configuration information received at Initialise.
     */
    ConfigurationDatabase originalSignalInformation;

    /**
     * Filter to receive the RPC which allows to change the pulse number.
     */
    ReferenceT<RegisteredMethodsMessageFilter> filter;
};
}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DATASOURCES_MDSWRITER_MDSWRITER_H_ */
