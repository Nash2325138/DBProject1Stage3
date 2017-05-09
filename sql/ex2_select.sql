select * from Customers;
select Customers.Name from Customers;
select C.* from Customers AS C;
select C.CID, C.Name from Customers AS C;

select * from Customers where CID > 6;
select * from Customers where CID > 6 OR Salary >50000;
select * from Customers where CID > 6 AND Salary >50000;

select * from Customers, Orders;
select C.CID, C.Name, O.ID from Customers AS C, Orders AS O;

select C.*, O.* from Customers AS C, Orders AS O WHERE C.CID = O.CID;
select C.*, O.* from Customers AS C, Orders AS O WHERE C.CID <> O.CID;

select C.*, O.* from Customers AS C, Orders AS O WHERE C.CID = O.CID AND C.Salary > 35000;
select C.*, O.* from Customers AS C, Orders AS O WHERE C.CID = O.CID AND 35000 < C.Salary;

select count(c.Name) from Customers AS c, Orders AS o WHERE C.CID = O.CID AND 35000 < C.Salary;
select sum(Customers.Salary) from Customers AS c, Orders AS o WHERE C.CID = O.CID AND 35000 < C.Salary;
