CREATE TABLE palabras
(
    id    serial primary key,
    palabra        text,
    referencia        text
);

INSERT INTO palabras (palabra, referencia) VALUES ('gato', 'malo');