#include"MYSQL.h"
#include"/usr/include/mysql/mysql.h"
bool reg(char* name,char* pwd)
{
	MYSQL mysql;
	mysql_init(&mysql);
	MYSQL *p=mysql_real_connect(&mysql, server.c_str(), user.c_str(),password.c_str(), dbname.c_str(), 3306,NULL, 0);
	if(p == NULL)
	{
		printf("connection fail!!!!\n");
	}

	char sql[1024] = {0};
	sprintf(sql, "insert into User(name,password) values('%s','%s');",name,pwd);
	if(mysql_query(p,sql) == true)
	{
		printf("insert fail!!!\n");
		 mysql_close(&mysql);

		return false;
	}
	else
	{
		printf("insert success!!\n");
		 mysql_close(&mysql);
		 return true;
	}
}

bool login(char* name,char* pwd)
{
	MYSQL mysql;
	mysql_init(&mysql);
	MYSQL *p=NULL;
	p=mysql_real_connect(&mysql, server.c_str(), user.c_str(),password.c_str(), dbname.c_str(), 3306,NULL, 0);
	if(p == NULL)
	{
		 printf("connection fail!!!!\n");
	}

	char sql1[1024] = {0};
	sprintf(sql1,"select id from User where name='%s' and password='%s';",name,pwd);
	printf("there\n");

	mysql_query(p,sql1);

	MYSQL_RES* res = mysql_use_result(p);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row != NULL)
	{
		printf("login success\n");
		return true;
	}
	printf("login fail\n");
	return false;
}



bool is_exist(char*name,char *md5buff)
{
	MYSQL mysql;
	mysql_init(&mysql);
	MYSQL *p=NULL;
	p=mysql_real_connect(&mysql, server.c_str(), user.c_str(),password.c_str(), dbname.c_str(), 3306,NULL, 0);
	if(p == NULL)
	{
		 printf("connection fail!!!!\n");
	}

	char sql1[1024] = {0};
	sprintf(sql1,"select fileownnerid from MD5 where MD5sum='%s';",md5buff);
	printf("there\n");

	mysql_query(p,sql1);

	MYSQL_RES* res = mysql_use_result(p);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row != NULL)
	{
		printf("file exist!\n");
		return true;
	}
	printf("file not_exist!\n");
	return false;
}




bool updatmysql(char *MD5sum,char *filename,char *uname)
{
	MYSQL mysql;
	mysql_init(&mysql);
	MYSQL *p=mysql_real_connect(&mysql, server.c_str(), user.c_str(),password.c_str(), dbname.c_str(), 3306,NULL, 0);
	if(p == NULL)
	{
		printf("connection fail!!!!\n");
	}
	
	char sql1[1024] = {0};
	sprintf(sql1, "select id from User where name='%s';",uname);

	mysql_query(p,sql1);
	MYSQL_RES* res = mysql_store_result(p);
	MYSQL_ROW row = mysql_fetch_row(res);
	char sql[1024] = {0};
	
	sprintf(sql, "insert into MD5(MD5sum,filename,fileownnerid) values('%s','%s','%d');",MD5sum,filename,atoi(row[0]));

	if(mysql_query(p,sql) == true)
	{
		printf("file insert fail!!!\n");
		 mysql_close(&mysql);

		return false;
	}
	else
	{
		printf("file insert success!!\n");
		mysql_close(&mysql);
		return true;
	}
}
