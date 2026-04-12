/**
 * @file TicTacToe.c
 * @brief Implementation of a Tic-Tac-Toe game with an AI opponent using the Minimax algorithm with alpha-beta pruning.
 *
 * This program implements a command-line Tic-Tac-Toe game where a human player competes against
 * an AI opponent. The AI uses the Minimax algorithm with alpha-beta pruning to make optimal moves,
 * making it unbeatable when playing optimally.
 *
 * @author Görkem
 * @date Year One, Part 1
 */

/**
 * @def COMPUTER
 * @brief Identifier for the computer player.
 */

/**
 * @def HUMAN
 * @brief Identifier for the human player.
 */

/**
 * @def SIDE
 * @brief The dimension of the Tic-Tac-Toe board (3x3).
 */

/**
 * @def COMPUTERMOVE
 * @brief The symbol used by the computer player ('O').
 */

/**
 * @def HUMANMOVE
 * @brief The symbol used by the human player ('X').
 */

/**
 * @struct Move
 * @brief Represents a move on the Tic-Tac-Toe board.
 *
 * @var Move::row
 * Row index of the move (0-2).
 *
 * @var Move::col
 * Column index of the move (0-2).
 */

/**
 * @brief Checks if there are any empty cells remaining on the board.
 *
 * @param board The current state of the game board.
 * @return true if there are moves left, false otherwise.
 */

/**
 * @brief Evaluates the current board state and returns a score.
 *
 * This function checks all possible winning conditions (rows, columns, and diagonals)
 * and returns a score based on the game state.
 *
 * @param board The current state of the game board.
 * @param ai The symbol representing the AI player.
 * @param human The symbol representing the human player.
 * @return +10 if AI wins, -10 if human wins, 0 for ongoing game or draw.
 */

/**
 * @brief Implements the Minimax algorithm with alpha-beta pruning.
 *
 * This recursive function explores all possible game states to find the optimal move
 * for the AI. Alpha-beta pruning is used to eliminate branches that don't need to be
 * explored, improving performance.
 *
 * @param board The current state of the game board (modified during recursion).
 * @param depth Current depth in the game tree.
 * @param isMax True if it's the maximizer's (AI) turn, false for minimizer (human).
 * @param alpha The best value the maximizer can guarantee at current level or above.
 * @param beta The best value the minimizer can guarantee at current level or above.
 * @param ai The symbol representing the AI player.
 * @param human The symbol representing the human player.
 * @return The best score achievable from the current position.
 */

/**
 * @brief Finds the best move for the AI player.
 *
 * Iterates through all empty cells and uses the minimax algorithm to determine
 * which move yields the best outcome for the AI.
 *
 * @param board The current state of the game board.
 * @param ai The symbol representing the AI player.
 * @param human The symbol representing the human player.
 * @return A Move struct containing the row and column of the best move.
 */

/**
 * @brief Displays the current state of the game board.
 *
 * Prints a formatted representation of the board to the console.
 *
 * @param board The current state of the game board.
 */

/**
 * @brief Displays game instructions to the player.
 *
 * Shows the numbered grid layout (1-9) that corresponds to board positions
 * and provides basic gameplay instructions.
 */

/**
 * @brief Announces the winner of the game.
 *
 * @param whoseTurn The player who won (COMPUTER or HUMAN).
 */

/**
 * @brief Main game loop that manages the Tic-Tac-Toe game flow.
 *
 * Initializes the board, handles alternating turns between computer and human,
 * validates human input, updates the board state, and determines game outcome
 * (win, loss, or draw).
 *
 * @param whoseTurn The player who makes the first move (COMPUTER or HUMAN).
 */

/**
 * @brief Entry point of the program.
 *
 * Starts a game of Tic-Tac-Toe with the computer making the first move.
 *
 * @return 0 on successful execution.
 */
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#define COMPUTER 1
#define HUMAN 2
#define SIDE 3
#define COMPUTERMOVE 'O'
#define HUMANMOVE 'X'

struct Move {
    int row;
    int col;
};

// Check if there are any empty cells left on the board
static bool isMovesLeft(const char board[SIDE][SIDE]) {
    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            if (board[i][j] == ' ') {
                return true;
            }
        }
    }
    return false;
}

// Evaluate the board state: +10 if AI wins, -10 if human wins, 0 otherwise
static int evaluate(const char board[SIDE][SIDE], char ai, char human) {
    // Check rows
    for (int i = 0; i < SIDE; ++i) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return board[i][0] == ai ? 10 : -10;
        }
    }
    
    // Check columns
    for (int i = 0; i < SIDE; ++i) {
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return board[0][i] == ai ? 10 : -10;
        }
    }
    
    // Check main diagonal (top-left to bottom-right)
    if (board[1][1] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return board[1][1] == ai ? 10 : -10;
    }
    
    // Check anti-diagonal (top-right to bottom-left)
    if (board[1][1] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return board[1][1] == ai ? 10 : -10;
    }
    
    return 0;
}

// Minimax algorithm with alpha-beta pruning
static int minimax(char board[SIDE][SIDE], int depth, bool isMax, int alpha, int beta, char ai, char human) {
    int score = evaluate(board, ai, human);
    
    // If AI won, return positive score (favor earlier wins)
    if (score == 10) {
        return score - depth;
    }
    
    // If human won, return negative score (favor later losses)
    if (score == -10) {
        return score + depth;
    }
    
    // If no moves left, it's a draw
    if (!isMovesLeft(board)) {
        return 0;
    }

    // Maximizer's turn (AI)
    if (isMax) {
        int best = INT_MIN;
        
        for (int i = 0; i < SIDE; ++i) {
            for (int j = 0; j < SIDE; ++j) {
                if (board[i][j] == ' ') {
                    // Try this move
                    board[i][j] = ai;
                    int val = minimax(board, depth + 1, false, alpha, beta, ai, human);
                    // Undo the move
                    board[i][j] = ' ';
                    
                    if (val > best) {
                        best = val;
                    }
                    if (best > alpha) {
                        alpha = best;
                    }
                    // Alpha-beta pruning
                    if (alpha >= beta) {
                        return best;
                    }
                }
            }
        }
        return best;
    }

    // Minimizer's turn (Human)
    int best = INT_MAX;
    
    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            if (board[i][j] == ' ') {
                // Try this move
                board[i][j] = human;
                int val = minimax(board, depth + 1, true, alpha, beta, ai, human);
                // Undo the move
                board[i][j] = ' ';
                
                if (val < best) {
                    best = val;
                }
                if (best < beta) {
                    beta = best;
                }
                // Alpha-beta pruning
                if (alpha >= beta) {
                    return best;
                }
            }
        }
    }
    return best;
}

// Find the best move for the AI using minimax
static struct Move findBestMove(char board[SIDE][SIDE], char ai, char human) {
    int bestVal = INT_MIN;
    int alpha = INT_MIN;
    struct Move bestMove = { -1, -1 };

    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            if (board[i][j] == ' ') {
                // Try this move
                board[i][j] = ai;
                int moveVal = minimax(board, 0, false, alpha, INT_MAX, ai, human);
                // Undo the move
                board[i][j] = ' ';
                
                if (moveVal > bestVal) {
                    bestVal = moveVal;
                    bestMove.row = i;
                    bestMove.col = j;
                }
                if (bestVal > alpha) {
                    alpha = bestVal;
                }
            }
        }
    }
    return bestMove;
}

// Display the current game board
static void showBoard(const char board[][SIDE]) {
    printf("\n\t\t\t %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("\t\t\t-----------\n");
    printf("\t\t\t %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("\t\t\t-----------\n");
    printf("\t\t\t %c | %c | %c \n\n", board[2][0], board[2][1], board[2][2]);
}

// Display game instructions
static void showInstructions(void) {
    printf("\t\t\tTic-Tac-Toe\n\n");
    printf("Choose a cell numbered from 1 to 9 as below and play\n\n");
    printf("\t\t\t 1 | 2 | 3 \n");
    printf("\t\t\t-----------\n");
    printf("\t\t\t 4 | 5 | 6 \n");
    printf("\t\t\t-----------\n");
    printf("\t\t\t 7 | 8 | 9 \n\n");
    printf("---------------------------------------\n");
}

// Announce the winner
static void declareWinner(int whoseTurn) {
    if (whoseTurn == COMPUTER) {
        printf("COMPUTER has won\n");
    } else {
        printf("HUMAN has won\n");
    }
}

// Main game loop
static void playTicTacToe(int whoseTurn) {
    char board[SIDE][SIDE];
    
    // Initialize board with empty spaces
    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            board[i][j] = ' ';
        }
    }

    showInstructions();
    showBoard(board);

    for (int moves = 0; moves < SIDE * SIDE; ) {
        if (whoseTurn == COMPUTER) {
            // Computer's turn
            struct Move move = findBestMove(board, COMPUTERMOVE, HUMANMOVE);
            if (move.row < 0) {
                break;
            }
            board[move.row][move.col] = COMPUTERMOVE;
            printf("COMPUTER has put a %c in cell %d\n",
                   COMPUTERMOVE, move.row * SIDE + move.col + 1);
            ++moves;
        } else {
            // Human's turn
            int move;
            printf("Enter your move (1-9): ");
            if (scanf("%d", &move) != 1 || move < 1 || move > 9) {
                printf("Invalid input! Please enter a number between 1 and 9.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                continue;
            }
            
            // Convert move number to board coordinates
            int x = (move - 1) / SIDE;
            int y = (move - 1) % SIDE;
            
            if (board[x][y] != ' ') {
                printf("Cell %d is already occupied. Try again.\n", move);
                continue;
            }
            board[x][y] = HUMANMOVE;
            ++moves;
        }

        showBoard(board);
        
        // Check if game is over
        int score = evaluate(board, COMPUTERMOVE, HUMANMOVE);
        if (score == 10) {
            declareWinner(COMPUTER);
            return;
        }
        if (score == -10) {
            declareWinner(HUMAN);
            return;
        }
        if (!isMovesLeft(board)) {
            break;
        }
        
        // Switch turns
        whoseTurn = (whoseTurn == COMPUTER) ? HUMAN : COMPUTER;
    }
    
    printf("It's a draw\n");
}

int main(void) {
    playTicTacToe(COMPUTER);
    return 0;
}
