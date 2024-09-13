#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>
#include <ncursesw/curses.h>



void connect_client() {
    char ip_address[50];
    char port[10];
    char username[50];
    char command[150];

    printf("서버 IP 주소를 입력하세요: ");
    fgets(ip_address, sizeof(ip_address), stdin);
    ip_address[strcspn(ip_address, "\n")] = '\0';  // 개행 문자 제거

    printf("포트 번호를 입력하세요: ");
    fgets(port, sizeof(port), stdin);
    port[strcspn(port, "\n")] = '\0';  // 개행 문자 제거

    printf("사용자 이름을 입력하세요: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  // 개행 문자 제거

    // 입력받은 IP 주소, 포트, 사용자 이름을 사용해 명령어 생성
    snprintf(command, sizeof(command), "./client %s %s %s", ip_address, port, username);

    printf("클라이언트를 실행합니다...\n");
    system(command);  // 생성된 명령어 실행
}
// 메인 메뉴 함수
void display_main_menu() {
       clear(); // 화면 지우기
    printw("메인 메뉴\n");
    printw("%s 로그인\n", selected == 1 ? ">" : " ");
    printw("%s 회원가입\n", selected == 2 ? ">" : " ");
    printw("%s 로그아웃\n", selected == 3 ? ">" : " ");
    printw("%s 종료\n", selected == 4 ? ">" : " ");
    printw("선택하세요: ");
    refresh();
}

// 두 번째 메뉴 함수 (로그인 후)
void display_chat_menu() {
    printf("1. ncurses 채팅 클라이언트 실행\n");
    printf("2. 서버 접속\n");
    printf("3. 클라이언트 접속\n");
    printf("4. 메인 메뉴로\n");
    printf("선택하세요: ");
}

// 로그인 처리 함수
int login() {
    char username[50];
    char password[50];

    printf("사용자 이름: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("비밀번호: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    printf("%s님으로 로그인되었습니다.\n", username);
    return 1;
}

// 회원가입 처리 함수
void signup() {
    char username[50];
    char password[50];

    printf("새 사용자 이름: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("새 비밀번호: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    printf("%s님으로 회원가입 완료.\n", username);
}

// ncurses 채팅 클라이언트 실행 함수 (로그인 성공 후 실행됨)
void execute_ncurses_chat_client() {
    printf("ncurses 채팅 클라이언트를 실행합니다...\n");
    // system()을 사용하여 tcp_client 실행
    system("./tcp_client 127.0.0.1 4000");
}

// 서버 접속 실행 함수
void connect_server() {
    printf("서버를 실행합니다...\n");
    system("./server");  // server.c로부터 컴파일된 서버 실행 파일
}

// 클라이언트 접속 실행 함수
void connect_client() {
    printf("클라이언트를 실행합니다...\n");
    system("./client 127.0.0.1 4000 사용자이름");  // client.c로부터 컴파일된 클라이언트 실행 파일
}

int main() {
    int ch;
    int login_success = 0; 
    int selected = 1;
    
    initscr(); // ncurses 초기화
    cbreak(); // 입력을 버퍼링하지 않고 바로 읽어들임
    noecho(); // 입력된 문자를 화면에 출력하지 않음
    keypad(stdscr, TRUE); // 방향키 사용을 위한 설정;

    while (1) {
        display_main_menu();  
        ch =  getchar(); 

        switch (ch) {
            case KEY_UP:  // 로그인
                login_success = login();
                if (login_success) {
                    // 로그인 성공 후 두 번째 메뉴 출력
                    while (1) {
                        display_chat_menu();
                        scanf("%d", &choice);
                        getchar();  // 버퍼 개행 제거

                        if (choice == 1) {
                            execute_ncurses_chat_client();
                        } else if (choice == 2) {
                            connect_server();  // 서버 접속
                        } else if (choice == 3) {
                            connect_client();  // 클라이언트 접속
                        } else if (choice == 4) {
                            break;  // 메인 메뉴로 돌아가기
                        } else {
                            printf("잘못된 선택입니다. 다시 시도하세요.\n");
                        }
                    }
                }
                break;
            case 2:  // 회원가입
                signup();
                break;
            case 3:  // 로그아웃
                printf("로그아웃 되었습니다.\n");
                login_success = 0;
                break;
            case 4:  // 종료
                printf("프로그램을 종료합니다.\n");
                exit(0);
            default:
                printf("잘못된 선택입니다. 다시 시도하세요.\n");
                break;
        }
    }

    return 0;
}

