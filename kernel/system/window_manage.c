#include <kernel/system/window_manage.h>

#include <kernel/driver/vga/vga.h>

#include <stdlib.h>
#include <assert.h>

window_t* m_windows = 0;
window_t* drag_window = 0;
int drag_offset_x, drag_offset_y;
int window_count = 0;

extern int mouse_x;
extern int mouse_y;

window_t** sorted_by_depth() {
	window_t** out = malloc(sizeof(window_t*) * window_count);

	for (int i = 0; i < window_count; i++) {
		out[m_windows[i].depth] = &m_windows[i];
	}

	return out;
}

void bring_to_foreground(window_t w) {
	for (int i = 0; i < window_count; i++) {
		if (m_windows[i].depth > w.depth) {
			m_windows[i].depth--;
		}
		if (w.wid == m_windows[i].wid) {
			m_windows[i].depth = window_count - 1;
		}
	}
}

void process_left_mouse(int on, int x, int y) {
	if (on == 0) {
		if (drag_window != 0) {
			drag_window = 0;
		} else {
			// TODO: send mouse released message to the window's task
		}
	} else {
		window_t** sorted_windows = sorted_by_depth();
		for (int i = window_count - 1; i >= 0; i--) {
			if (x >= sorted_windows[i]->x && x <= sorted_windows[i]->x + sorted_windows[i]->w) {
				if (y >= sorted_windows[i]->y && y <= sorted_windows[i]->y + sorted_windows[i]->tb_h) {
					drag_window = sorted_windows[i];
					drag_offset_x = sorted_windows[i]->x - x;
					drag_offset_y = sorted_windows[i]->y - y;
					bring_to_foreground(*sorted_windows[i]);
					break;
				}
			}
			if (x >= sorted_windows[i]->x && x <= sorted_windows[i]->x + sorted_windows[i]->w) {
				if (y >= sorted_windows[i]->y + sorted_windows[i]->tb_h && y <= sorted_windows[i]->h + sorted_windows[i]->tb_h + sorted_windows[i]->y) {
					bring_to_foreground(*sorted_windows[i]);
					// TODO: send window clicked message tot he window's task
					break;
				}
			}
			if (x >= 64 + (96*i) && x <= 64 + 96 + (96*i)) {
				if (y >= 1024-32 && y <= 1024) {
					bring_to_foreground(*sorted_windows[i]);
				}
			}
 		}
		free(sorted_windows);
	}
}

void wm_mouse_button_down(int button, int x, int y) {
	switch(button) {
	case 0:
		process_left_mouse(1, x, y);
		break;
	default:
		break;
	}
}

void wm_mouse_button_up(int button, int x, int y) {
	switch(button) {
	case 0:
		process_left_mouse(0, x, y);
		break;
	default:
		break;
	}
}

void wm_init() {
	window_t* term = window_create(512, 384, "Terminal");
	term->bg_r = 32;
	term->bg_g = 32;
	term->bg_b = 32;
	term->tid = 0;
	window_register(term);

	window_t* t = window_create(128, 128, "Test");
	t->x = 900;
	t->tid = 0;
	window_register(t);

	window_t* r = window_create(128, 128, "A Test Window");
	r->y = 700;
	r->tid = 0;
	window_register(r);
}

void wm_draw() {
	{		// Draw the task bar
		vga_drawrect(0, 1024-32, 1279, 32, 32, 32, 255, 0);
		vga_drawrect(0, 1024-32, 64, 32, 0, 156, 0, 1);
		vga_drawstr("Start", 4, 1024-24, 255, 255, 255);

		for (int i = 0; i < window_count; i++) {				// Draw the window titles in the task bar
			int col = 48;
			if (m_windows[i].depth == window_count - 1) col = 64;
			vga_drawrect(64 + (96*i), 1023-32, 96, 32, col, col, 255, 0);
			if (strlen(m_windows[i].title) <= 8) {
				vga_drawstr(m_windows[i].title, 64 + (96*i) + 2, 1023-24, 255, 255, 255);
			} else {
				vga_drawstr("...", 64 + (96*i) + 2, 1023-24, 255, 255, 255);
			}
		}
	}

	if (drag_window != 0) {
		drag_window->x = mouse_x + drag_offset_x;
		drag_window->y = mouse_y + drag_offset_y;
	}

	window_t** sorted_windows = sorted_by_depth();
	for (int i = 0; i < window_count; i++) {
		vga_drawwindow(*sorted_windows[i]);
	}
	free(sorted_windows);
}

void window_register(window_t* w) {
	w->depth = window_count;
	w->wid = window_count;

	if (window_count == 0) {
		m_windows = malloc(sizeof(window_t));
		m_windows[0] = *w;
		window_count++;
	} else {
		window_count++;
		window_t* new_m_windows = malloc(window_count * sizeof(window_t));
		memcpy(new_m_windows, m_windows, (window_count-1)*sizeof(window_t));
		new_m_windows[window_count-1] = *w;
		free(m_windows);
		m_windows = new_m_windows;
	}
}

void window_deregister(window_t* w) {
	if (window_count == 0) {
		return;
	} else {
		
	}
}

window_t* window_create(int w, int h, char* title) {
	window_t* out = malloc(sizeof(window_t));
	assert(out != 0);
	out->x = 100;
	out->y = 100;
	out->w = w;
	out->h = h;
	out->tb_h = 18;
	out->border_radius = 1;
	out->rounded = 1;
	out->bg_r = 255;
	out->bg_r = 200;
	out->bg_r = 200;
	out->framebuffer = malloc(w*h*4);
	out->title = title;
	return out;
}