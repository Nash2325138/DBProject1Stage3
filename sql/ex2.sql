create table Customers(CID int primary key, Name varchar(40),
Gender varchar(5), Age int, Salary int);
create table Orders(OID int primary key, Order_No int, CID int);
create table OrderInfo(Order_No int, Price int);
insert into Customers values(1, 'Jimmy', 'F', 45, 600000);
insert into Customers values(2, 'Pan', 'M', 30, 40000);
insert into Customers values(3, 'Cindy', 'F', 18, 35000);
insert into Customers values(4, 'Joe', 'M', 26, 80000);
insert into Customers values(5, 'Peter', 'M', 26, 45000);
insert into Customers values(6, 'Yuki', 'F', 22, 30000);
insert into Customers values(7, 'Joe', 'M', 50, 42000);
insert into Customers values(8, 'Adam', 'M', 30, 37000);
insert into Customers values(9, 'Lopi', 'F', 28, 60000);
insert into Orders values(1, 7520, 3);
insert into Orders values(2, 3212, 3);
insert into Orders values(3, 2572, 7);
insert into Orders values(4, 7375, 6);
insert into Orders values(5, 7840, 2);
insert into Orders values(6, 1054, 2);
insert into Orders values(7, 8254, 8);
insert into OrderInfo values(1054, 5000);
insert into OrderInfo values(1257, 1000);
insert into OrderInfo values(2572, 500);
insert into OrderInfo values(3212, 4000);
insert into OrderInfo values(7375, 7000);
insert into OrderInfo values(7520, 3200);
insert into OrderInfo values(7840, 2500);
insert into OrderInfo values(8254, 20000);


select CID, Name, Salary, o.Order_No from Customers, Orders AS o;
select c.CID, c.Name, c.Salary from Customers AS c;
select * from Orders AS o;
select o.* from Orders AS o;
select c.*, o.* from Customers AS c, Orders AS o;
select c.CID, c.Name, c.Salary, o.Order_No from Customers AS c, Orders AS o;
select c.CID, c.Name, c.Salary, o.Order_No from Customers AS c, Orders AS o WHERE c.CID > 3 AND c.Salary > 40000;
select c.CID, c.Name, c.Salary, o.Order_No from Customers AS c, Orders AS o WHERE c.CID > 3 OR c.Salary > 40000;
select c.CID, c.Name, c.Salary, o.Order_No from Customers AS c, Orders AS o WHERE c.* > 3 AND c.Salary > 40000;
select Order_No from OrderInfo AS c, Orders AS o;
select c.Order_No, o.Order_No from OrderInfo AS c, Orders AS o;
select c.CID, c.Name, c.Salary, o.Order_No from Customers AS c, Orders AS o WHERE c.CID > 3 AND c.Salary > 40000;
select CID, Name, Salary, o.Order_No from Customersss, Ordessrs AS o;
select o.CID, Name, COUNT(Salary), o.Order_No from Customers, Orders AS o;
select COUNT(Salary) from Customers, Orders AS o;
select COUNT(Salary), SUM(Salary) from Customers, Orders AS o;

