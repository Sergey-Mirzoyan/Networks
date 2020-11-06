#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>
#include "info.h"

void perror_and_exit(char *s)
{
	perror(s);
	exit(1);
}

void DecToBin( int n )
 {
 
if ( n >= 2 )
{
DecToBin( n/2 );
}
printf("%d", n % 2);

}

void DecToOct(int n)
{
	int b,k,c;
	k=1;
	c=0;
	while (n>0)
	{
	    b=n%8; //остаток от деления на 8
	    n/=8;  //каждый раз будет делиться на 8, уменьшаясь
	    c+=b*pow(10,k-1); //остаток от деления умножить на 10 в степени  к-1, т.е. последнее число - 10^0, следующее число  - 10^1 и т.д.
	    b=0;   //всегда будет обнуляться, чтобы в переменную можно было вновь занести остаток при делении на 8
	    k++; //будет увеличиваться на один(переменная для степени при составлении числа)
	}
 
    printf ("%d\n",c);
}

void DecToHex (int inDEC)
{
    char* outHEX;
    int remain, i;
    
    //Calculating
    i = 0;
    while ( inDEC != 0)
    {
        remain = inDEC % 16;
        switch(remain)
        {
            case 10: outHEX[i] = 'A'; break;
            case 11: outHEX[i] = 'B'; break;
            case 12: outHEX[i] = 'C'; break;
            case 13: outHEX[i] = 'D'; break;
            case 14: outHEX[i] = 'E'; break;
            case 15: outHEX[i] = 'F'; break;
            default: outHEX[i] = remain+'0'; break;
        }
        
        inDEC /= 16;
        i++;
    }
    outHEX[i] = '\0';
    
    //Revert for printing => There got some problem with reverting the char*...
    //outHEX = RevertHEX(outHEX);
    printf("%c", outHEX);
    return;
}


int main(void)
{
	struct sockaddr_in server_addr, client_addr;
	int sock, slen = sizeof(client_addr);
	char buf[MSG_LEN];

	printf("Server started\n");

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		perror_and_exit("socket");

	memset((char *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SOCK_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, &server_addr, sizeof(server_addr)) == -1)
		perror_and_exit("bind");

	while (1)
	{
		if (recvfrom(sock, buf, MSG_LEN, 0, &client_addr, &slen) == -1)
			perror_and_exit("recvfrom()");
		int x = atoi(buf);
		
		printf("Received packet from %s:%d\nData: %s\n\n",
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
		DecToBin(x);
		printf("\n");
		DecToOct(x);
		printf("\n");
			printf("%x\n",x);
		
		printf("A3");
	}

	close(sock);
	return 0;
}
