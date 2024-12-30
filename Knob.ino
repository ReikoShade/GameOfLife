#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 9;
int numberOfHorizontalDisplays = 4; // количество матриц по-горизонтали (в этом примере мы исп 1 модуль)
int numberOfVerticalDisplays = 1; // количество матриц по-вертикали

#define ROWS 32
#define COLUMNS 8


void init_game(int field[ROWS][COLUMNS]);
int tick_game(int field[ROWS][COLUMNS], int *speed);
void set_init_state(int field[ROWS][COLUMNS]);
void field_init(int field[ROWS][COLUMNS], int choice);
void output(int field[ROWS][COLUMNS]);
int neighbor_count(int field[ROWS][COLUMNS], int ceil_row, int ceil_column);
int refresh_field(int field[ROWS][COLUMNS]);
int game_of_life(int width, int height);
int clear_field();
 

 
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
 
void setup() {
    matrix.setIntensity(4); // Яркость от 0 до 15
    Serial.begin(9600);
    matrix.setRotation(0, 1);
    matrix.setRotation(1, 1);
    matrix.setRotation(2, 1);
    matrix.setRotation(3, 1);

}



void loop() {
    clear_field();
    //  matrix.fillScreen(LOW); // Очищаем экран перед рисованием

    // // Рисуем линию от (0, 0) до (31, 0) (32 пикселя)
    // matrix.drawLine(0, 0, 31, 0, HIGH); // Рисуем линию

    // matrix.write(); // Обновляем матрицу для отображения изменений
    // delay(1000); // Задержка для визуализации
    game_of_life(numberOfHorizontalDisplays * 8, numberOfVerticalDisplays * 8);
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MOVING 1
#define TRAIN 2
#define FILE_DATA 3



int game_of_life(int width, int height) {
    int field[ROWS][COLUMNS] = {0};

    int gameover = 0;
    int speed = 200;

    init_game(field);

    while (!gameover) {
        gameover = tick_game(field, &speed);
    }

    return 0;
}


void init_game(int field[ROWS][COLUMNS]) {
    set_init_state(field);
}

int tick_game(int field[ROWS][COLUMNS], int *speed) {
    int flag = 0;
    delay(*speed);
    clear_field();
    flag = refresh_field(field);
    
    return flag;
}

void set_init_state(int field[ROWS][COLUMNS]) {
    int choice = 4;
    field_init(field, choice);
}

void field_init(int field[ROWS][COLUMNS], int choice) {
    srand(time(NULL));
    int cells = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int column = 0; column < COLUMNS; column++) {
            field[row][column] = 0;
        }
    }

    switch (choice) {
        case MOVING:
            field[1][2] = 1;
            field[2][3] = 1;
            field[3][1] = 1;
            field[3][2] = 1;
            field[3][3] = 1;
            break;
        case TRAIN:
            field[1][2] = 1;
            field[1][5] = 1;
            field[2][6] = 1;
            field[3][6] = 1;
            field[4][6] = 1;
            field[4][5] = 1;
            field[4][4] = 1;
            field[4][3] = 1;
            break;
        case FILE_DATA:
            break;
        default:
            cells = rand() % 50 + 50;
            for (int i = 0; i < cells; i++) {
                int row = rand() % ROWS;
                int column = rand() % COLUMNS;
                field[row][column] = 1;
            }
            break;
    }

    output(field);
}

void output(int field[ROWS][COLUMNS]) {
  clear_field();
    for (int row = 0; row < ROWS; row++) {
        for (int column = 0; column < COLUMNS; column++) {
            if (field[row][column]) {
                matrix.drawPixel(row, column,  HIGH);
            } else {
                matrix.drawPixel(row, column, LOW);
            }

        }
    }
  matrix.write();

}

int neighbor_count(int field[ROWS][COLUMNS], int ceil_row, int ceil_column) {
    int count = 0;
    for (int row = -1; row <= 1; row++) {
        for (int column = -1; column <= 1; column++) {
            if (!(row == 0 && column == 0)) {
                int neighbor_row = (ceil_row + row + ROWS) % ROWS;
                int neighbor_column = (ceil_column + column + COLUMNS) % COLUMNS;
                if (field[neighbor_row][neighbor_column]) count++;
            }
        }
    }
    return count;
}

int refresh_field(int field[ROWS][COLUMNS]) {
    int new_field[ROWS][COLUMNS]; 

    int counter = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int column = 0; column < COLUMNS; column++) {
            int neighbors = neighbor_count(field, row, column);
            if (neighbors > 0) counter++;

            if (field[row][column]) {
                if (neighbors == 2 || neighbors == 3) {
                    new_field[row][column] = 1;
                } else {
                    new_field[row][column] = 0;
                }
            } else {
                if (neighbors == 3) {
                    new_field[row][column] = 1;
                } else {
                    new_field[row][column] = 0;
                }
            }
        }
    }

    for (int row = 0; row < ROWS; row++) {
      for (int column = 0; column < COLUMNS; column++) {
        field[row][column] = new_field[row][column];
      }
    }

    output(field);
    return counter ? 0 : 1;
}

int clear_field(){
  matrix.fillScreen(LOW);
  matrix.write();
  return 0;
}
