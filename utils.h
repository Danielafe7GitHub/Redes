#include <iostream>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <vector>
#include <cstring>
#include <mutex>

// ENVIRONMENT CONFIGURATIONS
#define APP_PORT 3562
#define KA_PORT 4056

using namespace std;

// GLOBAL VARIABLES
string ACK_MESSAGE = "ACK";
string LIMIT_QUERY = "10";


string format_message_plus_size(string message) {
    ostringstream to;
    string result;

    int TamTot = message.size() + 1;

    if (TamTot <= 9)
        to  << '0'<< '0'<<TamTot << '#' << message;
    else if(TamTot<=99)
        to  << '0'<<TamTot << '#' << message;
    else
        to  <<TamTot << '#' << message;
    result = to.str() + message;
    return result;
}