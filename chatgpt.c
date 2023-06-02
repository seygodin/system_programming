#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
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
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer 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");

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        while(1) {
		char input[MAX_INPUT_SIZE];

    		FILE *file = fopen("input.txt", "r");
    		if (file == NULL) {
        		printf("Could not open file 'input.txt'\n");
        		return 1;
    		}

    		while(fgets(input, MAX_INPUT_SIZE, file)) {
        	input[strcspn(input, "\n")] = 0; // remove trailing newline
    		printf("work\n");
		}

    		fclose(file);

    		if (strcmp(input, "exit") == 0) {
        		return 0;
    		}

            char data[MAX_INPUT_SIZE + 200]; // extra space for JSON formatting
	    snprintf(data, sizeof(data), "{\"model\":\"gpt-3.5-turbo\", \"messages\":[{\"role\":\"system\", \"content\":\"ChatGPT session\"}, {\"role\":\"user\", \"content\":\"%s\"}]}", input);

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
	    
   	    FILE *out_file = fopen("output.txt", "a");
		if (file == NULL) {
    		printf("Could not open file 'output.txt'\n");
    		return 1;
		}

		fprintf(out_file, "Content: %s\n", content->valuestring);
		fclose(out_file);

	    //printf("Content: %s\n", content->valuestring);
	    cJSON_Delete(root);
	    //printf("%s\n", s.ptr);
            free(s.ptr);
	    return 0;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

