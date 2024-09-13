#include "server.h"

char *EXIT_STRING = "나가기";
int client_sockets[MAX_CLIENTS];  // 클라이언트 소켓들을 저장하는 배열
char client_names[MAX_CLIENTS][50];  // 각 클라이언트의 이름 저장

// 클라이언트 소켓에 해당하는 클라이언트 이름을 반환하는 함수
char* get_client_name(int fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == fd) {
            return client_names[i];
        }
    }
    return NULL;
}

// 모든 클라이언트에게 메시지를 전송하는 함수
void broadcast_message(char *message, int sender_fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_fd) {
            send(client_sockets[i], message, strlen(message)+ 1, 0);
        }
    }
}

int main(int argc, char *argv[]) {
    int listen_fd, new_fd, len, max_clients = MAX_CLIENTS;
    struct sockaddr_in server_addr, client_addr;
    char message[MAXLINE + 1], client_name[50];
    char buffer[MAXLINE];
    int pipe_fds[2];  // 부모-자식 간 통신용 파이프
    // 메시지의 최대 길이를 제한하는 코드
    size_t name_len = strlen(client_name);
    size_t msg_len = strlen(message);


    if (argc != 2) {
        printf("사용법: %s 포트번호\n", argv[0]);
        exit(0);
    }

    // 소켓 생성
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("소켓 생성 실패");
        exit(1);
    }

    // 파이프 생성
    if (pipe(pipe_fds) == -1) {
        perror("pipe 생성 실패");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // 소켓 바인딩
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        close(listen_fd);
        exit(1);
    }

    // 클라이언트 연결 대기
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen 실패");
        close(listen_fd);
        exit(1);
    }

    printf("서버가 준비되었습니다. 포트번호: %s\n", argv[1]);

    // 클라이언트 소켓 배열 초기화
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    len = sizeof(client_addr);

    while (1) {
        new_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
        if (new_fd < 0) {
            perror("accept 실패");
            continue;
        }

        // 빈 자리 찾기
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_fd;

                // 클라이언트가 연결되면 이름을 받음
                recv(new_fd, client_name, MAXLINE + 50, 0);
                client_name[strcspn(client_name, "\r\n")] = 0;  // 개행 제거
                strncpy(client_names[i], client_name, sizeof(client_name) - 1);

                // 새로운 클라이언트 접속 메시지를 부모 프로세스에 전송
                char connect_msg[MAXLINE + 50];
                sprintf(connect_msg, "[%s]님이 입장했습니다.\n", client_names[i]);
                broadcast_message(connect_msg, new_fd);
                write(pipe_fds[1], connect_msg, strlen(connect_msg));  // 파이프에 메시지 전송

                break;
            }
        }

        if (fork() == 0) {  // 자식 프로세스
            close(listen_fd);  // 자식은 리슨 소켓 닫기

            while (1) {
                memset(message, 0, MAXLINE);
                int n = recv(new_fd, message, MAXLINE, 0);
                if (n <= 0) {
                    printf("[%s]가 채팅방을 나갔습니다.\n", client_name);
                    break;
                }

                // 개행 문자 제거
                message[strcspn(message, "\r\n")] = 0;

                printf("[%s]: %s\n", client_name, message);
                // 버퍼 오버플로우 방지를 위해 크기를 제한
                 if (name_len > 50) name_len = 50;  // client_name의 최대 길이를 50자로 제한
                 if (msg_len > 1024) msg_len = 1024;
                char full_message[MAXLINE + 50];
                snprintf(full_message,sizeof(full_message), "[%.*s]: %.*s",(int)name_len,client_name,(int)msg_len, message);
                broadcast_message(full_message, new_fd);

                // 종료 메시지 처리
                if (strstr(message, EXIT_STRING) != NULL) {
                    printf("[%s]가 채팅방을 나갔습니다.\n", client_name);
                    break;
                }
            }
            close(new_fd);
            exit(0);
        } else {
            close(new_fd);  // 부모는 새 연결을 닫고 계속 대기

            // 파이프에서 새로운 클라이언트 접속 메시지 읽음
            char pipe_buf[MAXLINE + 50];
            read(pipe_fds[0], pipe_buf, sizeof(pipe_buf));
            printf("%s", pipe_buf);  // 화면에 새로운 클라이언트 접속 메시지 출력
        }
    }

    close(listen_fd);
    return 0;
}

