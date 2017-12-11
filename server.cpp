#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>

#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>

#include "utils.h"

struct sockaddr_in stSockAddr;
struct sockaddr_in stSockAddrKA;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Se trabaja con protocolo TCP Handshaking
int SocketKA = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
int optval;
socklen_t optlen = sizeof(optval);
string buffer;

int n;
int cliente;
int slaveServer; /*Te dice a que servidor Esclavo ir*/
string tabla; /*Identificado el esclavo, elegimos una de sus tablas*/
vector<int> clients_id; /*Vector que recopila id de los clientes conectados*/

mutex mtx;

string arToStr(char* a,int size)
{
    string res;
    for(int i=0;i<size;i++)res+=a[i];
    return res;
}

void verify_alive(int& ka, int ConnectKA, int ConnectFD) {
    usleep(7000000);
    if (ka == 1) {
        cout << "Verification goes true." << endl;
        pthread_exit(nullptr);
    } else {
        cout << "Client " << ConnectKA  << " didnt answer, is dead." << endl;
        clients_id.erase(remove(clients_id.begin(), clients_id.end(), ConnectFD), clients_id.end());
        shutdown(ConnectKA, SHUT_RDWR);
        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectKA);
        close(ConnectFD);
        pthread_exit(nullptr);
    }
}

void keepAlive(int ConnectKA, int ConnectFD)
{
    // Engaña al interprete para que no de warnings por bucle infinito
    bool fake_bool = true;
    int ka;
    cout << "KeepAlive - Starting service ...\n";
    do {
        char* buff;
        buff = new char[3];
        usleep(10000000);
        ka = 0; // important do not move this

        cout << "Verify connection with [" << ConnectKA << "] ..." << endl;

        thread (verify_alive, ref(ka), ConnectKA, ConnectFD).detach();

        if ((ka = write(ConnectKA, ACK_MESSAGE.c_str(), 3)) <= 0) {
            perror("ERROR sending AKC to client.");
            clients_id.erase(remove(clients_id.begin(), clients_id.end(), ConnectFD), clients_id.end());
            shutdown(ConnectKA, SHUT_RDWR);
            shutdown(ConnectFD, SHUT_RDWR);
            close(ConnectKA);
            close(ConnectFD);
            pthread_exit(nullptr);
        }

        if ((ka = read(ConnectKA, buff, 3)) <= 0) {
            perror("ERROR getting AKC from client.");
            clients_id.erase(remove(clients_id.begin(), clients_id.end(), ConnectFD), clients_id.end());
            shutdown(ConnectKA, SHUT_RDWR);
            shutdown(ConnectFD, SHUT_RDWR);
            close(ConnectKA);
            close(ConnectFD);
            pthread_exit(nullptr);
        } else {
            cout << "ACK received from [" << ConnectKA << "] ..." << endl;
        }
        ka = 1;
    } while (ka);

    if (ka == 0) {
        clients_id.erase(remove(clients_id.begin(), clients_id.end(), ConnectFD), clients_id.end());
        cout << "ERROR reading from socket KeepAlive." << endl;
        shutdown(ConnectKA, SHUT_RDWR);
        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectKA);
        close(ConnectFD);
        pthread_exit(nullptr);
    }
}


//Daniel = 5 y  13  Sergio = 6 y 12 Karelia = 7 y 11 y 13 Daniela = 8 y 12 
int seleccionarSlave(string palabra)
{
    tabla = palabra[0];
    if (palabra[0] >= 'a' and palabra[0]<='c')
    {
        return 5;
    }
    else if (palabra[0] >= 'd' and palabra[0]<='f')
    {
        return 6;
    }
    else if (palabra[0] >= 'g' and palabra[0]<='i')
    {
        return 7;
    }
    else if (palabra[0] >= 'j' and palabra[0]<='l')
    {
        return 8;
    }
    else if (palabra[0] >= 'm' and palabra[0]<='o')
    {
        return 9;
    }
    else if (palabra[0] >= 'p' and palabra[0]<='r')
    {
        return 10;
    }
    else if (palabra[0] >= 's' and palabra[0]<='u')
    {
        return 11;
    }
    else if (palabra[0] >= 'v' and palabra[0]<='x')
    {
        return 12;
    }
    else if (palabra[0] >= 'y' and palabra[0]<='z')
    {
        return 13;
    }

}
vector<string> divide_mensaje_michi(string temporal)
{
    string token;
    vector<string> paquetes;
    istringstream iss(temporal);
    while(getline(iss,token,'#'))
    {
        if(token.size()>0)
            paquetes.push_back(token);
    }
    return paquetes;
}

void aceptClient(int ConnectFD) {
    char* buff;
    buff = new char[3]; //El cliente le envia 7, pero solo decide leer el primero (N)

    while(n = read(ConnectFD, buff, 3) > 0 && find(clients_id.begin(), clients_id.end(), ConnectFD) != clients_id.end()) {
        string aux = arToStr(buff, 3);
        int tamanio = atoi(aux.c_str());
        buff = new char[tamanio];
        n = read(ConnectFD, buff, tamanio);

        if (n < 0)
            perror("ERROR reading from socket");

        // string aux1(buff);
        string aux1="";
        aux1=arToStr(buff,tamanio);
        vector<string>palabras = divide_mensaje_michi(aux1);
        string palabra = palabras[1];
        ostringstream protocolo0;
        ostringstream protocolo;
        slaveServer = seleccionarSlave(palabra);

        protocolo0.clear();
        protocolo0<<aux1<<'&'<<"Tabla_"<<tabla;
        tamanio = protocolo0.str().size();
        ostringstream to;

        if(tamanio<=9)
            to  << '0'<< '0'<<tamanio;
        else if(tamanio<=99)
            to  << '0'<<tamanio;
        else
            to  <<tamanio;
        protocolo<<to.str()<<protocolo0.str();
        string protocolo1=protocolo.str();
        cout<<"Enviando a Slave Numero "<<slaveServer<<" a la Tabla "<<tabla<<" El msg:  "<<protocolo1<<endl;
        for(int i=1;i<clients_id.size();i++)
        {
            cout<<"Enviando a Esclavo: "<<clients_id[i]<<" El msg:  "<<buffer<<endl;
            n = write(slaveServer, protocolo1.c_str(), protocolo1.size());
            if (n < 0) perror("ERROR writing to socket");
        }

        buffer.clear();
    }

    if (n <= 0)		//recv timedout implies client no longer alive
    {
        cout << "Client unreachable. It will be disconnected!"<<endl;
        clients_id.erase(remove(clients_id.begin(), clients_id.end(), ConnectFD), clients_id.end());
        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectFD); //Cierra el Socket ( Socket : puente para que 2 computadoras se comuniquen remota o localmente) CIERRA la comunicación
        pthread_exit(NULL);
    }
}


int main()
{
    /*
    This function creates a socket and specifies communication style style,
    which should be one of the socket styles listed in Communication Styles. (TCP en este caso)
    The namespace argument specifies the namespace; it must be PF_LOCAL (see Local Namespace) or PF_INET (see Internet Namespace).

    */

    cout << "Starting Server" << endl;

    if(-1 == SocketFD || SocketKA == -1)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in)); //Separa espcio en memoria para la Estructura
    memset(&stSockAddrKA, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;      //Protocolo para la conexión
    stSockAddr.sin_port = htons(APP_PORT);      //Puerto para la conexión
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    stSockAddrKA.sin_family = AF_INET;
    stSockAddrKA.sin_port = htons(KA_PORT);
    stSockAddrKA.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == bind(SocketKA,(const struct sockaddr *)&stSockAddrKA, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketKA);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(SocketFD, 10))
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(SocketKA, 10))
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    //Hace que el Servidor siempre escuche


    cout << "Waiting for connections ..." << endl;

    while(true)
    {
        int ConnectFD = accept(SocketFD, NULL, NULL);
        int KAConnectFD = accept(SocketKA, NULL, NULL);

        if(0 > ConnectFD) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        } else {
            cout << "Accepted Connection! - Socket:" << ConnectFD << endl;
        }

        if(KAConnectFD < 0) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        } else {
            cout << "Accepted Connection! - Socket:" << KAConnectFD << endl;
        }

        if (clients_id.size() == 0)
            cliente = ConnectFD;
        clients_id.push_back(ConnectFD);

        char id[1];
        sprintf(id, "%d", ConnectFD); //De entero a char
        write(ConnectFD, id, 1);      //Luego que el cliente se conecta, el servidor halla su id, y se lo envía.

        char id_ka[1];
        sprintf(id_ka, "%d", KAConnectFD);
        write(KAConnectFD, id_ka, 1);

        thread (aceptClient, ConnectFD).detach();
        thread (keepAlive, KAConnectFD, ConnectFD).detach();
    }
}