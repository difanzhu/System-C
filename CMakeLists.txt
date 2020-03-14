cmake_minimum_required(VERSION 3.0.0)
project(ha2 VERSION 0.1.0)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(ha2 ${SOURCES})

set(SYSTEMC_PATH $ENV{SYSTEMC_HOME})
target_include_directories(ha2 PRIVATE ${SYSTEMC_PATH}/include)
find_library(SYSTEMC_LIBRARY NAMES systemc
    PATHS ${SYSTEMC_PATH}/lib-linux64 ${SYSTEMC_PATH}/lib-linux ${SYSTEMC_PATH}/lib
)
target_link_libraries(ha2 ${SYSTEMC_LIBRARY})
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)


enable_testing()
set(TEST_CASES 
    testBlockedAuto;
    testBlockedManual;
    testBlockedDuringLocking;

    testMotorFailManualDown;
    testMotorFailManualUp;
    testMotorFailAutoUp;
    testMotorFailAutoDown;

    testManualButtonWhenLocked;
    testLockingWindows;

    testAutoManualUp;
    testAutoManualDown;
    testManualUpRelease;
    testManualUpSensor;
    testManualDownRelease;
    testManualDownSensor;
    testAutoDownStopSensor;
    testAutoUpStopSensor;
)

foreach(test_case ${TEST_CASES})
    add_test(NAME Test_${test_case} COMMAND ha2 ${test_case})
endforeach()