#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ncursesw/curses.h>
#include <sys/socket.h>
#include <locale.h>

// Function prototypes
int login();
void logout();
void display_main_menu(int selected);
void display_chat_menu(int selected);
void execute_ncurses_chat_client();
void connect_server();
void connect_client();

// Global variables
int login_success = 0; // Tracks if user is logged in

int main() {
	 setlocale(LC_CTYPE, "ko_KR.utf-8"); /* 로케일 설정을 했는데도 */
    int ch;
    int selected = 1;
    int choice;

    initscr(); // Initialize ncurses
    cbreak(); // Disable line buffering
    noecho(); // Do not echo user input
    keypad(stdscr, TRUE); // Enable special keys handling

    while (1) {
        display_main_menu(selected);
        ch = wgetch(stdscr); // Get user input

        switch (ch) {
            case KEY_UP:
                selected = (selected > 1) ? selected - 1 : 4;
                break;
            case KEY_DOWN:
                selected = (selected < 4) ? selected + 1 : 1;
                break;
            case 10: // Enter key
                if (selected == 1) {
                    login_success = login();
                } else if (selected == 2) {
                    // Signup functionality here (not implemented in this code)
                } else if (selected == 3) {
                    logout();
                    login_success = 0;
                } else if (selected == 4) {
                    endwin(); // End ncurses mode
                    printf("Program terminated.\n");
                    exit(0);
                }
                break;
        }

        if (login_success) {
            while (login_success) {
                display_chat_menu(selected);
                ch = wgetch(stdscr);

                switch (ch) {
                    case KEY_UP:
                        selected = (selected > 1) ? selected - 1 : 4;
                        break;
                    case KEY_DOWN:
                        selected = (selected < 4) ? selected + 1 : 1;
                        break;
                    case 10: // Enter key
                        if (selected == 1) {
                            execute_ncurses_chat_client();
                        } else if (selected == 2) {
                            connect_server();
                        } else if (selected == 3) {
                            connect_client();
                        } else if (selected == 4) {
                            login_success = 0;
                            break;
                        }
                        break;
                }
            }
        }
    }

    endwin(); // End ncurses mode
    return 0;
}

int login() {
    char username[50];
    char password[50];

    clear();
    printw("사용자 이름: ");
    refresh();
    echo(); // Enable echo for user input
    scanw("%49s", username);
    noecho(); // Disable echo after input

    printw("비밀번호: ");
    refresh();
    echo();
    scanw("%49s", password);
    noecho();

    printw("%s님으로 로그인되었습니다.\n", username);
    refresh();
    getch(); // Wait for user input to proceed

    return 1; // Indicate successful login
}

void logout() {
    clear();
    printw("로그아웃 되었습니다.\n");
    refresh();
    getch(); // Wait for user input to proceed
}

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

void display_chat_menu(int selected) {
    clear();
    printw("채팅 메뉴\n");
    printw("%s ncurses 채팅 클라이언트 실행\n", selected == 1 ? ">" : " ");
    printw("%s 서버 접속\n", selected == 2 ? ">" : " ");
    printw("%s 클라이언트 접속\n", selected == 3 ? ">" : " ");
    printw("%s 메인 메뉴로\n", selected == 4 ? ">" : " ");
    printw("선택하세요: ");
    refresh();
}

void execute_ncurses_chat_client() {
    clear();
    printw("ncurses 채팅 클라이언트를 실행합니다...\n");
    refresh();
    system("./tcp_client 127.0.0.1 4000");
    getch();
}

void connect_server() {
    clear();
    printw("서버를 실행합니다...\n");
    refresh();
    system("./server");
    getch();
}

void connect_client() {
    char ip_address[50];
    char port[10];
    char username[50];
    char command[150];

    clear();
    printw("서버 IP 주소를 입력하세요: ");
    refresh();
    echo();
    scanw("%49s", ip_address);
    noecho();

    printw("포트 번호를 입력하세요: ");
    refresh();
    echo();
    scanw("%9s", port);
    noecho();

    printw("사용자 이름을 입력하세요: ");
    refresh();
    echo();
    scanw("%49s", username);
    noecho();

    snprintf(command, sizeof(command), "./client %s %s %s", ip_address, port, username);

    printw("클라이언트를 실행합니다...\n");
    refresh();
    system(command);
    getch();
}

