CREATE TABLE palabras
(
    id    serial primary key,
    palabra        text,
    referencia        text
);

INSERT INTO palabras (palabra, referencia) VALUES ('gato', 'malo');

CREATE TABLE sinonimos
(
    id    serial primary key,
    palabra            text,
    sinonimos        text[]
);

INSERT INTO sinonimos (palabra, sinonimo) VALUES 
('Gato','{"Felino","Garabato", "Michi", "Felix"}'),
('Perro','{"Can","Canino", "Cachorro", "Sabueso"}'); 


/*Para ingresar un txt como una tabla*/
COPY tu_tabla from 'ruta_completa_del_archivo' USING DELIMITERS ' '
