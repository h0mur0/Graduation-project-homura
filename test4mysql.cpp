#include<iostream>
#include <mysql.h>
 
int main(){
    MYSQL *mysql = mysql_init(0);
    if(!mysql_real_connect(mysql,"localhost","root","homura721223","mybase",0,0,0)) 
    {
        printf("连接数据库出错：%s",mysql_error(mysql));   
        return -1;    
    }
    std::cout << "DB connect success" << std::endl;
 
    if(mysql_query(mysql,"insert into mentor values ('abandon')"))
    {
        printf("Info : %s",mysql_error(mysql));
    }
    return 0;
}
