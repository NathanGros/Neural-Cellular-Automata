#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



//global values

#define bg_color (Color) {0, 0, 0, 255}
#define dead_color (Color) {20, 20, 20, 255}
#define alive_color (Color) {0, 0, 255, 255}
#define complementary_color (Color) {0, 255, 200, 255}



//types

typedef struct {
	int width;
	int height;
	float *tab; //values
} Board;

Board* init_board(int n, int m) {
	Board *b = malloc(sizeof(Board));
	b->width = n;
	b->height = m;
	b->tab = malloc(n*m * (sizeof(float)));
	return b;
}

void free_board(Board *b) {
	free(b->tab);
	free(b);
}



//functions

void window_init(Color bg) {
	InitWindow(0, 0, "Neural Cellular Automata Maker");
	ClearBackground(bg);
	SetTargetFPS(100);
}

int min(int x, int y) {
	if (x < y) x;
	else y;
}

Color compute_color(Color alive_col, Color dead_col, float p) {
	float r1 = alive_col.r;
	float g1 = alive_col.g;
	float b1 = alive_col.b;
	float r2 = dead_col.r;
	float g2 = dead_col.g;
	float b2 = dead_col.b;
	float newred = sqrt(r1 * r1 * p + r2 * r2 * (1. - p));                                                             
	float newgreen = sqrt(g1 * g1 * p + g2 * g2 * (1. - p));
	float newblue = sqrt(b1 * b1 * p + b2 * b2 * (1. - p));
	return (Color) {((int) newred), ((int) newgreen), ((int) newblue), 255};
}

void draw(Board *b, bool display_grid, int cell_size) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int grid_w = b->width;
	int grid_h = b->height;
	int start_x = h / 2 - cell_size * (grid_h) / 2;
	int start_y = w * 5/8 - cell_size * (grid_w) / 2;

	DrawRectangle(start_y-cell_size, start_x-cell_size, w, h+cell_size, bg_color);
	for (int i=0; i < grid_h; i++) {
		for (int j=0; j < grid_w; j++) {
			int cell_px = cell_size;
			float cell_val = b->tab[j * grid_h + i];
			if (display_grid) cell_px--;
			if (cell_val == 0) DrawRectangle(start_y + j * cell_size, start_x + i * cell_size, cell_px, cell_px, dead_color);
			else {
				Color cell_color = compute_color(alive_color, dead_color, cell_val);
				DrawRectangle(start_y + j * cell_size, start_x + i * cell_size, cell_px, cell_px, cell_color);
			}
		}
	}
}

float convolution_corners(Board *b, float *weights, int x, int y) {
	int grid_w = b->width;
	int grid_h = b->height;
	float sum;
	int ym = (y-1) * grid_h;
	int ye = y * grid_h;
	int yp = (y+1) * grid_h;
	int xm = (x-1);
	int xp = (x+1);
	if (x == 0)
		if (y == 0) sum =
			weights[0] * b->tab[(grid_w-1) * grid_h + (grid_h-1)] +
			weights[1] * b->tab[(grid_w-1) * grid_h] +
			weights[2] * b->tab[(grid_w-1) * grid_h + xp] +
			weights[3] * b->tab[(grid_h-1)] +
			weights[4] * b->tab[0] +
			weights[5] * b->tab[xp] +
			weights[6] * b->tab[yp + (grid_h-1)] +
			weights[7] * b->tab[yp] +
			weights[8] * b->tab[yp + xp];
		else sum =
			weights[0] * b->tab[ym + (grid_h-1)] +
			weights[1] * b->tab[ym] +
			weights[2] * b->tab[ym + xp] +
			weights[3] * b->tab[ye + (grid_h-1)] +
			weights[4] * b->tab[ye] +
			weights[5] * b->tab[ye + xp] +
			weights[6] * b->tab[(grid_h-1)] +
			weights[7] * b->tab[0] +
			weights[8] * b->tab[xp];
	else
		if (y == 0) sum =
			weights[0] * b->tab[(grid_w-1) * grid_h + (grid_h-2)] +
			weights[1] * b->tab[(grid_w-1) * grid_h + grid_h-1] +
			weights[2] * b->tab[(grid_w-1) * grid_h] +
			weights[3] * b->tab[xm] +
			weights[4] * b->tab[x] +
			weights[5] * b->tab[0] +
			weights[6] * b->tab[yp + xm] +
			weights[7] * b->tab[yp + x] +
			weights[8] * b->tab[yp];
		else sum =
			weights[0] * b->tab[ym + xm] +
			weights[1] * b->tab[ym + x] +
			weights[2] * b->tab[ym] +
			weights[3] * b->tab[ye + xm] +
			weights[4] * b->tab[ye + x] +
			weights[5] * b->tab[ye] +
			weights[6] * b->tab[xm] +
			weights[7] * b->tab[x] +
			weights[8] * b->tab[0];
	return sum;
}

float convolution_edges(Board *b, float *weights, int x, int y) {
	int grid_w = b->width;
	int grid_h = b->height;
	float sum;
	int ym = (y-1) * grid_h;
	int ye = y * grid_h;
	int yp = (y+1) * grid_h;
	int xm = (x-1);
	int xp = (x+1);
	if (x == 0) sum =
		weights[0] * b->tab[ym + (grid_h-1)] +
		weights[1] * b->tab[ym] +
		weights[2] * b->tab[ym + 1] +
		weights[3] * b->tab[ye + (grid_h-1)] +
		weights[4] * b->tab[ye] +
		weights[5] * b->tab[ye + 1] +
		weights[6] * b->tab[yp + (grid_h-1)] +
		weights[7] * b->tab[yp] +
		weights[8] * b->tab[yp + 1]
	;
	else if (x == grid_h-1) sum =
		weights[0] * b->tab[ym + xm] +
		weights[1] * b->tab[ym + x] +
		weights[2] * b->tab[ym] +
		weights[3] * b->tab[ye + xm] +
		weights[4] * b->tab[ye + x] +
		weights[5] * b->tab[ye] +
		weights[6] * b->tab[yp + xm] +
		weights[7] * b->tab[yp + x] +
		weights[8] * b->tab[yp]
	;
	else if (y == 0) sum =
		weights[0] * b->tab[(grid_w-1) * grid_h + xm] +
		weights[1] * b->tab[(grid_w-1) * grid_h + x] +
		weights[2] * b->tab[(grid_w-1) * grid_h + xp] +
		weights[3] * b->tab[xm] +
		weights[4] * b->tab[x] +
		weights[5] * b->tab[xp] +
		weights[6] * b->tab[yp + xm] +
		weights[7] * b->tab[yp + x] +
		weights[8] * b->tab[yp + xp]
	;
	else sum =
		weights[0] * b->tab[ym + xm] +
		weights[1] * b->tab[ym + x] +
		weights[2] * b->tab[ym + xp] +
		weights[3] * b->tab[ye + xm] +
		weights[4] * b->tab[ye + x] +
		weights[5] * b->tab[ye + xp] +
		weights[6] * b->tab[xm] +
		weights[7] * b->tab[x] +
		weights[8] * b->tab[xp]
	;
	return sum;
}

float convolution_middle(Board *b, float *weights, int x, int y) {
	int grid_w = b->width;
	int grid_h = b->height;
	int ym = (y-1) * grid_h;
	int ye = y * grid_h;
	int yp = (y+1) * grid_h;
	int xm = (x-1);
	int xp = (x+1);
	return
		weights[0] * b->tab[ym + xm] +
		weights[1] * b->tab[ym + x] +
		weights[2] * b->tab[ym + xp] +
		weights[3] * b->tab[ye + xm] +
		weights[4] * b->tab[ye + x] +
		weights[5] * b->tab[ye + xp] +
		weights[6] * b->tab[yp + xm] +
		weights[7] * b->tab[yp + x] +
		weights[8] * b->tab[yp + xp]
	;
}

void draw_convolution(float *weights, bool symmetry) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int display_size = w/6;
	int display_cell_size = display_size/3;
	int start_x = h/4 - display_size/2;
	int start_y = w/8 - display_size/2;
	for (int i=0; i < 3; i++) {
		for (int j=0; j < 3; j++) {
			int n = 3*i + j;
			//draw cell
			float p = weights[n];
			Color cell_color;
			if (p>0) cell_color = compute_color(alive_color, dead_color, p);
			else cell_color = compute_color(complementary_color, dead_color, -1. * p);
			int cell_start_x = start_x + i * display_cell_size;
			int cell_start_y = start_y + j * display_cell_size;
			DrawRectangle(cell_start_y, cell_start_x, display_cell_size, display_cell_size, cell_color);
			//draw weight
			if (!symmetry || (n == 0 || n == 1 || n == 4)) {
				char weight_string[5];
				float val = weights[n];
				int text_size = 30;
				if (val < 0.01 && val > -0.01)
					DrawText("0.00", cell_start_y + display_cell_size/6, cell_start_x + display_cell_size/6, text_size, dead_color);
				else {
					gcvt(val, 3, weight_string);
					DrawText(weight_string, cell_start_y + display_cell_size/6, cell_start_x + display_cell_size/6, text_size, dead_color);
	 			}
	 		}
	  	}
	} 
}

void draw_symmetry(bool symmetry, int sym_button_height) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int display_size = w/6;
	int start_x = h/4 - display_size/2;
	int start_y = w/8 - display_size/2;
	DrawRectangle(start_y, start_x + display_size + 20, display_size, sym_button_height, dead_color);
	DrawRectangle(start_y + 10, start_x + display_size + 30, sym_button_height - 20, sym_button_height - 20, WHITE);
	if (!symmetry) DrawRectangle(start_y + 15, start_x + display_size + 35, sym_button_height - 30, sym_button_height - 30, dead_color);
	DrawText("Symmetry", start_y + 50, start_x + display_size + 25, 40, WHITE);
}

float activation(float c) { //return value between 0. and 1. for the new cell
	//float res = -1. / pow(2., (0.6 * pow(c, 2.))) + 1.;
	float res = -1. / (0.89 * pow(c, 2.) + 1.) + 1.;
	if (res > 1.) res = 1.;
	if (res < 0.) res = 0.;
	return res;
}

void draw_activation() {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int px_size = 4;
	int px_nb = w/6 / px_size;
	int display_size = px_nb * px_size;
	int start_x = h * 3/4 - display_size/2;
	int start_y = w/8 - display_size/2;
	DrawRectangle(start_y, start_x, display_size, display_size, dead_color);
	for (int i=0; i < px_nb; i++) {
		float p = 2. / px_nb * ((float) i) - 1.;
		Color axis_color;
		if (p>0) axis_color = compute_color(alive_color, dead_color, p);
		else axis_color = compute_color(complementary_color, dead_color, -1. * p);
		DrawRectangle(start_y + i*px_size, start_x + display_size, px_size, px_size, axis_color);
	}
	Vector2 point1 = (Vector2) {start_y, start_x + display_size - px_size - (int) ((activation(-9.) * px_nb)) * px_size};
	for (int i=1; i < px_nb; i++) {
		float res = activation(9. * (2. / px_nb * ((float) i) - 1.));
		int pos = (int) (res / (1. / px_nb));
		Vector2 point2 = (Vector2) {start_y + i*px_size, start_x + display_size - px_size - pos*px_size};
		DrawLineEx(point1, point2, (float) px_size, alive_color);
		point1 = point2;
	}
	int text_size = 30;
	DrawText("-9", start_y, start_x + display_size + 2*px_size, text_size, complementary_color);
	DrawText("0", start_y + (display_size - text_size/2) / 2, start_x + display_size + 2*px_size, text_size, WHITE);
	DrawText("1", start_y + (display_size - text_size/2) / 2, start_x - text_size, text_size, WHITE);
	DrawText("9", start_y + display_size - text_size/2, start_x + display_size + 2*px_size, text_size, alive_color);
}

void update(Board *b, float *weights) {
	//new updated board
	int grid_w = b->width;
	int grid_h = b->height;
	Board *b2 = init_board(grid_w, grid_h);

	//apply NCA rules

	//corners
	b2->tab[0] = activation(convolution_corners(b, weights, 0, 0));
	b2->tab[grid_h-1] = activation(convolution_corners(b, weights, grid_h-1, 0));
	b2->tab[(grid_w-1) * grid_h] = activation(convolution_corners(b, weights, 0, grid_w-1));
	b2->tab[grid_w * grid_h - 1] = activation(convolution_corners(b, weights, grid_h-1, grid_w-1));
	//up edge (i = 0)
	for (int j=1; j < grid_w-1; j++) {
		b2->tab[j * grid_h] = activation(convolution_edges(b, weights, 0, j));
	}
	//down edge (i = grid_h-1)
	for (int j=1; j < grid_w-1; j++) {
		b2->tab[(j+1) * grid_h - 1] = activation(convolution_edges(b, weights, grid_h - 1, j));
	}
	//left edge (j = 0)
	for (int i=1; i < grid_h-1; i++) {
		b2->tab[i] = activation(convolution_edges(b, weights, i, 0));
	}
	//right edge (j = grid_w-1)
	for (int i=1; i < grid_h-1; i++) {
		b2->tab[(grid_w - 1) * grid_h + i] = activation(convolution_edges(b, weights, i, grid_w - 1));
	}
	//middle
	for (int i=1; i < grid_h - 1; i++) {
		for (int j=1; j < grid_w - 1; j++) {
			b2->tab[j * grid_h + i] = activation(convolution_middle(b, weights, i, j));
		}
	}
	//paste values in main board and free the temp board
	free(b->tab);
	b->tab = b2->tab;
	free(b2);
}

void main() {
	//initialization
	srand(time(NULL));
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	window_init(bg_color);
	bool display_grid = false;
	bool running = true;
	bool half_frames = true;
	bool symmetry = true;
	int cell_size = 8;

	//create randomized board
	int grid_w_px = GetScreenWidth() * 3/4;
	int grid_h_px = GetScreenHeight();
	int grid_w = grid_w_px / cell_size;
	int grid_h = grid_h_px / cell_size;
	Board *b = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h; i++) {
		for (int j=0; j < grid_w; j++) {
			b->tab[j*grid_h + i] = (float) (rand() % 11) / 10.;
		}
	}

	//create neighbors ponderation table
	float *weights = malloc(9*sizeof(float));
	weights[0] = 0.8;
	weights[1] = -0.85;
	weights[2] = 0.8;
	weights[3] = -0.85;
	weights[4] = -0.2;
	weights[5] = -0.85;
	weights[6] = 0.8;
	weights[7] = -0.85;
	weights[8] = 0.8;

	//pre drawing
	draw_convolution(weights, symmetry);
	draw_activation();
	draw_symmetry(symmetry, 50);

	//loop
	while (!WindowShouldClose()) {
		//resize
		if (IsKeyPressed(KEY_KP_ADD) && cell_size < 100) {
			cell_size++;
			int old_grid_h = grid_h;
			grid_w_px = GetScreenWidth() * 3/4;
			grid_h_px = GetScreenHeight();
			grid_w = grid_w_px / cell_size;
			grid_h = grid_h_px / cell_size;
			Board *b2 = init_board(grid_w, grid_h);
			for (int i=0; i < grid_h; i++) {
				for (int j=0; j < grid_w; j++) {
					b2->tab[j*grid_h + i] = b->tab[j*old_grid_h + i];
				}
			}
			free(b->tab);
			b->width = grid_w;
			b->height = grid_h;
			b->tab = b2->tab;
			free(b2);
			draw(b, display_grid, cell_size);
		}
		if (IsKeyPressed(KEY_KP_SUBTRACT) && cell_size > 1) {
			cell_size--;
			int old_grid_w = grid_w;
			int old_grid_h = grid_h;
			grid_w_px = GetScreenWidth() * 3/4;
			grid_h_px = GetScreenHeight();
			grid_w = grid_w_px / cell_size;
			grid_h = grid_h_px / cell_size;
			Board *b2 = init_board(grid_w, grid_h);
			for (int i=0; i < grid_h; i++) {
				for (int j=0; j < grid_w; j++) {
					if (i < old_grid_h && j < old_grid_w) b2->tab[j*grid_h + i] = b->tab[j*old_grid_h + i];
					else b2->tab[j*grid_h + i] = 0.;
				}
			}
			free(b->tab);
			b->width = grid_w;
			b->height = grid_h;
			b->tab = b2->tab;
			free(b2);
			draw(b, display_grid, cell_size);
		}
		if (IsWindowResized()) {
			int old_grid_w = grid_w;
			int old_grid_h = grid_h;
			grid_w_px = GetScreenWidth() * 3/4;
			grid_h_px = GetScreenHeight();
			grid_w = grid_w_px / cell_size;
			grid_h = grid_h_px / cell_size;
			Board *b2 = init_board(grid_w, grid_h);
			for (int i=0; i < grid_h; i++) {
				for (int j=0; j < grid_w; j++) {
					if (i < old_grid_h && j < old_grid_w) b2->tab[j*grid_h + i] = b->tab[j*old_grid_h + i];
					else b2->tab[j*grid_h + i] = 0.;
				}
			}
			free(b->tab);
			b->width = grid_w;
			b->height = grid_h;
			b->tab = b2->tab;
			free(b2);
			ClearBackground(bg_color);
			draw(b, display_grid, cell_size);
			draw_convolution(weights, symmetry);
			draw_activation();
			draw_symmetry(symmetry, 50);
		}

		//controls : pause - unpause
		if (IsKeyPressed(KEY_SPACE)) running = !running;

		//controls : draw only even frames
		if (IsKeyPressed(KEY_H)) half_frames = !half_frames;

		//controls : switch grid
		if (IsKeyPressed(KEY_G)) {display_grid = !display_grid; draw(b, display_grid, cell_size);}

		//controls : clear grid
		if (IsKeyPressed(KEY_C)) {
			for (int i=0; i < grid_h; i++) {
				for (int j=0; j < grid_w; j++) {
					b->tab[j*grid_h + i] = 0.;
				}
			}
			draw(b, display_grid, cell_size);
		}

		//controls : randomize grid
		if (IsKeyPressed(KEY_R)) {
			srand(time(NULL));
			for (int i=0; i < grid_h; i++) {
				for (int j=0; j < grid_w; j++) {
					b->tab[j*grid_h + i] = (float) (rand() % 11) / 10.;
				}
			}
			draw(b, display_grid, cell_size);
		}


		//controls : add cells
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int start_x = h / 2 - cell_size * (grid_h) / 2;
			int start_y = w * 5/8 - cell_size * (grid_w) / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - (start_y - w/3)) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / cell_size) + ((my - start_x) / cell_size)] = 1.;
				draw(b, display_grid, cell_size);
			}
		}

		//controls : remove cells
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int start_x = h / 2 - cell_size * (grid_h) / 2;
			int start_y = w * 5/8 - cell_size * (grid_w) / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - (start_y - w/3)) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / cell_size) + ((my - start_x) / cell_size)] = 0.;
				draw(b, display_grid, cell_size);
			}
		}
		
		//controls : add weights Or switch symmetry
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int mx = GetMouseX();
			int my = GetMouseY();
			int conv_display_size = w/6;
			int display_cell_size = conv_display_size/3;
			int conv_start_x = h/4 - conv_display_size/2;
			int conv_start_y = w/8 - conv_display_size/2;
			int sym_button_height = 50;
			//add weights
			if (mx > conv_start_y && my > conv_start_x && mx < conv_start_y + conv_display_size && my < conv_start_x + conv_display_size) {
				int relative_display_mx = my - conv_start_x;
				int relative_display_my = mx - conv_start_y;
				int i = relative_display_mx / display_cell_size;
				int j = relative_display_my / display_cell_size;
				int n = 3*i + j;
				bool is_left_side = (relative_display_my - j * display_cell_size) < (display_cell_size / 2);
				float delta;
				if (is_left_side) delta = 0.1;
				else delta = 0.01;
				if (symmetry) {
					if (n == 0) {
						weights[0] += delta;
						weights[2] = weights[0];
						weights[6] = weights[0];
						weights[8] = weights[0];
					}
					if (n == 1) {
						weights[1] += delta;
						weights[3] = weights[1];
						weights[5] = weights[1];
						weights[7] = weights[1];
					}
					if (n == 4) weights[4] += delta;
					for (int i=0; i<9; i++) if (weights[i] > 1.) weights[i] -= 2.;
				}
				else {
					weights[n] += delta;
					if (weights[n] > 1.) weights[n] -= 2.;
				}
				draw_convolution(weights, symmetry);
			}
			//switch symmetry
			else if (mx > conv_start_y && my > conv_start_x + conv_display_size + 20 && mx < conv_start_y + conv_display_size && my < conv_start_x + conv_display_size + 20 + sym_button_height) {
				symmetry = !symmetry;
				weights[2] = weights[0];
				weights[6] = weights[0];
				weights[8] = weights[0];
				weights[3] = weights[1];
				weights[5] = weights[1];
				weights[7] = weights[1];
				draw_convolution(weights, symmetry);
				draw_symmetry(symmetry, sym_button_height);
			}
		}

		//controls : remove weights
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int mx = GetMouseX();
			int my = GetMouseY();
			int conv_display_size = w/6;
			int display_cell_size = conv_display_size/3;
			int conv_start_x = h/4 - conv_display_size/2;
			int conv_start_y = w/8 - conv_display_size/2;
			if (mx > conv_start_y && my > conv_start_x && mx < conv_start_y + conv_display_size && my < conv_start_x + conv_display_size) {
				int relative_display_mx = my - conv_start_x;
				int relative_display_my = mx - conv_start_y;
				int i = relative_display_mx / display_cell_size;
				int j = relative_display_my / display_cell_size;
				int n = 3*i + j;
				bool is_left_side = (relative_display_my - j * display_cell_size) < (display_cell_size / 2);
				float delta;
				if (is_left_side) delta = 0.1;
				else delta = 0.01;
				if (symmetry) {
					if (n == 0) {
						weights[0] -= delta;
						weights[2] = weights[0];
						weights[6] = weights[0];
						weights[8] = weights[0];
					}
					if (n == 1) {
						weights[1] -= delta;
						weights[3] = weights[1];
						weights[5] = weights[1];
						weights[7] = weights[1];
					}
					if (n == 4) weights[4] -= delta;
					for (int i=0; i<9; i++) if (weights[i] < -1.) weights[i] += 2.;
				}
				else {
					weights[n] -= delta;
					if (weights[n] < -1.) weights[n] += 2.;
				}
				draw_convolution(weights, symmetry);
			}
		}

		//update board
		if (running) {
			if (half_frames) {
				update(b, weights);
				update(b, weights);
				update(b, weights);
			}
			update(b, weights);
		}

		//draw grid
		if (running) draw(b, display_grid, cell_size);

		//one step
		if (IsKeyPressed(KEY_S)) {
			update(b, weights);
			draw(b, display_grid, cell_size);
		}

		//refresh all
		if (IsKeyPressed(KEY_ENTER)) {
			ClearBackground(bg_color);
			draw(b, display_grid, cell_size);
			draw_activation();
			draw_convolution(weights, symmetry);
			draw_symmetry(symmetry, 50);
		}

		BeginDrawing();
		EndDrawing();
	}

	//de-initialization
	free_board(b);
}
