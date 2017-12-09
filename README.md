CLUSTER
===================


Implementación en C++, cluster de servidores con consultas a una base de datos Postgresql.

----------


Instalación
-------------

Librerias, programas y procedimientos necesarios para ejecutar el proyecto.

> **Librerias:**

> - sudo apt-get install libpq5 libpq-dev

> **Programas:**

> - sudo apt-get install postgresql postgresql-contrib postgresql-server-dev-all memcached libmemcached-dev

> **Configuración Base de Datos:**

> - CREATE DATABASE redes;
> - sudo -u postgres psql
> - CREATE USER redes WITH PASSWORD 'redes';
> - ALTER ROLE redes SUPERUSER;
> - \q

Ejecución
-------------

Comandos para Ejecutar el Proyecto.

> **Compilar:**

> - make

> **Ejecutar:**

> - ./server
> - ./client

----------
