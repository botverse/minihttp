#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXRCVLEN 1023

static int is_ip_address(const char *s) {
    while (*s) {
        if ((isdigit(*s)) || (*s == '.')) {
            s++;
            continue;
        }
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
    in_addr_t host;
    int len, mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    if (is_ip_address(argv[1])) {
        host = inet_addr(argv[1]);
    } else {
        struct hostent hostst = *gethostbyname(argv[1]);
        memcpy(&host, *(hostst.h_addr_list), sizeof (in_addr_t));
    }
    printf("Host %s\n", argv[1]);
    memset(&dest, 0, sizeof (dest)); /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = host; /* set destination IP number */
    dest.sin_port = htons(80); /* set destination port number */

    connect(mysocket, (struct sockaddr *) &dest, sizeof (struct sockaddr));

    sprintf(buffer, "GET %s HTTP/1.0\n\n", argv[2]);
    send(mysocket, buffer, strlen(buffer), 0);

    len = recv(mysocket, buffer, MAXRCVLEN, 0);

    /* We have to null terminate the received data ourselves */
    buffer[len] = '\0';

    printf("Received %s (%d bytes).\n", buffer, len);

    close(mysocket);
    return EXIT_SUCCESS;
}
