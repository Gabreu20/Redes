#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <string> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
    
#define PORT     8080 
#define JANELA 1024 * 10
    
using namespace std;

int main() { 
    srand(time(NULL));
    int sockfd; 
    char buffer[JANELA]; 
    int num_seq = 0;

    int janela = 10;

    int acks = 0;
    int ackSeq = 0;

    char const *ack = "ACK"; 
    struct sockaddr_in servaddr, cliaddr; 
        
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
        
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
        
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    int n; //tamanho do em bytes do pacote recebido
    unsigned int len;
    
    len = sizeof(cliaddr);   

    //RECEBE INFORMAÇÃO QUE O CLIENT CONECTOU-SE
    n = recvfrom(sockfd, (char *)buffer, JANELA,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len);
    printf("%s\n", buffer); 

    while(true){
        n = recvfrom(sockfd, (char *)buffer, JANELA,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 
        //==========================Perdas Arbitrárias de Pacotes==========================
        //Dessa forma, cerca de 3/4 dos pacotes serão recebidos e 1/4 será "Perdido"
        //No relatoŕio, Os números sem perdas serão apresentados com testes feitos sem essa implementação de perdas de pacotes
        int R = rand()%4;
        if(R < 3){
            acks++;
            ackSeq = ackSeq + n;
            buffer[n] = '\0'; 
            printf("Client : %s  \nNum. de Sequencia: %i\n", buffer, num_seq); 
            num_seq = num_seq + n;
        }
        else{
            //PERDEU O PACOTE
        }
        //==========================Perdas Arbitrárias de Pacotes==========================

        //envia um ACK
        if(acks%janela == 0 && acks > 0){
            string send_ack = "ACK " + to_string(ackSeq);

            ack = send_ack.c_str();

            sendto(sockfd, (const char *)ack, strlen(ack),  
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len); 
            printf("ACK SENT\n"); 
        }
        else if(acks%janela == 0 && acks == 0){
            const char *empty = "";
            sendto(sockfd, (const char *)empty, strlen(empty),  
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len);
        }
        else{
            const char *empty = "";
            sendto(sockfd, (const char *)empty, strlen(empty),  
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len);
        }
    }

    return 0; 
}
