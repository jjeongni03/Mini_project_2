#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ncursesw/curses.h>
#include <sys/socket.h>
#include <locale.h>
void display_main_menu(int selected);
void display_chat_menu();
int login();
void signup();
void execute_ncurses_chat_client();
void connect_server();
void connect_client();

void display_main_menu(int selected) {
    clear();
    printw("메인 메뉴\n");
    printw("%s 로그인\n", selected == 1 ? ">" : " ");
    printw("%s 회원가입\n", selected == 2 ? ">" : " ");
    printw("%s 로그아웃\n", selected == 3 ? ">" : " ");
    printw("%s 종료\n", selected == 4 ? ">" : " ");
    printw("선택하세요: ");
    refresh();
}

void display_chat_menu() {
    clear();
    printw("채팅 메뉴\n");
    printw("1. ncurses 채팅 클라이언트 실행\n");
    printw("2. 서버 접속\n");
    printw("3. 클라이언트 접속\n");
    printw("4. 메인 메뉴로\n");
    printw("선택하세요 (숫자 입력): ");
    refresh();
}

int login() {
    char username[50];
    char password[50];

    printw("사용자 이름: ");
    refresh();
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printw("비밀번호: ");
    refresh();
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    printw("%s님으로 로그인되었습니다.\n", username);
    refresh();
    return 1;
}

void signup() {
    char username[50];
    char password[50];

    printw("새 사용자 이름: ");
    refresh();
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printw("새 비밀번호: ");
    refresh();
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    printw("%s님으로 회원가입 완료.\n", username);
    refresh();
}

void execute_ncurses_chat_client() {
    printw("ncurses 채팅 클라이언트를 실행합니다...\n");
    refresh();
    system("./tcp_client 127.0.0.1 4000");
}

void connect_server() {
    char port[10];
    char command[50];

    printw("서버 포트 번호를 입력하세요: ");
    refresh();
    fgets(port, sizeof(port), stdin);
    port[strcspn(port, "\n")] = '\0';

    snprintf(command, sizeof(command), "./server %s", port);

    printw("서버를 실행합니다...\n");
    refresh();
    system(command);
}

void connect_client() {
    char ip_address[50];
    char port[10];
    char username[50];
    char command[150];

    printw("서버 IP 주소를 입력하세요: ");
    refresh();
    fgets(ip_address, sizeof(ip_address), stdin);
    ip_address[strcspn(ip_address, "\n")] = '\0';

    printw("포트 번호를 입력하세요: ");
    refresh();
    fgets(port, sizeof(port), stdin);
    port[strcspn(port, "\n")] = '\0';

    printw("사용자 이름을 입력하세요: ");
    refresh();
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    snprintf(command, sizeof(command), "./client %s %s %s", ip_address, port, username);

    printw("클라이언트를 실행합니다...\n");
    refresh();
    system(command);
}

int main() {
	 setlocale(LC_CTYPE, "ko_KR.utf-8"); /* 로케일 설정을 했는데도 */
    int ch;
    int login_success = 0;
    int selected = 1;
    int choice;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (1) {
        display_main_menu(selected);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                selected = (selected > 1) ? selected - 1 : 4;
                break;
            case KEY_DOWN:
                selected = (selected < 4) ? selected + 1 : 1;
                break;
            case 10:
                if (selected == 1) {
                    login_success = login();
                    if (login_success) {
                        while (1) {
                            display_chat_menu();
                            scanw("%d", &choice);
                            clear();

                            switch (choice) {
                                case 1:
                                    execute_ncurses_chat_client();
                                    break;
                                case 2:
                                    connect_server();
                                    break;
                                case 3:
                                    connect_client();
                                    break;
                                case 4:
                                    break;
                                default:
                                    printw("잘못된 선택입니다. 다시 시도하세요.\n");
                                    refresh();
                                    break;
                            }
                            if (choice == 4) break;
                        }
                    }
                } else if (selected == 2) {
                    signup();
                } else if (selected == 3) {
                    printw("로그아웃 되었습니다.\n");
                    refresh();
                    login_success = 0;
                } else if (selected == 4) {
                    endwin();
                    return 0;
                }
                break;
            default:
                printw("잘못된 선택입니다. 다시 시도하세요.\n");
                refresh();
                break;
        }
    }

    endwin();
    return 0;
}

