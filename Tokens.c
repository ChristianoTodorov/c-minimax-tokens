#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

typedef struct GameBoard{
    int* tokens;
    int head;
    int tail;
}GameBoardT;

typedef struct Player{
    int tokenSum;
    bool takenTwo;
}PlayerT;

typedef enum{
    takeFirst,
    takeFirstTwo,
    takeLast,
    takeLastTwo,
    takeFirstLast
}TurnType;

typedef struct takingTurn{
    int index1;
    int index2;
    TurnType turnT;
}activeTurnT;

#define MAX_LINE_LENGTH 2000
#define MAX_TOKEN_CAP 100

int* readTokens(size_t *count) {
    printf("Zetony:\n");

    char line[MAX_LINE_LENGTH];
    size_t size = 0;
    int *tokens = (int*)malloc(MAX_TOKEN_CAP * sizeof(*tokens));

    if (tokens == NULL) {
        printf("Neuspesna alokace pameti.\n");
        return NULL;
    }
    // Read input
    if (!fgets(line, sizeof(line), stdin)) {
        free(tokens);
        return NULL;
    }
    char* ptr = line;
    char* endptr;
    long tokenValue;

    while (*ptr != '\0' && *ptr != '\n') {
        // Skip whitespace
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        // Convert the found integer to long
        tokenValue = strtol(ptr, &endptr, 10);
        
        // Invalid input check
        bool invalidRange = (tokenValue < INT_MIN || tokenValue > INT_MAX);
        bool no_val_converted = (ptr == endptr);
        bool invalidEndPtr = (*endptr != '\0' && !isspace((unsigned char)*endptr));
        
        if(invalidEndPtr || invalidRange || no_val_converted) {
            free(tokens);
            return NULL;
        }
        // Move to the first whitespace after number
        ptr = endptr;
        // Capacity exceeded
        if (size >= MAX_TOKEN_CAP) {
            free(tokens);
            return NULL;
        }
        // Add token to array
        tokens[size++] = (int)tokenValue;
    }
    // No tokens found
    if (size == 0) {
        free(tokens);
        return NULL;
    }
    *count = size;
    return tokens;
}

// Returns the best move for playerA in the given state of the game
activeTurnT getBestMove(GameBoardT* game, PlayerT* playerA, PlayerT* playerB);

// Returns the score of the current game for the maximizing player using alpha beta pruning
int Minimax(GameBoardT* game, PlayerT* currPlayer, PlayerT* opponent, int alpha, int beta, bool maximizing);

// Initialize the game and play it
void playGame(GameBoardT* game, PlayerT* playerA, PlayerT* playerB);

// Play a given turn for the given player
void playTurn(GameBoardT* game, TurnType turn, PlayerT* player);

int main(){
    size_t count = 0;
    int *tokens = readTokens(&count);
    PlayerT p1 = {0, false};
    PlayerT p2 = {0, false};

    if (tokens == NULL) {
        printf("Nespravny vstup.\n");
        return EXIT_FAILURE;
    }

    GameBoardT game = {tokens, 0, (int)(count-1)};
    playGame(&game, &p1, &p2);
    
    free(tokens);
    return EXIT_SUCCESS;
}

void playGame(GameBoardT* game, PlayerT* playerA, PlayerT* playerB){
    // Player A is first
    bool isPlayerATurn = true; 
    // While there is tokens left, players take turns
    while(!(game->head > game->tail)){

        activeTurnT bestMove = getBestMove(game, (isPlayerATurn ? playerA : playerB), (isPlayerATurn ? playerB : playerA));
        playTurn(game, bestMove.turnT, (isPlayerATurn ? playerA : playerB));
        char playing = (isPlayerATurn ? 'A' : 'B');

        if(bestMove.turnT == takeFirstTwo || bestMove.turnT == takeLastTwo || bestMove.turnT == takeFirstLast)
            printf("%c [%d], [%d]: %d + %d\n", playing, bestMove.index1, bestMove.index2, game->tokens[bestMove.index1], game->tokens[bestMove.index2]);
        else
            printf("%c [%d]: %d\n", playing, bestMove.index1, game->tokens[bestMove.index1]);

        isPlayerATurn = !isPlayerATurn;
    }
    // Game Over
    printf("A: %d, B: %d\n", playerA->tokenSum, playerB->tokenSum);
}

activeTurnT getBestMove(GameBoardT* game, PlayerT* playerA, PlayerT* playerB){
    activeTurnT bestMove;
    int bestSum = INT_MIN;
    
    TurnType takingTurn;
    
    int NUM_OF_MOVES = 5;
    if(game->head == game->tail) NUM_OF_MOVES = 1;
    if(game->head + 1 == game->tail) NUM_OF_MOVES = 3;

    // Generate all possible moves
    for(int i = 0; i < NUM_OF_MOVES; i++){
        // Initial game state
        GameBoardT gameCopy = *game;
        PlayerT playerACopy = *playerA;
        PlayerT playerBCopy = *playerB;

        takingTurn = (TurnType)i;
        playTurn(&gameCopy, takingTurn, &playerACopy);
        
        bool tokenTaken = (game->head != gameCopy.head || game->tail != gameCopy.tail);
        if(!tokenTaken) continue; // Couldn't take two tokens
        
        int sum = Minimax(&gameCopy, &playerBCopy, &playerACopy, INT_MIN, INT_MAX, false);

        // Update best move
        if(sum > bestSum){
            bestSum = sum;
            bestMove.turnT = takingTurn;
            switch(bestMove.turnT){
                case takeFirst:
                    bestMove.index1 = game->head; 
                    break;
                case takeFirstTwo:
                    bestMove.index1 = game->head; 
                    bestMove.index2 = game->head + 1;
                    break;
                case takeLast:
                    bestMove.index1 = game->tail;
                    break;
                case takeLastTwo:
                    bestMove.index1 = game->tail;
                    bestMove.index2 = game->tail - 1;
                    break;
                case takeFirstLast:
                    bestMove.index1 = game->head; 
                    bestMove.index2 = game->tail;
                    break;
            }
        }
    }
    return bestMove;
}

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
                            
int Minimax(GameBoardT* game, PlayerT* currPlayer, PlayerT* opponent, int alpha, int beta, bool maximizing){
    // Game Over => return score
    if(game->head > game->tail)
        return maximizing ? (currPlayer->tokenSum - opponent->tokenSum) : (opponent->tokenSum - currPlayer->tokenSum);
    
    int bestSum = maximizing ? INT_MIN : INT_MAX;
    TurnType takingTurn;

    int NUM_OF_MOVES = 5;
    if(game->head == game->tail) NUM_OF_MOVES = 1;
    if(game->head + 1 == game->tail) NUM_OF_MOVES = 3;

    // Generate all possible moves
    for(int i = 0; i < NUM_OF_MOVES; i++){
        // Initial game state
        GameBoardT gameCopy = *game;
        PlayerT currPlayerCopy = *currPlayer;
        PlayerT opponentCopy = *opponent;

        takingTurn = (TurnType)i;
        playTurn(&gameCopy, takingTurn, &currPlayerCopy);
        
        bool tokenTaken = (game->head != gameCopy.head || game->tail != gameCopy.tail);
        if(!tokenTaken) continue;

        int score = Minimax(&gameCopy, &opponentCopy, &currPlayerCopy, alpha, beta, !maximizing);

        // Update bestSum
        if (maximizing) {
            bestSum = MAX(bestSum, score);
            alpha = MAX(alpha, score);
        } else {
            bestSum = MIN(bestSum, score);
            beta = MIN(beta, score);
        }
        if(beta <= alpha) break;
    }
    
    return bestSum;
}

// Handle player turn
void playTurn(GameBoardT* game, TurnType turn, PlayerT* player){
    bool canTakeTwo = (!player->takenTwo && game->head < game->tail);
    if(!canTakeTwo && turn != takeFirst && turn != takeLast) return;

    switch(turn){
            case takeFirst:
                player->tokenSum += game->tokens[game->head++];
                player->takenTwo = false;
                break;
            case takeLast:
                player->tokenSum += game->tokens[game->tail--];
                player->takenTwo = false;
                break;
            case takeFirstTwo:
                player->tokenSum += game->tokens[game->head++];
                player->tokenSum += game->tokens[game->head++];
                player->takenTwo = true;
                break;
            case takeLastTwo:
                player->tokenSum += game->tokens[game->tail--];
                player->tokenSum += game->tokens[game->tail--];
                player->takenTwo = true;
                break;
            case takeFirstLast:
                player->tokenSum += game->tokens[game->head++];
                player->tokenSum += game->tokens[game->tail--];
                player->takenTwo = true;
                break;
        }
}