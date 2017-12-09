#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>

#include <cstdlib>
#include <cstring>
#include <chrono>
#include <string>
#include <sstream>

#include "utils.h"



struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Se trabaja con protocolo TCP Handshaking
int optval;
socklen_t optlen = sizeof(optval);
string buffer;
int n;
int cliente;
int slaveServer; /*Te dice a que servidor Esclavo ir*/
string tabla; /*Identificado el esclavo, elegimos una de sus tablas*/
vector<int> iD; /*Vector que recopila id de los clientes conectados*/


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

    while(n = read(ConnectFD, buff, 3) > 0) {
        string aux(buff);
        int tamanio = atoi(aux.c_str());
        buff = new char[tamanio];
        n = read(ConnectFD, buff, tamanio);

        if (n < 0)
            perror("ERROR reading from socket");

        string aux1(buff);
        vector<string>palabras = divide_mensaje_michi(aux1);
        string palabra = palabras[1];
        ostringstream protocolo0;
        ostringstream protocolo;
        slaveServer = seleccionarSlave(palabra);


        protocolo0<<aux1<<'#'<<"Tabla_"<<tabla;
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
        for(int i=1;i<iD.size();i++)
        {
            cout<<"Enviando a Esclavo: "<<iD[i]<<" El msg:  "<<buffer<<endl;
            n = write(slaveServer,protocolo1.c_str(),protocolo1.size());
            if (n < 0) perror("ERROR writing to socket");
        }

        buffer.clear();
    }

    if(n <= 0)		//recv timedout implies client no longer alive
    {
        cout << "Client unreachable. It will be disconnected!"<<endl;
        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectFD); //Cierra el Socket ( Socket : puente para que 2 computadoras se comuniquen remota o localmente) CIERRA la comunicación
        pthread_exit(NULL);
    }
}


int main(void)
{
    /*
    This function creates a socket and specifies communication style style,
    which should be one of the socket styles listed in Communication Styles. (TCP en este caso)
    The namespace argument specifies the namespace; it must be PF_LOCAL (see Local Namespace) or PF_INET (see Internet Namespace).

    */

    if(-1 == SocketFD)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in)); //Separa espcio en memoria para la Estructura

    stSockAddr.sin_family = AF_INET;      //Protocolo para la conexión
    stSockAddr.sin_port = htons(APP_PORT);      //Puerto para la conexión
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(SocketFD, 10))
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

//Hace que el Servidor siempre escuche

    while(1)
    {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        if(0 > ConnectFD) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        if(iD.size()==0)
            cliente =ConnectFD;
        iD.push_back(ConnectFD);

        char id[1];
        sprintf(id,"%d",ConnectFD); //De entero a char
        write(ConnectFD,id,1);      //Luego que el cliente se conecta, el servidor halla su id, y se lo envía.

        //std::thread (keepAlive).detach();
        std::thread (aceptClient, ConnectFD).detach();
    }
}