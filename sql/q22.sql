create table Orders(OID int primary key, Order_No int, CID int);
insert into Orders values(1, 7520, 3);
insert into Orders values(2, 3212, 3);
insert into Orders values(3, 2572, 7);
insert into Orders values(4, 7375, 6);
insert into Orders values(5, 7840, 2);
insert into Orders values(6, 1054, 2);
insert into Orders values(7, 8254, 8);

select * from Orders AS o;