CREATE TABLE palabras
(
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
COPY palabras from '/home/sergio/Redes/Trabajo Final/Data/Data/yZ.txt' USING DELIMITERS ' '
