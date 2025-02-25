#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

#define WINDOW_WIDTH 600	// Game window width
#define WINDOW_HEIGHT 600	// Game window height


// Structure: Coordinates
typedef struct coordinate
{
	int x;
	int y;
}Pos;

// Attributes of snakes
typedef struct snakeAttributes
{
	Pos location;	// Coordinates
	int direction;  // Snake direction

	// Point to the next node
	struct snakeAttributes* next;

}Snake;


// Create a game window
void createWindow();

// Initialization of snake attributes
void initSnake(Snake** snake_head);

// Draw the snake
void displaySnake(Snake* snake_head);

// Movement
void movement(Snake* snake_head);

// Getting Player Input
void playControl(Snake* snake_head);

// Initialization of food
void initFood(Snake* snake_head);

// Draw the food
void displayFood();

// Enumeration: Set the direction of the snake
enum snakeDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};


Snake* addSnakeLength(Snake* snakeHead, int last_x, int last_y)
{
	Snake* newBody = (Snake*)calloc(1, sizeof(Snake));
	if (newBody == NULL)
	{
		perror("calloc");
		return NULL;
	}

	newBody->location.x = last_x;
	newBody->location.y = last_y;

	newBody->next = NULL;

	if (snakeHead == NULL)
	{
		return newBody;
	}

	Snake* temp = snakeHead;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newBody;

	return snakeHead;
}


Snake* deleteSnakeLength(Snake* snakeHead) 
{
	if (snakeHead == NULL)
	{
		return NULL;
	}

	Snake* temp_body = NULL;
	temp_body = snakeHead->next;

	Snake* separate = NULL;
	separate = snakeHead;

	while (temp_body != NULL)
	{
		if (snakeHead->location.x == temp_body->location.x && snakeHead->location.y == temp_body->location.y)
		{
			// Disconnect the linked list
			separate->next = NULL;

			// At this time, only temp_body knows the address of the linked list that is disconnected and to be released.
			// temp_body cannot be moved easily
			Snake* free_body = NULL;
			Snake* move_next_body = NULL;
			free_body = temp_body;
			while (free_body != NULL)
			{
				move_next_body = free_body->next;
				free(free_body);
				free_body = move_next_body;
			}

			break;
		}

		separate = temp_body;
		temp_body = temp_body->next;
	}

	return snakeHead;
}


// Attributes of snakes
struct foodAttributes
{
	Pos location;
	int flag;

}Food;


int main()
{
	Snake* head = NULL;
	createWindow();
	
	initSnake(&head);
	initFood(head);

	while (1)
	{
		displaySnake(head);

		movement(head);
		playControl(head);
		displayFood();

		// Check for collision with walls
		if (head->location.x < 0 || head->location.x >= WINDOW_WIDTH || head->location.y < 0 || head->location.y >= WINDOW_HEIGHT)
		{
			break;
		}

		// The snake's head comes into contact with the food, and the snake's body increases by one node.
		if (head->location.x == Food.location.x && head->location.y == Food.location.y)
		{
			Snake* temp = head;
			Food.flag = 1;

			addSnakeLength(head, temp->next->next->location.x, temp->next->next->location.y);

			if (Food.flag == 1)
			{
				// reset the food
				Food.flag = 0;
				initFood(head);
			}
		}
		
		head = deleteSnakeLength(head);

		Sleep(100);
	}

	// Free up memory space
	Snake* free_entire_snake = NULL;
	free_entire_snake = head;
	Snake* temp_free = NULL;
	while (free_entire_snake != NULL)
	{
		// temp_free points to free_entire_snake
		temp_free = free_entire_snake;

		// update free_entire_snake
		free_entire_snake = free_entire_snake->next;

		// Free up memory space
		free(temp_free);
	}

	getchar();
	// Close the windows
	closegraph();

	return 0;
}

void createWindow()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	setbkcolor(RGB(199, 255, 132));

	cleardevice();

	return;
}

void initSnake(Snake** snake_head)
{
	*snake_head = NULL;
	
	// Snake head
	*snake_head = addSnakeLength(*snake_head, 20, 0);
	
	// Snake body
	*snake_head = addSnakeLength(*snake_head, 10, 0);
	*snake_head = addSnakeLength(*snake_head, 0, 0);

	// Snake direction
	(*snake_head)->direction = RIGHT;

	return;
}

void displaySnake(Snake* snake_head)
{
	cleardevice();

	setfillcolor(BLUE);

	Snake* head_temp = snake_head;
	while (head_temp != NULL)
	{
		if (head_temp->location.x >= 0 && head_temp->location.y >= 0 && head_temp->location.x + 10 <= WINDOW_WIDTH && head_temp->location.y + 10 <= WINDOW_HEIGHT)
		{
			fillrectangle(head_temp->location.x, head_temp->location.y, head_temp->location.x + 10, head_temp->location.y + 10);
		}
		
		// update head_temp
		head_temp = head_temp->next;
	}

	return;
}


void movement(Snake* snake_head)
{
	if (snake_head == NULL)
	{
		return;
	}

	Snake* temp_head = snake_head;
	Snake* temp_body = NULL;
	
	// Save the coordinates of the current snake head
	int prev_x = temp_head->location.x;
	int prev_y = temp_head->location.y;

	temp_body = temp_head->next;
	while (temp_body != NULL)
	{
		// Save the coordinates of the current snake body
		int temp_x = temp_body->location.x;
		int temp_y = temp_body->location.y;

		temp_body->location.x = prev_x;
		temp_body->location.y = prev_y;

		// update prev_x and prev_y
		prev_x = temp_x;
		prev_y = temp_y;

		temp_body = temp_body->next;
	}

	switch (temp_head->direction)
	{
	case UP:
		temp_head->location.y -= 10;
		break;

	case DOWN:
		temp_head->location.y += 10;
		break;

	case LEFT:
		temp_head->location.x -= 10;
		break;

	case RIGHT:
		temp_head->location.x += 10;
		break;

	default:
		break;
	}

	return;
}


void playControl(Snake* snake_head)
{
	Snake* temp_head = snake_head;
	if (_kbhit())
	{
		char ch = _getch();

		switch (ch)
		{
		case 'W':
		case'w':
		case 72:
			//temp_head->direction = UP;
			if (temp_head->direction != DOWN)
			{
				temp_head->direction = UP;
			}
			break;

		case 'S':
		case 's':
		case 80:
			if (temp_head->direction != UP)
			{
				temp_head->direction = DOWN;
			}
			break;

		case 'A':
		case 'a':
		case 75:
			if (temp_head->direction != RIGHT)
			{
				temp_head->direction = LEFT;
			}
			break;

		case 'D':
		case 'd':
		case 77:
			if (temp_head->direction != LEFT)
			{
				temp_head->direction = RIGHT;
			}
			break;

		default:
			break;

		}
	}

	return;
}

void initFood(Snake* snake_head)
{
	Snake* temp_head = snake_head;

	srand((unsigned)time(NULL));

	do
	{
		temp_head = snake_head;

		Food.location.x = (rand() % (WINDOW_WIDTH / 10)) * 10;
		Food.location.y = (rand() % (WINDOW_HEIGHT / 10)) * 10;

		while (temp_head != NULL)
		{
			if (temp_head->location.x == Food.location.x && temp_head->location.y == Food.location.y)
			{
				break;
			}

			temp_head = temp_head->next;
		}
	} while (temp_head!= NULL);	

	return;
}

void displayFood()
{
	setfillcolor(RED);

	fillrectangle(Food.location.x, Food.location.y, Food.location.x + 10, Food.location.y + 10);

	return;
}