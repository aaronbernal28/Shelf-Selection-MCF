# my_MCF_db

## Building a PostgreSQL Database
This database is built using PostgreSQL to manage data related to the Shelf Selection MCF project. Below are some useful commands to interact with the database.

### Create Database
Before running the application, ensure that the database is created:
```bash
createdb my_MCF_db
```

### Open Database
To connect to the database, use the following command:
```bash
psql -d my_MCF_db
```

### Create the main Table Orders
Run `schema.sql` to create the main table `orders`:
```bash
psql -d my_MCF_db -f schema.sql
```

### Verify Table Creation
To verify that the table has been created, you can use:
```bash
psql -d my_MCF_db -c "\dt"
```