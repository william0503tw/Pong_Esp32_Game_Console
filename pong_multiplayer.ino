#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB

// hardware config
#define VRx 34
#define VRy 35
#define ButtonRight 33
#define ButtonLeft 32
#define ButtonUp 26
#define ButtonDown 25
#define ButtonSelect 4
#define ButtonStart 27

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library


//--------------------------
int16_t h = 240;
int16_t w = 320;

int dly = 5;

int16_t paddle_h = 30;
int16_t paddle_w = 4;

int16_t lpaddle_x = 0;
int16_t rpaddle_x = w - paddle_w;

int16_t lpaddle_y = 0;
int16_t rpaddle_y = h - paddle_h;

int16_t lpaddle_d = 1;
int16_t rpaddle_d = -1;

int16_t lpaddle_ball_t = w - w / 4;
int16_t rpaddle_ball_t = w / 4;

int16_t target_y = 0;

int16_t speed = 2 ;

int16_t ball_x = 2;
int16_t ball_y = 2;
int16_t oldball_x = 2;
int16_t oldball_y = 2;

int16_t ball_dx = -1;
int16_t ball_dy = -1;

int16_t ball_w = 7;
int16_t ball_h = 7;

int16_t dashline_h = 4;
int16_t dashline_w = 2;
int16_t dashline_n = h / dashline_h;
int16_t dashline_x = w / 2 - 1;
int16_t dashline_y = dashline_h / 2;

int16_t lscore = 0;
int16_t rscore = 0;
uint8_t target_score = 11 ;


bool startFlag = 1 ;
bool modeChosen = 0 ;

bool MULTIPLAYER = 0 ;

//--------------------------------

void setup(void) {
  // Game Console Setup
  pinMode(ButtonDown, INPUT_PULLUP);
  pinMode(ButtonUp, INPUT_PULLUP);
  pinMode(ButtonLeft, INPUT_PULLUP);
  pinMode(ButtonRight, INPUT_PULLUP);
  pinMode(ButtonStart, INPUT_PULLUP);
  pinMode(ButtonSelect, INPUT_PULLUP);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  //
   
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(WHITE);
  menu();
  initgame();
  tft.setTextColor(WHITE, BLACK);
}

void loop() {
  delay(dly);
  lpaddle();
  rpaddle();
  midline();
  ball();
  showScore();
}

void initgame() {
  if(MULTIPLAYER){
    ball_x = 160 ;
    ball_y = 120;
  }else{
    ball_x = 280 ;
    ball_y = 30;
  }
  tft.fillScreen(BLACK);
  tft.fillRect(ball_x, ball_y , ball_w, ball_h, WHITE);
  tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, WHITE); 
  tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, WHITE); 
  midline();
  showScore();
  // ball is placed on the center of the left paddle


  while(!startFlag){
    checkScore();
    if(digitalRead(ButtonSelect) == LOW || digitalRead(ButtonStart) == LOW){
      startFlag = 1 ;
      delay(10);
    }
  }
}

void midline() {

  // If the ball is not on the line then don't redraw the line
  if ((ball_x<dashline_x-ball_w) && (ball_x > dashline_x+dashline_w)) return;

  tft.startWrite();

  // Quick way to draw a dashed line
  tft.setAddrWindow(dashline_x, 0, dashline_w, h);
  
  for(int16_t i = 0; i < dashline_n; i+=2) {
    tft.pushColor(WHITE, dashline_w*dashline_h); // push dash pixels
    tft.pushColor(BLACK, dashline_w*dashline_h); // push gap pixels
  }

  tft.endWrite();
}

void lpaddle() {
  if(analogRead(VRy) <= 240){
    lpaddle_d = -1 ;
  }else if(analogRead(VRy) >= 3500){
    lpaddle_d = 1 ;
  }else{
    lpaddle_d = 0 ;
  }
  if (lpaddle_d == 1) {
    tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, speed, BLACK);
  } 
  else if (lpaddle_d == -1) {
    tft.fillRect(lpaddle_x, lpaddle_y + paddle_h - 1, paddle_w, speed, BLACK);
  }

  // Boundary restriction
  if (lpaddle_y + paddle_h >= h && lpaddle_d == 1) lpaddle_d = 0; // Upper
  else if (lpaddle_y <= 0 && lpaddle_d == -1) lpaddle_d = 0;      // Lower

  // Update Y Position
  lpaddle_y = lpaddle_y + lpaddle_d * speed;

  tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, WHITE); 
}

void rpaddle() {
  if(MULTIPLAYER){
    if(digitalRead(ButtonUp) == LOW){
      rpaddle_d = -1 ;
    }else if(digitalRead(ButtonDown) == LOW){
      rpaddle_d = 1 ;
    }else{
      rpaddle_d = 0 ;
    }

    if (rpaddle_d == 1) {
      tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, speed, BLACK);
    } 
    else if (rpaddle_d == -1) {
      tft.fillRect(rpaddle_x, rpaddle_y + paddle_h - 1, paddle_w, speed, BLACK);
    }

    // Boundary restriction
    if (rpaddle_y + paddle_h >= h && rpaddle_d == 1) rpaddle_d = 0; // Upper
    else if (rpaddle_y <= 0 && rpaddle_d == -1) rpaddle_d = 0;      // Lower

    // Update Y Position
    rpaddle_y = rpaddle_y + rpaddle_d * speed;

    tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, WHITE); 


  }else{


    if (rpaddle_d == 1) {
      tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, speed, BLACK);
    } 
    else if (rpaddle_d == -1) {
      tft.fillRect(rpaddle_x, rpaddle_y + paddle_h - 1, paddle_w, speed, BLACK);
    }

  rpaddle_y = rpaddle_y + rpaddle_d * speed;

  if (ball_dx == -1) rpaddle_d = 0;
  else {
    if (rpaddle_y + paddle_h / 2 == target_y) rpaddle_d = 0;
    else if (rpaddle_y + paddle_h / 2 > target_y) rpaddle_d = -1;
    else rpaddle_d = 1;
  }

  if (rpaddle_y + paddle_h >= h && rpaddle_d == 1) rpaddle_d = 0;
  else if (rpaddle_y <= 0 && rpaddle_d == -1) rpaddle_d = 0;

  tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, WHITE);

  }
}



void calc_target_y() {
  // Calculate target Y
  int16_t target_x;
  int16_t reflections;
  int16_t y;

  if (ball_dx == 1) {
    target_x = w - ball_w;
  }else {
    // ball_dx == -1
    target_x = -1 * (w - ball_w);
  }

  y = abs(target_x * (ball_dy / ball_dx) + ball_y);

  reflections = floor(y / h);

  if (reflections % 2 == 0) {
    target_y = y % h;
  } 
  else {
    target_y = h - (y % h);
  }
}

void ball() {
  // Update new position
  ball_x = ball_x + ball_dx;
  ball_y = ball_y + ball_dy;

  // -1 --------- 1 (x)
  //  |
  //  |
  //  |
  //  |
  // 1 (y)

  if (ball_dx == -1 && ball_x == paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h) {
    // direction -> left
    ball_dx = ball_dx * -1; // change direction
    dly = 5; 

    if(!MULTIPLAYER) calc_target_y(); 
    

  } else if (ball_dx == 1 && ball_x + ball_w == w - paddle_w && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h) {
    ball_dx = ball_dx * -1;
    dly = 5; 
    
    if(!MULTIPLAYER)calc_target_y(); 

  } else if ((ball_dx == 1 && ball_x >= w) || (ball_dx == -1 && ball_x + ball_w < 0) ) {
    // reset ball
    if(ball_dx == 1){
      lscore++ ;
    }else if(ball_dx == -1){
      rscore++ ;
    }
    startFlag = 0 ;
    initgame();
  }

  if (ball_y > h - ball_w || ball_y < 0) {
    ball_dy = ball_dy * -1;
    ball_y += ball_dy; // Keep in bounds
  }

  //tft.fillRect(oldball_x, oldball_y, ball_w, ball_h, BLACK);
  tft.drawRect(oldball_x, oldball_y, ball_w, ball_h, BLACK); // Less TFT refresh aliasing than line above for large balls
  tft.fillRect(   ball_x,    ball_y, ball_w, ball_h, WHITE);
  oldball_x = ball_x;
  oldball_y = ball_y;
}

void showScore(){
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(WHITE, WHITE);
  tft.drawString(String(lscore, DEC), w/2 - 20, 10 , 4);
  tft.drawString(String(rscore, DEC), w/2 + 20, 10 , 4);
}

void checkScore(){
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(BLACK, WHITE);
  if(lscore == target_score || rscore == target_score){
    if(lscore == target_score){
        tft.fillScreen(WHITE);
        tft.drawString("<- You Win", w/2 -30, h/2 - 20, 4);
        tft.drawString("BAD BAD->", w/2 + 30, h/2 + 20, 4);
    }else if(rscore == target_score){
        tft.fillScreen(WHITE);
        tft.drawString("You Win ->", w/2 + 30, h/2 - 20, 4);
        tft.drawString("<- BAD BAD", w/2 - 30, h/2 + 20, 4);
    }
    tft.drawString("Press START restart the game", w/2, 220, 2);
    while(true){
      if(digitalRead(ButtonStart) == LOW){
        break ;
      }
    }
    lscore = 0 ;
    rscore = 0 ;
    initgame();
  }
}

void menu(){
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(BLACK, WHITE);

  tft.drawString("MULTIPLAYER", w/2, h/2 - 20, 4);
  tft.drawString("SINGLE", w/2, h/2 + 20, 4);

  tft.drawString("Press UP or DOWN button", w/2, 220, 2);

  while(!modeChosen){
    if(digitalRead(ButtonUp) == LOW){
      MULTIPLAYER = 1 ;
      break ;
    }else if(digitalRead(ButtonDown) == LOW){
      MULTIPLAYER = 0 ;
      break ; 
    }   
  }
  tft.fillScreen(WHITE);
  tft.drawString("First to reach", w/2, h/2 - 20, 4);
  tft.drawString("11 points wins!", w/2, h/2 + 20, 4);
  delay(3000);
}

