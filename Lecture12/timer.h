#include <time.h>
#include <windows.h> 
#include<iostream>

using namespace std;

static float averageFrameTime = 0.0001f;
static float lastFiveTimes[5] = { 0.0001f,0.0001f,0.0001f,0.0001f,0.0001f };
static unsigned char dx = 0;
float	startOfFrame, endOfFrame, frequency;
LONGLONG start_clock;

//	Initialise timer
void  init_frame_timer() {
	LONGLONG rate;
	frequency = 1.0f / (float)CLOCKS_PER_SEC;

	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&rate)) return;
	cout << rate << endl;
	if (!rate) return;
	frequency = 1.0f / (float)rate;
	if (!QueryPerformanceCounter((LARGE_INTEGER*)&start_clock)) return;
	cout << start_clock << "**" << endl;
}

// Get the current time
float get_current_time() {
	LONGLONG end_clock;
	QueryPerformanceCounter((LARGE_INTEGER*)&end_clock);
	return  (end_clock - start_clock) * frequency;
}

//	calculate frame time
float calculate_frame_time() {
	++dx;
	dx %= 0x05;
	do {
		endOfFrame = get_current_time();
	} while (endOfFrame == startOfFrame);
	float f = endOfFrame - startOfFrame;
	lastFiveTimes[dx] = f;
	startOfFrame = endOfFrame;
	float *p = lastFiveTimes;
	float *e = p + 5;
	averageFrameTime = 0.0f;
	for (; p != e; ++p) {
		averageFrameTime += *p;
	}
	averageFrameTime *= 0.2f;
	return averageFrameTime;
}
