 /* Client code in C */
 
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>

 //Cambiando a C++
 #include <iostream>
 #include <thread>

 #include <sstream>
 #include <cstring>
 #include <vector>
 using namespace std;
//COMPILAR: g++ client.cpp -std=c++11  -pthread  -o c -Wall -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq

/*Conectando  bd*/
 #include </usr/include/postgresql/libpq-fe.h>

PGconn *cnn = NULL;
PGresult *result = NULL;

char *host = "localhost";
char *port = "5432";
char *dataBase = "REDES";
char *user = "postgres";
char *passwd = "123456";


 struct sockaddr_in stSockAddr;
 int Res;
 int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 int n; 
 char id_usuario[1];

string protocolos(string temporal)
{
    
    int tam=0;
    ostringstream os,osi,to;
    if(temporal[0] !='N' and temporal[0] !='A' and temporal[0] !='L' and temporal[0] !='Q' and temporal[0] !='P' and temporal[0] !='C' and temporal[0] !='S')
    {
      cout<<"Protocolo invalido"<<endl;
      return "ERROR";
    } 
    osi << temporal[0];
 
   
    temporal=temporal.substr(2,tam-1);
    istringstream iss(temporal);
    string token;
    while(getline(iss,token,' '))
    {
      os << '#' << token;
      //tam+=token.size();
    }
    osi/*<<tam*/<<os.str();
    int TamTot = osi.str().size()+1;
    if(TamTot<=9)
      to  << '0'<< '0'<<TamTot << '#' << osi.str();
    else if(TamTot<=99)
      to  << '0'<<TamTot << '#' << osi.str();
    else
      to  <<TamTot << '#' << osi.str();

    return to.str();
    //cout<<"Este es el buffer: "<<osi.str()<<endl;
}
  vector<string> divide_mensaje(string temporal)
 {
    string token;
    vector<string> paquetes;
    istringstream iss(temporal);
    while(getline(iss,token,'-'))
    {
        if(token.size()>0)
          paquetes.push_back(protocolos(token));
    }
   /* cout<<"Here we go"<<endl;
    for(int i=0;i<palabras.size();i++)
      cout<<i<<" "<<palabras[i]<<endl;*/
    return paquetes;
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
  

 void writeS()
 {   
     cout<<"Mi id es: "<<id_usuario[0]<<endl;
     while(true)
     {
       string temporal;
       cout<<"Ingrese su consulta"<<endl;
       getline(cin,temporal);
      
      vector<string> buffer=divide_mensaje(temporal);
      for(int i=0; i< buffer.size();i++)
      {
          if(buffer[i]!="ERROR")
          {
            cout<<"Buffer: "<<buffer[i]<<endl;
            n = write(SocketFD, buffer[i].c_str(),buffer[i].length());
          }
      }
       
     }

 }
/*
 void writeS()
 {   
     cout<<"Mi id es: "<<id_usuario[0]<<endl;
     while(true)
     {
       string buffer;
       string letra;
       cin>>letra;

       buffer="";
       buffer[0] = id_usuario[0];
       buffer[0] = letra[0];
       n = write(SocketFD, buffer.c_str(),1);
       
     }

 }*/
 void readS()
 {  
    while(true)
    {
      string buffer;
      char* buff;
      buff=new char[3];
      n = read(SocketFD,buff,3);

      string aux(buff);
      int tamanio = atoi(aux.c_str());  
      cout <<"tam"<<tamanio<<endl;      
      buff = new char[tamanio];
      n = read(SocketFD,buff,tamanio);
      string aux1(buff);
      cout <<"aux1 "<< aux1<<endl;
      vector<string>palabras=divide_mensaje_michi(aux1);
      string comando = palabras[0];
      cout<<"comando "<<comando<<endl;
      if(comando == "N")
      {
          string palabra = palabras[1];
          string tabla = palabras[2];
          if (PQstatus(cnn) != CONNECTION_BAD) {
            string instruccion= "INSERT INTO'"+tabla+"' (palabra1) VALUES ('"+palabra+"')";
            result = PQexec(cnn,instruccion.c_str());
          }
           else
          {
            cout<<"No se conecto a la BD"<<endl;
          }

      }
     // cout<<"Se ha finalizado la insercion "<<endl;




    }
      
 
 }


 int main(void)
 {
  /*  Conectando bd*/
  cnn = PQsetdbLogin(host,port,NULL,NULL,dataBase,user,passwd);


   if (-1 == SocketFD)
   {
     perror("cannot create socket");
     exit(EXIT_FAILURE);
   }

   memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

   stSockAddr.sin_family = AF_INET;
   stSockAddr.sin_port = htons(2102);
   Res = inet_pton(AF_INET,  "127.0.1.1", &stSockAddr.sin_addr);

   if (0 > Res)
   {
     perror("error: first parameter is not a valid address family");
     close(SocketFD);
     exit(EXIT_FAILURE);
   }
   else if (0 == Res)
   {
     perror("char string (second parameter does not contain valid ipaddress");
     close(SocketFD);
     exit(EXIT_FAILURE);
   }

   if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
   {
     perror("connect failed");
     close(SocketFD);
     exit(EXIT_FAILURE);
   }
  
    read(SocketFD,id_usuario,1); //el cliente almacena en  id_usuario el id enviado por el servidor
     

   std::thread t1 (readS); 
   std::thread t2 (writeS); 
   t1.join();
   t2.join();
    

   shutdown(SocketFD, SHUT_RDWR); 
   close(SocketFD);
   return 0;
 }