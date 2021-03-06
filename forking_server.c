#include "pipe_networking.h"
#include <signal.h>
#include <ctype.h>

void process(char *s);
void subserver(int from_client);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove(WKP);
    exit(0);
  }
}

char* rot13(char *text) {
	//printf("str: %s len: %d\n", text, strlen(text));
	char *new_text = (char*)malloc(strlen(text)*sizeof(char));
	
	unsigned char temp;
	int x;
	for (x = 0; x < strlen(text); x++) {
		temp = isalpha(text[x]) ? text[x] + 13 : text[x];
		if ((text[x] >= 'A' && text[x] <= 'Z') && temp > 'Z') {
			temp -= 26;
		}
		else if ((text[x] >= 'a' && text[x] <= 'z') && temp > 'z') {
			temp -= 26;
		}
		new_text[x] = temp;
	}
	
	return new_text;
}

int main() {
	signal(SIGINT, sighandler);
	
	int fd, is_parent;
	while(1) {
		fd = server_setup();
		is_parent = fork();
		
		if(!is_parent) {
			//to_client = server_connect(fd);
			subserver(fd);
		}
	}
}

void subserver(int from_client) {
	int to_client = server_connect(from_client);
	char data[BUFFER_SIZE];
	
	while(read(from_client, data, sizeof(data))) {
		printf("\n[SUBSERVER %d] Waiting for client message\n", getpid());
		
		/*
		if (!strcmp(data, FIN)) {
			printf("[SUBSERVER] received FIN from client\n");
			//server_fin(from_client, to_client);
			break;
		}
		*/
		
		printf("[SUBSERVER %d] Read message from client: %s\n", getpid(), data);
		//len = strlen(rot13(data));
		process(data);
		printf("[SUBSERVER %d] Sending response to client\n", getpid());
		write(to_client, data, strlen(data));
	}
	exit(0);
}

void process(char * s) {
	char *new = rot13(s);
	
	int x = 0;
	for (; x < strlen(s)+1; x++) {
		s[x] = new[x];
	}
}
