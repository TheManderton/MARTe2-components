/**
 * @file SharedDataAreaTest.h
 * @brief Header file for class SharedDataAreaTest
 * @date 25/01/2017
 * @author Ivan Herrero
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

 * @details This header file contains the declaration of the class SharedDataAreaTest
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef SHAREDDATAAREATEST_H_
#define SHAREDDATAAREATEST_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

/**
 * @brief Class for testing SharedDataArea.
 */
class SharedDataAreaTest {
public:

    /**
     * @brief Tests the constructor.
     */
    bool TestConstructor();

    /**
     * @brief Tests the GetSigblockProducerInterface method.
     */
    bool TestGetSigblockProducerInterface();

    /**
     * @brief Tests the GetSigblockConsumerInterface method.
     */
    bool TestGetSigblockConsumerInterface();

    /**
     * @brief Tests the BuildSharedDataAreaForMARTe method.
     */
    bool TestBuildSharedDataAreaForMARTe();

    /**
     * @brief Tests the BuildSharedDataAreaForEPICS method.
     */
    bool TestBuildSharedDataAreaForEPICS();

private:
    /**
     * @brief Test the interchange of data between a producer and a consumer
     * using one single thread.
     */
    template<typename SignalType>
    bool TestProducerConsumerInSingleThread(const char* const shmName,
                                            const unsigned int maxTests = 100);

    /**
     * @brief Test the interchange of data between a producer and a consumer
     * using two threads executing on the same process.
     *
     * @details
     *
     * This test assumes that 2 threads are concurrently passing sigblocks between
     * each other using the SharedDataArea mechanism. The threads can execute at
     * different paces, one acting as the producer and the other as the consumer.
     *
     * Assuming that the SharedDataArea uses a double buffer, it is expected than
     * if the producer runs faster than the consumer, some sigblocks will be
     * discarded (the double buffer always offers to the consumer the last written
     * sigblock).
     *
     * The test must assure than each read sigblock contains the same values
     * that were generated by the producer, and it must also check that each read
     * sigblock is fresher than the previous one (it is ok to not receive all the
     * sigblocks generated by the producer, but each new read sigblock must been
     * generated after the previous read one).
     *
     * I any case, the consumer always has to read, at least, the last sigblock
     * generated by the producer.
     */
    template<typename SignalType>
    bool TestProducerConsumerWithTwoThreads(const char* const shmName,
                                            const unsigned int maxTests = 100);

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* SHAREDDATAAREATEST_H_ */
