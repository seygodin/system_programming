#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int serverSocket, clientSocket, bytesRead;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[1024];

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(1);
    }

    // 서버 주소 설정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // 서버 소켓과 주소를 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    // 클라이언트의 연결을 대기
    if (listen(serverSocket, 3) < 0) {
        perror("Failed to listen");
        exit(1);
    }

    printf("Server started. Waiting for incoming connections...\n");

    // 클라이언트의 연결을 수락
    socklen_t clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket < 0) {
        perror("Failed to accept connection");
        exit(1);
    }

    printf("Client connected\n");

    // 클라이언트와의 통신 시작
    while (1) {
        // 클라이언트로부터 데이터 수신
        memset(buffer, 0, sizeof(buffer));
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("Failed to read from socket");
            break;
        }

        printf("Client: %s\n", buffer);

        // 클라이언트로 데이터 송신
        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (write(clientSocket, buffer, strlen(buffer)) < 0) {
            perror("Failed to write to socket");
            break;
        }

        // 종료 커맨드 확인
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
    }

    // 소켓 종료
    close(clientSocket);
    close(serverSocket);

    return 0;
}
