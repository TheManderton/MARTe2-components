/**
 * @file Waveform.h
 * @brief Header file for class Waveform
 * @date May 19, 2017
 * @author aneto
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

 * @details This header file contains the declaration of the class Waveform
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef WAVEFORMGENERATOR_WAVEFORM_H_
#define WAVEFORMGENERATOR_WAVEFORM_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "GAM.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
namespace MARTe {

/**
 * @brief Generic class to implement any kind of waveform
 * @details this class contains the generic common functions and variables to implement any output signal
 *
 * Three different GAMs are foreseen to use this generic Class:
 * WaveformSiGAM
 * WaveformDotsDefGAM
 * WaveformChirpGAM
 */

class Waveform: public GAM {
public:

    /**
     * @brief Default constructor
     * @post
     * input = NULL_PTR(uint32 *);
     * output = NULL_PTR(void **);
     * nOfInputSignals = 0u;
     * nOfOutputSignals= 0u;
     * numberOfInputElements = 0u;
     * numberOfOutputElements = 0u;
     * numberOfInputSamples = 0u;
     * numberOfOutputSamples = 0u;
     * startTriggerTime = NULL_PTR(uint32 *);
     * stopTriggerTime = NULL_PTR(uint32 *);
     * numberOfStartTriggers = 0u;
     * numberOfStopTriggers = 0u;
     * triggersOn = false;
     * indexStartTriggersArray = 0u;
     * indexStopTriggersArray = 0u;
     * signalOn = false;
     * utimeIncrement = 0u;
     * ucurrentTime = 0u;
     * time0 = 0u;
     * time1 = 0u;
     * timeIncrement = 0.0;
     * timeState = 0u;
     */
    Waveform();

    /**
     * @brief Default constructor
     * @details Frees only the allocated memory by this class and set the pointers to 0
     * @post
     *  outputValue = NULL_PTR(void **);
     *  startTriggerTime = NULL_PTR(uint32 *);
     */
    virtual ~Waveform();

    /**
     * @brief Initialize the common variable from a configration file
     * @details Initialise and verifies the trigger mechanism. This function checks the size of the startTriggerTime
     * and stopTriggerTime and checks that startTriggerTime[i] < stopTriggerTime[i]
     * @param[in] data. the GAM configuration
     * @return true if the trigger times are valid
     */
    virtual bool Initialise(StructuredDataI & data);

    /**
     * @brief Setup the input output variables
     * @details Initialize the input output pointers and verify the number of inputs and output sizes
     * @return true if the input and outputs are correctly initialize
     */
    virtual bool Setup();

    /**
     * @brief Generic implementation of the output
     * @detail This function decides the type function to call and calls the specific implementation of the signal generator
     * with the correct type defined in the configuration file
     */
    virtual bool Execute();

    virtual bool GetUInt8Value() = 0;

    virtual bool GetInt8Value() = 0;

    virtual bool GetUInt16Value() = 0;

    virtual bool GetInt16Value() = 0;

    virtual bool GetUInt32Value() = 0;

    virtual bool GetInt32Value() = 0;

    virtual bool GetUInt64Value() = 0;

    virtual bool GetInt64Value() = 0;

    virtual bool GetFloat32Value() = 0;

    virtual bool GetFloat64Value() = 0;

protected:
    uint32 *inputTime;
    void **outputValue;
    uint32 numberOfOutputElements;
    uint32 nOfOutputSignals;
    /*Number of start triggers which will be executed.*/
    uint32 *startTriggerTime;
    uint32 *stopTriggerTime;
    uint32 numberOfStartTriggers;
    uint32 numberOfStopTriggers;
    uint32 indexStartTriggersArray;
    uint32 indexStopTriggersArray;
    uint32 ucurrentTime;
    uint32 utimeIncrement;
    bool triggersOn;
    bool signalOn;
    /*first time received in us */
    uint32 time0;
    /*first time received in us */
    uint32 time1;
    /* diffTime = time1 - time0. It is used to know the sample frequency */
    float64 timeIncrement;
    /* it is used to determine when to save time0, time1 or when generate the signal */
    uint8 timeState;

    uint32 indexOutputSignal;

private:
    TypeDescriptor typeVariableIn;
    TypeDescriptor *typeVariableOut;
    uint32 nOfInputSignals;
    uint32 numberOfInputElements;
    uint32 numberOfInputSamples;
    uint32 numberOfOutputSamples;

    bool ValidateTimeTriggers() const {
        bool ret = true;
        if (numberOfStopTriggers > 0u) {
            for (uint32 i = 0u; (i < (numberOfStopTriggers - 1u)) && ret; i++) {
                //ret = ret && (stopTriggerTime[i] > startTriggerTime[i]);
                ret = (stopTriggerTime[i] > startTriggerTime[i]);
                if (ret) {
                    //MISRA...Suspicious truncation..
                    uint32 aux = i + 1u;
                    ret = (stopTriggerTime[i] < startTriggerTime[aux]);
                }
            }
            if (ret) {
                ret = (stopTriggerTime[numberOfStopTriggers - 1u] > startTriggerTime[numberOfStopTriggers - 1u]);
            }
            if (ret) {
                if (numberOfStartTriggers > numberOfStopTriggers) {
                    ret = (stopTriggerTime[numberOfStopTriggers - 1u] < startTriggerTime[numberOfStartTriggers - 1u]);
                }
            }
        }
        return ret;
    }

    bool IsValidType(TypeDescriptor const &typeRef) const{
        bool retVal;
        bool *auxBool = (new bool[10u]);
        auxBool[0] = typeRef == Float32Bit;
        auxBool[1] = typeRef == Float64Bit;
        auxBool[2] = typeRef == SignedInteger8Bit;
        auxBool[3] = typeRef == UnsignedInteger8Bit;
        auxBool[4] = typeRef == SignedInteger16Bit;
        auxBool[5] = typeRef == UnsignedInteger16Bit;
        auxBool[6] = typeRef == SignedInteger32Bit;
        auxBool[7] = typeRef == UnsignedInteger32Bit;
        auxBool[8] = typeRef == SignedInteger64Bit;
        auxBool[9] = typeRef == UnsignedInteger64Bit;
        retVal = ((auxBool[0]) || (auxBool[1]) || (auxBool[2])
                || (auxBool[3]) || (auxBool[4]) || (auxBool[5])
                || (auxBool[6]) || (auxBool[7]) || (auxBool[8])
                || (auxBool[9]));
        delete [] auxBool;
        return retVal;
    }
};

}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* WAVEFORM_H_ */

