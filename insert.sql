CREATE TABLE palabras
(
    id    serial primary key,
    palabra        text,
    referencia        text
);

INSERT INTO palabras (palabra, referencia) VALUES ('gato', 'malo');
INSERT INTO palabras (palabra, referencia) VALUES ('gato', 'atun');
INSERT INTO palabras (palabra, referencia) VALUES ('gato', 'ave');

INSERT INTO palabras (palabra, referencia) VALUES ('malo', 'malo1');
INSERT INTO palabras (palabra, referencia) VALUES ('malo', 'malo2');
INSERT INTO palabras (palabra, referencia) VALUES ('malo', 'malo3');


INSERT INTO palabras (palabra, referencia) VALUES ('atun', 'atun1');
INSERT INTO palabras (palabra, referencia) VALUES ('atun', 'atun2');
INSERT INTO palabras (palabra, referencia) VALUES ('atun', 'atun3');


INSERT INTO palabras (palabra, referencia) VALUES ('ave', 'ave1');
INSERT INTO palabras (palabra, referencia) VALUES ('ave', 'ave2');
INSERT INTO palabras (palabra, referencia) VALUES ('ave', 'ave3');
select * from palabras WHERE palabra = 'atun'; 


CREATE FUNCTION sum(int[]) RETURNS int8 AS $$
DECLARE
  s int8 := 0;
  x int;
BEGIN
  FOREACH x IN ARRAY $1
  LOOP
    s := s + x;
  END LOOP;
  RETURN s;
END;
$$ LANGUAGE plpgsql;
select sum(ARRAY[1,2,3]) 

CREATE FUNCTION fun(text[]) RETURNS text[] AS $$
DECLARE
  s int8 := 0;
  x int;
BEGIN
  FOREACH x IN ARRAY $1
  LOOP
    s := s + x;
  END LOOP;
  RETURN s;
END;
$$ LANGUAGE plpgsql;



SELECT COUNT(palabra)  FROM palabras WHERE palabra = 'gato'; 

SELECT referencia FROM palabras WHERE palabra = 'gato' LIMIT 2;

CREATE TABLE sinonimos (
    palabra            text,
    sinonimo        text[]
);
INSERT INTO sinonimos (palabra, sinonimo) VALUES 
('Gato','{"Felino","Garabato", "Michi", "Felix"}'),
('Perro','{"Can","Canino", "Cachorro", "Sabueso"}'); 

select * from sinonimos