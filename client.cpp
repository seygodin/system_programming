#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int clientSocket, bytesRead;
    struct sockaddr_in serverAddress;
    char buffer[1024];

    // 클라이언트 소켓 생성
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Failed to create socket");
        exit(1);
    }

    // 서버 주소 설정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.54.49");

    // 서버에 연결
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    printf("Connected to server\n");

    // 서버와의 통신 시작
    while (1) {
        // 서버로 데이터 송신
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (write(clientSocket, buffer, strlen(buffer)) < 0) {
            perror("Failed to write to socket");
            break;
        }

        // 서버로부터 데이터 수신
        memset(buffer, 0, sizeof(buffer));
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("Failed to read from socket");
            break;
        }

        printf("Server: %s\n", buffer);

        // 종료 커맨드 확인
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
    }

    // 소켓 종료
    close(clientSocket);

    return 0;
}
