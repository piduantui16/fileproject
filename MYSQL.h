#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include"/usr/include/mysql/mysql.h"

static std::string server = "127.0.0.1";
static std::string user = "root";
static std::string password = "111111";
static std::string dbname = "fileproject";

class User
{
public:
		User(){}
		void setName(std::string name){_name=name;}
		void setPassword(std::string pwd){_password=pwd;}

		std::string getName()const{return _name;}
		std::string getPassword()const{return _password;}
private:
		int _id;
		std::string _name;
		std::string _password;
};

bool login(char* name,char *pwd);
bool reg(char *name,char *pwd);
bool is_exist(char*name,char *md5buff);
bool updatmysql(char *filename,char *md5buff,char *uname);
