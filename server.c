#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define MAX_INPUT_SIZE 1024

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

int main(void) {
    //Comunication
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
    
    
    
    //chatgpt init
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer serverkey");

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        

        while(1) {
            // 클라이언트로부터 데이터 수신
            memset(buffer, 0, sizeof(buffer));
            bytesRead = read(clientSocket, buffer, sizeof(buffer));
            if (bytesRead < 0) {
                perror("Failed to read from socket");
                break;
            }

            printf("Client: %s\n", buffer);
            
         

    		if (strcmp(buffer, "exit") == 0) {
        		return 0;
    		}

            char data[MAX_INPUT_SIZE + 200]; // extra space for JSON formatting
	    snprintf(data, sizeof(data), "{\"model\":\"gpt-3.5-turbo\", \"messages\":[{\"role\":\"system\", \"content\":\"ChatGPT session\"}, {\"role\":\"user\", \"content\":\"%s\"}]}", buffer);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

            struct string s;
            init_string(&s);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            cJSON *root = cJSON_Parse(s.ptr);
            cJSON *choices = cJSON_GetObjectItemCaseSensitive(root, "choices");
            cJSON *firstChoice = cJSON_GetArrayItem(choices, 0);
            cJSON *message = cJSON_GetObjectItemCaseSensitive(firstChoice, "message");
            cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");
            



            printf("Server:  ");
            const char* sourceString = content->valuestring;
            size_t sourceLength = strlen(sourceString);
            size_t maxCopyLength = sizeof(buffer)-1;
            size_t copyLength = (sourceLength < maxCopyLength)?sourceLength : maxCopyLength;
	    printf(content->valuestring);
	    strncpy(buffer, sourceString,copyLength);
            buffer[copyLength] = '\0';
//fgets(buffer, sizeof(buffer), stdin);
            
            if (write(clientSocket, buffer, strlen(buffer)) < 0) {
        
        perror("Failed to write to socket");
                break;
            }
            
            
            
	    //printf("Content: %s\n", content->valuestring);
	    cJSON_Delete(root);
            free(s.ptr);
	 
        }
        close(clientSocket);
        close(serverSocket);


        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

