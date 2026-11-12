// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"

/**
 * BeatShot-local automation-test assertion macros.
 *
 * The engine's UTEST_* macros stop the test on the first failing assertion (each one does
 * `if (!TestX(...)) return false;`), so when several assertions describe independent
 * results, only the first failure is ever reported. That hides the fuller picture from a
 * single red test.
 *
 * The soft BS_TEST_* macros below instead run every assertion: each TestX() call records its
 * own error when it fails (via AddError) and we discard the returned bool so execution
 * continues. The test still ends up Failed, because the framework's final verdict is
 *   bTestSuccessful = bTestSuccessful && !HasAnyErrors() && ...
 * (AutomationTest.cpp, InternalStopTest) and HasAnyErrors() == GetErrorTotal() > 0 --
 * i.e. any recorded error fails the test regardless of what RunTest returns. So the test
 * passes only when all assertions pass, and every failing assertion is reported.
 *
 * Use the soft macros for independent assertions. For a genuine precondition that the next
 * line depends on (e.g. a pointer about to be dereferenced), use BS_TEST_REQUIRE so the test
 * bails out instead of continuing into an invalid state.
 */

// Assert Actual equals Expected, record an error on mismatch, and continue.
#define BS_TEST_EQUAL(What, Actual, Expected) static_cast<void>(TestEqual(What, Actual, Expected))

// Assert Value is true, record an error if false, and continue.
#define BS_TEST_TRUE(What, Value) static_cast<void>(TestTrue(What, Value))

// Assert Value is false, record an error if true, and continue.
#define BS_TEST_FALSE(What, Value) static_cast<void>(TestFalse(What, Value))

// Assert Pointer is non-null, record an error if null, and continue.
#define BS_TEST_NOT_NULL(What, Pointer) static_cast<void>(TestNotNull(What, Pointer))

// Hard precondition: Condition must be true to continue; record an error and bail out on failure.
// Use only where subsequent lines rely on the condition holding (e.g. the pointer is dereferenced next).
#define BS_TEST_REQUIRE(What, Condition) \
	if (!TestTrue(What, (Condition))) \
	{ \
		return false; \
	} \
	CA_ASSUME(Condition)
