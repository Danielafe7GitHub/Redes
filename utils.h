#include <iostream>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <vector>
#include <cstring>
#include <mutex>

// ENVIRONMENT CONFIGURATIONS
#define APP_PORT 3122

using namespace std;

// GLOBAL VARIABLES
string ACK_MESSAGE = "ACK";
