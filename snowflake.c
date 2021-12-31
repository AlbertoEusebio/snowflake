#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <SDL2/SDL.h>

#include "my_functions.c"

#define WIDTH 600
#define HEIGHT 600

#define SIDE 400


//Liked list node
typedef struct l{
	double x;
	double y;
	struct l *next_point;
}Point;


//Functions' caller
void Draw(SDL_Renderer *renderer, Point *points);
void Algorithm(SDL_Renderer *renderer, Point *points, int level);
void drawShape(SDL_Renderer *renderer, Point *points);
void addPoint(Point *add_point, Point *points, int pos, _Bool is_closed);
void rotate(Point origin, Point *p, double angle);
void printList(Point *points);

int main(){
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Event event;	
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("Snowflake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	_Bool ON = 1;
	_Bool pressed = 0;

	int level = 0;

	Point a = {WIDTH/2, 	     HEIGHT/2 - ((SIDE/2) * sqrt(3))/2, NULL};
	Point b = {WIDTH/2+(SIDE/2), HEIGHT/2 + ((SIDE/2) * sqrt(3))/2, NULL};
	Point c = {WIDTH/2-(SIDE/2), HEIGHT/2 + ((SIDE/2) * sqrt(3))/2, &a};

	addPoint(&b, &a,  1, 1);
	addPoint(&c, &a,  2, 1);

	//GAME LOOP
	while(ON){
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT){
			ON = 0;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && !pressed){
			pressed = 1;
			fprintf(stdout, "level: %d\n", level);
			if(level < 7){
				level++;
				Algorithm(renderer, &a, level);
			}else{
				fprintf(stdout, "MEMORY CONSUMING\n");
			}
		}
		if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && pressed){
			pressed  = 0;
		}

		Draw(renderer, &a);
	}
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void Draw(SDL_Renderer *renderer, Point *points){

	clock_t start = clock();
	//sets the background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	
	//Draw the shape
	SDL_SetRenderDrawColor(renderer, 0,0,255, SDL_ALPHA_OPAQUE);
	drawShape(renderer, points);

	SDL_RenderPresent(renderer);
	clock_t end = clock();
	if((end-start) < (6/100)){
		delay((int)((6/100)-(end-start)));
		printf("%ld",((6/100)-(end-start)));
	}
}

void Algorithm(SDL_Renderer *renderer, Point *points, int level){
	//the vertex of the triangle is the starting point
	Point *p = points;
	int i = 0;

	while(p->next_point != NULL && (p->next_point != points->next_point || i == 0)){	
		//calculating the middle point
		double xp = p->x + (p->next_point->x - p->x)/3;
		double yp = p->y + (p->next_point->y - p->y)/3;

		//calculating the previous point and the following point on the triangle
		double xm = xp + (p->next_point->x - p->x)/3;
		double xf = xm;

		double ym = yp + (p->next_point->y - p->y)/3;
		double yf = ym;

		//creating th points those points to the list
		Point *previous = malloc(sizeof(Point));
		previous->x = xp;
		previous->y = yp;
		
		Point *following = malloc(sizeof(Point));
		following->x = xf;
		following->y = yf;

		Point *middle = malloc(sizeof(Point));
		middle->x = xf;
		middle->y = yf;

		//updating the coordinates of the new vertex
		rotate(*previous, middle, -M_PI/3);

		//I can't increment the pointer in advance nor after, otherwise it'll cause an infinite loop
		Point *buff = p->next_point;	

		//adding the points to the list
		addPoint(previous, p, 0, 1);
		addPoint(middle,   p, 1, 1);
		addPoint(following, p, 2, 1);

		//in this way the pointer skips the just added points 
		p = buff;
		i++;		
	}

}

void drawShape(SDL_Renderer *renderer, Point *points){
	Point *p = points;
	while(p->next_point != points){
		SDL_RenderDrawLine(renderer, p->x, p->y, p->next_point->x, p->next_point->y);
		p = p->next_point;
	}
	SDL_RenderDrawLine(renderer, p->x, p->y, p->next_point->x, p->next_point->y);
}

void addPoint(Point *add_point, Point *points, int pos, _Bool is_closed){
	
	Point *p = points;
	Point *buff;


	if(pos < 0){
		add_point->next_point = points;
		points = add_point;
	}else{
		for(int i = 0; i<pos; i++){
			if(p->next_point != NULL && p->next_point != points){
				p = p->next_point;
			}else{
				break;
			}
		}
		
		buff = p->next_point;

		p->next_point = add_point;
		add_point->next_point = buff;
	}

	if(is_closed){
		while(p->next_point != NULL && p->next_point != points){
			p = p->next_point;
		}
		p->next_point = points;
	}

}

void rotate(Point origin, Point *p, double angle){
	double lenght = sqrt((origin.x-p->x)*(origin.x-p->x) + (origin.y-p->y)*(origin.y-p->y));
	double initialAngle = acos((p->x-origin.x)/lenght);
	if((p->y-origin.y)<0){
		initialAngle *= -1;
	}
	p->y = origin.y+lenght*sin((initialAngle+angle));
	p->x = origin.x+lenght*cos((initialAngle+angle));
}

void printList(Point *points){
	Point *p = points;
	int pos = 0;
	printf("[%d, %d]-->", (int)p->x, (int)p->y);
	while(p->next_point != points && p->next_point != NULL){
		p = p->next_point;
		printf("[%d, %d]-->", (int)p->x, (int)p->y);
		pos++;
	}
	if(p->next_point == NULL){
		printf("NULL\n");
	}
	else{
		p = p->next_point;
		printf("[%d, %d]\n", (int)p->x, (int)p->y);
	}
}
