#include <sys/time.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <iterator>


#include "epub_renderer.h"
#include "eink_renderer_driver.h"

#include "ifusb_host.h"
#include "eink.h"

#include <ncurses.h>

using namespace std;
using namespace Epub;

int main(int argc, char *argv[]) {

	Size page_size = Size{600, 800};

	if (argc < 2) {
		printf("Usage: eink_reader filename.epub\n");
		exit(1);
	}

	if (!ifusb_init()) {

		ifusb_close();
		exit(1);
	}

	eink_init();

	EinkRendererDriver drv;

	EpubRenderer epub( &drv);
	epub.resize(page_size);

	if (!epub.open(argv[1])) {
		fprintf(stderr, "eink_reader: Error: %s not an ePub file\n", argv[1]);
		exit(1);
	}
eink_clear();
/*
epub.go_to_next_page();
epub.go_to_next_page();
epub.go_to_next_page();
epub.go_to_next_page();
epub.go_to_next_page();
*/

				epub.repaint();
	sleep(1);

	int ch;

	/* Curses Initialisations */

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

	printw("eink_reader - Press q to Exit\n");

	while ((ch = getch()) != 'q')
	{
		switch (ch)
		{
		case KEY_UP: printw("\nUp Arrow");
			epub.increase_text_size();
			epub.repaint();
			break;
		case KEY_DOWN: printw("\nDown Arrow");
			epub.decrease_text_size();
			epub.repaint();
			break;
		case KEY_LEFT: printw("\nLeft Arrow");
			epub.go_to_previous_page();
			epub.repaint();

			break;
		case KEY_RIGHT: printw("\nRight Arrow");
			epub.go_to_next_page();
			epub.repaint();
			break;
		default:
			break;
		}
	}

	refresh();
	getch();
	endwin();


	epub.close();
	eink_exit();
	ifusb_close();
	exit(0);
}