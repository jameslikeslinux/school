#include <ncurses.h>
#include <form.h>

void create_windows();
void create_input_form();
void draw_stdscr();
void draw_input();
void draw_commands();
void draw_cancel();

static int log_lines, log_cols, input_lines, input_cols, command_lines, command_cols;
static WINDOW *log_window, *input_window, *command_window;
static FORM *url_form;
static FIELD *url_field[2];

int main() {
	int logi, ch;

	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);

	create_windows();
	create_input_form();
	draw_stdscr();
	draw_input();
	draw_commands();

	form_driver(url_form, REQ_FIRST_FIELD);
	refresh();

	while ((ch = getch()) != '')
		switch (ch) {
			case KEY_LEFT:
				form_driver(url_form, REQ_LEFT_CHAR);
				break;

			case KEY_RIGHT:
				form_driver(url_form, REQ_RIGHT_CHAR);
				break;

			case KEY_BACKSPACE:
				form_driver(url_form, REQ_LEFT_CHAR);
				form_driver(url_form, REQ_DEL_CHAR);
				break;

			case KEY_DC:
				form_driver(url_form, REQ_DEL_CHAR);
				break;

			default:
				form_driver(url_form, ch);
				break;
		}

	unpost_form(url_form);
	free_form(url_form);
	free_field(url_field[0]);

	endwin();

	return 0;
}

void create_windows() {
	log_lines = LINES - 6;
	log_cols = COLS;
	log_window = newwin(log_lines, log_cols, 2, 0);

	input_lines = 2;
	input_cols = COLS;
	input_window = newwin(input_lines, input_cols, LINES - 3, 0);

	command_lines = 1;
	command_cols = COLS;
	command_window = newwin(command_lines, command_cols, LINES - 1, 0);
}

void create_input_form() {
	url_field[0] = new_field(1, input_cols - 7, LINES - 2, 5, 0, 0);
	url_field[1] = NULL;

	set_field_back(url_field[0], A_UNDERLINE);
	field_opts_off(url_field[0], O_AUTOSKIP);
	field_opts_off(url_field[0], O_STATIC);
	set_max_field(url_field[0], 255);

	url_form = new_form(url_field);
	post_form(url_form);

	refresh();
}

void draw_stdscr() {
	attron(A_BOLD);
	mvprintw(0, 0, "HTTP Client");
	attroff(A_BOLD);

	refresh();
}

void draw_input() {
	int y, x;

	wclear(input_window);
	
	wprintw(input_window, "Enter a URL and choose a method.\n");
	wprintw(input_window, "URL: ");

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
	wprintw(command_window, " Errors ");

	wrefresh(command_window);
}

void draw_cancel() {
	wclear(command_window);
}
