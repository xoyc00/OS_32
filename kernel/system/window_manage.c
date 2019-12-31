#include <kernel/system/window_manage.h>

#include <kernel/driver/vga/vga.h>

#include <stdlib.h>
#include <assert.h>

window_t** m_windows = 0;
window_t* drag_window = 0;
int drag_offset_x, drag_offset_y;
int window_count = 0;

extern int mouse_x;
extern int mouse_y;

unsigned char* wallpaper = 0;
int wallpaper_w = 0, wallpaper_h = 0, wallpaper_bpp = 0;

window_t** sorted_by_depth() {
	window_t** out = malloc(sizeof(window_t*) * window_count);

	for (int i = 0; i < window_count; i++) {
		out[m_windows[i]->depth] = m_windows[i];
	}

	return out;
}

void bring_to_foreground(window_t w) {
	for (int i = 0; i < window_count; i++) {
		if (m_windows[i]->depth > w.depth) {
			m_windows[i]->depth--;
		}
		if (w.wid == m_windows[i]->wid) {
			m_windows[i]->depth = window_count - 1;
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
	wallpaper = vga_load_bitmap_to_buffer("/USER/WALLPA~1.BMP", &wallpaper_w, &wallpaper_h, &wallpaper_bpp);
}

void wm_draw() {
	vga_clearscreen(0, 0, 0);

	if (wallpaper != 0) {
		vga_blit_buffer(wallpaper, 0, 0, 1280, 1024, wallpaper_bpp);
	}

	vga_terminal_draw();

	{		// Draw the task bar
		vga_drawrect(0, 1024-32, 1280, 32, 32, 32, 255, 0);
		vga_drawrect(0, 1024-32, 64, 32, 0, 156, 0, 1);
		vga_drawstr("Start", 4, 1024-24, 255, 255, 255);

		for (int i = 0; i < window_count; i++) {				// Draw the window titles in the task bar
			int col = 48;
			if (m_windows[i]->depth == window_count - 1) col = 64;
			vga_drawrect(64 + (96*i), 1024-32, 96, 32, col, col, 255, 0);
			if (strlen(m_windows[i]->title) <= 8) {
				vga_drawstr(m_windows[i]->title, 64 + (96*i) + 2, 1024-24, 255, 255, 255);
			} else {
				vga_drawstr("...", 64 + (96*i) + 2, 1024-24, 255, 255, 255);
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
		m_windows[0] = w;
		window_count++;
	} else {
		window_count++;
		window_t** new_m_windows = malloc(window_count * sizeof(window_t));
		memcpy(new_m_windows, m_windows, (window_count-1)*sizeof(window_t));
		new_m_windows[window_count-1] = w;
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

window_t* window_create(size_t w, size_t h, char* title) {
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
	out->bg_g = 200;
	out->bg_b = 200;
	out->framebuffer = malloc(out->w*out->h*4U);
	out->title = title;
	return out;
}

void wm_clearwindow(window_t* w) {
	for (int x = 0; x < w->w; x++) {
		for (int y = 0; y < w->h; y++) {
			wm_putpixel(w, x, y, w->bg_r, w->bg_g, w->bg_b, 255);
		}
	}
}

void wm_putpixel(window_t* w, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	unsigned int bytes_per_line = w->w * 4;

	if ((y * bytes_per_line) + (x * 4) + 3 > w->w*w->h*4U) {
		return;
	}

	w->framebuffer[(y * bytes_per_line) + (x * 4) + 2] = r;
	w->framebuffer[(y * bytes_per_line) + (x * 4) + 1] = g;
	w->framebuffer[(y * bytes_per_line) + (x * 4) + 0] = b;
	w->framebuffer[(y * bytes_per_line) + (x * 4) + 3] = a;
}
