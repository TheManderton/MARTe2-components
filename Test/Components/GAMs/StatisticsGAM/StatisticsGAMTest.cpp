/**
 * @file StatisticsGAMTest.cpp
 * @brief Source file for class StatisticsGAMTest
 * @date 30/01/2017
 * @author Bauvir Bertrand 
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
 * the class StatisticsGAMTest (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "AdvancedErrorManagement.h"
#include "ErrorManagement.h"
#include "ErrorInformation.h"
#include "ObjectRegistryDatabase.h"
#include "GlobalObjectsDatabase.h"
#include "ConfigurationDatabase.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"
#include "GAM.h"
#include "StatisticsGAM.h"
#include "StatisticsGAMTest.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

class ConstantGAM: public GAM {
public:
    CLASS_REGISTER_DECLARATION()

    ConstantGAM();
    virtual ~ConstantGAM();
    virtual bool Setup();
    virtual bool Execute();
};

ConstantGAM::ConstantGAM() :
        GAM() {

}

ConstantGAM::~ConstantGAM() {

}

bool ConstantGAM::Setup() {

    bool ret = (GetNumberOfInputSignals() == 0u);

    if (!ret) {
        REPORT_ERROR(ErrorManagement::InitialisationError, "GetNumberOfInputSignals() != 0u");
    }

    uint32 signalIndex;

    for (signalIndex = 0u; (signalIndex < GetNumberOfOutputSignals()) && (ret); signalIndex++) {

        StreamString signalName;

	if (ret) {
	    ret = GetSignalName(OutputSignals, signalIndex, signalName);
	}

        uint32 signalByteSize = 0u;

	if (ret) {
	    ret = GetSignalByteSize(OutputSignals, signalIndex, signalByteSize);
	}

        TypeDescriptor signalType = InvalidType;

	if (ret) {
	    signalType = GetSignalType(OutputSignals, signalIndex);
	    ret = (signalType != InvalidType);
	}

	/* This is the basic element type */
	/* Handle the multi-dimensional case */
	
        AnyType signalDefValue (signalType, 0u, GetOutputSignalMemory(signalIndex));

	uint32 signalNumberOfDimensions = 0u;

	if (ret) {
	    ret = GetSignalNumberOfDimensions(OutputSignals, signalIndex, signalNumberOfDimensions);
	}

	if (ret) {
	    REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "signalDefValue.SetNumberOfDimensions(%u)", signalNumberOfDimensions);
	    signalDefValue.SetNumberOfDimensions(signalNumberOfDimensions);
	}

	uint32 signalNumberOfElements = 0u;

	if (ret) {
	    ret = GetSignalNumberOfElements(OutputSignals, signalIndex, signalNumberOfElements);
	}

	if (ret) {
	    uint32 dimensionIndex;

	    for (dimensionIndex = 0u; dimensionIndex < signalNumberOfDimensions; dimensionIndex++) {
	        REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "signalDefValue.SetNumberOfElements(%u, %u)", dimensionIndex, signalNumberOfElements);
	        signalDefValue.SetNumberOfElements(dimensionIndex, signalNumberOfElements);
	    }
	}

	if (ret) {
	    ret = GetSignalDefaultValue(OutputSignals, signalIndex, signalDefValue);
	}

	if (ret) {
	    REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "ConstantGAM::Setup - '%s' '%!'", signalName.Buffer(), signalDefValue);
	} else {
	    REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "ConstantGAM::Setup - GetSignalDefaultValue '%s'", signalName.Buffer());
	} 
	
    }

    return ret;
}

bool ConstantGAM::Execute() {
    return true;
}

CLASS_REGISTER(ConstantGAM, "1.0")

}

/**
 * Starts a MARTe application that uses this driver instance.
 */

namespace StatisticsGAMTestHelper {

bool ConfigureApplication(const MARTe::char8 * const config) {

    using namespace MARTe;

    ConfigurationDatabase cdb;
    StreamString configStream = config;
    StreamString err;
    configStream.Seek(0);
    StandardParser parser(configStream, cdb, &err);

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();

    bool ok = parser.Parse();

    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "StandardParser::Parse failed");
    } else {
        god->Purge();
        ok = god->Initialise(cdb);
    }

    ReferenceT<RealTimeApplication> application;

    if (ok) {
        application = god->Find("Test");
        ok = application.IsValid();
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::IsValid failed");
    } else {
        ok = application->ConfigureApplication();
    }

    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::ConfigureApplication failed");
    } 

    return ok;

}

static inline bool StartApplication(const MARTe::char8 * const state = "Running") {

    using namespace MARTe;

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    ReferenceT<RealTimeApplication> application = god->Find("Test");;
    bool ok = application.IsValid();

    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::IsValid failed");
    } else {
        ok = application->PrepareNextState(state);
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::PrepareNextState failed");
    } else {
        ok = application->StartNextStateExecution();
    }

    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::StartNextStateExecution failed");
    } 

    return ok;

}

static inline bool StopApplication() {

    using namespace MARTe;

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    ReferenceT<RealTimeApplication> application = god->Find("Test");;
    bool ok = application.IsValid();

    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::IsValid failed");
    } else {
        ok = application->StopCurrentStateExecution();
    }

    if (!ok) {
        REPORT_ERROR(ErrorManagement::InternalSetupError, "RealTimeApplication::StopCurrentStateExecution failed");
    } 

    god->Purge();

    return ok;

}

static inline bool TestIntegratedInApplication(const MARTe::char8 * const config) {

    using namespace MARTe;

    bool ok = ConfigureApplication(config);

    if (ok) {
        ok = StartApplication();
    }

    Sleep::Sec(1.0);

    if (ok) {
        ok = StopApplication();
    }

    return ok;

}

} /* namespace StatisticsGAMTestHelper */

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

StatisticsGAMTest::StatisticsGAMTest() {
}

StatisticsGAMTest::~StatisticsGAMTest() {
}

bool StatisticsGAMTest::TestConstructor() {
    using namespace MARTe;
    StatisticsGAM gam;

    bool ok = true;

    return ok;
}

bool StatisticsGAMTest::TestInitialise() {
    using namespace MARTe;
    StatisticsGAM gam;
    ConfigurationDatabase config;
    bool ok = gam.Initialise(config);
    return ok;
}

bool StatisticsGAMTest::TestInitialise_WindowSize() {
    using namespace MARTe;
    StatisticsGAM gam;
    ConfigurationDatabase config;

    uint32 size = 1000u;
    bool ok = true;

    ok &= config.Write("WindowSize", size);
    ok &= gam.Initialise(config);
    return ok;
}

bool StatisticsGAMTest::TestSetup_float32() {
    const MARTe::char8 * const config = ""
            "$Test = {"
            "    Class = RealTimeApplication"
            "    +Functions = {"
            "        Class = ReferenceContainer"
            "        +Constants = {"
            "            Class = ConstantGAM"
            "            OutputSignals = {"
            "                Constant_float32 = {"
            "                    DataSource = DDB"
            "                    Type = float32"
            "                    Default = 0.0"
            "                }"
            "                Constant_float64 = {"
            "                    DataSource = DDB"
            "                    Type = float64"
            "                    Default = 0.0"
            "                }"
            "            }"
            "        }"
            "        +Statistics = {"
            "            Class = StatisticsGAM"
            "            WindowSize = 100"
            "            InputSignals = {"
            "               Constant_float32 = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "            }"
            "            OutputSignals = {"
            "               Average_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "               Stdev_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "               Max_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "               Min_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "            }"
            "        }"
            "    }"
            "    +Data = {"
            "        Class = ReferenceContainer"
            "        DefaultDataSource = DDB"
            "        +DDB = {"
            "            Class = GAMDataSource"
            "        }"
            "        +Timings = {"
            "            Class = TimingDataSource"
            "        }"
            "    }"
            "    +States = {"
            "        Class = ReferenceContainer"
            "        +Running = {"
            "            Class = RealTimeState"
            "            +Threads = {"
            "                Class = ReferenceContainer"
            "                +Thread = {"
            "                    Class = RealTimeThread"
            "                    Functions = {Constants Statistics}"
            "                }"
            "            }"
            "        }"
            "    }"
            "    +Scheduler = {"
            "        Class = GAMScheduler"
            "        TimingDataSource = Timings"
            "    }"
            "}";

    bool ok = StatisticsGAMTestHelper::ConfigureApplication(config);

    if (ok) {
        using namespace MARTe;

        ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
        ReferenceT<RealTimeApplication> application = god->Find("Test");
        ReferenceT<StatisticsGAM> gam = application->Find("Functions.Statistics");

        ok = gam.IsValid();

        if (ok) {
	    ok = StatisticsGAMTestHelper::StartApplication();
	}

	if (ok) {
	  Sleep::Sec(1.0);
	}
    }

    if (ok) {
        ok = StatisticsGAMTestHelper::StopApplication();
    }
    
    return ok;
}

bool StatisticsGAMTest::TestExecute_Error() {
    using namespace MARTe;
    StatisticsGAM gam;

    bool ok = gam.Execute(); // Test the instantiated class
    return !ok; // Expect failure since StatisticsHelperT<> class has not yet been instantiated
}

bool StatisticsGAMTest::TestPrepareForNextState_Error() {
    using namespace MARTe;
    StatisticsGAM gam;

    bool ok = gam.PrepareNextState("FromCurrent","ToNext"); // Test the instantiated class
    return !ok; // Expect failure since StatisticsHelperT<> class has not yet been instantiated
}

bool StatisticsGAMTest::TestPrepareForNextState_Success() {
    using namespace MARTe;
    StatisticsGAM gam;

    const MARTe::char8 * const config = ""
            "$Test = {"
            "    Class = RealTimeApplication"
            "    +Functions = {"
            "        Class = ReferenceContainer"
            "        +Constants = {"
            "            Class = ConstantGAM"
            "            OutputSignals = {"
            "                Constant_float32 = {"
            "                    DataSource = DDB"
            "                    Type = float32"
            "                    Default = 0.0"
            "                }"
            "                Constant_float64 = {"
            "                    DataSource = DDB"
            "                    Type = float64"
            "                    Default = 0.0"
            "                }"
            "            }"
            "        }"
            "        +Statistics = {"
            "            Class = StatisticsGAM"
            "            WindowSize = 100"
            "            InputSignals = {"
            "               Constant_float32 = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "            }"
            "            OutputSignals = {"
            "               Average_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "               Stdev_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "               Max_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "               Min_ExecTime = {"
            "                   DataSource = DDB"
            "                   Type = float32"
            "               }"
            "            }"
            "        }"
            "    }"
            "    +Data = {"
            "        Class = ReferenceContainer"
            "        DefaultDataSource = DDB"
            "        +DDB = {"
            "            Class = GAMDataSource"
            "        }"
            "        +Timings = {"
            "            Class = TimingDataSource"
            "        }"
            "    }"
            "    +States = {"
            "        Class = ReferenceContainer"
            "        +Running = {"
            "            Class = RealTimeState"
            "            +Threads = {"
            "                Class = ReferenceContainer"
            "                +Thread = {"
            "                    Class = RealTimeThread"
            "                    Functions = {Constants Statistics}"
            "                }"
            "            }"
            "        }"
            "    }"
            "    +Scheduler = {"
            "        Class = GAMScheduler"
            "        TimingDataSource = Timings"
            "    }"
            "}";

    bool ok = StatisticsGAMTestHelper::ConfigureApplication(config);

    if (ok) {
        using namespace MARTe;

        ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
        ReferenceT<RealTimeApplication> application = god->Find("Test");
        ReferenceT<StatisticsGAM> gam = application->Find("Functions.Statistics");

        ok = gam.IsValid();

        if (ok) {
	    ok = StatisticsGAMTestHelper::StartApplication();
	}

	if (ok) {
	  Sleep::Sec(0.1);
	}
    }

    if (ok) {
        ok = StatisticsGAMTestHelper::StopApplication();
    }
    
    return ok;
}

