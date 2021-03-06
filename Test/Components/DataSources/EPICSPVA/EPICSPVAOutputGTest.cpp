/**
 * @file EPICSPVAOutputGTest.cpp
 * @brief Source file for class EPICSPVAOutputGTest
 * @date 22/10/2018
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
 * the class EPICSPVAOutputGTest (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <limits.h>
#include "gtest/gtest.h"

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "EPICSPVAOutputTest.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
TEST(EPICSPVAOutputGTest,TestConstructor) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestConstructor());
}

TEST(EPICSPVAOutputGTest,TestAllocateMemory) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestAllocateMemory());
}

TEST(EPICSPVAOutputGTest,TestGetNumberOfMemoryBuffers) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetNumberOfMemoryBuffers());
}

TEST(EPICSPVAOutputGTest,TestGetSignalMemoryBuffer) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetSignalMemoryBuffer());
}

TEST(EPICSPVAOutputGTest,TestGetBrokerName) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetBrokerName());
}

TEST(EPICSPVAOutputGTest,TestGetBrokerName_InputSignals) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetBrokerName_InputSignals());
}

TEST(EPICSPVAOutputGTest,TestGetOutputBrokers) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetOutputBrokers());
}

TEST(EPICSPVAOutputGTest,TestInitialise) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestInitialise());
}

TEST(EPICSPVAOutputGTest,TestInitialise_Defaults) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestInitialise_Defaults());
}

TEST(EPICSPVAOutputGTest,TestInitialise_False_Signals) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestInitialise_False_Signals());
}

TEST(EPICSPVAOutputGTest,TestGetCPUMask) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetCPUMask());
}

TEST(EPICSPVAOutputGTest,TestGetStackSize) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetStackSize());
}

TEST(EPICSPVAOutputGTest,TestGetNumberOfBuffers) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestGetNumberOfBuffers());
}

TEST(EPICSPVAOutputGTest,TestSetConfiguredDatabase) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase());
}

TEST(EPICSPVAOutputGTest,TestSetConfiguredDatabase_False_NoSignals) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_NoSignals());
}

TEST(EPICSPVAOutputGTest,TestSetConfiguredDatabase_False_MoreThanOneGAM) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_MoreThanOneGAM());
}

TEST(EPICSPVAOutputGTest,TestSetConfiguredDatabase_False_Samples) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_Samples());
}

TEST(EPICSPVAOutputGTest,TestIsIgnoringBufferOverrun) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestIsIgnoringBufferOverrun());
}

TEST(EPICSPVAOutputGTest,TestSynchronise) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSynchronise());
}

TEST(EPICSPVAOutputGTest,TestSynchronise_Arrays) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSynchronise_Arrays());
}

TEST(EPICSPVAOutputGTest,TestSynchronise_StructuredType) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSynchronise_StructuredType());
}

TEST(EPICSPVAOutputGTest,TestSynchronise_Arrays_StructuredType) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSynchronise_Arrays_StructuredType());
}

TEST(EPICSPVAOutputGTest,TestSynchronise_Arrays_Elements) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSynchronise_Arrays_Elements());
}

TEST(EPICSPVAOutputGTest,TestSynchronise_False_BadSignal) {
    EPICSPVAOutputTest test;
    ASSERT_TRUE(test.TestSynchronise_False_BadSignal());
}

