/**
 * SER 431
 * https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#pragma once

#include <math.h>
#include <GL/glut.h>

int window_width = 800, window_height = 800;
const float window_ratio = 1.0;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
float x_angle = 0.0;
float y_angle = 0.0;

// camera
float camera_x = 0.0;
float camera_y = 300.0;
float camera_z = 320.0;

float camera_viewing_x = 0.0;
float camera_viewing_y = 301.0;
float camera_viewing_z = 0.0;

float total_moving_angle = 0.0;
