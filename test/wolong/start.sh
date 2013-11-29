#!/bin/bash
#export PATH=$PATH:/tmp/mysql/bin

#mycnf=$(pwd)/my.cnf
mycnf=/tmp/mysql/my.cnf

#cd /tmp/mysql
#rm /tmp/data/core.*
exec /tmp/mysql/bin/mysqld_safe --defaults-file=$mycnf

