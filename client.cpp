#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "utils.h"

// POSTGRESQL structure
#include </usr/include/postgresql/libpq-fe.h>

PGconn *cnn = NULL;
PGresult *result = NULL;

char host[] = "localhost";
char port[] = "5432";
char dataBase[] = "redes";
char user[] = "redes";
char passwd[] = "redes";

mutex mtx;

struct sockaddr_in stSockAddr;
struct sockaddr_in stSockAddrKA;
int Res;
int ResKA;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
int SocketKA = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
int n;
char id_usuario[1];
char id_usuario_ka[1];

bool ALIVE_RW = true;
bool ALIVE_P = true;


vector<string> resultado_palabras;
string consulta(string palabra)
{
    string resultado="";
    string instruccion= "SELECT referencia FROM palabras  WHERE palabra='"+palabra+"' LIMIT "+LIMIT_QUERY;
    result = PQexec(cnn,instruccion.c_str());
    int tuplas = PQntuples(result);
    int campos = PQnfields(result);
    for (int i=0; i<tuplas; i++) {
        for (int j=0; j<campos; j++) {
            string aux=PQgetvalue(result,i,j);
            resultado += aux;
            resultado += " ";
            resultado_palabras.push_back(aux);
        }
    }
    if(resultado.size()) 
        resultado+= "\n";
    PQclear(result);
    //cout<<"resultado: "<<resultado<<endl;
    return resultado;
}
string consulta_profundidad(string palabra,int profundidad)
{
    string res="";
    int cont=0;
    unsigned int tam=resultado_palabras.size();
    res+=consulta(palabra);
    for(unsigned int i=0;i<resultado_palabras.size();i++)
    {
        if(i==tam)
        {
            tam=resultado_palabras.size();
            cont++;
        }
        if(cont==profundidad)break;
        res+=consulta(resultado_palabras[i]);
    }
    resultado_palabras.clear();
    return res;

}

vector<string> consultaSinonimo(string dato)
{
    string resultado;
        string instruccion = "Select sinonimo From sinonimos where palabra = '"+dato+"' limit 1";
        result = PQexec(cnn,instruccion.c_str());
        int tuplas = PQntuples(result);
        // int campos = PQnfields(result);
        for (int i=0; i<tuplas; i++) {
                string aux=PQgetvalue(result,i,0);
                resultado += aux;
                resultado += " ";
        }
        PQclear(result);
    return sndivide_mensaje(resultado,',');

}

vector<string> consultaSinonimosProfundidad(string palabra,int profundidad)
{
    vector<string> result,aux;

    result=consultaSinonimo(palabra);
    int cont = 1;
    int tam = result.size(); 
    
    for(int i=0;i<tam;i++)
    {
        if(i == tam)
        {
            tam = result.size();
            cont++;
        }
        aux = consultaSinonimo(result[i]);
        if(cont==profundidad)break;
        result.insert(result.end(),aux.begin(),aux.end());        
        aux.clear();
    }

    
    return result;
}


string protocolos(string temporal)
{

    int tam = 0;
    ostringstream os,osi;
    if(temporal[0] !='N' and temporal[0] !='A' and temporal[0] !='L' and temporal[0] !='Q' and temporal[0] !='P' and temporal[0] !='C' and temporal[0] !='S')
    {
        cout<<"Protocolo invalido"<<endl;
        return "ERROR";
    }
    osi << temporal[0];

    temporal=temporal.substr(2, tam-1);
    istringstream iss(temporal);
    string token;
    while(getline(iss,token,' '))
    {
        os << '#' << token;
        //tam+=token.size();
    }
    osi/*<<tam*/<<os.str();
    return osi.str();
    //cout<<"Este es el buffer: "<<osi.str()<<endl;
}
vector<string> divide_mensaje(string temporal, char separador)
{
    string token;
    vector<string> paquetes;
    istringstream iss(temporal);
    while(getline(iss, token,separador))
    {
        if(token.size()>0)
            paquetes.push_back(token);
    }
    return paquetes;
}

void keepAlive() {
    string buffer;
    char *buff;
    buff = new char[3];

    while ((n = read(SocketKA, buff, 3)) > 0) {
        string aux(buff);

        if (aux == ACK_MESSAGE) {
            if ((n = write(SocketKA, ACK_MESSAGE.c_str(), 3)) <= 0) {
                perror("Error at send ACK to server.");
                ALIVE_RW = false;
                shutdown(SocketKA, SHUT_RDWR);
                close(SocketKA);
                pthread_exit(nullptr);
            }
            continue;
        }
    }

    if (n <= 0)
    {
        cout << "Client unreachable keepalive. It will be disconnected!"<<endl;
        shutdown(SocketFD, SHUT_RDWR);
        shutdown(SocketKA, SHUT_RDWR);
        close(SocketFD);
        close(SocketKA);
        pthread_exit(nullptr);
    }
}

void writeS()
{
    cout<<"Mi id es: "<<id_usuario[0]<<endl;
    cout<<"Mi id KA es: "<<id_usuario_ka[0]<<endl;
    do
    {
        string temporal;
        cout<<"Ingrese su consulta"<<endl;
        getline(cin,temporal);

        vector<string> buffer=divide_mensaje(temporal,'-');
        string bufferJunto="";
        for(unsigned int i=0; i < buffer.size(); i++)
        {
            if(buffer[i]!="ERROR")
            {
                //cout<<"Buffer: "<<buffer[i]<<endl;
                if(i!=0) bufferJunto+="$";
                bufferJunto+=protocolos(buffer[i]);
            }
        }
        ostringstream to;

        int TamTot = bufferJunto.size()+1;
        if(TamTot<=9)
            to  << '0'<< '0'<<TamTot << '#' << bufferJunto;
        else if(TamTot<=99)
            to  << '0'<<TamTot << '#' <<bufferJunto;
        else
            to  <<TamTot << '#' <<bufferJunto;

        cout<<"Buffer Junto: "<<to.str()<<endl;
        bufferJunto=to.str();
        n = write(SocketFD, bufferJunto.c_str(),bufferJunto.length());
        bufferJunto.clear();
    } while (n > 0);

    if (n <= 0) {
        cout << "Client unreachable while writting. It will be disconnected!"<<endl;
        shutdown(SocketFD, SHUT_RDWR);
        shutdown(SocketKA, SHUT_RDWR);
        close(SocketFD);
        close(SocketKA);
        pthread_exit(nullptr);
    }

}

void readS()
{
    string buffer;
    char* buff;
    buff=new char[3];

    while((n = read(SocketFD, buff, 3)) > 0)
    {
        string aux(buff);
        string to_be_synonym;
        int tamanio = atoi(aux.c_str());

        //cout<<aux<<" = "<<tamanio<<endl;
        //cout <<"tam"<<tamanio<<endl;
        buff = new char[tamanio];
        n = read(SocketFD, buff, tamanio);
        string aux1(buff);
        //cout <<"aux1 "<< aux1<<endl;
        vector<string> separacion=divide_mensaje(aux1,'&');
        //string tabla=separacion[1];
        vector<string> protocolos=divide_mensaje(separacion[0],'$');
         //cout<<"protocolos"<<protocolos[0]<<endl;
        for(unsigned int i = 0; i < protocolos.size(); i++)
        {
            vector<string> palabras = divide_mensaje(protocolos[i],'#');
            cout<<"palabras"<<palabras[0]<<endl;
            for (unsigned int i = 0; i < palabras.size(); ++i) {
                cout << "ITEMS:" << i << palabras[i] << " - " << endl;
            }

            string comando = palabras[0];
            //cout << "Command: "<< comando << endl;
            //mtx.lock();
            if (comando == "N")
            {
                if (palabras.size() > 2) {
                    cout << "Too many arguments." << endl;
                    break;
                }
                string palabra = palabras[1];
                string referencia = " ";

                to_be_synonym = palabra;

                if (PQstatus(cnn) != CONNECTION_BAD) {
                    string query = "INSERT INTO palabras (palabra, referencia) VALUES ('"+palabra+"', '"+referencia+"');";
                    result = PQexec(cnn, query.c_str());
                    if (!result)
                    {
                        cout << "Problem at executing Query." << endl;
                    }
                }
                else
                {
                    cout<<"No se conecto a la BD"<<endl;
                }
            }
            else if(comando == "L")
            {
                if (palabras.size() > 4) {
                    cout << "Too many arguments." << endl;
                    break;
                }

                string palabra = palabras[1];
                string referencia = palabras[2];

                to_be_synonym = palabra;

                if (PQstatus(cnn) != CONNECTION_BAD) {
                    string query = "INSERT INTO palabras (palabra, referencia) VALUES ('"+palabra+"', '"+referencia+"');";
                    cout << query << endl;
                    result = PQexec(cnn, query.c_str());
                    if (!result)
                    {
                        cout << "Problem at executing Query." << endl;
                    }
                }
                else
                {
                    cout<<"No se conecto a la BD"<<endl;
                }
            } 
            else if (comando == "A") {
                string name = palabras[1];
                string value = palabras[2];

                if (name != "sinonimo")
                {
                    cout << "This method is not being recognized." << endl;
                    break;
                }

                if (PQstatus(cnn) != CONNECTION_BAD) {
                    string query =  "SELECT COUNT(palabra) FROM sinonimos  WHERE palabra='"+to_be_synonym+"'";
                    result = PQexec(cnn, query.c_str());
                    if (!result)
                    {
                        cout << "Problem at executing Query." << endl;
                    }
                    PQnfields(result);
                
                    if (PQgetvalue(result,0,0)[0] == '0')
                    {
                        string query = "INSERT INTO sinonimos (palabra) VALUES ('"+to_be_synonym+"');";
                        cout << query << endl;
                        result = PQexec(cnn, query.c_str());
                        if (!result)
                        {
                            cout << "Problem at executing Query." << endl;
                        }
                    }
                    query = "UPDATE sinonimos SET sinonimo = '{"+value+"}' WHERE palabra = '"+to_be_synonym+"';";
                    cout << query << endl;
                    result = PQexec(cnn, query.c_str());
                    if (!result)
                    {
                        cout << "Problem at executing Query." << endl;
                    }
                }
                else
                {
                    cout<<"No se conecto a la BD"<<endl;
                }
            }
            else if(comando == "Q")
            {
               // cout<<"SUPER ENTRE A ESTE SITIO "<<endl;
                string dato = palabras[1]; 
                string profundidad = palabras[2]; 
               // cout<<"La profundidad es: "<<profundidad<<endl;
                string query;
    
                if (PQstatus(cnn) != CONNECTION_BAD) {
                    if (profundidad.size())
                    {
                        string resultado;
                        resultado = "RES"+format_message_plus_size(consulta_profundidad(dato,atoi(profundidad.c_str())));
                        cout << "La Rpta al query es: " << resultado << endl;
                        write(SocketFD, resultado.c_str(), resultado.size()); 
                    }
                    else
                    {
                        cout<<"No se conecto a la BD"<<endl;
                    }
                }  
                else
                {
                    cout<<"No se conecto a la BD"<<endl;
                }                 
            }
            //este comando solo sirve responder
            else if(comando=="R"){
                cout<<"rpta: "<<palabras[1]<<endl;
            }
            else if(comando == "P"){
                cout<<"entre a hacer la consulta del gato andino peruano!!"<<endl;
                vector< vector< string > > lasPalabras;
                for(int i=1;i<palabras.size()-1;i++)
                {
                    lasPalabras.push_back( consultaSinonimosProfundidad(palabras[i],atoi(palabras[palabras.size()-1].c_str()) ) );
                }
                cout<<"las posibles combinaciones son:"<<endl;

                for(int i=0;i<lasPalabras.size();i++)
                {
                    cout<<"PARA LA PALABRA: "<<palabras[i+1]<<": ";
                    for(int j=0;j<lasPalabras[i].size();j++)
                    {
                        cout<<lasPalabras[i][j]<<"=>";
                    }
                    cout<<endl;
                }


             }
             else if(comando == "S")
             {
                   cout<<"entre a hacer la consulta del gasdesfaeawefawefweafwaefewafato!!"<<endl;
                 string resultados = getData();
                 vector <string> res = divide_mensaje(resultados,'\n');
                 cout<<"La ip es: "<<res[0]<<endl;
                 cout<<"El puerto es: "<<res[1]<<endl;
                 cout<<"el id del cliente es: "<<id_usuario[0]<<endl;

             }
            else {
                cout << "Option no valid." << endl;
            }
        }
    }

    if (n <= 0)
    {
        cout << "Client unreachable while reading. It will be disconnected!"<<endl;
        shutdown(SocketFD, SHUT_RDWR);
        shutdown(SocketKA, SHUT_RDWR);
        close(SocketFD);
        close(SocketKA);
        pthread_exit(nullptr);
    }
}


int main(int argc, char *argv[])
{
    if (!argv[1]) {
        cout << "Please enter the server IP Address." << endl;
        return 0;
    }
    cout << "Client Services Started" << endl;

    char* SERVER_IP = argv[1];
    /*  Conectando bd*/
    cnn = PQsetdbLogin(host, port, NULL, NULL, dataBase, user, passwd);

    if(-1 == SocketFD || SocketKA == -1)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    memset(&stSockAddrKA, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(APP_PORT);

    stSockAddrKA.sin_family = AF_INET;
    stSockAddrKA.sin_port = htons(KA_PORT);

    Res = inet_pton(AF_INET, SERVER_IP, &stSockAddr.sin_addr);
    ResKA = inet_pton(AF_INET, SERVER_IP, &stSockAddrKA.sin_addr);

    if (0 > Res)
    {
        perror("ERROR: First parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("Char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (ResKA < 0)
    {
        perror("ERROR: First parameter is not a valid address family");
        close(SocketKA);
        exit(EXIT_FAILURE);
    }
    else if (ResKA == 0)
    {
        perror("Char string (second parameter does not contain valid ipaddress");
        close(SocketKA);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("Connection failed!");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketKA, (const struct sockaddr *)&stSockAddrKA, sizeof(struct sockaddr_in)))
    {
        perror("Connection failed!");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    read(SocketFD, id_usuario, 1); //el cliente almacena en  id_usuario el id enviado por el servidor
    read(SocketKA, id_usuario_ka, 1);

    std::thread t1 (readS);
    std::thread t2 (writeS);
    std::thread t3 (keepAlive);
    t1.join();
    t2.join();
    t3.join();

    PQfinish(cnn);
    shutdown(SocketFD, SHUT_RDWR);
    shutdown(SocketKA, SHUT_RDWR);
    close(SocketFD);
    close(SocketKA);
    return 0;
}