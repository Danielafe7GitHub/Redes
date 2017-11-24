/* Server code in C++ */
// Compilar: g++ -std=c++11 -pthread -o s.exe server.cpp
//  Ejecutar: ./s.exe
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <string.h>
#include <string>

//Cambiando a C++
#include <iostream>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <vector>
#include <sstream>
#include <cstring>
using namespace std;


struct sockaddr_in stSockAddr;                
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Se trabaja con protocolo TCP Handshaking
int optval;
socklen_t optlen = sizeof(optval);
string buffer;
int n;
int cliente;
int slaveServer;
vector<int> iD; /*Vector que recopila id de los clientes conectados*/



void keepAlive()
{
    for(int i=0;i<iD.size();i++)
    {
      if(0 > iD[i]) {
        perror("Socket Fuera de Servicio");
        close(SocketFD);
        exit(EXIT_FAILURE);
        //int a; cin>>a;
      }
      else
      {
        cout <<iD[i]<<"  Keep Alive "<<endl;
      }
    }
}

unsigned long hashTexto(unsigned char *str)
{
  
  unsigned long hash = 5381;
  int c;
  while (c = *str++)
      hash = ((hash << 5) + hash) + c;  /*hash * 33 + c;*/


  return hash%1000;
}

int seleccionarSlave(string palabra)
{
    unsigned long H = hashTexto((unsigned char *) palabra.c_str());
    if(H<100)
    {
        slaveServer = 5; 
        return 0;
    }
    if(H<200)
    {
        slaveServer = 5; 
        return 1;
    }
    if(H<300)
    {
        slaveServer = 6; 
        return 2;
    }
    if(H<400)
    {
        slaveServer = 6; 
        return 3;
    }
    if(H<500)
    {
        slaveServer = 6; 
        return 4;
    }
    if(H<600)
    {
        slaveServer = 7; 
        return 5;
    }
    if(H<700)
    {
        slaveServer = 7; 
        return 6;
    }
    if(H<800)
    {
        slaveServer = 7; 
        return 7;
    }
    if(H<900)
    {
        slaveServer = 8; 
        return 8;
    }
    if(H<1000)
    {
        slaveServer = 8; 
        return 9;
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
  do {

    //keepAlive();
    char* buff;    
    buff = new char[3]; //El cliente le envia 7, pero solo decide leer el primero (N)
    n = read(ConnectFD,buff,3);
    if (n < 0) perror("ERROR reading from socket");
    string aux(buff);
    int tamanio = atoi(aux.c_str());        
    buff = new char[tamanio];
    n = read(ConnectFD,buff,tamanio);
    string aux1(buff);
    vector<string>palabras=divide_mensaje_michi(aux1);
    string palabra = palabras[1];
    ostringstream protocolo;
    int slave = seleccionarSlave(palabra);
    protocolo<<aux.c_str()<<aux1<<'#'<<"grafo_link"<<slave;
    string protocolo1=protocolo.str();
    cout<<"Enviando a Slave, la siguiente Petición: "<<slaveServer<<" El msg:  "<<protocolo1<<endl;          
    n = write(slaveServer,protocolo1.c_str(),protocolo1.length()); //Poner al Protocolo
    if (n < 0) perror("ERROR writing to socket"); 

    
   

    buffer.clear();

 } while(buffer != "END");

    shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD); //Cierra el Socket ( Socket : puente para que 2 computadoras se comuniquen remota o localmente) CIERRA la comunicación
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
  stSockAddr.sin_port = htons(2102);      //Puerto para la conexión
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

close(SocketFD);
return 0;
}