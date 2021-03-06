/**
 * @file EPICSPVADatabaseGTest.cpp
 * @brief Source file for class EPICSPVADatabaseGTest
 * @date 25/09/2018
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
 * the class EPICSPVADatabaseGTest (public, protected, and private). Be aware that some 
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
#include "EPICSPVADatabaseTest.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
TEST(EPICSPVADatabaseGTest,TestConstructor) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestConstructor());
}

TEST(EPICSPVADatabaseGTest,TestInitialise_Defaults) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestInitialise_Defaults());
}
	
TEST(EPICSPVADatabaseGTest,TestInitialise_NoDefaults) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestInitialise_NoDefaults());
}

TEST(EPICSPVADatabaseGTest,TestExecute) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestExecute());
}

TEST(EPICSPVADatabaseGTest,TestExecute_StructuredTypes) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestExecute_StructuredTypes());
}

TEST(EPICSPVADatabaseGTest,TestExecute_Array) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestExecute_Array());
}

TEST(EPICSPVADatabaseGTest,TestStart) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestStart());
}

TEST(EPICSPVADatabaseGTest,TestGetStackSize) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestGetStackSize());
}

TEST(EPICSPVADatabaseGTest,TestGetCPUMask) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestGetCPUMask());
}

TEST(EPICSPVADatabaseGTest,TestGetStatus) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestGetStatus());
}

TEST(EPICSPVADatabaseGTest,TestGetServerContext) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestGetServerContext());
}

TEST(EPICSPVADatabaseGTest,TestExecute_False_NoRecord) {
    EPICSPVADatabaseTest test;
    ASSERT_TRUE(test.TestExecute_False_NoRecord());
}
