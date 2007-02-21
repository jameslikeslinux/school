#include <ncurses.h>

void create_windows();
void draw_input();
void draw_commands();
void draw_cancel();

static WINDOW *log_window, *progress_window, *command_window;

int main() {
	int logi;

	initscr();
	cbreak();
	create_windows();

	attron(A_BOLD);
	printw("HTTP Client");
	attroff(A_BOLD);

	refresh();

	draw_input();
	draw_commands();

	log_window = newwin(LINES - 6, COLS, 2, 0);

	for (logi = 0; logi < 100; logi++) {
		wattron(log_window, A_BOLD);
		wprintw(log_window, "12:34:56  ");
		wattroff(log_window, A_BOLD);
		wprintw(log_window, "This is a log message %d\n", logi);
		wrefresh(log_window);
	}

	while (mvwgetch(progress_window, 1, 5));

	endwin();

	return 0;
}

void create_windows() {
	log_window = newwin(LINES - 6, COLS, 2, 0);
	progress_window = newwin(2, COLS, LINES - 3, 0);
	command_window = newwin(1, COLS, LINES - 1, 0);
}

void draw_input() {
	int y, x;

	wclear(progress_window);
	
	wprintw(progress_window, "Enter a URL and choose a method.\n");
	wprintw(progress_window, "URL: ");

	wrefresh(progress_window);
}

void draw_commands() {
	wclear(command_window);

	wprintw(command_window, "Methods: ");
	
	wattron(command_window, A_BOLD | A_REVERSE);
	wprintw(command_window, "^G");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " GET ");
	wattroff(command_window, A_REVERSE);

	wprintw(command_window, "  ");

	wattron(command_window, A_BOLD | A_REVERSE);
	wprintw(command_window, "^H");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " HEAD ");
	wattroff(command_window, A_REVERSE);

	wprintw(command_window, "  ");

	wattron(command_window, A_BOLD | A_REVERSE);
	wprintw(command_window, "^P");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " POST ");
	wattroff(command_window, A_REVERSE);

	wprintw(command_window, "   Logger: ");

	wattron(command_window, A_BOLD | A_REVERSE);
	wprintw(command_window, "^I");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Info ");
	wattroff(command_window, A_REVERSE);

	wprintw(command_window, "  ");

	wattron(command_window, A_BOLD | A_REVERSE);
	wprintw(command_window, "^E");
	wattroff(command_window, A_BOLD);
	wprintw(command_window, " Errors ");
	wattroff(command_window, A_REVERSE);

	wrefresh(command_window);
}

void draw_cancel() {
	wclear(command_window);
}
