## Building a PostgreSQL Database
This database is built using PostgreSQL to manage data related to the Shelf Selection MCF project. Below are some useful commands to interact with the database.

### Create User
To create a new PostgreSQL user, use the following command:
```bash    
sudo -u postgres createuser --superuser --pwprompt
```

### Create Database
Before running the application, ensure that the database is created:
```bash
sudo -u postgres createdb <env.DB_NAME>
```

### Open Database
To connect to the database, use the following command:
```bash
psql -d <env.DB_NAME>
```

### Create the main Table Backlog
Run `schema.sql` to create the main table `backlog`:
```bash
psql -d <env.DB_NAME> -f schema.sql
```

### Verify Table Creation
To verify that the table has been created, you can use:
```bash
psql -d <env.DB_NAME> -c "\dt"
```
### View backlog Table
To view the contents of the `backlog` table, you can run:
```bash
psql -d <env.DB_NAME> -c "SELECT * FROM backlog;"
```