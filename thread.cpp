#include "thread.h"
#include <fcntl.h>
#include<signal.h>
#include<iostream>
#include"MYSQL.h"
#define     ARGC    10
#define     READ_BUFF   4096

void send_file(int c, char * name)
{
    if ( name == NULL )
    {
        send(c,"err#no name",11,0);
        return;
    }

    int fd = open(name,O_RDONLY);
    if ( fd == -1 )
    {
        send(c,"err",3,0);
        return;
    }
	
    int size = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);

    char res_buff[128] = {0};
    sprintf(res_buff,"ok#%d",size);

    send(c,res_buff,strlen(res_buff),0);
	
	int clifilesize[1] = {0};
	recv(c,clifilesize,sizeof(clifilesize),0);
	if(clifilesize[0] == size)
	{
		return ;
	}
	lseek(fd,clifilesize[0],SEEK_SET);

    char cli_status[64] = {0};
    if ( recv(c,cli_status,63,0) <= 0 )
    {
        close(fd);
        return;
    }

    if ( strncmp(cli_status,"ok",2) != 0 )
    {
        close(fd);
        return;
    }

    int num = 0;
    char sendbuff[1024] = {0};
    while( (num = read(fd,sendbuff,1024)) > 0 )
    {

        send(c,sendbuff,num,0);
		signal(SIGPIPE, SIG_IGN);
    }

    close(fd);

    return;
}

void recv_file(int c,char *name,char *uname)
{
	char buff[128] = {0};
	if(recv(c,buff,127,0) <= 0)
	{
		return ;
	}

	if(strncmp(buff,"ok#",3) != 0)
	{
		printf("Error:%s\n",buff+3);
		return;
	}

	int size = 0;
	printf("file size:%s\n",buff+3);
	sscanf(buff+3,"%d",&size);

	if(size == 0)
	{
		send(c,"err",3,0);
		return ;
	}
	
	int fd = open(name,O_WRONLY|O_CREAT,0600);
	if(fd == -1)
	{
		send(c,"err",3,0);
		return ;
	}

	send(c,"ok",2,0);

	char recvbuff[1024] = {0};

	int num = 0 ;
	int cur_size = 0;
	while((num = recv(c,recvbuff,1024,0)) > 0)
	{
		cur_size += num;
		write(fd,recvbuff,num);
		if(cur_size >= size)
		{
			break;
		}
		if(num <= 0)
		{
			break;
		}
		
	}
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
	send(c,md5buff,128,0);
	updatmysql(md5buff,name,uname);//更新数据库
	fclose(fp);
	close(fd);
}

void * work_thread(void * arg)
{
    int c = (int)arg;
	char name1[20] = {0};
	while(1)
	{
		char i[2] = {0};
		int n = recv(c,i,2,0);
		if(n <= 0)
		{
			printf("client over\n");
			break;
		}
		printf("%c\n",i[0]);
		if(i[0] == '1')
		{
			//login;
			
			recv(c,name1,20,0);
			char password1[20] = {0};
			recv(c,password1,20,0);
			if(login(name1,password1))
			{
				char resbf1[20] = {"ok"};
				send(c,resbf1,20,0);
				break;
			}
			else
			{
				char resbf2[20] = {"no"};
				send(c,resbf2,20,0);
			}
			continue;
		}
		else if(i[0]=='2')
		{
			//reg;
 			char name[20] = {0};
			recv(c,name,20,0);
			char password[20] = {0};
			recv(c,password,20,0);
			if(reg(name,password))
			{
				char resbf[20] = {"ok"};
				send(c,resbf,20,0);
				continue;
			}
		}
	}
    while( 1 )
    {
        char buff[256] = {0};
        int n = recv(c,buff,255,0);//ls, mv a.c b.c , rm a.c ,get a.c put, aa
		
		if ( n <= 0 )
        {
            printf("one client over\n");
            break;
        }

        int i = 0;
        char* myargv[ARGC] = {0};
        char * ptr = NULL;
        char * s = strtok_r(buff," ",&ptr);
        while( s != NULL )
        {
            myargv[i++] = s;
            s = strtok_r(NULL," ",&ptr);
        }

        char * cmd = myargv[0];//cmd 

        if ( cmd == NULL )
        {
            send(c,"err",3,0);
            continue;
        }

        if ( strcmp(cmd,"get") == 0 )
        {
            //下载
			send_file(c,myargv[1]);
        }
        else if ( strcmp( cmd, "put") == 0 )
        {
            //上传
			char Md5buff[128]={0};
			recv(c,Md5buff,128,0);
			if(is_exist(myargv[1],Md5buff))
			{
				updatmysql(Md5buff,myargv[1],name1);
				send(c,"ok",2,0);
				printf("file send over!\n");
			}
			else
			{
				send(c,"no",2,0);
				recv_file(c,myargv[1],name1);
			}
        }
        else
        {
            int pipefd[2];
            pipe(pipefd);

            pid_t pid = fork();
            if ( pid == -1 )
            {
                send(c,"err",3,0);
                continue;
            }

            if ( pid == 0 )
            {
                dup2(pipefd[1],1);
                dup2(pipefd[1],2);
                
                execvp(cmd,myargv);
                perror("cmd err");
                exit(0);
            }

            close(pipefd[1]);
            wait();
            char readbuff[READ_BUFF] = {"ok#"};
            read(pipefd[0],readbuff+3,READ_BUFF-4);

            send(c,readbuff,strlen(readbuff),0);
            close(pipefd[0]);
        }
    }

    close(c);
}

int thread_start(int c )
{
    pthread_t id;
    int res = pthread_create(&id,NULL,work_thread,(void*)c);
    if ( res != 0  )
    {
        return -1;
    }

    return 0;
}
