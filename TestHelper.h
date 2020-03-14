#ifndef TESTHELPER_H_
#define TESTHELPER_H_

#include "systemc.h"

#define SEQ_WAIT 100 // Time to wait between subsequent raise statements

void print(string s)
{
    cout << "[" << sc_time_stamp() << " / " << sc_delta_count()
         << "](TestBench): " << s << endl;
}

// Helper functions
template <typename Type1>
void send_signal(sc_out<Type1> *port, string port_name, Type1 value)
{
    print("Setting '" + port_name + "'='" + std::to_string(value) + "'");
    port->write(value);
    wait(SEQ_WAIT, SC_MS);
}

template <typename Type2>
void assert_now(sc_in<Type2> *port, string port_name, Type2 expected, string message)
{
    // Check if signal value matches currently
    if (port->read() != expected)
    {
        print("Assertion failed: '" + port_name + "' is not '" + std::to_string(expected) + "'");
        exit(1);
    }
}

void toggle(sc_out<bool> *port, string port_name)
{
    // Set port to true, wait a cycle + set to false
    print("Setting '" + port_name + "'='" + std::to_string(true) + "'");
    port->write(true);
    wait(SEQ_WAIT, SC_MS);

    print("Setting '" + port_name + "'='" + std::to_string(false) + "'");
    port->write(false);
    wait(SEQ_WAIT, SC_MS);
}

#endif