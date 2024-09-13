#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ncursesw/curses.h>
#include <sys/socket.h>
#include <locale.h>
#include <curses.h>

void display_main_menu(int selected);
void display_chat_menu(int selected);
int login();
void logout();
void ncurses_chat_client();
void connect_server();
void connect_client();

int login_success = 0;

void display_main_menu(int selected) {
    clear();
    printw("메인 메뉴\n");
    printw("%s 로그인\n", selected == 1 ? ">" : " ");
    printw("%s 로그아웃\n", selected == 2 ? ">" : " ");
    printw("%s 종료\n", selected == 3 ? ">" : " ");
    printw("선택하세요: ");
    refresh();
}

void display_chat_menu(int selected) {
    clear();
    printw("채팅 메뉴\n");
    printw("%s 채팅 실행\n", selected == 1 ? ">" : " ");
    printw("%s 서버 접속\n", selected == 2 ? ">" : " ");
    printw("%s 클라이언트 접속\n", selected == 3 ? ">" : " ");
    printw("%s 메인 메뉴로\n", selected == 4 ? ">" : " ");
    printw("선택하세요: ");
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

void ncurses_chat_client() {
    printw("채팅을 실행합니다...\n");
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

    clear();
    printw("서버 IP 주소를 입력하세요: ");
    refresh();
    fgets(ip_address, sizeof(ip_address), stdin);
   // ip_address[strcspn(ip_address, "\n")] = '\0';

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
    getch();
}

int main() {
    setlocale(LC_CTYPE, "ko_KR.utf-8"); /* 로케일 설정을 했는데도 */
    int ch;
    int selected = 1;
    int choice;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (1) {
        display_main_menu(selected);
        ch = wgetch(stdscr);

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
		}else if (selected == 2) {
		   logout();
		   login_success = 0;
		}else if (selected == 3) {
			endwin();
			printf("프로그램이 종료되었습니다.\n");
		        exit(0);
		}
		break;
		default:
                printw("잘못된 선택입니다. 다시 시도하세요.\n");
                refresh();
                break;
		
	}

         if(login_success){
                           while (1) {
                            display_chat_menu(selected);
                            ch = wgetch(stdscr);
                         
                            switch (ch) {
                               
				   case KEY_UP:
                                    selected = (selected > 1) ? selected - 1 : 4;
                                    break;
                                   case KEY_DOWN:
                                     selected = (selected < 4) ? selected + 1 : 1;
                                     break;
				   case 10:
				     if (selected == 1) {
                                    ncurses_chat_client();
				     }
                                    else if(selected == 2) {
                                    connect_server();
				    }
                                    else if(selected == 3) {
                                    connect_client();
				    }
                                    else if(selected == 4) {
				     break;
				    }
                                   else{
                                    printw("잘못된 선택입니다. 다시 시도하세요.\n");
                                    refresh();
                                    break;
                            }                          
                    }
	       }
	}

    endwin();
    return 0;
}

}
