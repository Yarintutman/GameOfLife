#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include <conio.h>
#include<time.h>
#include<windows.h>
#define BUFFSIZE 256

const char ALIVE[] = "\u2B1C", DEAD[] = "\u2B1B"; //How living cells and dead cells are represented on the game board
const char FILE_ALIVE = '+', FILE_DEAD = '-'; //How living and dead cells are represented in the game board creation file
const double TIME_BETWEEN_STAGES = 500; //The number of milliseconds between each state of the cells in the board
const char FILE_DIRECTORY[] = "C:\\networks\\GameOfLife"; //Location of the game board creation file
static int sizeOfBoard;

char* getFileDirectory(){
    int lengthOfDirectory = strlen(FILE_DIRECTORY);
    char fileDirectory[lengthOfDirectory];
    for (int i = 0; i < lengthOfDirectory; i++)
    {
        fileDirectory[i] = FILE_DIRECTORY[i];
    }
    strcat(fileDirectory, "\\GameBoard.txt\0");
    char *file = fileDirectory;
    printf("%s", file);
    return file;
}

void getSizeOfBoard(){
    int num;
    char *buffer;
    printf("Enter the board size: ");
    int result = scanf("%d", &num);
    while (result == 0 || num <= 0)
    {
        if (result == 0)
        {
            scanf("%s", &buffer);
        }
        printf("\nInvalid size...use a number greater than 0\nEnter the board size: ");
        result = scanf("%d", &num);
    }
    sizeOfBoard = num;
}

void resetBoard(bool board[sizeOfBoard][sizeOfBoard]){
    for (int i = 0; i < sizeOfBoard; i++)
    {
        for (int j = 0; j < sizeOfBoard; j++)
        {
            board[i][j] = false;
        }
        
    }
}

void printBoard(bool board[sizeOfBoard][sizeOfBoard]){
    printf("\n");
    for (int i = 0; i < sizeOfBoard; i++)
    {
        for (int j = 0; j < sizeOfBoard; j++)
        {
            bool alive = board[i][j];

            if (alive){
                printf("%s ", ALIVE);
            }
            else{
                printf("%s ", DEAD);
            }
        }
        printf("\n");
    }
}

void randomBoardCreation(bool board[sizeOfBoard][sizeOfBoard], int x_coordinate, int y_coordinate){
    board[x_coordinate][y_coordinate] = true;
    int temp_x, temp_y;
    for (int i = -1; i <= 1; i++)
    {
        temp_x = x_coordinate + i;
        
        if(temp_x < 0 || temp_x >= sizeOfBoard)
        {
            continue;
        }

        for (int j = -1; j <= 1; j++)
        {
            temp_y = y_coordinate + j;
            
            if(temp_y < 0 || temp_y >= sizeOfBoard)
            {
                continue;
            }

            if (!board[temp_x][temp_y])
            {
                int randomNumber = rand() % 4;

                if (randomNumber == 0)
                {
                    randomBoardCreation(board, temp_x, temp_y);
                }
            }
            
        }
    }
}

void createRandomBoard(bool board[sizeOfBoard][sizeOfBoard]){
    int coordinate = sizeOfBoard/2;
    resetBoard(board);
    srand(time(0));
    randomBoardCreation(board, coordinate, coordinate);
}

void createFile(){
    char *fileDirectory = getFileDirectory();
    char file[sizeof(fileDirectory)/8];
    int index = 0;
    do
    {
        file[index] = *fileDirectory;
        fileDirectory++;
        index++;
    } while (*fileDirectory != '\0');
    FILE *gameBoard = fopen(file, "w+");
    for (int i = 0; i < sizeOfBoard; i++)
    {
        for (int j = 0; j < sizeOfBoard; j++)
        {
            fputs("- ", gameBoard);
        }
        fputs("\n", gameBoard);
    }
    fclose(gameBoard);
}

int setBoardTOFile(bool board[sizeOfBoard][sizeOfBoard]){
    FILE *gameBoard = fopen(getFileDirectory(), "r");

    if (gameBoard == NULL)
    {
        printf("Invalid input... Generating random board\n");
        createRandomBoard(board);
        return 0;
    }

    char character, overFlow;

    for (int i = 0; i < sizeOfBoard && character != EOF; i++)
    {
        character = fgetc(gameBoard);
        for (int j = 0; j < sizeOfBoard && character != EOF; j++)
        {
            if (character == FILE_ALIVE)
            {
                board[i][j] = true;
            }
            else if (character == FILE_DEAD)
            {
                board[i][j] = false;
            }
            else
            {
                fclose(gameBoard);
                printf("Invalid input... Generating random board\n");
                createRandomBoard(board);
                return 0;
            }
            overFlow = fgetc(gameBoard);
            character = fgetc(gameBoard);
        }
    }
    fclose(gameBoard);
}

void createGameBoard(bool board[sizeOfBoard][sizeOfBoard])
{
    char buffer[BUFFSIZE];
    char makingBoard[4];
    fgets(buffer, BUFFSIZE, stdin);
    while(true)
    {
        printf("Would you like to make your own game board? (yes/no)\n");
        fgets(buffer, BUFFSIZE, stdin);
        memcpy(makingBoard, buffer, 3);
        if (strncmp(makingBoard, "yes", 3) == 0) 
        {
            createFile();
            printf("Go to the file \"GameBoard.txt\" in the directory: %s and replace the dead cells(-) that you want to change to living cells(+), press any key when you are done.\n", getFileDirectory());
            getch(); 
            setBoardTOFile(board);
            break;
        }
        else if(strncmp(makingBoard, "no", 2) == 0)
        {
            createRandomBoard(board);
            break;
        }
        else
        {
            printf("Invalid input...\n");
        }
    }
}

int amountOflivingCellsAround(bool board[sizeOfBoard][sizeOfBoard], int x_coordinate, int y_coordinate){
    int counter = 0;
    int temp_x, temp_y;
    for (int i = -1; i <= 1; i++)
    {
        temp_x = x_coordinate + i;
        
        if(temp_x < 0 || temp_x >= sizeOfBoard)
        {
            continue;
        }

        for (int j = -1; j <= 1; j++)
        {
            temp_y = y_coordinate + j;
            
            if(temp_y < 0 || temp_y >= sizeOfBoard)
            {
                continue;
            }

            if (temp_x != x_coordinate || temp_y != y_coordinate)
            {
                if(board[temp_x][temp_y])
                {
                    counter++;
                }
            }
        }
    }
    return counter;
}

void updateCells(bool board[sizeOfBoard][sizeOfBoard], bool helpBoard[sizeOfBoard][sizeOfBoard]){
    int livingCellsAround;
    for (int x = 0; x <= sizeOfBoard; x++)
    {
        for (int y = 0; y <= sizeOfBoard; y++)
        {
            livingCellsAround = amountOflivingCellsAround(board, x, y);

            if (board[x][y])
            {
                if (livingCellsAround == 2 || livingCellsAround == 3)
                {
                    helpBoard[x][y] = true;
                }   
            }
            else if (livingCellsAround == 3)
            {
                helpBoard[x][y] = true;
            }
        }
    }
}

bool compareBoard(bool board[sizeOfBoard][sizeOfBoard], bool board2[sizeOfBoard][sizeOfBoard]){
    bool sameValue = true;
    for (int i = 0; i < sizeOfBoard && sameValue; i++)
    {
        for (int j = 0; j < sizeOfBoard && sameValue; j++)
        {
            sameValue = board[i][j] == board2[i][j];
        }
    }
    return sameValue;
}

void copyBoard(bool copyBoard[sizeOfBoard][sizeOfBoard], bool board[sizeOfBoard][sizeOfBoard]){
    for (int i = 0; i < sizeOfBoard; i++)
    {
        for (int j = 0; j < sizeOfBoard; j++)
        {
            copyBoard[i][j] = (board[i][j]);
        }
    }
}

void startGameLoop(bool board[sizeOfBoard][sizeOfBoard]){
    bool helpBoard[sizeOfBoard][sizeOfBoard];
    int livingCellsAround;
    resetBoard(helpBoard);
    printBoard(board);
    while (true)
    {
        updateCells(board, helpBoard);
        Sleep(TIME_BETWEEN_STAGES);
        if (compareBoard(board, helpBoard))
        {
            break;
        }
        copyBoard(board, helpBoard);
        resetBoard(helpBoard);
        printBoard(board);
        if (compareBoard(board, helpBoard))
        {
            break;
        }
    }
}

void main(){
    printf("Welcome to Game Of Life!\n");
    getSizeOfBoard();
    bool board[sizeOfBoard][sizeOfBoard]; 
    createGameBoard(board);
    startGameLoop(board);
}

//White square - 2B1C
//black square - 2B1B


/*    int temp_x, temp_y;
    int livingCellsAround;
    for (int i = -1; i <= 1; i++)
    {
        temp_x = x_coordinate + i;
        
        if(temp_x < 0 || temp_x >= sizeOfBoard)
        {
            continue;
        }

        for (int j = -1; j <= 1; j++)
        {
            temp_y = y_coordinate + j;
            
            if(temp_y < 0 || temp_y >= sizeOfBoard)
            {
                continue;
            }

            if (!board[temp_x][temp_y] && !helpBoard[temp_x][temp_y])
            {
                livingCellsAround = amountOflivingCellsAround(board, temp_x, temp_y);
                helpBoard[temp_x][temp_y] = livingCellsAround == 3;
            }
        }
    }
    
    
            for (int i = 0; i < sizeOfBoard; i++)
        {
            for (int j = 0; j < sizeOfBoard; j++)
            {
                livingCellsAround = amountOflivingCellsAround(board, i, j);

                if (livingCellsAround == 2 || livingCellsAround == 3)
                {
                    helpBoard[i][j] = true;
                }
            }
        }
    */
