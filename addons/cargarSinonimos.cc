#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include </usr/include/postgresql/libpq-fe.h>

using namespace std;

//COMPILAR: g++ cargarSinonimos.cc -o e -Wall -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq

#define RUTAARCHIVO "/home/sergio/Redes/Trabajo Final/Data/cleanSinonimo.txt"

PGconn *cnn = NULL;
PGresult *result = NULL;

string host = "localhost";
string port = "5432";
string dataBase = "redes";
string user = "redes";
string passwd = "redes";

int find(string principal,char letra)
{
    for(unsigned int i=0;i<principal.size();i++)
    {
        if(principal[i]==letra)
        {
            return i;
        }
    }
    return -1;
}

bool findInVector(vector<string>vector,string palabra)
{
    for(unsigned int i=0;i<vector.size();i++)
    {
        if(vector[i]==palabra)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    cnn = PQsetdbLogin(host.c_str(),port.c_str(),NULL,NULL,dataBase.c_str(),user.c_str(),passwd.c_str());
    vector<string>lPalabras;
    vector<string>lSinonimos;
    ifstream toRead;
    toRead.open(RUTAARCHIVO);
    if(!toRead.is_open())
    {
        cout<<"no se pudo abrir el fichero.."<<endl;
        return 0;
    }
    string linea, palabra, sinonimos;
    sinonimos="{\"";
    bool flag=0;
    while( !toRead.eof() )
    {
        getline(toRead,linea);
        if(linea.size()!=0)
        {
            int pos = find(linea,',');
            palabra=linea.substr(0,pos);
            if(findInVector(lPalabras,palabra))
            {
                flag=1;
                sinonimos += linea.substr(pos+1,linea.size()-1) + "\",\"";       
            }else{
                if(flag)
                {
                    lSinonimos.push_back(sinonimos.substr(0,sinonimos.size()-2)+"}");
                    sinonimos="{\"";
                    flag=0;
                    if (!flag && PQstatus(cnn) != CONNECTION_BAD) 
                    {
                        string instruccion;
                        instruccion = "INSERT INTO sinonimos (palabra, sinonimos) VALUES('" + palabra + "','" + lSinonimos[0] + "')";
                        result = PQexec(cnn,instruccion.c_str());
                        lSinonimos.clear();
                        lPalabras.clear();

                        // Ahora nos toca liberar la memoria
                        PQclear(result);
                    }else{
                        cout<<"Error de Coneccion"<<endl;
                    }
                }
                sinonimos += linea.substr(pos+1,linea.size()-1) + "\",\"";          
                lPalabras.push_back(palabra);
            }
        }
    }
    lSinonimos.push_back(sinonimos.substr(0,sinonimos.size()-2)+"}");
    toRead.close();


   if (PQstatus(cnn) != CONNECTION_BAD) 
    {
        string instruccion;
        instruccion = "INSERT INTO sinonimos (palabra, sinonimos) VALUES('" + palabra + "','" + lSinonimos[0] + "')";
        result = PQexec(cnn,instruccion.c_str());

        // Ahora nos toca liberar la memoria
        PQclear(result);
    }else{
        cout<<"Error de Coneccion"<<endl;
    }

    return 0;
}