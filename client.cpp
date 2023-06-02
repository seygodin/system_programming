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

    // Ŭ���̾�Ʈ ���� ����
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Failed to create socket");
        exit(1);
    }

    // ���� �ּ� ����
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.54.49");

    // ������ ����
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    printf("Connected to server\n");

    // �������� ��� ����
    while (1) {
        // ������ ������ �۽�
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (write(clientSocket, buffer, strlen(buffer)) < 0) {
            perror("Failed to write to socket");
            break;
        }

        // �����κ��� ������ ����
        memset(buffer, 0, sizeof(buffer));
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("Failed to read from socket");
            break;
        }

        printf("Server: %s\n", buffer);

        // ���� Ŀ�ǵ� Ȯ��
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
    }

    // ���� ����
    close(clientSocket);

    return 0;
}
