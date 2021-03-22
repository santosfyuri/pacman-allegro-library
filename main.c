#include <stdio.h>
#include <allegro.h>

int** load_scene(const char* file, int* line, int* column);

void draw_scene(BITMAP* buffer, int** scene, int line, int column);

void track_pacman(BITMAP* buffer, int** scene, int line, int column, int walk);

void chase_pacman(BITMAP* buffer, int** scene, int line, int column);

void free_scene(int** scene, int line);

void free_elements();

void fill_vector_aux();

/**
* Variables used from the allegro library
* for the construction of game elements.
*/
BITMAP* buffer;
BITMAP* pacman[3];
BITMAP* wall;
BITMAP* ghost_red_h[2];
BITMAP* ghost_red_u[2];
BITMAP* ghost_red_d[2];
BITMAP* ghost_green_h[2];
BITMAP* ghost_green_u[2];
BITMAP* ghost_green_d[2];
SAMPLE* eat;
SAMPLE* start;
FONT* text;

/**
* variables used to set initial values
*/
int dir_pacman = 4;
int tile = 16;
int pos_pac_x = 232, pos_pac_y = 472;
int pos_gr_x = 208 , pos_gr_y = 176;
int pos_gg_x = 208 , pos_gg_y = 176;
int counter = 0;
int score;
int dir_ghost;
int vector_aux[100][100];

/**
* Timers to control the frame rate of the game
*/
volatile long speed;
void speed_counter() {
    speed++;
}
END_OF_FUNCTION(speed_counter());

volatile long ms;
void ms_time(){
    ms++;
}
END_OF_FUNCTION(ms_time());

void start_game();

int main() {

    //Initialization
    start_game();
    LOCK_FUNCTION(speed_counter());
    LOCK_VARIABLE(speed);
    install_int_ex(speed_counter, BPS_TO_TIMER(8));

    LOCK_FUNCTION(ms_time());
    LOCK_VARIABLE(ms);
    install_int_ex(ms_time, MSEC_TO_TIMER(1));

    //Load all images
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    wall = load_bitmap("images/scene/wall.pcx", NULL);
    pacman[0] = load_bitmap("images/pacman/pacman_frame_1.pcx", NULL);
    pacman[1] = load_bitmap("images/pacman/pacman_frame_2.pcx", NULL);
    pacman[2] = load_bitmap("images/pacman/pacman_frame_3.pcx", NULL);
    ghost_red_h[0] = load_bitmap("images/ghost_red/moviment_horizontal/ghost_frame_1.pcx", NULL);
    ghost_red_h[1] = load_bitmap("images/ghost_red/moviment_horizontal/ghost_frame_2.pcx", NULL);
    ghost_red_u[0] = load_bitmap("images/ghost_red/moviment_vertical_up/ghost_frame_1.pcx", NULL);
    ghost_red_u[1] = load_bitmap("images/ghost_red/moviment_vertical_up/ghost_frame_2.pcx", NULL);
    ghost_red_d[0] = load_bitmap("images/ghost_red/moviment_vertical_down/ghost_frame_1.pcx", NULL);
    ghost_red_d[1] = load_bitmap("images/ghost_red/moviment_vertical_down/ghost_frame_2.pcx", NULL);
    ghost_green_h[0] = load_bitmap("images/ghost_green/moviment_horizontal/ghost_frame_1.pcx", NULL);
    ghost_green_h[1] = load_bitmap("images/ghost_green/moviment_horizontal/ghost_frame_2.pcx", NULL);
    ghost_green_u[0] = load_bitmap("images/ghost_green/moviment_vertical_up/ghost_frame_1.pcx", NULL);
    ghost_green_u[1] = load_bitmap("images/ghost_green/moviment_vertical_up/ghost_frame2.pcx", NULL);
    ghost_green_d[0] = load_bitmap("images/ghost_green/moviment_vertical_down/ghost_frame1.pcx", NULL);
    ghost_green_d[1] = load_bitmap("images/ghost_green/moviment_vertical_down/ghost_frame2.pcx", NULL);

    //Load custom text font
    text = load_font("fonts/pacman_pixel_font.pcx", NULL, NULL);

    //Load wav samples
    eat = load_sample("sounds/chomp.wav");
    start = load_sample("sounds/begin.wav");

    int lines, columns;
    int** scene = load_scene("scenes/standard.txt", &lines, &columns);
    int frames = 0;
    int max_frames = 3;
    int change = 100;

    int frames_ghost = 0;
    int max_ghosts = 2;

    int lose = 1;

    fill_vector_aux();

    play_sample(start, 150, 128, 1000, FALSE);

    //Main loop
    while (!key[KEY_ESC]){

        while (speed > 0 && counter != 0 && lose != 0) {

            track_pacman(buffer, scene, lines, columns, dir_pacman);
            chase_pacman(buffer, scene, lines, columns);

            //Pacman controll direction sprite
            if (key[KEY_RIGHT] && scene[(pos_pac_y - 8) / 16][((pos_pac_x - 8) + 16) / 16] != 1) {
                dir_pacman = 1;
            } else if (key[KEY_LEFT] && scene[(pos_pac_y - 8) / 16][((pos_pac_x - 8) - 16) / 16] != 1) {
                dir_pacman = 0;
            } else if (key[KEY_UP] && scene[((pos_pac_y - 8) - 16) / 16][(pos_pac_x - 8) / 16] != 1) {
                dir_pacman = 2;
            } else if (key[KEY_DOWN]&& scene[((pos_pac_y - 8) + 16) / 16][(pos_pac_x - 8) / 16] != 1) {
                dir_pacman = 3;
            } else {
                dir_pacman = dir_pacman;
            }

            if (dir_pacman == 0 && scene[(pos_pac_y - 8) / 16][((pos_pac_x - 8) - 16) / 16] != 1) {
                vector_aux[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] = 0;
                pos_pac_x -= 16;
                if (pos_pac_x < 0) {
                    pos_pac_x = SCREEN_W - 8;
                }
            }
            else if (dir_pacman == 1 && scene[(pos_pac_y - 8) / 16][((pos_pac_x - 8) + 16) / 16] != 1) {
                vector_aux[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] = 1;
                pos_pac_x += 16;
                if (pos_pac_x > SCREEN_W - 8) {
                    pos_pac_x = 8;
                }
            }
            else if (dir_pacman == 2 && scene[((pos_pac_y - 8) - 16) / 16][(pos_pac_x - 8) / 16] != 1) {
                vector_aux[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] = 2;
                pos_pac_y -= 16;
            }
            else if (dir_pacman == 3 && scene[((pos_pac_y - 8) + 16) / 16][(pos_pac_x - 8) / 16] != 1) {
                vector_aux[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] = 3;
                pos_pac_y += 16;
            }

            if (pos_gr_x == pos_pac_x - 8) {
                if (pos_gr_y == pos_pac_y - 8) {
                    lose = 0;
                }
            }
            if (pos_gg_x == pos_pac_x - 8) {
                if (pos_gg_y == pos_pac_y - 8) {
                    lose = 0;
                }
            }

            frames = (ms / change) % max_frames;
            frames_ghost = (ms/ change) % max_ghosts;

            speed--;
        }

        //Draw pacman sprites
        if (dir_pacman != 0 && dir_pacman != 1 && dir_pacman != 2 && dir_pacman != 3) {
            draw_sprite(buffer, pacman[2], pos_pac_x - 8, pos_pac_y - 8);
        } else if (dir_pacman == 0) {
            rotate_sprite(buffer, pacman[frames], pos_pac_x - 8, pos_pac_y - 8, ftofix (128));
        } else if (dir_pacman == 1) {
            rotate_sprite(buffer, pacman[frames], pos_pac_x - 8, pos_pac_y - 8, ftofix (0));
        } else if (dir_pacman == 2) {
            rotate_sprite(buffer, pacman[frames], pos_pac_x - 8, pos_pac_y - 8, ftofix (190));
        } else if (dir_pacman == 3) {
            rotate_sprite(buffer, pacman[frames], pos_pac_x - 8, pos_pac_y - 8, ftofix (-190));
        }

        draw_scene(buffer, scene, lines, columns);

        if (dir_ghost != 0 && dir_ghost != 1 && dir_ghost != 2 && dir_ghost != 3) {
            draw_sprite(buffer, ghost_red_h[frames_ghost], pos_gr_x, pos_gr_y);
        } else if (dir_ghost == 0) {
            draw_sprite_ex(buffer, ghost_red_h[frames_ghost], pos_gr_x, pos_gr_y, DRAW_SPRITE_NORMAL, DRAW_SPRITE_H_FLIP);
        } else if (dir_ghost == 1) {
            draw_sprite(buffer, ghost_red_h[frames_ghost], pos_gr_x, pos_gr_y);
        } else if (dir_ghost == 2) {
            draw_sprite(buffer, ghost_red_u[frames_ghost], pos_gr_x, pos_gr_y);
        } else if (dir_ghost == 3) {
            draw_sprite(buffer, ghost_red_d[frames_ghost], pos_gr_x, pos_gr_y);
        }

        if(counter == 0) {
            textprintf_ex(buffer, text, 186, 224, makecol(255, 255, 255), -1, "YOU WIN!");
        }
        if(lose == 0) {
            textprintf_ex(buffer, text, 180, 224, makecol(255, 255, 255), -1, "YOU LOSE!");
        }

        draw_sprite(buffer, ghost_green_h[frames_ghost], pos_gg_x, pos_gg_y);
        textprintf_ex(buffer, text, 0, 496, makecol(255, 255, 255), -1, "SCORE: %d", score);
        textprintf_ex(buffer, text, 348, 496, makecol(255, 255, 255), -1, "santosfyuri");
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        clear(buffer);
    }
    free_scene(scene, lines);
    free_elements();
    allegro_exit();

    return 0;
}
END_OF_MAIN();

void start_game() {
    allegro_init();
    install_timer();
    install_keyboard();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 448, 512, 0, 0);
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    set_window_title("Pacman - Allegro Library");
}

int** load_scene(const char* file, int* lines, int* columns){
    int** vector_2;
    int i, j;
    FILE* scene_file = fopen(file, "r");

    if (scene_file == NULL) {
        allegro_message("Error opening scene file!");
        exit(-1);
    }

    if (scene_file != NULL) {
        fscanf(scene_file, "%d %d", lines, columns);

        vector_2 = (int**) malloc((*lines) * sizeof(int*));

        for (i = 0; i < *lines; i++) {
            vector_2[i] = (int*) malloc((*columns) * sizeof(int));
        }

        for (i = 0; i < *lines; ++i) {
            for (j = 0; j < *columns; ++j) {
               fscanf(scene_file, "%d", &vector_2[i][j]);
               if (vector_2[i][j] == 0 || vector_2[i][j] == 3) {
                    counter++;
               }
            }
        }
        fclose(scene_file);
    }

    return vector_2;
}

void draw_scene(BITMAP* buffer, int** scene, int lines, int columns) {
    int i, j;

    for (i = 0; i < lines; i++) {
        for (j = 0; j < columns; j++) {
            if (scene[i][j] == 1) {
                draw_sprite(buffer, wall, j * tile, i * tile);
            } else if (scene[i][j] == 0) {
                circlefill(buffer, (j *16) + 8,(i * 16) + 8, 2, makecol(255, 255, 255));
            } else if (scene[i][j] == 3) {
                circlefill(buffer, (j *16) + 8,(i * 16) + 8, 4, makecol(255, 255, 255));
            }
        }
        if(scene[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] == 0){
            play_sample(eat, 125, 128, 1000, FALSE);
            scene[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] = 2;
            counter--;
            score += 10;
        }
        if (scene[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] == 3){
            play_sample(eat, 125, 128, 1500, FALSE);
            scene[(pos_pac_y - 8) / 16][(pos_pac_x - 8) / 16] = 2;
            counter--;
            score += 50;
        }
    }
}

void track_pacman(BITMAP* buffer, int** scene, int lines, int columns, int walk) {

    if (vector_aux[pos_gg_y / 16][pos_gg_x / 16] == 4) {
        switch (walk) {
        case 0:
            if (scene[pos_gg_y / 16][(pos_gg_x + 16) / 16] != 1) {
                if (pos_gg_x > SCREEN_W - 17) {
                    pos_gg_x = -16;
                }
                pos_gg_x += 16;
            } else if (scene[(pos_gg_y - 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y -=16;
            } else if (scene[(pos_gg_y + 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y += 16;
            } else if (scene[pos_gg_y / 16][(pos_gg_x - 16) / 16] != 1) {
                if (pos_gg_x < 16) {
                    pos_gg_x = SCREEN_W;
                }
                pos_gg_x -= 16;
            }
            break;
        case 1:
            if (scene[pos_gg_y / 16][(pos_gg_x - 16) / 16] != 1) {
                if (pos_gg_x < 16) {
                    pos_gg_x = SCREEN_W;
                }
                pos_gg_x -= 16;
            } else if (scene[(pos_gg_y + 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y += 16;
            } else if (scene[(pos_gg_y - 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y -= 16;
            } else if (scene[pos_gg_y / 16][(pos_gg_x + 16) / 16] != 1) {
                if (pos_gg_x > SCREEN_W - 17) {
                    pos_gg_x = -16;
                }
                pos_gg_x += 16;
            }
            break;
        case 2:
            if (scene[(pos_gg_y + 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y += 16;
            } else if (scene[pos_gg_y / 16][(pos_gg_x - 16) / 16] != 1) {
                if (pos_gg_x < 16) {
                    pos_gg_x = SCREEN_W;
                }
                pos_gg_x -= 16;
            } else if (scene[pos_gg_y / 16][(pos_gg_x + 16) / 16] != 1) {
                if (pos_gg_x > SCREEN_W - 17) {
                    pos_gg_x = -16;
                }
                pos_gg_x += 16;
            } else if (scene[(pos_gg_y - 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y -= 16;
            }
            break;
        case 3:
            if (scene[(pos_gg_y - 16) / 16][pos_gg_x / 16] != 1) {
                pos_gg_y -= 16;
            } else if (scene[pos_gg_y / 16][(pos_gg_x + 16) / 16] != 1) {
                if (pos_gg_x > SCREEN_W - 17) {
                    pos_gg_x = -16;
                }
                pos_gg_x += 16;
            } else if (scene[pos_gg_y / 16][(pos_gg_x - 16) / 16] != 1) {
                if (pos_gg_x < 16) {
                    pos_gg_x = SCREEN_W;
                }
                pos_gg_x -= 16;
            } else if (scene[(pos_gg_y + 16) / 16][pos_gg_x / 16] != 1){
                pos_gg_y += 16;
            }
            break;
        }
    } else {
        switch (vector_aux[pos_gg_y / 16][pos_gg_x / 16]){
            case 0:
                if (pos_gg_x < 16) {
                    pos_gg_x = SCREEN_W;
                }
                pos_gg_x -= 16;
                break;
            case 1:
                if (pos_gg_x > SCREEN_W - 17) {
                    pos_gg_x = -16;
                }
                pos_gg_x += 16;
                break;
            case 2:
                pos_gg_y -= 16;
                break;
            case 3:
                pos_gg_y += 16;
                break;
        }
    }
}

void chase_pacman(BITMAP* buffer, int** scene, int lines, int columns) {
    int a, b;
	int down = 0, left = 0, right = 0, up = 0;
	int up_down = 0, left_right = 0, t, v;
	int side_a = 0, side_b = 0, more = 0;

	dir_ghost = 8;
    t = pos_gr_y / 16;
    v = pos_gr_x / 16;
    a = (pos_pac_y - 8) / 16;
    b = (pos_pac_x - 8) / 16;

    if (t < a) {
        up_down = 1;
    }
    else if (t > a) {
        up_down = 2;
    } else {
        up_down = 0;
    }

    if (v < b) {
        left_right = 1;
    }
    else if (v > b) {
        left_right = 2;
    } else {
        left_right = 0;
    }

    if (up_down == 1) {
        if (scene[t + 1][v] != 1 && t != a) {
            side_b++;
            t++;
            down++;
            printf("\nDirection: Down");
            if (more == 0) {
                dir_ghost = 3;
                more++;
            }
        }
    }
    if (up_down == 2) {
        if (scene[t - 1][v] != 1 && t != a) {
            side_b++;
            t--;
            up++;
            printf("\nDirection: Up");
            if (more == 0) {
                dir_ghost = 2;
                more++;
            }
        }
    }
    if (left_right == 1) {
        if(scene[t][v + 1] != 1 && v != b) {
            side_a++;
            v++;
            right++;
            printf("\nDirection: Right");
            if (more == 0) {
                dir_ghost = 1;
                more++;
            }
        }
    }
    if (left_right == 2) {
        if (scene[t][v - 1] != 1 && v != b) {
            side_a++;
            v--;
            left++;
            printf("\nDirection: Left");
            if (more == 0) {
                dir_ghost = 0;
                more++;
            }
        }
    }

	switch (dir_ghost) {
        case 0:
            pos_gr_x -= 16;
        break;
        case 1:
            pos_gr_x += 16;
        break;
        case 2:
            pos_gr_y -= 16;
        break;
        case 3:
            pos_gr_y += 16;
        break;
	}
}

void free_scene(int** scene, int lines) {
    int i;

    for (i = 0; i < lines; i++){
            free(scene[i]);
    }
    free(scene);
}

void free_elements() {
    destroy_bitmap(buffer);
    destroy_bitmap(wall);
    destroy_bitmap(pacman[0]);
    destroy_bitmap(pacman[1]);
    destroy_bitmap(pacman[2]);
    destroy_bitmap(ghost_red_d[0]);
    destroy_bitmap(ghost_red_d[1]);
    destroy_bitmap(ghost_red_h[0]);
    destroy_bitmap(ghost_red_h[1]);
    destroy_bitmap(ghost_red_u[0]);
    destroy_bitmap(ghost_red_u[1]);
    destroy_bitmap(ghost_green_d[0]);
    destroy_bitmap(ghost_green_d[1]);
    destroy_bitmap(ghost_green_h[0]);
    destroy_bitmap(ghost_green_h[1]);
    destroy_bitmap(ghost_green_u[0]);
    destroy_bitmap(ghost_green_u[1]);
    destroy_font(text);
    destroy_sample(start);
    destroy_sample(eat);
}

void fill_vector_aux() {
    int i, j;

    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
            vector_aux[i][j] = 4;
        }
    }
}
