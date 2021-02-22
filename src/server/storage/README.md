# How to create and launch database:

 1. Install mariadb-server 
 for example: `pacman -S mariadb`
 
 2. Start Mariadb
 `sudo systemctl start mysqld`
 
 3. Do secure installation 
 `sudo mysql_secure_installation`
 
 4. Verify your root credentials by loggin on Mariadb
` mysql -u root -p`
 or (if you skipped secure installation):
` sudo mysql -u root`
 
 5. Enter/copy-paste sql commands:
```sql CREATE DATABASE melon;
CREATE USER 'melon'@'localhost' IDENTIFIED BY 'melonpass';
GRANT ALL PRIVILEGES ON melon.* TO 'melon'@'localhost'; 
quit;
```
6. Log as created user with created password:

```bash
mysql -u melon -p
(enter melonpass)
```
7. Check that there is `melon` database:
```sql
SHOW DATABASES LIKE 'melon';
```
 and `quit;`
 
 8. Create tables in `melon` database from melondb.sql
 
``` mysql -u melon -p melon < script/melondb.sql ```
