#include <iostream>
#include <string>
#include <stdlib.h>
#include "Pokitto.h"
#include "PokittoFonts.h"

//create game object
Pokitto::Core game;

//include textures
#include <textures.h>

//variable declarations
short gameMode = -2; //-2 = title, -1 = settings, 0 = menu, 1 = pong, 2 = breakout, 3 = snake, 4 = stacker, 5 = columns
unsigned int paddleA = 36;
unsigned int paddleB = 36;
unsigned int paddleBr = 46;
int paddleBMotion = 0;
int ballX = 52;
int ballY = 42;
float ballSpeed = 1.0f;
int bricksHit = 0;
int ballXR;
int ballYR;
short ballDX = 1;
short ballDY = 1;
int RNG = rand() % 100 + 1;
int RNGX = rand() % 25 + 1;
int RNGY = rand() % 18 + 1;
int difficulty = 70;
unsigned int scoreA = 0;
unsigned int scoreB = 0;
unsigned int scoreBr = 0;
unsigned short breakoutGrid [13][5]; //0=air, 1=redWide1, 2=redWide2, 3=yellowWide1, 4=yellowWide2, 5=greenWide, 6=cyanWide, 7=blueWide
bool secondLevel = false;
unsigned int lives = 3;
short gameMap [27][22]; //0 = air, -1 = wall, -2 = food, 1 = snake head, 2 = snake body
short snakeLength = 3;
short headX = 13;
short headY = 10;
short direction = 0; //0 = up, 1 = right, 2 = down, 3 = left
bool snakeUpdate = false;
bool foodPlacementSuccess = false;
short counter = 0;
bool snakeInitializing = true;
int breakoutHS = 0;
int snakeHS = 0;
unsigned short blockX = 0;
unsigned short blockSize = 4;
short blockDir = 1;
short blockH = 0;
short oldBlockData [20][2];
short blockHOffset = 0;
short blockLoss = 0;
short blockMove = 0;
bool stackLost = false;
int stackerScore = 0;
int stackerHS = 0;
int stackerDelay = 3;
short menuSelection = 1;
bool grayscale = false;
short optionSelection = 1;
bool sound = true;
double timeTime = 0.0;
/*short currentColumn [3];
short currentColumnDupe [3];
short nextColumn [3];
short columnsMap [9][22];*/
void resetPongValues() {
    ballX = 52;
    ballY = 42;
    paddleBMotion = 0;
    paddleA = 36;
    paddleB = 36;
    ballDX = 1;
    ballDY = 1;
}
void pongLogic() {
    //update difficulty
    difficulty = 70 + 2.5*(scoreA - scoreB);
    //input, move paddle 1
    if (game.buttons.repeat(BTN_UP,0) && paddleA > 0) {
        paddleA -= 2;
    }
    if (game.buttons.repeat(BTN_DOWN,0) && paddleA < 66) {
        paddleA += 2;
    }
    //move paddle 2
    if (paddleBMotion == 0) {
        if (ballY < paddleB + 8 && paddleB > 0) {
            if (RNG < difficulty) {
                paddleB -= 1;
                paddleBMotion = -6;
            } else {
                paddleB += 1;
                paddleBMotion = 6;
            }
        }
        if (ballY > paddleB + 8 && paddleB < 66) {
            if (RNG < difficulty) {
                paddleB += 1;
                paddleBMotion = 6;
            } else {
                paddleB -= 1;
                paddleBMotion = -6;
            }
        }
    } else if (paddleBMotion < 0) {
        paddleBMotion++;
        if (paddleB > 0)
        paddleB -= 1;
    } else if (paddleBMotion > 0) {
        paddleBMotion--;
        if (paddleB < 66)
        paddleB += 1;
    }
    //fix paddle locations
    if (paddleA < 0) {
        paddleA = 0;
    }
    if (paddleB < 0) {
        paddleB = 0;
    }
    if (paddleA > 66) {
        paddleA = 66;
    }
    if (paddleB > 66) {
        paddleB = 66;
    }
    //move ball
    ballX += ballDX;
    ballY += ballDY;
    //bounce off walls
    if (ballX <= 0) {scoreB++; resetPongValues(); if (sound) game.sound.playOK();}
    if (ballX >= 106) {scoreA++; resetPongValues(); if (sound) game.sound.playOK();}
    if (ballX <= 4 && ballY >= paddleA && ballY <= paddleA + 16) {
        ballDX = 1;
        if (sound) game.sound.playTick();
    }
    if (ballX >= 102 && ballY >= paddleB && ballY <= paddleB + 16) {
        ballDX = -1;
        if (sound) game.sound.playTick();
    }
    if (ballY <= 0 || ballY >= 78) {ballDY *= -1; if (sound) game.sound.playTick();}
}
void pongDraw() {
    //line
    game.display.color = 2;
    game.display.drawFastVLine(55,0,82);
    game.display.color = 13;
    game.display.drawFastVLine(56,0,82);
    //paddles & ball
    game.display.drawBitmap(0,paddleA,paddle1);
    game.display.drawBitmap(106,paddleB,paddle1);
    game.display.drawBitmap(ballX,ballY,gray);
    //score
    game.display.color = 2;
    game.display.fillRectangle(0,82,110,6);
    game.display.setFont(font3x5);
    game.display.setCursor(50,83);
    game.display.color = 15;
    game.display.print(scoreA);
    game.display.print("-");
    game.display.print(scoreB);
}
void breakoutGridInit() {
    //build basic grid by row
    for (int x=0;x<13;x++) {
        breakoutGrid[x][0] = 1;
    }
    for (int x=0;x<13;x++) {
        breakoutGrid[x][1] = 3;
    }
    for (int x=0;x<13;x++) {
        breakoutGrid[x][2] = 5;
    }
    for (int x=0;x<13;x++) {
        breakoutGrid[x][3] = 6;
    }
    for (int x=0;x<13;x++) {
        breakoutGrid[x][4] = 7;
    }
}
void breakoutLogic() {
    //input
    if (game.buttons.repeat(BTN_LEFT,0) && paddleBr > 0) {
        paddleBr -= 2;
    }
    if (game.buttons.repeat(BTN_RIGHT,0) && paddleBr < 94) {
        paddleBr += 2;
    }
    //fix paddle position
    if (paddleBr < 0) paddleBr = 0;
    if (paddleBr > 94) paddleBr = 94;
    //move ball, bounce off walls & paddle
    ballX += ballDX * ballSpeed;
    ballY += ballDY * ballSpeed;
    if (ballX <= 0 || ballX >= 106) {ballDX *= -1; if (sound) game.sound.playTick();}
    if (ballY <= 0) {ballDY = 1; if (sound) game.sound.playTick();}
    if (ballY >= 78) {lives--; ballX = 52; ballY = 42; ballDX = 1; ballDY = 1; paddleBr = 46; if (sound) game.sound.playCancel();}
    if (ballY >= 74 && ballX >= paddleBr && ballX <= paddleBr + 16) {ballDY = -1; if (sound) game.sound.playTick();}
    //hiscore
    if (scoreBr > breakoutHS) {
        breakoutHS = scoreBr;
    }
    //reset game if lives out
    if (lives == 0) {
        scoreBr = 0; ballX = 52; ballY = 42; ballDX = 1; ballDY = 1; paddleBr = 46; lives = 3;
        breakoutGridInit();
    }
    //check for brick collision
    if (ballY <= 20) {
        ballYR = ballY / 4;
        if (ballYR % 2 == 0) {
            ballXR = ballX / 8;
        } else {
            ballXR = (ballX - 4) / 8;
        }
        if (ballYR <= 4) {
            switch (breakoutGrid[ballXR][ballYR]) {
            case 0 :
                break;
            case 1 :
                breakoutGrid[ballXR][ballYR] = 2;
                ballDY *= -1;
                scoreBr += 4;
                bricksHit++;
                if (ballSpeed <= 1.75f) {
                    ballSpeed = 2.0f;
                    if (sound) game.sound.playOK();
                } else {
                    if (sound) game.sound.playTick();
                }
                break;
            case 2 :
                breakoutGrid[ballXR][ballYR] = 0;
                ballDY *= -1;
                scoreBr += 4;
                bricksHit++;
                if (sound) game.sound.playTick();
                break;
            case 3 :
                breakoutGrid[ballXR][ballYR] = 4;
                ballDY *= -1;
                scoreBr += 3;
                bricksHit++;
                if (ballSpeed <= 1.5f) {
                    ballSpeed = 1.75f;
                    if (sound) game.sound.playOK();
                } else {
                    if (sound) game.sound.playTick();
                }
                break;
            case 4 :
                breakoutGrid[ballXR][ballYR] = 0;
                ballDY *= -1;
                scoreBr += 3;
                bricksHit++;
                if (sound) game.sound.playTick();
                break;
            case 5 :
                breakoutGrid[ballXR][ballYR] = 0;
                ballDY *= -1;
                scoreBr += 3;
                bricksHit++;
                if (sound) game.sound.playTick();
                break;
            case 6 :
                breakoutGrid[ballXR][ballYR] = 0;
                ballDY *= -1;
                scoreBr += 2;
                bricksHit++;
                if (sound) game.sound.playTick();
                break;
            case 7 :
                breakoutGrid[ballXR][ballYR] = 0;
                ballDY *= -1;
                scoreBr += 1;
                bricksHit++;
                if (sound) game.sound.playTick();
                break;
            }
        }
    }
    //second level
    if (scoreBr == 260 && !secondLevel) {
        secondLevel = true;
        paddleBr = 46;
        ballX = 52;
        ballY = 42;
        ballDX = 1;
        ballDY = 1;
        breakoutGridInit();
    }
    //change speeds
    if (ballSpeed == 1.0f && bricksHit == 4) {
        ballSpeed = 1.25f;
        if (sound) game.sound.playOK();
    } else if (ballSpeed <= 1.25f && bricksHit == 12) {
        ballSpeed == 1.5f;
        if (sound) game.sound.playOK();
    }
}
void breakoutDraw() {
    //draw bricks
    for(int y=0;y<5;y++) {
        for(int x=0;x<13;x++) {
            switch (breakoutGrid[x][y]) {
            case 0 :
                break;
            case 1 :
                if (y%2==0) {
                game.display.drawBitmap(x*8 + 1,y*4,redWide1);
                } else {
                game.display.drawBitmap(x*8 + 5,y*4,redWide1);
                }
                break;
            case 2 :
                if (y%2==0) {
                game.display.drawBitmap(x*8 + 1,y*4,redWide2);
                } else {
                game.display.drawBitmap(x*8 + 5,y*4,redWide2);
                }
                break;
            case 3 :
                if (y%2==0) {
                    game.display.drawBitmap(x*8 + 1,y*4,yellowWide1);
                } else {
                    game.display.drawBitmap(x*8 + 5,y*4,yellowWide1);
                }
                break;
            case 4 :
                if (y%2==0) {
                    game.display.drawBitmap(x*8 + 1,y*4,yellowWide2);
                } else {
                    game.display.drawBitmap(x*8 + 5,y*4,yellowWide2);
                }
                break;
            case 5 :
                if (y%2==0) {
                    game.display.drawBitmap(x*8 + 1,y*4,greenWide);
                } else {
                    game.display.drawBitmap(x*8 + 5,y*4,greenWide);
                }
                break;
            case 6 :
                if (y%2==0) {
                    game.display.drawBitmap(x*8 + 1,y*4,cyanWide);
                } else {
                    game.display.drawBitmap(x*8 + 5,y*4,cyanWide);
                }
                break;
            case 7 :
                if (y%2==0) {
                    game.display.drawBitmap(x*8 + 1,y*4,blueWide);
                } else {
                    game.display.drawBitmap(x*8 + 5,y*4,blueWide);
                }
                break;
            }
        }
    }
    //draw paddle & ball
    game.display.drawBitmap(paddleBr,78,paddle2);
    game.display.drawBitmap(ballX,ballY,gray);
    //draw score
    game.display.color = 2;
    game.display.fillRectangle(0,82,110,6);
    game.display.setFont(font3x5);
    game.display.setCursor(1,83);
    game.display.color = 15;
    game.display.print("SCORE ");
    game.display.print(scoreBr);
    game.display.setCursor(37,83);
    game.display.color = 15;
    game.display.print("HISCORE ");
    game.display.print(breakoutHS);
    //draw HP
    game.display.drawBitmap(97,84,gray);
    game.display.setCursor(102,83);
    game.display.print("x");
    game.display.print(lives);
}
void snakePlaceFood() {
    foodPlacementSuccess = false;
    while (!foodPlacementSuccess) {
        RNGX = rand() % 25 + 1;
        RNGY = rand() % 18 + 1;
        if (gameMap [RNGX][RNGY] == 0) {
            gameMap [RNGX][RNGY] = -2;
            foodPlacementSuccess = true;
        }
    }
}
void snakeGridInit() {
    for (int y=0;y<20;y++) {
        for (int x=0;x<27;x++) {
            gameMap [x][y] = 0;
        }
    }
    for (int y=0;y<22;y++) {
        gameMap [0][y] = -1;
        gameMap [26][y] = -1;
    }
    for (int x=0;x<27;x++) {
        gameMap [x][0] = -1;
        gameMap [x][19] = -1;
    }
    for (int i=0;i<snakeLength;i++) {
    gameMap [13][10 + i] = 1 + i;
    }
    snakePlaceFood();
    snakeInitializing = false;
}
void snakeInput() {
    //input
    if (game.buttons.held(BTN_UP,1) && direction != 2) {
        direction = 0;
    }
    if (game.buttons.held(BTN_DOWN,1) && direction != 0) {
        direction = 2;
    }
    if (game.buttons.held(BTN_RIGHT,1) && direction != 3) {
        direction = 1;
    }
    if (game.buttons.held(BTN_LEFT,1) && direction != 1) {
        direction = 3;
    }
}
void snakeLogic() {
    if (counter == 0) {
        counter = 2;
        //move
        snakeUpdate = false;
        gameMap [headX][headY] = 2;
        if (direction == 0 && gameMap [headX][headY - 1] != -1 && gameMap [headX][headY - 1] < 2) {
            headY--;
            snakeUpdate = true;
        } else if (direction == 1 && gameMap [headX + 1][headY] != -1 && gameMap [headX + 1][headY] < 2) {
            headX++;
            snakeUpdate = true;
        } else if (direction == 2 && gameMap [headX][headY + 1] != -1 && gameMap [headX][headY + 1] < 2) {
            headY++;
            snakeUpdate = true;
        } else if (direction == 3 && gameMap [headX - 1][headY] != -1 && gameMap [headX - 1][headY] < 2) {
            headX--;
            snakeUpdate = true;
        }
        //collision w/ food
        if (gameMap [headX][headY] == -2) {
            snakeLength += 1;
            snakePlaceFood();
            if (sound) game.sound.playOK();
        }
        gameMap [headX][headY] = 1;
        //destroy old snake bricks
        for (int y=0;y<20;y++) {
            for (int x=0;x<27;x++) {
                if (snakeUpdate) {
                    if (gameMap [x][y] >= snakeLength + 1) {
                        gameMap [x][y] = 0;
                    } else if (gameMap [x][y] >= 2) {
                        gameMap [x][y] += 1;
                    }
                }
                if (gameMap [x][y] == 1 && !(x == headX && y == headY)) {
                    gameMap [x][y] = 0;
                }
            }
        }
        if (snakeLength - 3 > snakeHS) {
            snakeHS = snakeLength - 3;
        }
        //death
        if (!snakeUpdate) {
            snakeLength = 3;
            headX = 13;
            headY = 10;
            snakeInitializing = true;
            snakeGridInit;
            if (sound) game.sound.playCancel();
        }
    } else {
        counter--;
    }
}
void snakeDraw() {
    //draw BG
    game.display.color = 14;
    game.display.fillRectangle(4,4,100,72);
    //draw map
    for (int y=0;y<20;y++) {
        for (int x=0;x<27;x++) {
            switch (gameMap[x][y]) {
            case -2 :
                game.display.drawBitmap(x*4,y*4,pink);
                break;
            case -1 :
                game.display.drawBitmap(x*4,y*4,red);
                break;
            case 0 :
                //game.display.drawBitmap(x*4,y*4,yellow);
                break;
            case 1 :
                game.display.drawBitmap(x*4,y*4,cyan);
                break;
            default :
                if (gameMap[x][y] % 2 == 1) {
                    game.display.drawBitmap(x*4,y*4,blue);
                } else {
                    game.display.drawBitmap(x*4,y*4,green);
                }
                break;
            }
        }

    }
    //draw score
    game.display.color = 2;
    game.display.fillRectangle(0,80,110,8);
    game.display.setFont(font3x5);
    game.display.setCursor(1,82);
    game.display.color = 15;
    game.display.print("SCORE ");
    game.display.print(snakeLength - 3);
    game.display.setCursor(65,82);
    game.display.color = 15;
    game.display.print("HISCORE ");
    game.display.print(snakeHS);
}
void stackerDataInit() {
    //initialize data list
    for (int y=0;y<2;y++) {
        for (int x=0;x<20;x++) {
            oldBlockData[x][y] = 0;
        }
    }
}
void stackerInput() {
    if (game.buttons.held(BTN_A,1)) {
            if (sound) game.sound.playTick();
            if (blockH >= 0) {
                //place old blocks
                oldBlockData[blockH - blockHOffset][0] = blockX;
                oldBlockData[blockH - blockHOffset][1] = blockSize;
                blockH += 1;
                if (blockH > 1) {
                    stackLost = false;
                    //basic block reduction algorithm
                    switch (blockX - oldBlockData[blockH - blockHOffset - 2][0]) {
                    case -3 :
                        blockLoss = 3;
                        blockMove = 3;
                        break;
                    case -2 :
                        blockLoss = 2;
                        blockMove = 2;
                        break;
                    case -1 :
                        blockLoss = 1;
                        blockMove = 1;
                        break;
                    case 0 :
                        blockLoss = 0;
                        blockMove = 0;
                        break;
                    case 1 :
                        blockLoss = 1;
                        blockMove = 0;
                        break;
                    case 2 :
                        blockLoss = 2;
                        blockMove = 0;
                        break;
                    case 3 :
                        blockLoss = 3;
                        blockMove = 0;
                        break;
                    default :
                        blockLoss = 4;
                        stackLost = true;

                    }
                    //score
                    stackerScore += (4-blockLoss);
                    //block reduction in action!
                    if (!stackLost) {
                        blockSize -= blockLoss;
                        blockX += blockMove;
                        oldBlockData[blockH - blockHOffset - 1][1] -= blockLoss;
                        oldBlockData[blockH - blockHOffset - 1][0] += blockMove;
                    }
                } else {
                    stackerScore += 4;
                }
                //scroll through blocks
                if (blockH >= 13) {
                    blockHOffset += 1;
                    for (int y=0;y<15;y++) {
                        oldBlockData[y][0] = oldBlockData[y+1][0];
                        oldBlockData[y][1] = oldBlockData[y+1][1];
                    }
                }
            }
        }
}
void stackerLogic() {
    if (counter == 0) {
        counter = stackerDelay;
        //loss
        if (stackLost || blockSize <= 0) {
            if (sound) game.sound.playCancel();
            stackerDataInit();
            blockSize = 4;
            blockDir = 1;
            blockH = 0;
            blockX = 0;
            blockHOffset = 0;
            stackerScore = 0;
            stackLost = false;
        }
        //move blocks
        blockX += blockDir;
        if (blockX == 0) {
            blockDir = 1;
        } else if (blockX == 27 - blockSize) {
            blockDir = -1;
        }
        //hiscore
        if (stackerScore > stackerHS) {
            stackerHS = stackerScore;
        }
        //speed up
        if (blockH >= 15) {
            if (sound && stackerDelay == 1) game.sound.playOK();
            stackerDelay = 0;
        } else if (blockH >= 10) {
            if (sound && stackerDelay == 2) game.sound.playOK();
            stackerDelay = 1;
        } else if (blockH >= 5) {
            if (sound && stackerDelay == 3) game.sound.playOK();
            stackerDelay = 2;
        } else {
            stackerDelay = 3;
        }
    } else {
        counter--;
    }
}
void stackerDraw() {
    //draw old blocks
    for (int indx=0;indx<22;indx++) {
        for (int a=0;a<oldBlockData[indx][1];a++) {
            switch ((blockHOffset + indx) % 6) {
            case 0 :
                game.display.drawBitmap((oldBlockData[indx][0] + a) * 4,80 - (indx + 1) * 4,red);
                break;
            case 1 :
                game.display.drawBitmap((oldBlockData[indx][0] + a) * 4,80 - (indx + 1) * 4,yellow);
                break;
            case 2 :
                game.display.drawBitmap((oldBlockData[indx][0] + a) * 4,80 - (indx + 1) * 4,green);
                break;
            case 3 :
                game.display.drawBitmap((oldBlockData[indx][0] + a) * 4,80 - (indx + 1) * 4,cyan);
                break;
            case 4 :
                game.display.drawBitmap((oldBlockData[indx][0] + a) * 4,80 - (indx + 1) * 4,blue);
                break;
            case 5 :
                game.display.drawBitmap((oldBlockData[indx][0] + a) * 4,80 - (indx + 1) * 4,pink);
                break;
            }

        }
    }
    //draw current blocks
    for (int a=0;a<blockSize;a++) {
            switch (blockH % 6) {
            case 0 :
                game.display.drawBitmap((blockX + a) * 4,80 - ((blockH - blockHOffset) + 1) * 4,red);
                break;
            case 1 :
                game.display.drawBitmap((blockX + a) * 4,80 - ((blockH - blockHOffset) + 1) * 4,yellow);
                break;
            case 2 :
                game.display.drawBitmap((blockX + a) * 4,80 - ((blockH - blockHOffset) + 1) * 4,green);
                break;
            case 3 :
                game.display.drawBitmap((blockX + a) * 4,80 - ((blockH - blockHOffset) + 1) * 4,cyan);
                break;
            case 4 :
                game.display.drawBitmap((blockX + a) * 4,80 - ((blockH - blockHOffset) + 1) * 4,blue);
                break;
            case 5 :
                game.display.drawBitmap((blockX + a) * 4,80 - ((blockH - blockHOffset) + 1) * 4,pink);
                break;
            }

    }
    //score
    game.display.color = 2;
    game.display.fillRectangle(0,80,110,8);
    game.display.setFont(font3x5);
    game.display.setCursor(1,82);
    game.display.color = 15;
    game.display.print("SCORE ");
    game.display.print(stackerScore);
    game.display.setCursor(65,82);
    game.display.color = 15;
    game.display.print("HISCORE ");
    game.display.print(stackerHS);
}
void menuLogic() {
    if (game.buttons.held(BTN_RIGHT,1)) {
        menuSelection++;
        if (sound) game.sound.playTick();
        if (menuSelection == 5) {
            menuSelection = 1;
        }
    }
    if (game.buttons.held(BTN_LEFT,1)) {
        menuSelection--;
        if (sound) game.sound.playTick();
        if (menuSelection == 0) {
            menuSelection = 4;
        }
    }
    if (game.buttons.held(BTN_A,1)) {
        if (sound) game.sound.playOK();
        switch (menuSelection) {
        case 1 :
            scoreA = 0;
            scoreB = 0;
            resetPongValues();
            gameMode = 1;
            break;
        case 2 :
            scoreBr = 0;
            secondLevel = false;
            lives = 3;
            resetPongValues();
            ballSpeed = 1.0f;
            breakoutGridInit();
            gameMode = 2;
            break;
        case 3 :
            snakeLength = 3;
            headX = 13;
            headY = 10;
            direction = 0;
            snakeUpdate = false;
            foodPlacementSuccess = false;
            snakeInitializing = true;
            snakeGridInit();
            gameMode = 3;
            break;
        case 4 :
            blockX = 0;
            blockSize = 4;
            blockDir = 1;
            blockH = 0;
            blockHOffset = 0;
            blockLoss = 0;
            blockMove = 0;
            stackLost = false;
            stackerScore = 0;
            stackerDelay = 3;
            stackerDataInit();
            gameMode = 4;
            break;
        }
    }
    if (game.buttons.held(BTN_B,1)) {
        gameMode = -1;
    }
}
void menuDraw() {
    game.display.drawBitmap(30,10,logo);
    switch (menuSelection) {
    case 1 :
        game.display.drawBitmap(28,30,pongSS);
        game.display.color = 15;
        game.display.setCursor(40,70);
        game.display.setFont(font3x5);
        game.display.print("Pong \n");
        break;
    case 2 :
        game.display.drawBitmap(28,30,breakoutSS);
        game.display.color = 15;
        game.display.setCursor(40,70);
        game.display.setFont(font3x5);
        game.display.print("Breakout \n");
        break;
    case 3 :
        game.display.drawBitmap(28,30,snakeSS);
        game.display.color = 15;
        game.display.setCursor(40,70);
        game.display.setFont(font3x5);
        game.display.print("Snake \n");
        break;
    case 4 :
        game.display.drawBitmap(28,30,stackerSS);
        game.display.color = 15;
        game.display.setCursor(40,70);
        game.display.setFont(font3x5);
        game.display.print("Stacker \n");
        break;
    }
    game.display.print("A: Play \n");
    game.display.print("B: Settings \n");

}
void optionsLogic() {
    if (game.buttons.held(BTN_UP,1)) {
        optionSelection--;
        if (sound) game.sound.playTick();
        if (optionSelection == 0) {
            optionSelection = 3;
        }
    }
    if (game.buttons.held(BTN_DOWN,1)) {
        optionSelection++;
        if (sound) game.sound.playTick();
        if (optionSelection == 4) {
            optionSelection = 1;
        }
    }
    if (game.buttons.held(BTN_A,1)) {
        if (sound) game.sound.playOK();
        switch (optionSelection) {
        case 1 :
            if (grayscale) {
                grayscale = false;
                game.display.load565Palette(color_pal);
            } else {
                grayscale = true;
                game.display.load565Palette(grayscale_pal);
            }
            break;
        case 2 :
            sound = !sound;
            break;
        case 3 :
            gameMode = 0;
            break;
        }
    }
}
void optionsDraw() {
    game.display.setCursor(0,30);
    game.display.setFont(font5x7);
    game.display.color = 15;
    game.display.print("OPTIONS \n");
    game.display.setFont(font3x5);
    if (optionSelection == 1) {
        if (grayscale) {
            game.display.color = 2;
        } else {
            game.display.color = 11;
        }
    } else {
        game.display.color = 15;
    }
    if (grayscale) {
        game.display.print("GRAYSCALE \n");
    } else {
        game.display.print("COLOR \n");
    }
    if (optionSelection == 2) {
        if (grayscale) {
            game.display.color = 2;
        } else {
            game.display.color = 11;
        }
    } else {
        game.display.color = 15;
    }
    if (sound) {
        game.display.print("SOUND \n");
    } else {
        game.display.print("MUTE \n");
    }
    if (optionSelection == 3) {
        if (grayscale) {
            game.display.color = 2;
        } else {
            game.display.color = 11;
        }
    } else {
        game.display.color = 15;
    }
    game.display.print("BACK TO MENU \n");
}
void gameTitle() {
    timeTime++;
    game.display.drawBitmap(30,25+5*sin(timeTime/15),logo);
    game.display.color = 15;
    game.display.setCursor(25,45+5*sin(timeTime/15));
    game.display.setFont(font3x5);
    game.display.print("Press A to begin");
    if (game.buttons.held(BTN_A,1)) {
        gameMode = 0;
        if (sound) game.sound.playOK();
    }
}
int main() {
    //initialization
    game.begin();
    game.display.load565Palette(color_pal);
    game.display.width = 110;
    game.display.height = 88;
    breakoutGridInit();
    snakeGridInit();
    stackerDataInit();
    game.sound.playMusicStream("MENU.SND");
    //game.sound.playMusicStream("Menu.snd");
//    columnsGridInit();
    //main game loop
    while (game.isRunning) {
        if (game.update()) {
                //update RNG
                RNG = rand() % 100 + 1;
                switch (gameMode) {
                case -2 :
                    //Title
                    gameTitle();
                    break;
                case -1 :
                    //Options
                    optionsLogic();
                    optionsDraw();
                    break;
                case 0 :
                    //Menu
                    menuLogic();
                    menuDraw();
                    break;
                case 1 :
                    //Pong
                    pongLogic();
                    pongDraw();
                    break;
                case 2 :
                    //Breakout
                    breakoutLogic();
                    breakoutDraw();
                    break;
                case 3 :
                    //Snake
                    snakeInput();
                    snakeLogic();
                    snakeDraw();
                    break;
                case 4 :
                    //Stacker
                    stackerInput();
                    stackerLogic();
                    stackerDraw();
                    break;
                case 5 :
                    //Columns
//                    columnsLogic();
//                    columnsDraw();
                    break;
                }
                if (game.buttons.held(BTN_C,1)) {
                    if (gameMode > 0) {
                        game.sound.playCancel();
                        gameMode = 0;
                    }
                }
        }
    }
}
