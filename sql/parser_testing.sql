SELECT Lname  FROM names
SELECT Lname, Fname FROM names
SELECT Lname, M, Fname FROM names
SELECT N1.Lname  FROM names AS N1
SELECT N1.Lname, N1.Fname FROM names AS N1
SELECT N1.Lname, N1.M, N1.Fname FROM names AS N1
SELECT N1.Lname, N2.M, N3.Fname FROM names AS N1, names AS N2, names AS N3
SELECT Fname  FROM names WHERE name='DBMS'
SELECT Fname FROM names WHERE name = 'DBMS' AND ID < 10
SELECT Fname FROM names WHERE ID < 10
SELECT Fname FROM names WHERE ID > 10
SELECT Fname FROM names WHERE ID <> 10
SELECT Fname FROM names WHERE ID<>10
SELECT Fname FROM names WHERE ID = 10
SELECT Fname FROM names WHERE Fname = 'DBMS' AND        Lname = 'DBMSSSSSS' 
SELECT Fname FROM names WHERE Fname = 'DBMS' OR        Lname = 'DBMSSSSSS' 
SELECT N1.Lname, N2.M FROM names AS N1, names AS N2 WHERE N1.Fname = N2.Fname
SELECT N1.Lname, N2.M FROM names AS N1, names AS N2 WHERE N1.Fname = N2.Fname OR 1 = 1
SELECT N1.Lname, N2.M FROM names AS N1, names AS N2 WHERE N1.Fname
SELECT N1.Lname, N2.M FROM names AS N1, names AS N2 WHERE L1.name = N2.Fname OR 1 <> 29239


