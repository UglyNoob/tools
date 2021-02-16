#include<GLFW/glfw3.h>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<list>
#include<thread>
#include<cstring>
#ifdef __WIN32
#include<windows.h>
#else
#include<unistd.h>
#endif
using namespace std;
struct color{
	GLclampf r,g,b,a;
};
struct position{
	int x,y;
};
struct node{
	GLFWwindow* window;
	position pos;
};

const int UP=0;
const int DOWN=1;
const int LEFT=2;
const int RIGHT=3;
const int CREATE_WINDOW=4;
const int DESTROY_WINDOW=5;
const int WINDOW_WIDTH=60;
const int WINDOW_HEIGHT=60;
const int step_per_sec=3;
const char *WINDOW_NAME="The Snake Game";
const color APPLE_COLOR={1.f,0.25f,0.25f,1.f};
const color HEAD_COLOR={0.25f,0.25f,255,1.f};
const color BODY_COLOR={0.5f,0.5f,0.9f,0.9f};

void error_callback(int,const char*);
void window_close_callback(GLFWwindow*);
void key_callback(GLFWwindow*,int,int,int,int);

void game_main();

void initialize_window(GLFWwindow*);
void delay(int);
position random_pos(GLFWwindow*);
void refresh(GLFWwindow*);
void log_error(const char*);
void end(int);

list<node> snake;
node apple;
GLFWmonitor *monitor;
GLFWvidmode *vidmode;

int map_width,map_height,direction,direction_pre,score;
bool time_to_exit=false,snake_forward=false;


int main(int argc, char **argv){
	glfwSetErrorCallback(error_callback);
	glfwInit();
	monitor=glfwGetPrimaryMonitor();
	vidmode=(GLFWvidmode*)glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_DECORATED,GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
	map_width=vidmode->width/WINDOW_WIDTH;
	map_height=vidmode->height/WINDOW_HEIGHT;
	glfwWindowHint(GLFW_FOCUSED,GLFW_TRUE);
	apple.window=glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_NAME,NULL,NULL);
	glfwWindowHint(GLFW_FOCUSED,GLFW_FALSE);
	initialize_window(apple.window);
	glClearColor(APPLE_COLOR.r,APPLE_COLOR.g,APPLE_COLOR.b,APPLE_COLOR.a);
	srand((unsigned int)time(NULL));
	apple.pos=random_pos(apple.window);
	
	GLFWwindow *head_window=glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_NAME,NULL,NULL);
	initialize_window(head_window);
	node head={head_window,random_pos(head_window)};
	snake.push_back(head);
	direction=rand()%4;
	direction_pre=direction;
	
	thread game_thread(game_main);
	
	while(!time_to_exit){
		glfwMakeContextCurrent(apple.window);
		refresh(apple.window);
		for(list<node>::iterator iter=snake.begin();iter!=snake.end();iter++){
			glfwMakeContextCurrent(iter->window);
			if(iter==snake.begin()){
				glClearColor(HEAD_COLOR.r,HEAD_COLOR.g,HEAD_COLOR.b,HEAD_COLOR.a);
			}else{
				glClearColor(BODY_COLOR.r,BODY_COLOR.g,BODY_COLOR.b,BODY_COLOR.a);
			}
			refresh(iter->window);
			
		}
		glfwPollEvents();
		if(snake_forward){
			snake_forward=false;
			position pos,old_pos=snake.front().pos;
			switch(direction){
				case UP:
					pos={old_pos.x,old_pos.y-1};
					break;
				case DOWN:
					pos={old_pos.x,old_pos.y+1};
					break;
				case LEFT:
					pos={old_pos.x-1,old_pos.y};
					break;
				case RIGHT:
					pos={old_pos.x+1,old_pos.y};
					break;
			}
			if(pos.x<0)pos.x=map_width-1;
			if(pos.x>=map_width)pos.x=0;
			if(pos.y<0)pos.y=map_height-1;
			if(pos.y>=map_height)pos.y=0;
			GLFWwindow *win=glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_NAME,NULL,NULL);
			initialize_window(win);
			glfwSetWindowPos(win,pos.x*WINDOW_WIDTH,pos.y*WINDOW_HEIGHT);
			node head={win,pos};
			snake.push_front(head);
			if(pos.x==apple.pos.x&&pos.y==apple.pos.y){
				apple.pos=random_pos(apple.window);
				score++;
                printf("Apple eaten detected, now score %d\n",score);
			}else{
				node tail=snake.back();
				glfwDestroyWindow(tail.window);
				snake.pop_back();
			}
			list<node>::iterator iter=snake.begin();
			for(iter++;iter!=snake.end();iter++){
				if(pos.x==iter->pos.x&&pos.y==iter->pos.y){
					printf("\033[91m\033[1mYou died with score %d\n\033[0m",score);
					end(0);
				}
			}
			delay(1);
		}
	}
	end(0);
	return 0;
}
void game_main(){
	delay(1000/step_per_sec);
	while(true){
		delay(1000/step_per_sec);
		switch(direction_pre){
			case UP:
				if(direction!=DOWN){
					direction=direction_pre;
				}
				break;
			case DOWN:
				if(direction!=UP){
					direction=direction_pre;
				}
				break;
			case LEFT:
				if(direction!=RIGHT){
					direction=direction_pre;
				}
				break;
			case RIGHT:
				if(direction!=LEFT){
					direction=direction_pre;
				}
				break;
		}
		snake_forward=true;
	}
}


void initialize_window(GLFWwindow *window){
	glfwMakeContextCurrent(window);
	glfwSetWindowCloseCallback(window,window_close_callback);
	glfwSetKeyCallback(window,key_callback);
	glfwSwapInterval(0);
}
void error_callback(int code,const char *description){
	log_error(description);
	end(code);
}
void window_close_callback(GLFWwindow *window){
	log_error("Now exiting...");
	time_to_exit=true;
}
void key_callback(GLFWwindow* window,int key,int scancode,int action,int mods){
	if(action==GLFW_PRESS){
		switch(key){
			case GLFW_KEY_UP:
				direction_pre=UP;
				break;
			case GLFW_KEY_DOWN:
				direction_pre=DOWN;
				break;
			case GLFW_KEY_LEFT:
				direction_pre=LEFT;
				break;
			case GLFW_KEY_RIGHT:
				direction_pre=RIGHT;
				break;
		}
	}
}

void delay(int ms){
#ifdef __WIN32
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
}
position random_pos(GLFWwindow *window){
	position p={rand()%map_width,rand()%map_height};
	glfwSetWindowPos(window,WINDOW_WIDTH*p.x,WINDOW_HEIGHT*p.y);
	return p;
}
void refresh(GLFWwindow *window){
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}
void log_error(const char *info){
	fprintf(stderr,"\033[91m\033[1m%s\n\033[0m",info);
}
void end(int code){
	glfwTerminate();
	exit(code);
}
