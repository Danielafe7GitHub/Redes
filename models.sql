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

UPDATE palabras SET referencia = 'cuadrupedo' WHERE palabra = 'asno';
