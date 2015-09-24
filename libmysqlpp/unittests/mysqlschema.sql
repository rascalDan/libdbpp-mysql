CREATE TABLE test(
		id int,
		fl numeric(5,2),
		string varchar(30),
		boolean bool,
		dt timestamp,
		ts time);

INSERT INTO test VALUES(4, 123.45, 'some text', true, '2015-04-27 23:06:03', '38:13:12');

CREATE TABLE bulktest(
		intcol int,
		stringcol varchar(3000));

