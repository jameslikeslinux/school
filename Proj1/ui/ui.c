#include <string.h>
#include <ncurses.h>
#include <form.h>
#include "cdk.h"
#include "log.h"

#define TITLE "HTTP Client"
#define AUTHOR "James Lee <jlee23@umbc.edu>"
#define FIELD_START 6
#define FIELD_BUF_SIZE 255

void create_windows();
void create_input_form();
void select_input_form();
void delete_input_form();
void draw_stdscr();
void draw_url_input();
void draw_post_input();
void draw_commands();
void draw_cancel();
void message_logged();

static log_t log;
static int log_lines, log_cols, input_lines, input_cols, command_lines, command_cols, cancel_showing;
static WINDOW *log_window, *input_window, *command_window;
static CDKSCREEN *cdk_screen;
static CDKSWINDOW *log_swindow;
static FORM *url_form;
static FIELD *url_field[2];

int main() {
	int logi, ch, y, x;
	char *url;

	log_init(&log);
	log_register_message_callback(&log, message_logged);

	initscr();

	create_windows();

	cdk_screen = initCDKScreen(log_window);
	log_swindow = newCDKSwindow(cdk_screen, 0, 0, log_lines, log_cols, "", 255, 0, 0);

	raw();
	create_input_form();
	draw_stdscr();
	draw_url_input();
	draw_commands();

	select_input_form();

	raw();
	noecho();
	keypad(stdscr, TRUE);

	while ((ch = wgetch(input_window)) != '' || cancel_showing)
		switch (ch) {
			case KEY_LEFT:
				form_driver(url_form, REQ_LEFT_CHAR);
				break;

			case KEY_RIGHT:
				form_driver(url_form, REQ_RIGHT_CHAR);
				break;

			case KEY_BACKSPACE:
			case 127:
				getyx(input_window, y, x);
				if (x == FIELD_START + 1) {
					form_driver(url_form, REQ_SCR_HBHALF);
					form_driver(url_form, REQ_END_LINE);
				}
				form_driver(url_form, REQ_LEFT_CHAR);
				form_driver(url_form, REQ_DEL_CHAR);
				break;

			case KEY_DC:
				form_driver(url_form, REQ_DEL_CHAR);
				break;

			case KEY_HOME:
				form_driver(url_form, REQ_BEG_FIELD);
				break;

			case KEY_END:
				form_driver(url_form, REQ_END_FIELD);
				break;

			case KEY_UP:
			case KEY_DOWN:
				injectCDKSwindow(log_swindow, ch);
				break;

			case '':
				form_driver(url_form, REQ_BEG_FIELD);
				url = field_buffer(url_field[0], 0);
				log_message(&log, INFO, "ui", url);

				delete_input_form(); create_input_form();
				draw_post_input();
				draw_cancel();

				select_input_form();
	
				break;

			case '':
				delete_input_form(); create_input_form();
				draw_url_input();
				draw_commands();
				
				select_input_form();

				break;

			default:
				form_driver(url_form, ch);
				break;
		}

	delete_input_form();
	destroyCDKSwindow(log_swindow);
	destroyCDKScreen(cdk_screen);
	endwin();

	return 0;
}

void create_windows() {
	log_lines = LINES - 4;
	log_cols = COLS;
	log_window = newwin(log_lines, log_cols, 1, 0);

	input_lines = 2;
	input_cols = COLS;
	input_window = newwin(input_lines, input_cols, LINES - 3, 0);

	command_lines = 1;
	command_cols = COLS;
	command_window = newwin(command_lines, command_cols, LINES - 1, 0);
}

void create_input_form() {
	keypad(input_window, TRUE);

	url_field[0] = new_field(1, input_cols - (FIELD_START + 1), 0, 0, 0, 0);
	url_field[1] = NULL;

	set_field_back(url_field[0], A_REVERSE);
	field_opts_off(url_field[0], O_AUTOSKIP);
	field_opts_off(url_field[0], O_STATIC);
	set_max_field(url_field[0], FIELD_BUF_SIZE);

	url_form = new_form(url_field);
	set_form_win(url_form, input_window);
	set_form_sub(url_form, derwin(input_window, 1, input_cols - (FIELD_START + 1), 1, FIELD_START));
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
	free_form(url_form);
	free_field(url_field[0]);
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
	wclear(input_window);
	
	wprintw(input_window, "Enter a URL and choose a method.\n");
	wprintw(input_window, "URL: ");

	wrefresh(input_window);
}

void draw_post_input() {
	wclear(input_window);
	
	wprintw(input_window, "Enter the data to POST.\n");
	wprintw(input_window, "POST: ");

	wrefresh(input_window);
}

void draw_commands() {
	wclear(command_window);

	wprintw(command_window, "Methods: ");
	
	wattron(command_window, A_BOLD);
	wprintw(command_window, "^G");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " GET   ");

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^H");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " HEAD   ");

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^P");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " POST ");

	wprintw(command_window, "   Logger: ");

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^I");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Info   ");

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^E");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Errors");

	wrefresh(command_window);

	cancel_showing = 0;
}

void draw_cancel() {
	wclear(command_window);

	wattron(command_window, A_BOLD);
	wprintw(command_window, "^C");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Cancel");

	wrefresh(command_window);

	cancel_showing = 1;
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
