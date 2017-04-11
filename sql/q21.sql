CREATE TABLE Author(id INT PRIMARY KEY, name varchar(50), nation varchar(50));

INSERT INTO Author
VALUES (1, 'Jim Chen', 'Taiwan');
INSERT INTO Author
VALUES (2, 'John   Goodman', 'Zaire');
INSERT INTO Author
VALUES (3, 'Michael Crichton', 'USA');
INSERT INTO Author
VALUES (4, 'Shakespeare', 'England');
INSERT INTO Author
VALUES (5, 'Tim Chang', 'Taiwan');
INSERT INTO Author
VALUES (6, 'George Lucas', 'USA');
INSERT INTO Author
VALUES (7, 'Garcia Marquez', 'Colombia');
INSERT INTO Author
VALUES (8, 'Katsu  Moto', 'Japan');
INSERT INTO Author
VALUES (9, 'Confucius', 'China');
INSERT INTO Author
VALUES (10, 'Jesus', 'Nazareth');

CREATE TABLE Book(bookId int PRIMARY KEY, title varchar(100), pages int, authorId int, editorial varchar(100));
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (3, 'utdggnqupbamlqna', 1, 990, 'jcpm');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (4, 'lglmpm yax vgytul', 62, 133, 'wjqsq');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (5, 'oj', 747, 517, 'xxyxwrnclpb');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (6, 'buhdlvhgmr', 184, 759, 'tottqf x bvehespaz bd');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (7, 'reaiox', 810, 403, 'xg');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (8, 'ivrbbkul', 103, 149, 'ysnrjmkiwdfqgoxeo');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (9, 'xk', 867, 572, 'vu lwsyagzevwdtdo xce');
INSERT INTO Book (bookId, title, pages, authorId, editorial) 
VALUES (0, 'b', 472, 35, 'fclnhbmjmzeqrxlwytxvjozam');

select * from Book where bookId > 5 or pages < 500;
select * from Book where bookId > 5 and pages < 500;

select * from Book, Author where Author.id > 5;
