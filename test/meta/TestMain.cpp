/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#include "1test/TestRunner.h"
#include "1test/TestOutput.h"

#include <cstring>
#include <iostream>
#include <list>
#include <algorithm>

using namespace pet;

struct FailureRecord {
	std::string testName;
	std::string sourceInfo;
	std::string failureSourceInfo;
	std::string text;

	inline FailureRecord (const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text):
		testName(testName), text(text) {
		this->sourceInfo = rindex(sourceInfo, '/') ? rindex(sourceInfo, '/') + 1: sourceInfo;
		this->failureSourceInfo = rindex(failureSourceInfo, '/') ? rindex(failureSourceInfo, '/') + 1: failureSourceInfo;
	}

	bool operator ==(const FailureRecord& other) const {
		return sourceInfo + testName + failureSourceInfo + text == other.sourceInfo + other.testName + other.failureSourceInfo + other.text;
	}
};

////
////
////

bool printActualFailureRecords = false;

int normalExpected = 53;
int syntheticExpected = 9;

std::list<FailureRecord> expectedFailures{
	FailureRecord("CheckAlways@FailureInjector", "TestFailureInjector.cpp:45", "TestFailureInjector.cpp:47", "Expectation: 'false' failed"),
	FailureRecord("CheckAlways@FailureInjector", "TestFailureInjector.cpp:45", "TestFailureInjector.cpp:47", "Expectation: 'false' failed"),
	FailureRecord("Check@FailureInjector", "TestFailureInjector.cpp:41", "TestFailureInjector.cpp:42", "Expectation: 'FailureInjector::shouldSimulateError()' failed"),
    FailureRecord("CheckWithText", "TestNoGroup.cpp:41", "TestNoGroup.cpp:42", "Expectation: '\"Message can be hacked here\", true == false' failed"),
    FailureRecord("NonEmptyMessageFailure", "TestNoGroup.cpp:37", "TestNoGroup.cpp:38", "NonEmpty"),
    FailureRecord("EmptyMessageFailure", "TestNoGroup.cpp:33", "TestNoGroup.cpp:34", ""),
    FailureRecord("CheckFalse", "TestNoGroup.cpp:29", "TestNoGroup.cpp:30", "Expectation: 'false' failed"),
    FailureRecord("CapacityOverload@Mock", "TestMock.cpp:166", "TestMock.cpp:168", "Mock - pool is full"),
    FailureRecord("DisabledIndependentUnexpected@Mock", "TestMock.cpp:160", "--- Finalization ---", "Mock - unmet expectations"),
    FailureRecord("ParamsPointerWrong@Mock", "TestMock.cpp:143", "TestMock.cpp:146", "Mock - pointer parameter error"),
    FailureRecord("ParamsDataWrongLength@Mock", "TestMock.cpp:130", "TestMock.cpp:134", "Mock - data block parameter error"),
    FailureRecord("ParamsDataWrong@Mock", "TestMock.cpp:123", "TestMock.cpp:127", "Mock - data block parameter error"),
    FailureRecord("ParamsStringWrong@Mock", "TestMock.cpp:102", "TestMock.cpp:106", "Mock - string parameter error"),
    FailureRecord("ParamsWrong@Mock", "TestMock.cpp:90", "TestMock.cpp:92", "Mock - parameter error"),
    FailureRecord("ParamsNoParamConsumed@Mock", "TestMock.cpp:80", "--- Finalization ---", "Mock - unmet expectations"),
    FailureRecord("ParamsNoParamExpected@Mock", "TestMock.cpp:75", "TestMock.cpp:77", "Mock - parameter error"),
    FailureRecord("IndependentSourcesFail@Mock", "TestMock.cpp:65", "TestMock.cpp:67", "Mock - unexpected call"),
    FailureRecord("Unexpected@Mock", "TestMock.cpp:53", "TestMock.cpp:54", "Mock - unexpected call"),
    FailureRecord("WrongOrder@Mock", "TestMock.cpp:47", "TestMock.cpp:50", "Mock - unexpected call"),
    FailureRecord("MultipleIncomplete@Mock", "TestMock.cpp:41", "--- Finalization ---", "Mock - unmet expectations"),
    FailureRecord("Incomplete@Mock", "TestMock.cpp:30", "--- Finalization ---", "Mock - unmet expectations"),
    FailureRecord("SetupNotOk@OtherGroup", "TestGroups.cpp:60", "TestGroups.cpp:56", "User message"),
    FailureRecord("BodyNotOk@Group", "TestGroups.cpp:47", "TestGroups.cpp:28", "Expectation: 'ok' failed"),
    FailureRecord("TeardownNotOk@Group", "TestGroups.cpp:44", "TestGroups.cpp:28", "Expectation: 'ok' failed"),
    FailureRecord("ParametricTest2@ParametricGroup1", "TestGenericSuite.cpp:41", "TestGenericSuite.cpp:43", "Expectation: '!this->ok' failed"),
    FailureRecord("ParametricTest3@ParametricGroup2", "TestGenericSuite.cpp:51", "TestGenericSuite.cpp:53", "Expectation: 'this->ok' failed"),
    FailureRecord("ParametricTest2@ParametricGroup1", "TestGenericSuite.cpp:41", "TestGenericSuite.cpp:43", "Expectation: '!this->ok' failed"),
    FailureRecord("ParametricTest3@ParametricGroup2", "TestGenericSuite.cpp:51", "TestGenericSuite.cpp:53", "Expectation: 'this->ok' failed"),
};

////
////
////


struct MetaTestOutput: public TestOutput {
	uint32_t progress = 0;
	uint32_t normal = 0;
	uint32_t failed = 0;
	uint32_t synthetic = 0;

	std::list<FailureRecord> failures;

	virtual void reportProgress() {
		progress++;
	}

	virtual void reportTestFailure(bool isSynthetic, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char* text) {
	    FailureRecord record(testName, sourceInfo, failureSourceInfo, text);

        if(printActualFailureRecords)
            std::cout << "FailureRecord(\"" << record.testName << "\", \"" << record.sourceInfo << "\", \"" <<  record.failureSourceInfo << "\", \"" << record.text << "\")," << std::endl;

        failures.push_back(record);
	}


	virtual void reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic) {
		this->normal = normal;
		this->failed = failure;
		this->synthetic = synthetic;
	}

	virtual inline ~MetaTestOutput() {}
};

struct MetaTestPlugin: pet::TestPlugin{
	int before = 0, after = 0;

	virtual void beforeTest() {
		before++;
	}

	virtual void afterTest() {
		after++;
	}

	inline virtual ~MetaTestPlugin() {}
};

int main(int ac, char** av)
{
	MetaTestOutput output;

	int ret = 0;

	auto dotsExpected = normalExpected + syntheticExpected;

	MetaTestPlugin plugin;
	pet::TestRunner::installPlugin(&plugin);

    if(pet::TestRunner::runAllTests(&output) != -1) {
    	std::cerr << "runAllTests return value error" << std::endl;
    	ret = -1;
    } else if(normalExpected != output.normal){
    	std::cerr << "number of tests error (expected " << normalExpected << " got " << output.normal << ")" << std::endl;
    	ret = -1;
    } else if(syntheticExpected != output.synthetic){
        std::cerr << "number of synthetic tests error (expected " << syntheticExpected << " got " << output.synthetic << ")" << std::endl;
        ret = -1;
    } else if(dotsExpected != output.progress){
    	std::cerr << "progress reporting error (expected " << dotsExpected << " got " << output.progress << ")" << std::endl;
    	ret = -1;
    } else if(expectedFailures.size() != output.failed){
    	std::cerr << "final number of failures reporting error (expected " << expectedFailures.size() << " got " << output.failed << ")" << std::endl;
    	ret = -1;
    } else if(expectedFailures.size() != output.failures.size()) {
    	std::cerr << "number of failures error (expected " << expectedFailures.size() << " got " << output.failures.size() << ")" << std::endl;
    	ret = -1;
    } else if(plugin.before != dotsExpected || plugin.after != dotsExpected){
    	std::cerr << "plugin error (expected " << dotsExpected << " got " << plugin.before << " calls before test and "<< plugin.after << " after)" << std::endl;
    	ret = -1;
    } else {
    	for(auto &x: expectedFailures) {
    		auto y = std::find_if(output.failures.begin(), output.failures.end(), [&](const FailureRecord& one){
    		    return one.testName == x.testName;
    		});

    		if(y == output.failures.end()) {
    		    std::cerr << "missing failure (" << x.testName << ")" << std::endl;
    			ret = -1;
    		} else if(!(*y == x)) {
    		    std::cerr << "mismatching ";

    		    if((*y).sourceInfo != x.sourceInfo) {
    		        std::cerr << "test location (expected " << x.sourceInfo << " got " << (*y).sourceInfo << ")";
                } else if((*y).failureSourceInfo != x.failureSourceInfo) {
                    std::cerr << "failure location (expected " << x.failureSourceInfo << " got " << (*y).failureSourceInfo << ")";
                } else if((*y).text != x.text) {
                    std::cerr << "message (expected '" << x.text << "' got '" << (*y).text << "')";
                }

    		    std::cerr << " for test " << x.testName << std::endl;
    		    ret = -1;
    		}
    	}

    	if(!ret)
    	    std::cerr << "OK (expected and got " << output.failures.size() << " failures out of " << normalExpected << " normal and "
    	    << syntheticExpected << " synthetic tests with correct reports)" << std::endl;
    }

    return ret;
}
