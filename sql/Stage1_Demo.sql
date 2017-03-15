CREATE TABLE Article(
    articleId int PRIMARY KEY,
    title varchar(20),
    author varchar(20),
    view int
);
CREATE TABLE Comment
    commentId int PRIMARY KEY,
    author varchar(20),
    content varchar(40),
    articleId int
);
CREATE TABLE Comment(
    commentId int PRIMARY KEY,
    author varchar(20),
    content varchar(40),
    articleId int
);
CREATE TABLE Links(
    linkId int PRIMARY KEY,
    url text
);
CREATE TABLE Links(
    linkId int PRIMARY KEY,
    url varchar(40)
);
INSERT INTO Article VALUES(1, 'Hello', 'Sys', 33);
INSERT INTO Article VALUES(2, 'Hello Space!', 'Astronaut', 33);
INSERT INTO Article VALUES(3, 'Cool #Hashtag', 'Sys', 33);
INSERT INTO Article VALUES(4, 'Hello', 'Sys', 0);
show;
INSERT INTO Article
VALUES(1, 'Last One', 'Sys', 33);
INSERT INTO Comment
VALUES(1, 'Sys', 'I can leave a comment!', 'Hello');
INSERT INTO Comment
VALUES(1, 'SomeoneReallyHaveALongName', 'Blahblah', 1);
InSeRt InTo Links
ValUeS(1, 'http://www.nthu.edu.tw');

