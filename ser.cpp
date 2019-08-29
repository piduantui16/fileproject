#include "socket.h"
#include "thread.h"
#include <assert.h>
#include<signal.h>
int main()
{
    int sockfd = create_socket();
    assert( sockfd != -1 );

    while( 1 )
    {
        struct sockaddr_in caddr;
        int len = sizeof(caddr);

        int c = accept(sockfd,(struct sockaddr*)&caddr,(socklen_t*)&len);
        if ( c < 0 )
        {
            continue;
        }

        printf("accept c =%d\n",c);

        //启动线程 
        int res = thread_start(c);
        if ( res  == -1 )
        {
            close(c);
        }
    }
}
