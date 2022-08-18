#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <iostream>
    
#define PORT     8080 
#define MAXLINE 1024 
    
using namespace std;

int num = 0;

void receberInfo(int n, int sockfd, struct sockaddr_in     servaddr, char buffer[MAXLINE], unsigned int len);

void enviar1Pacote10M(int sockfd, const char *message, struct sockaddr_in     servaddr,  char buffer[MAXLINE], unsigned int len, int n){
    char letras[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    string msg;
    int r;
    for(int i = 0; i < 10240; i++){
        r = rand() % 27;
        msg = msg + letras[r];
    }

    //Divide o Arquivo > 1024 Bytes em Pacotes de No máximo 1024 Bytes
    string pacotes[10];
    for(int i = 0; i < 10; i++){
        for(int j = 1024 * i; j < 1024 * (i + 1); j++){
            pacotes[i] = pacotes[i] + msg[j];
        }
    }

    for(int i = 0; i < 10; i++){
        message = pacotes[i].c_str();
        sendto(sockfd, (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr));
        receberInfo(n, sockfd, servaddr, buffer, len);
    }
}

void enviar1PacoteGrande(int sockfd, const char *message, struct sockaddr_in     servaddr,  char buffer[MAXLINE], unsigned int len, int n){
    char letras[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    string msg;
    int r;
    for(int i = 0; i < 1024; i++){
        r = rand() % 27;
        msg = msg + letras[r];
    }

    message = msg.c_str();
    sendto(sockfd, (const char *)message, strlen(message), 
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr));
    receberInfo(n, sockfd, servaddr, buffer, len);
    
}

void enviarXPacotes(int sockfd, const char *message, struct sockaddr_in     servaddr, int numPackages,  char buffer[MAXLINE], unsigned int len, int n){

    while(numPackages > 0){
        string msg = "PACOTE " + to_string(num);
        message = msg.c_str();
        sendto(sockfd, (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr));
        receberInfo(n, sockfd, servaddr, buffer, len);
        num++;
        numPackages--;    
    }
}

void connect(int sockfd, const char *message, struct sockaddr_in     servaddr,  char buffer[MAXLINE], unsigned int len, int n){
    string msg = "Client Conectou";
    message = msg.c_str();
    sendto(sockfd, (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr));
}

void disconnect(int sockfd, const char *message, struct sockaddr_in     servaddr,  char buffer[MAXLINE], unsigned int len, int n){
    string msg = "Client Desconectou";
    message = msg.c_str();
    sendto(sockfd, (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr));
}

void receberInfo(int n, int sockfd, struct sockaddr_in     servaddr, char buffer[MAXLINE], unsigned int len){
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len); 
    buffer[n] = '\0';
    if(n > 0) 
        printf("Server : %s\n", buffer); 
}

void menu(int n, char buffer[MAXLINE], unsigned int len, char const *message, int sockfd, struct sockaddr_in     servaddr){
    int escolha = -1;
    while(escolha < 1){
        cout << "===============================================" << endl << endl << endl;
        cout << "Escolha uma opção:" << endl;
        cout << "[1]Enviar 1 pacote para o Servidor" << endl;
        cout << "[2]Enviar 10 pacotes para o Servidor" << endl;
        cout << "[3]Enviar 1 pacote de 1MB(1024 Bytes) para o Servidor" << endl;
        cout << "[4]Enviar 1 pacote de 10MB(10240 Bytes) para o Servidor" << endl;
        cout << "[5]Desconectar" << endl;
        cin >> escolha;
        if(escolha < 1 || escolha > 5){
            cout << "OPÇÃO INVÁLIDA!" << endl;
            menu(n, buffer, len, message, sockfd, servaddr);
        }

    }
    switch(escolha){
        case 1:
        {
            num++;
            string msg = "PACOTE " + to_string(num);
            message = msg.c_str();

            sendto(sockfd, (const char *)message, strlen(message), 
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr)); 
            printf("enviado.\n"); 
            receberInfo(n, sockfd, servaddr, buffer, len);
            menu(n, buffer, len, message, sockfd, servaddr);
            break;
        }
        case 2:
        {
            enviarXPacotes(sockfd, message, servaddr, 10, buffer, len, n);
            menu(n, buffer, len, message, sockfd, servaddr);
            break;
        }
        case 3:
        {
            enviar1PacoteGrande(sockfd, message, servaddr, buffer, len, n);
            menu(n, buffer, len, message, sockfd, servaddr);
            break;
        }
        case 4:
        {
            enviar1Pacote10M(sockfd, message, servaddr, buffer, len, n);
            menu(n, buffer, len, message, sockfd, servaddr);
            break;
        }
        case 5:
        {
            disconnect(sockfd, message, servaddr, buffer, len, n);
            exit(0);
        }
    }
}

// Driver code 
int main() { 
    int sockfd; 
    char buffer[MAXLINE]; 
    char const *message = ""; 
    struct sockaddr_in     servaddr; 

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    memset(&servaddr, 0, sizeof(servaddr)); 
        
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
        
    int n; 
    unsigned int len;
        
    connect(sockfd, message, servaddr, buffer, len, n);

    menu(n, buffer, len, message, sockfd, servaddr);
    
    close(sockfd); 
    return 0; 
}
