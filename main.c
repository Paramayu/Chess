#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    int *from;
    int *to;
} Move;

void printBoard(int board[][8]);
int gameEnded(int board[][8]);
bool makeMove(int board[][8], char move[]);
bool validatePawn(Move m, int board[][8]);
bool validateKnight(Move m);

void clearScreen()
{
    // \e[1;1H moves cursor to row 1, column 1
    // \e[2J clears the entire screen
    printf("\033[H\033[J");
    system("cls");
}

const char pieces[] = {' ',
                       'K',
                       'Q',
                       'R',
                       'N',
                       'B',
                       'P',
                       'k',
                       'q',
                       'r',
                       'n',
                       'b',
                       'p'};
bool turnOfWhite = true;
int moveCount = 0;

int main(int argc, char const *argv[])
{
    clearScreen();
    int board[8][8] = {{9, 10, 11, 8, 7, 11, 10, 9},
                       {12, 12, 12, 12, 12, 12, 12, 12},
                       {0},
                       {0},
                       {0},
                       {0},
                       {6, 6, 6, 6, 6, 6, 6, 6},
                       {3, 4, 5, 2, 1, 5, 4, 3}};
    printBoard(board);
    while (gameEnded(board) == 0)
    {
        char move[10];
        printf("%s turn. Enter your move:\n", turnOfWhite ? "White's" : "Black's");
        fgets(move, sizeof(move), stdin);
        if (strchr(move, '\n') == NULL)
        {
            int c;
            // Read and discard characters until we hit a newline or EOF
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        if (makeMove(board, move))
        {
            clearScreen();
            printBoard(board);
            turnOfWhite = turnOfWhite ? false : true;
            moveCount++;
        }
    }

    return 0;
}

void printBoard(int board[][8])
{
    printf("\n       a     b     c     d     e     f     g     h\n");
    printf("    +-----+-----+-----+-----+-----+-----+-----+-----+\n");

    for (int i = 0; i < 8; i++)
    {
        printf(" %d  ", 8 - i);

        for (int j = 0; j < 8; j++)
        {
            printf("|  %c  ", pieces[board[i][j]]);
        }
        printf("|\n    +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    }
}

int gameEnded(int board[][8])
{
    return 0;
}

bool makeMove(int board[][8], char move[])
{
    Move m;
    m.fromCol = move[0] - 'a';
    m.fromRow = 8 - (move[1] - '0');
    m.toCol = move[2] - 'a';
    m.toRow = 8 - (move[3] - '0');
    if (m.fromCol < 0 || m.fromCol > 7 || m.fromRow < 0 || m.fromRow > 7 ||
        m.toCol < 0 || m.toCol > 7 || m.toRow < 0 || m.toRow > 7)
    {
        printf("\nInvalid Move!\n");
        return false;
    }

    m.from = &board[m.fromRow][m.fromCol];
    m.to = &board[m.toRow][m.toCol];

    // Checks that from is not 0 and then checks whose turn it is and their piece is moved. Then it also checks if either the destination is empty or contains enemy's piece and not ones own piece.
    if ((*m.from != 0 && ((turnOfWhite && *m.from <= 6) || (!turnOfWhite && *m.from > 6)) && (*m.to == 0 || (*m.to > 6 && turnOfWhite) || (*m.to <= 6 && !turnOfWhite))))
    {
        bool isValidMove = false;
        switch (*m.from)
        {
        case 6:
        case 12:
            isValidMove = validatePawn(m, board);
            break;
        case 4:
        case 10:
            isValidMove = validateKnight(m);
        }

        if (isValidMove)
        {
            *m.to = *m.from;
            *m.from = 0;
            return true;
        }
    }
    printf("\nInvalid Move.\n");
    return false;
}

bool validatePawn(Move m, int board[][8])
{
    // Checks if columns are same and destination is free
    if (m.fromCol == m.toCol && *m.to == 0)
    {
        // Checks if the pawn is moved for first time and if is tried to be moved two steps and no movement in column
        if (((turnOfWhite && m.fromRow == 6 && (m.fromRow - m.toRow) == 2) || (!turnOfWhite && m.fromRow == 1 && (m.fromRow - m.toRow) == -2)))
        {
            // Checks if the path to destination is completly free.
            if (board[(m.fromRow + m.toRow) / 2][m.fromCol] == 0)
                return true;
        }
        else if (turnOfWhite && (m.fromRow - m.toRow) == 1 || !turnOfWhite && (m.fromRow - m.toRow) == -1)
            return true;
    }
    else if (abs(m.fromCol - m.toCol) == 1 && *m.to != 0 && (turnOfWhite && (m.fromRow - m.toRow) == 1 || !turnOfWhite && (m.fromRow - m.toRow) == -1))
    {
        return true;
    }

    return false;
}

bool validateKnight(Move m)

{
    if (abs(m.fromRow - m.toRow) * abs(m.fromCol - m.toCol) == 2)
        return true;
}