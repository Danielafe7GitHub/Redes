#include <iostream>
#include <stdio.h>  
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <vector>
#include <cstring>
#include <mutex>

// ENVIRONMENT CONFIGURATIONS
#define APP_PORT 3112
#define KA_PORT 4521

using namespace std;

// GLOBAL VARIABLES
string ACK_MESSAGE = "ACK";
string LIMIT_QUERY = "10";

string vectorToString(vector<string> a)
{
    string b;
    for(int i=0;i<a.size();i++)
    {
        b+=a[i]+"=";
    }
    return b;
}

string format_message_plus_size(string message) {
    ostringstream to;
    string result;

    int TamTot = message.size() + 1;

    if (TamTot <= 9)
        to  << '0'<< '0'<<TamTot << message;
    else if(TamTot<=99)
        to  << '0'<<TamTot << message;
    else
        to  <<TamTot << message;
    result = to.str() + message;
    return result;
}



string deleteInperfections(string inperfect)
{
    string result;
    for(unsigned int i=0;i<inperfect.size();i++)
    {
        if(inperfect[i]!='\"' && inperfect[i]!='{' && inperfect[i]!='}')result+=inperfect[i];
    }
    return result;
}


///quitando inperfecciones en los strings
vector<string> sndivide_mensaje(string temporal, char separador)
{
    string token;
    vector<string> paquetes;
    istringstream iss(temporal);
    while(getline(iss, token,separador))
    {
        if(token.size()>0)
            paquetes.push_back(deleteInperfections(token));
    }
    return paquetes;
}

void printVector(vector<string> a)
{
    for(unsigned int i=0;i<a.size();i++)
    {
        cout<<a[i]<<", ";
    }
    cout<<endl;
}

string GetStdoutFromCommand(string cmd) {

	string data;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
				pclose(stream);
			}

	return data;
}


string getData()
{
    string res;
    res=GetStdoutFromCommand("hostname -I | awk '{print $1}'");
    res+=to_string(APP_PORT);
    res += "\n";
    return res;
}
