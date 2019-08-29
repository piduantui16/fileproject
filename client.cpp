#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include<iostream>
#define     PORT    6000
#define     IPSTR   "127.0.0.1"

int connect_to_ser();
void recv_file(int sockfd, char* name);
void send_file(int sockfd,char * name);
int main()
{
    int sockfd = connect_to_ser();
    if ( sockfd == -1 )
    {
        printf("connect to ser failed!\n");
        return 0;
    }
	while(1)
	{
		printf("-------------1.login---------------\n");
		printf("--------------2.reg----------------\n");
		char buffi[2] = {0};
		fgets(buffi,2,stdin);
		send(sockfd,buffi,strlen(buffi),0);
		if(buffi[0] == '1')	
		{
	   		char name[20] = {0};
			char pwd[20] = {0};
			printf("please cin your name:");
			std::cin >> name;
			printf("\nplease cin your password:");
			std::cin >> pwd;
			send(sockfd,name,20,0);
			send(sockfd,pwd,20,0);
			char resser[20] ={0};
			recv(sockfd,resser,20,0);
			if(strcmp(resser,"ok") != 0)
			{   
				printf("login fail\n");
				continue;
			}
			printf("login success\n");
			break;
		}
		else if(buffi[0] =='2')
		{
			char name[20] = {0};
			char pwd[20] = {0};
			printf("please cin your name:");
			std::cin >> name;
			printf("\nplease cin your password:");
			std::cin >> pwd;
			send(sockfd,name,20,0);
			send(sockfd,pwd,20,0);
			char resser[20] ={0};
			recv(sockfd,resser,20,0);
			if(strcmp(resser,"ok") != 0)
			{
				printf("reg fail\n");
				continue;
			}
			printf("reg success\n");
			continue;
		}
	}
    while( 1 )
    {
        char buff[128] = {0};

        printf("Connect>>");
        fflush(stdout);

        fgets(buff,128,stdin);//  ls, rm a.c , get a.c ,sdfg , exit
        buff[strlen(buff)-1] = 0;

        if ( buff[0] == 0 )
        {
            continue;
        }

        char sendbuff[128] = {0};
        strcpy(sendbuff,buff);

        int i = 0;
        char * myargv[10] = {0};
        char * s = strtok(buff," ");
        while( s != NULL )
        {
            myargv[i++] = s;
            s = strtok(NULL," ");
        }

        if ( myargv[0] == NULL )
        {
            continue;
        }

        if ( strcmp(myargv[0],"exit") == 0 )
        {
            break;
        }

		if(strcmp(myargv[0],"ps")==0)
		{
			printf("禁止执行此操作！！\n");
		}
		else if ( strcmp(myargv[0],"get") == 0 )
        {
            if ( myargv[1] == NULL )
            {
                continue;
            }
            send(sockfd,sendbuff,strlen(sendbuff),0);//get a.c
            recv_file(sockfd,myargv[1]);

        }
		else if ( strcmp(myargv[0],"put") == 0 )
        {
			if(myargv[1] == NULL)
			{
				continue;
			}
			send(sockfd,sendbuff,strlen(sendbuff),0);

			char mdbuff[128] = {0};
			char md5buff[128] = {0};
			FILE* fp1 = fopen("tmp.txt","w+");
			sprintf(mdbuff,"md5sum %s %s",myargv[1],">tmp.txt");
			system(mdbuff);
			fgets(md5buff,128,fp1);
			char *p = md5buff;	
			while(*p != ' ')
			{
				p++;
			}
			*p='\0';
			fclose(fp1);
			send(sockfd,md5buff,128,0);
			char filesend[20]=  {0};
			recv(sockfd,filesend,20,0);
			if(strncmp(filesend,"ok",2)==0)
			{
				printf("filesend ok!\n");
			}
			else
			{
				send_file(sockfd,myargv[1]);
			}
        }
        else
        {
            send(sockfd,sendbuff,strlen(sendbuff),0);

            char readbuff[4096] = {0};
            recv(sockfd, readbuff,4095,0);
            if ( strncmp(readbuff,"ok#",3) != 0 )
            {
                printf("err");
                continue;
            }

            printf("%s\n",readbuff+3);

        }
    }

    close(sockfd);
}
bool check_file(char serbuff[128],char localbuff[128])
{
	if(strcmp(serbuff,localbuff)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void send_file(int sockfd, char *name)
{
	if(name == NULL)
	{
		send(sockfd,"err#no name",11,0);
		return ;
	}

	int fd = open(name,O_RDONLY);
	if(fd == -1)
	{
		send(sockfd,"err",3,0);
		return ;
	}

	int size = lseek(fd,0,SEEK_END);
	printf("file size:%d\n",size);
	lseek(fd,0,SEEK_SET);
	
	char res_buff[128] = {0};
	sprintf(res_buff,"ok#%d",size);

	send(sockfd,res_buff,strlen(res_buff),0);

	char ser_status[64] = {0};
	if(recv(sockfd,ser_status,63,0) <= 0)
	{
		close(fd);
		return ;
	}

	if(strncmp(ser_status,"ok",2) != 0)
	{
		close(fd);
		return ;
	}

	int num = 0;
	char sendbuff[1024] = {0};
	int cur_size = 0;
	while((num = read(fd,sendbuff,1024)) > 0)
	{
		send(sockfd,sendbuff,num,0);
		cur_size+=num;
		float f = cur_size * 100.0 / size ;
		printf("下载:%.2f%%\r",f);
		fflush(stdout);
		if(cur_size >= size)
		{
			break;
		}
	}
	printf("\n文件上传完成！\n");
	char mdbuff[128] = {0};
	char md5buff[128] = {0};
	FILE* fp = fopen("tmp.txt","w+");
	sprintf(mdbuff,"md5sum %s %s",name,">tmp.txt");
	system(mdbuff);
	fgets(md5buff,128,fp);
	char *p = md5buff;
	while(*p != ' ')
	{
		p++;
	}	
	*p='\0';
	fclose(fp);

	char sermd5buff[128]={0};
	recv(sockfd,sermd5buff,128,0);
	if(check_file(md5buff,sermd5buff))
	{
		printf("file check success!!\n");
	}
	else
	{
		printf("file check fail\n");
	}
	close(fd);

	return ;
}
void recv_file(int sockfd, char* name)
{
    char buff[128] = {0};
    if ( recv(sockfd,buff,127,0) <= 0 )
    {
        return;
    }

    if ( strncmp(buff,"ok#",3) != 0 )//ok#345
    {
        printf("Error:%s\n",buff+3);
        return;
    }

    int size = 0;
    printf("file size:%s\n",buff+3);
    sscanf(buff+3,"%d",&size);

    if( size == 0 )
    {
        send(sockfd,"err",3,0);
        return;
    }
    int fd = open(name,O_WRONLY|O_CREAT,0600);
    if ( fd == -1 )
    {
        send(sockfd,"err",3,0);
        return;
    }
	int localfilesize[1] = {0};
	if(access(".",F_OK)==0)
	{
		localfilesize[0] = lseek(fd,0,SEEK_END);
		send(sockfd,localfilesize,sizeof(localfilesize),0);
	}
	else
	{
		send(sockfd,localfilesize,sizeof(localfilesize),0);
	}
	if(localfilesize[0] == size)
	{
		printf("该文件已存在！\n");
		return ;
	}
    send(sockfd,"ok",2,0);

    char recvbuff[1024] = {0};

    int num = 0;
    int curr_size = localfilesize[0];

    while( ( num = recv(sockfd,recvbuff,1024,0)) > 0 )
    {
        write(fd,recvbuff,num);
        curr_size += num;

        float f = curr_size * 100.0 / size ;
        printf("下载:%.2f%%\r",f);
        fflush(stdout);

        if ( curr_size >= size )
        {
            break;
        }
    }

    printf("\n文件下载完成!\n");
    close(fd);

}

int connect_to_ser()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if ( sockfd == -1 )
    {
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = inet_addr(IPSTR);

    int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if ( res == -1 )
    {
        return -1;
    }

    return sockfd;
}
