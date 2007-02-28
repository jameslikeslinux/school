#include <string.h>
#include <ncurses.h>
#include <form.h>
#include "cdk.h"
#include "http.h"

#define TITLE "HTTP Client"
#define AUTHOR "James Lee <jlee23@umbc.edu>"
#define FIELD_START 6
#define FIELD_BUF_SIZE 255

#define NO_ACTION 0
#define URL_INPUTTING (1 << 0)
#define METHOD_SELECTING (1 << 1)
#define POST_INPUTTING (1 << 2)
#define CANCEL_SHOWING (1 << 3)

void create_windows();
void create_input_form();
void select_input_form();
void delete_input_form();
void draw_stdscr();
void draw_url_input();
void draw_method_selection();
void draw_post_input();
void draw_commands();
void draw_cancel();
void set_input_action(int action);
void message_logged();
void trim(char *string);

static log_t log;
static int current_action, log_lines, log_cols, input_lines, input_cols, command_lines, command_cols;
static char url[FIELD_BUF_SIZE], post[FIELD_BUF_SIZE];
static WINDOW *log_window, *input_window, *command_window;
static CDKSCREEN *cdk_screen;
static CDKSWINDOW *log_swindow;
static FORM *url_form;
static FIELD *url_field[2];

int main() {
	int logi, ch, y, x;
	http_t http;
	http_url_t http_url;

	log_init(&log);
	log_register_message_callback(&log, message_logged);

	current_action = NO_ACTION;

	initscr();
	create_windows();
	cdk_screen = initCDKScreen(log_window);
	log_swindow = newCDKSwindow(cdk_screen, 0, 0, log_lines, log_cols, "", 255, 0, 0);

	draw_stdscr();
	draw_url_input();
	draw_commands();

	select_input_form();

	raw();
	noecho();
	keypad(stdscr, TRUE);

	while ((ch = wgetch(input_window)) != '' || current_action & CANCEL_SHOWING)
		if (ch == KEY_LEFT)
			form_driver(url_form, REQ_LEFT_CHAR);
		else if (ch == KEY_RIGHT)
			form_driver(url_form, REQ_RIGHT_CHAR);
		else if (ch == KEY_BACKSPACE || ch == 127) {
			getyx(input_window, y, x);

			if (x == FIELD_START + 1) {
				form_driver(url_form, REQ_SCR_HBHALF);
				form_driver(url_form, REQ_END_LINE);
			}

			form_driver(url_form, REQ_LEFT_CHAR);
			form_driver(url_form, REQ_DEL_CHAR);
		} else if (ch == KEY_DC)
			form_driver(url_form, REQ_DEL_CHAR);
		else if (ch == KEY_HOME)
			form_driver(url_form, REQ_BEG_FIELD);
		else if (ch == KEY_END)
			form_driver(url_form, REQ_END_FIELD);
		else if (ch == KEY_UP || ch == KEY_DOWN)
			injectCDKSwindow(log_swindow, ch);
		else if (ch == '')
			cleanCDKSwindow(log_swindow);
		else if (ch == '\n' && current_action & URL_INPUTTING) {
			form_driver(url_form, REQ_END_FIELD);
			form_driver(url_form, 'a');
			form_driver(url_form, REQ_BEG_FIELD);
			strcpy(url, field_buffer(url_field[0], 0));
			*strrchr(url, 'a') = '\0';

			draw_method_selection();
			draw_cancel();
		} else if (tolower(ch) == 'g' && current_action & METHOD_SELECTING) {
			/* log_printf(&log, INFO, "ui", "GET %s", url); */
			http_url_init(&http_url);
			if (http_url_parse(&http_url, url))
				return;
			http_init(&http, &http_url, &log);
			http_go(&http, GET, NULL, 0, 3);
		} else if (tolower(ch) == 'h' && current_action & METHOD_SELECTING) {
			log_printf(&log, INFO, "ui", "HEAD %s", url);
		} else if (tolower(ch) == 'p' && current_action & METHOD_SELECTING) {
			draw_post_input();
			draw_cancel();
			select_input_form();
		} else if (ch == '\n' && current_action & POST_INPUTTING) {
			form_driver(url_form, REQ_END_FIELD);
			form_driver(url_form, 'a');
			form_driver(url_form, REQ_BEG_FIELD);
			strcpy(post, field_buffer(url_field[0], 0));
			*strrchr(url, 'a') = '\0';
			
			log_printf(&log, INFO, "ui", "POST %s", url);
			log_printf(&log, INFO, "ui", "%s", post);
		} else if (ch == '') {
			draw_url_input();
			draw_commands();
			select_input_form();
		} else
			form_driver(url_form, ch);

	delete_input_form();
	destroyCDKSwindow(log_swindow);
	destroyCDKScreen(cdk_screen);
	endwin();

	return 0;
}

void create_windows() {
	log_lines = LINES - 3;
	log_cols = COLS;
	log_window = newwin(log_lines, log_cols, 1, 0);

	input_lines = 1;
	input_cols = COLS;
	input_window = newwin(input_lines, input_cols, LINES - 2, 0);

	command_lines = 1;
	command_cols = COLS;
	command_window = newwin(command_lines, command_cols, LINES - 1, 0);
}

void create_input_form() {
	delete_input_form();

	keypad(input_window, TRUE);

	url_field[0] = new_field(1, input_cols - (FIELD_START + 1), 0, 0, 0, 0);
	url_field[1] = NULL;

	set_field_back(url_field[0], A_REVERSE);
	field_opts_off(url_field[0], O_AUTOSKIP);
	field_opts_off(url_field[0], O_STATIC);
	set_max_field(url_field[0], FIELD_BUF_SIZE);

	url_form = new_form(url_field);
	set_form_win(url_form, input_window);
	set_form_sub(url_form, derwin(input_window, 1, input_cols - (FIELD_START + 1), 0, FIELD_START));
	post_form(url_form);

	wrefresh(input_window);
}

void select_input_form() {
	form_driver(url_form, ' ');
	form_driver(url_form, REQ_LEFT_CHAR);
	form_driver(url_form, REQ_DEL_CHAR);
}

void delete_input_form() {
	unpost_form(url_form);

	if (url_form) {
		free_form(url_form);
		url_form = NULL;
	}

	if (url_field[0]) {
		free_field(url_field[0]);
		url_field[0] = NULL;
	}
}

void draw_stdscr() {
	clear();

	attron(A_BOLD);
	mvprintw(0, 0, TITLE);
	attroff(A_BOLD);
	mvprintw(0, COLS - strlen(AUTHOR), AUTHOR);

	refresh();
}

void draw_url_input() {
	create_input_form();

	wclear(input_window);
	wprintw(input_window, "URL: ");
	wrefresh(input_window);

	set_input_action(URL_INPUTTING);
}

void draw_method_selection() {
	delete_input_form();

	wclear(input_window);
	
	wattron(input_window, A_BOLD);
	wprintw(input_window, " G");
	wattroff(input_window, A_BOLD);
	wprintw(input_window, " GET  ");

	wattron(input_window, A_BOLD);
	wprintw(input_window, " H");
	wattroff(input_window, A_BOLD);
	wprintw(input_window, " HEAD  ");

	wattron(input_window, A_BOLD);
	wprintw(input_window, " P");
	wattroff(input_window, A_BOLD);
	wprintw(input_window, " POST");
	
	wrefresh(input_window);

	set_input_action(METHOD_SELECTING);
}

void draw_post_input() {
	create_input_form();

	wclear(input_window);
	wprintw(input_window, "POST: ");
	wrefresh(input_window);

	set_input_action(POST_INPUTTING);
}

void draw_commands() {
	wclear(command_window);

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^I");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Info  ");

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^E");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Errors  ");

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^C");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Quit");

	wrefresh(command_window);

	current_action &= ~CANCEL_SHOWING;
}

void draw_cancel() {
	wclear(command_window);

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^C");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Cancel");

	wrefresh(command_window);

	current_action |= CANCEL_SHOWING;
}

void set_input_action(int action) {
	current_action &= ~URL_INPUTTING;
	current_action &= ~METHOD_SELECTING;
	current_action &= ~POST_INPUTTING;
	current_action |= action;
}

void message_logged() {
	message_t *message;
	char time[12], line[255];

	message = (message_t*) log.messages.tail->data;

	strftime(time, 12, "%X", localtime(&message->time));
	snprintf(line, 255, "</B>%s<!B>  %s: %s", time, message->source, message->description);

	addCDKSwindow(log_swindow, line, BOTTOM);
	refreshCDKScreen(cdk_screen);
}
