#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
} Move;

typedef struct Pos
{
    int row, col;
} Pos;

void displayStartMenu();
void printBoard(int board[][8]);
int gameState(int board[][8]);
bool makeMove(int board[][8], Move m, bool test);
bool validatePawn(Move m, int board[][8]);
bool validateKnight(Move m);
bool validateKing(Move m);
bool validateRook(Move m, int board[][8]);
bool validateBishop(Move m, int board[][8]);
bool isChecked(int board[][8], bool whiteKingToCheck);
Pos findPiece(int board[][8], int pieceIndex);

void clearScreen()
{ // Help from gemini used here. :)
#ifdef _WIN32
  // Traditional Windows method
    system("cls");
#else
  // ANSI escape codes for Linux/Mac/Modern Terminal
    // \033 is the octal for the Escape character
    printf("\033[H\033[2J");
#endif
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
    displayStartMenu();
    clearScreen();
    // Manually Mapped the board at first.
    int board[8][8] = {{9, 10, 11, 8, 7, 11, 10, 9},
                       {12, 12, 12, 12, 12, 12, 12, 12},
                       {0},
                       {0},
                       {0},
                       {0},
                       {6, 6, 6, 6, 6, 6, 6, 6},
                       {3, 4, 5, 2, 1, 5, 4, 3}};
    printBoard(board);
    while (true)
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
        Move m;
        m.fromCol = move[0] - 'a';
        m.fromRow = 8 - (move[1] - '0');
        m.toCol = move[2] - 'a';
        m.toRow = 8 - (move[3] - '0');
        int makeMoveRes = makeMove(board, m, false);
        if (makeMoveRes == 1)
        {
            clearScreen();
            printBoard(board);
            turnOfWhite = !turnOfWhite;
            int gamestate = gameState(board);
            if (gamestate > 1)
            {
                printf("%s\nGame Over. %s", (gamestate == 2 ? "Checkmate" : "Stalemate"), ((gamestate == 3) ? "It is a Draw!!" : (turnOfWhite ? "Black Wins!!" : "White Wins!!")));
                return 1;
            }
            if (gamestate == 1)
            {
                printf("Check\n");
            }
            moveCount++;
        }
        else
            printf("%s", makeMoveRes == 0 ? "Invalid Move\n" : "Illegal Move. You cannot let your king be in check.\n");
    }

    return 0;
}

void printBoard(int board[8][8])
{
    // Used gemini to make this more preety. :)
    printf("\n       a     b     c     d     e     f     g     h\n");

    for (int i = 0; i < 8; i++)
    {
        printf("    +-----+-----+-----+-----+-----+-----+-----+-----+\n");
        printf(" %d  ", 8 - i);

        for (int j = 0; j < 8; j++)
        {
            printf("|");

            // 1. Set Background Color
            bool isLightSquare = ((i + j) % 2 == 0);
            if (isLightSquare)
                printf("\033[47m"); // Light Gray/White
            else
                printf("\033[100m"); // Dark Gray

            // 2. Set Text Color for Maximum Contrast
            // Light Square -> Black Text (30)
            // Dark Square -> White Text (37)
            if (isLightSquare)
                printf("\033[1;30m");
            else
                printf("\033[1;37m");

            int val = board[i][j];
            if (val == 0)
            {
                printf("     ");
            }
            else
            {
                printf("  %c  ", pieces[val]);
            }

            printf("\033[0m");
        }
        printf("| %d\n", 8 - i);
    }
    printf("    +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("       a     b     c     d     e     f     g     h\n\n");
}

void displayStartMenu()
{
    clearScreen();
    printf("====================================================\n");
    printf("            WELCOME TO THE GAME OF CHESS            \n");
    printf("====================================================\n\n");

    printf("  PIECE LEGEND:\n");
    printf("  --------------------------------------------------\n");
    printf("  WHITE (Capital)          BLACK (Small)\n");
    printf("  K : King                 k : King\n");
    printf("  Q : Queen                q : Queen\n");
    printf("  R : Rook                 r : Rook\n");
    printf("  B : Bishop               b : Bishop\n");
    printf("  N : Knight               n : Knight\n");
    printf("  P : Pawn                 p : Pawn\n\n");

    printf("  HOW TO MOVE:\n");
    printf("  --------------------------------------------------\n");
    printf("  Enter moves using coordinates (e.g., e2e4).\n");
    printf("  - 'e2' is the starting square.\n");
    printf("  - 'e4' is the destination square.\n\n");

    printf("  SPECIAL RULES:\n");
    printf("  --------------------------------------------------\n");
    printf("  - The game detects Check, Checkmate, and Stalemate.\n");
    printf("  - You cannot move into check.\n\n");

    printf("====================================================\n");
    printf("          PRESS [ENTER] TO START THE GAME          \n");
    printf("====================================================\n");

    // Wait for the user to press Enter
    getchar();
}

int gameState(int board[][8])
{
    bool hasLegalMove = false;
    for (int fR = 0; fR < 8; fR++)
    {
        for (int fC = 0; fC < 8; fC++)
        {
            if (board[fR][fC] < 6.5 == turnOfWhite)
            {
                for (int tR = 0; tR < 8; tR++)
                {
                    for (int tC = 0; tC < 8; tC++)
                    {
                        Move m = {fR, fC, tR, tC};
                        int tmp = board[tR][tC];
                        // Check if this move can be done
                        if (makeMove(board, m, true))
                        {
                            hasLegalMove = true;
                            // Rewind the change from makeMove.
                            board[fR][fC] = board[tR][tC];
                            board[tR][tC] = tmp;
                            goto end_loops;
                        }
                    }
                }
            }
        }
    }
end_loops:
    if (isChecked(board, turnOfWhite))
    {
        if (hasLegalMove)
            return 1; // For just check
        else
            return 2; // For checkmate
    }
    else if (!hasLegalMove)
        return 3; // For stalemate

    return 0;
}

bool makeMove(int board[][8], Move m, bool test)
{
    // Check out of bounds
    if (m.fromCol < 0 || m.fromCol > 7 || m.fromRow < 0 || m.fromRow > 7 ||
        m.toCol < 0 || m.toCol > 7 || m.toRow < 0 || m.toRow > 7)
    {
        return 0;
    }

    int *from = &board[m.fromRow][m.fromCol];
    int *to = &board[m.toRow][m.toCol];

    // Checks that from is not 0 and then checks whose turn it is and their piece is moved. Then it also checks if either the destination is empty or contains enemy's piece and not ones own piece.
    bool isValidMove = false;
    if ((*from != 0 && ((turnOfWhite && *from <= 6) || (!turnOfWhite && *from > 6)) && (*to == 0 || (*to > 6 && turnOfWhite) || (*to <= 6 && !turnOfWhite))))
    {
        switch (*from)
        {
        case 1:
        case 7:
            isValidMove = validateKing(m);
            break;
        case 2:
        case 8:
            isValidMove = validateBishop(m, board) || validateRook(m, board);
            break;

        case 3:
        case 9:
            isValidMove = validateRook(m, board);
            break;

        case 4:
        case 10:
            isValidMove = validateKnight(m);
            break;

        case 5:
        case 11:
            isValidMove = validateBishop(m, board);
            break;

        case 6:
        case 12:
            isValidMove = validatePawn(m, board);
            break;
        }
    }
    if (isValidMove)
    {
        int tmp = *to;
        *to = *from;
        *from = 0;
        if (!test && (*to == 6 || *to == 12) && (m.toRow == 0 || m.toRow == 7))
        {
            printf("You may promote your pawn.\n Enter Q,R,N or B for Queen, Rook, Knight or Bishop respectivly.\n Enter your choice: ");
            char choice;
            scanf(" %c", &choice);
            choice = toupper(choice);
            *to = (choice == 'Q' ? 2 : (choice == 'R' ? 3 : (choice == 'N' ? 4 : 5))) + (turnOfWhite ? 0 : 6);
        }

        if (!isChecked(board, turnOfWhite))
            return 1;
        // Rewind change if checked.
        *from = *to;
        *to = tmp;
        return 2;
    }
    return 0;
}

bool validatePawn(Move m, int board[][8])
{
    // Checks if columns are same and destination is free
    if (m.fromCol == m.toCol && board[m.toRow][m.toCol] == 0)
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
    else if (abs(m.fromCol - m.toCol) == 1 && board[m.toRow][m.toCol] != 0 && (turnOfWhite && (m.fromRow - m.toRow) == 1 || !turnOfWhite && (m.fromRow - m.toRow) == -1))
    {
        return true;
    }

    return false;
}

bool validateKnight(Move m)

{
    if (abs(m.fromRow - m.toRow) * abs(m.fromCol - m.toCol) == 2)
        return true;
    return false;
}

bool validateRook(Move m, int board[][8])
{
    if (m.fromRow == m.toRow)
    {
        int unitVector = (m.toCol - m.fromCol) / abs(m.toCol - m.fromCol);

        for (int i = m.fromCol + unitVector; i != m.toCol; i += unitVector)
        {
            if (board[m.fromRow][i] != 0)
            {
                return false;
            }
        }
        return true;
    }
    else if (m.fromCol == m.toCol)
    {
        int unitVector = (m.toRow - m.fromRow) / abs(m.toRow - m.fromRow);
        for (int i = m.fromRow + unitVector; i != m.toRow; i += unitVector)
        {
            if (board[i][m.fromCol] != 0)
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

bool validateBishop(Move m, int board[][8])
{
    if (abs(m.fromRow - m.toRow) == abs(m.fromCol - m.toCol))
    {
        int unitVectorj = (m.toCol - m.fromCol) / abs(m.toCol - m.fromCol);
        int unitVectori = (m.toRow - m.fromRow) / abs(m.toRow - m.fromRow);

        for (int i = m.fromRow + unitVectori, j = m.fromCol + unitVectorj; i != m.toRow; i += unitVectori, j += unitVectorj)
        {
            if (board[i][j] != 0)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool validateKing(Move m)
{
    if (abs(m.fromRow - m.toRow) <= 1 && abs(m.fromCol - m.toCol) <= 1)
    {
        return true;
    }
    return false;
}

bool isChecked(int board[][8], bool whiteKingToCheck)
{

    const Pos kingPos = findPiece(board, whiteKingToCheck ? 1 : 7);
    const int pieces[2][6] = {
        {1, 2, 3, 4, 5, 6},
        {7, 8, 9, 10, 11, 12}};
    const int *refIndex = whiteKingToCheck ? pieces[1] : pieces[0];

    int pawnDir = whiteKingToCheck ? -1 : 1;

    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            // If check piece is same or both i and j are 2,-2.
            if ((i == 0 && j == 0) || (abs(i * j) > 2))
                continue;
            // Bounds verification
            if (kingPos.row + i < 8 && kingPos.col + j < 8 && kingPos.row + i >= 0 && kingPos.col + j >= 0)
            {
                // Checking for knight threat.
                if ((abs(i * j) == 2))
                {
                    if (board[kingPos.row + i][kingPos.col + j] == refIndex[3])
                        return true;
                    continue;
                }
                // Checking for adjecent king
                if (board[kingPos.row + i][kingPos.col + j] == refIndex[0])
                    return true;
                // Checking for rook queen and bishop threats
                if (abs(i) <= 1 && abs(j) <= 1)
                {
                    Pos checkPos = kingPos;
                    checkPos.row += i;
                    checkPos.col += j;
                    while (checkPos.row < 8 && checkPos.col < 8 && checkPos.row >= 0 && checkPos.col >= 0)
                    {
                        int checkSq = board[checkPos.row][checkPos.col];
                        if (checkSq)
                        {
                            // Breaks if checkSq is own piece.
                            if (checkSq < 6.5 == whiteKingToCheck)
                                break;
                            // Check for pawn threat.
                            if ((i == pawnDir) && (abs(j) == 1) && (checkPos.row == kingPos.row + i) && (checkSq == refIndex[5]))
                                return true;

                            // Checks for rook and queen threat.
                            if ((i == 0 || j == 0) && (checkSq == refIndex[2] || checkSq == refIndex[1]))
                                return true;
                            // Checks for bishop and queen threat.
                            if (i != 0 && j != 0 && (checkSq == refIndex[4] || checkSq == refIndex[1]))
                                return true;
                            break;
                        }
                        checkPos.row += i;
                        checkPos.col += j;
                    }
                }
            }
        }
    }

    return false;
}

Pos findPiece(int board[][8], int pieceIndex)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == pieceIndex)
            {
                return (Pos){i, j};
            }
        }
    }
    return (Pos){-1, 0};
}