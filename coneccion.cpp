using namespace std;
#include <iostream>
#include </usr/include/postgresql/libpq-fe.h>
///COmpilar asi: g++ tuyo.cpp -o connPgSQL -Wall -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq
PGconn *cnn = NULL;
PGresult *result = NULL;

char *host = "localhost";
char *port = "5432";
char *dataBase = "redes";
char *user = "redes";
char *passwd = "redes";

int main(int argc, char * argv[])
{
    int i;
    string dato;
    cin>>dato;
    cnn = PQsetdbLogin(host,port,NULL,NULL,dataBase,user,passwd);

    if (PQstatus(cnn) != CONNECTION_BAD) {
        cout << "Estamos conectados a PostgreSQL!" << endl;
        string instruccion= "SELECT * FROM palabras  WHERE palabra1='"+dato+"'";
        result = PQexec(cnn,instruccion.c_str());

        if (result != NULL) {
            int tuplas = PQntuples(result);
            int campos = PQnfields(result);
            cout << "No. Filas:" << tuplas << endl;
            cout << "No. Campos:" << campos << endl;

            cout << "Los nombres de los campos son:" << endl;

            for (i=0; i<campos; i++) {
                cout << PQfname(result,i) << " | ";
            }

            cout << endl << "Contenido de la tabla" << endl;

            for (i=0; i<tuplas; i++) {
                for (int j=0; j<campos; j++) {
                    cout << PQgetvalue(result,i,j) << " | ";
                }
                cout << endl;
            }
        }

        // Ahora nos toca liberar la memoria
        PQclear(result);

    } else {
        cout << "Error de conexion" << endl;
        return 0;
    }

    PQfinish(cnn);

    return 0;
}

