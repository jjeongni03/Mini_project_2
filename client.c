#include "client.h"

char *EXIT_STRING = "나가기";

// 메시지를 서버로부터 받는 함수 정의
void receive_message(int sock) {
    char buffer[MAXLINE];
    int n;
    while ((n = recv(sock, buffer, MAXLINE, 0)) > 0) {
        buffer[n] = '\0';
        printf("%s\n", buffer);  // 서버로부터 받은 메시지를 출력
    }
    if (n == 0) {
        printf("서버 연결이 종료되었습니다.\n");
        close(sock);
        exit(0);
    } else {
        perror("메시지 수신 실패");
    }
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[MAXLINE + 1], sendline[MAXLINE + 1];
    char name[50];  // 클라이언트 이름
    pid_t pid;

    if (argc != 4) {
        printf("사용법 : %s 서버ip주소 포트번호 이름\n", argv[0]);
        exit(0);
    }

    // 이름 설정
    strncpy(name, argv[3], sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    // 1. 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("소켓 생성 실패");
        exit(1);
    }

    // 2. 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("잘못된 주소");
        exit(1);
    }

    // 3. 서버에 연결
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("서버 연결 실패");
        close(sockfd);
        exit(1);
    }

    // 4. 서버에 클라이언트 이름 전송
    send(sockfd, name, strlen(name), 0);
    printf("%s님으로 로그인하였습니다! 채팅을 시작하세요!\n", name);
    if (send(sockfd, name, strlen(name), 0) < 0) {
        perror("이름 전송 실패");
        close(sockfd);
        exit(1);
    }

    // 5. fork() 실행: 자식은 수신, 부모는 송신
    pid = fork();
    if (pid < 0) {
        perror("fork 실패");
        close(sockfd);
        exit(1);
    }

    if (pid == 0) {  // 자식 프로세스: 서버로부터 메시지 수신
        receive_message(sockfd);
    } else {  // 부모 프로세스: 사용자 입력 및 메시지 송신
        while (1) {
            // 채팅 인터페이스 출력(입력하기)
            printf("[Me]: ");
            fgets(message, MAXLINE, stdin);

            // 종료 문자 확인
            if (strstr(message, EXIT_STRING) != NULL) {
                printf("채팅을 종료합니다.\n");
                break;
            }

            // 서버에 메시지 전송
            send(sockfd, message, strlen(message), 0);
        }
    }

    close(sockfd);
    return 0;
}

