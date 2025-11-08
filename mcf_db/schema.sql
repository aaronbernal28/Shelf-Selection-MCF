CREATE TABLE backlog (
	order_id CHAR(25) PRIMARY KEY,
	item_id CHAR(17),
	quantity INTEGER DEFAULT 1,
	creation_date TIMESTAMP,
	due_date TIMESTAMP,
	closure_date TIMESTAMP DEFAULT NULL,
	status VARCHAR(11) DEFAULT 'PENDING'
);
