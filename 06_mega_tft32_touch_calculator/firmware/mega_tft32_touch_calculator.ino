#include <UTFT.h>
#include <URTouch.h>
#include <math.h>

// --------------------------------------------------
// AJUSTE O MODELO CONFORME O QUE FUNCIONOU NO SEU SETUP
// Exemplos já usados no seu contexto:
// UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
// ou
// UTFT myGLCD(CTE32_R2, 38, 39, 40, 41);
// --------------------------------------------------
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch myTouch(6, 5, 4, 3, 2);

extern uint8_t SmallFont[];
extern uint8_t BigFont[];

const int SCREEN_W = 320;
const int SCREEN_H = 240;

const int DISPLAY_X = 10; // Coluna onde inicia a caixas do display de resultados.
const int DISPLAY_Y = 27; // Linha superior da caixa de display de resultados
const int DISPLAY_W = 300;  // Largura do Display
const int DISPLAY_H = 35; // Altura do Display

const int BTN_COLS = 4;
const int BTN_ROWS = 5;
const int BTN_W = 70;
const int BTN_H = 30;
const int BTN_GAP = 6;
const int BTN_START_X = 10;
const int BTN_START_Y = 70;

String currentInput = "0";
double storedValue = 0.0;
char currentOp = 0;
bool newInput = true;
bool errorState = false;

struct Button {
  int x;
  int y;
  int w;
  int h;
  const char* label;
};

Button buttons[] = {
  {10,  70, 70, 30, "7"},
  {86,  70, 70, 30, "8"},
  {162, 70, 70, 30, "9"},
  {238, 70, 70, 30, "/"},

  {10,  106, 70, 30, "4"},
  {86,  106, 70, 30, "5"},
  {162, 106, 70, 30, "6"},
  {238, 106, 70, 30, "*"},

  {10,  142, 70, 30, "1"},
  {86,  142, 70, 30, "2"},
  {162, 142, 70, 30, "3"},
  {238, 142, 70, 30, "-"},

  {10,  178, 70, 30, "0"},
  {86,  178, 70, 30, "."},
  {162, 178, 70, 30, "="},
  {238, 178, 70, 30, "+"},

  {10,  214, 146, 20, "C"},
  {162, 214, 146, 20, "SQRT"}
};

const int BUTTON_COUNT = sizeof(buttons) / sizeof(buttons[0]);

// --------------------------------------------------
// AJUSTE ESTES LIMITES CONFORME SUA CALIBRACAO REAL
// Baseados no padrão já validado no seu contexto
// xReal = map(xRaw, 24, 302, 0, 319);
// yReal = map(yRaw, 10, 227, 0, 239);
// --------------------------------------------------
int mapTouchX(int rawX) {
  int x = map(rawX, 24, 302, 0, 319);
  return constrain(x, 0, 319);
}

int mapTouchY(int rawY) {
  int y = map(rawY, 10, 227, 0, 239);
  return constrain(y, 0, 239);
}

void drawDisplayBox() {
  myGLCD.setColor(220, 220, 220);
  myGLCD.fillRoundRect(DISPLAY_X, DISPLAY_Y, DISPLAY_X + DISPLAY_W, DISPLAY_Y + DISPLAY_H);

  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect(DISPLAY_X, DISPLAY_Y, DISPLAY_X + DISPLAY_W, DISPLAY_Y + DISPLAY_H);

  myGLCD.setColor(80, 80, 80);
  myGLCD.drawLine(DISPLAY_X + 5, DISPLAY_Y + DISPLAY_H - 12, DISPLAY_X + DISPLAY_W - 5, DISPLAY_Y + DISPLAY_H - 12);
}

void clearDisplayTextArea() {
  myGLCD.setColor(220, 220, 220);
  myGLCD.fillRect(DISPLAY_X + 4, DISPLAY_Y + 4, DISPLAY_X + DISPLAY_W - 4, DISPLAY_Y + DISPLAY_H - 14);
}

String formatNumber(double value) {
  char buffer[32];

  if (fabs(value - (long)value) < 0.000001) {
    ltoa((long)value, buffer, 10);
    return String(buffer);
  }

  dtostrf(value, 0, 4, buffer);
  String s = String(buffer);
  s.trim();

  while (s.endsWith("0")) {
    s.remove(s.length() - 1);
  }
  if (s.endsWith(".")) {
    s.remove(s.length() - 1);
  }

  return s;
}

void updateDisplay() {
  clearDisplayTextArea();
  //myGLCD.setBackColor(220, 220, 220);
  myGLCD.setBackColor(VGA_TRANSPARENT); // Cor de fundo do display de resultados

  if (errorState) {
    myGLCD.setColor(255, 0, 0);
    myGLCD.setFont(BigFont);
    myGLCD.print("ERRO", 20, 30); // Mensagem de erro
    return;
  }

  String shown = currentInput;
  if (shown.length() > 16) {
    shown = shown.substring(shown.length() - 16);
  }

  myGLCD.setColor(0, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print(shown, 20, 30);

  myGLCD.setFont(SmallFont);

  int opX = 245;
  int opY = 15;

  String clearText = "Op: /";   // tamanho máximo fixo
  int clearWidth  = clearText.length() * myGLCD.getFontXsize();
  int clearHeight = myGLCD.getFontYsize();

  // limpa a área máxima necessária
  myGLCD.setColor(220, 220, 220);
  myGLCD.fillRect(opX, opY, opX + clearWidth, opY + clearHeight);

  if (currentOp != 0) {
    myGLCD.setBackColor(220, 220, 220);
    myGLCD.setColor(0, 100, 180);

    String opText = "Op: ";
    opText += currentOp;
    myGLCD.print(opText, opX, opY);
  }
}

void drawButton(Button b, word fillColor, word textColor) {
  myGLCD.setColor(fillColor);
  myGLCD.fillRoundRect(b.x, b.y, b.x + b.w, b.y + b.h);

  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect(b.x, b.y, b.x + b.w, b.y + b.h);

  myGLCD.setBackColor(fillColor);
  myGLCD.setColor(textColor);

  if (strcmp(b.label, "SQRT") == 0 || strcmp(b.label, "C") == 0) {
    myGLCD.setFont(SmallFont);
    int tx = b.x + 8;
    int ty = b.y + 4;
    myGLCD.print((char*)b.label, tx, ty);
  } else {
    myGLCD.setFont(BigFont);
    int tx = b.x + 24;
    int ty = b.y + 6;
    myGLCD.print((char*)b.label, tx, ty);
  }
}

void drawUI() {
  myGLCD.clrScr();

  myGLCD.setColor(30, 30, 30);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("Calculadora", 70, 0);

  drawDisplayBox();

  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (strcmp(buttons[i].label, "/") == 0 ||
        strcmp(buttons[i].label, "*") == 0 ||
        strcmp(buttons[i].label, "-") == 0 ||
        strcmp(buttons[i].label, "+") == 0 ||
        strcmp(buttons[i].label, "=") == 0) {
      drawButton(buttons[i], VGA_AQUA, VGA_BLACK);
    }
    else if (strcmp(buttons[i].label, "C") == 0) {
      drawButton(buttons[i], VGA_RED, VGA_WHITE);
    }
    else if (strcmp(buttons[i].label, "SQRT") == 0) {
      drawButton(buttons[i], VGA_GREEN, VGA_BLACK);
    }
    else {
      drawButton(buttons[i], VGA_SILVER, VGA_BLACK);
    }
  }

  updateDisplay();
}

int getPressedButton(int x, int y) {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (x >= buttons[i].x && x <= (buttons[i].x + buttons[i].w) &&
        y >= buttons[i].y && y <= (buttons[i].y + buttons[i].h)) {
      return i;
    }
  }
  return -1;
}

void resetCalculator() {
  currentInput = "0";
  storedValue = 0.0;
  currentOp = 0;
  newInput = true;
  errorState = false;
  updateDisplay();
}

double getCurrentValue() {
  return currentInput.toFloat();
}

void setCurrentValue(double value) {
  currentInput = formatNumber(value);
  newInput = true;
}

void inputDigit(const char* digit) {
  if (errorState) {
    resetCalculator();
  }

  if (newInput) {
    currentInput = "";
    newInput = false;
  }

  if (strcmp(digit, ".") == 0) {
    if (currentInput.indexOf('.') == -1) {
      if (currentInput.length() == 0) currentInput = "0";
      currentInput += ".";
    }
  } else {
    if (currentInput == "0") currentInput = "";
    if (currentInput.length() < 16) {
      currentInput += digit;
    }
  }

  if (currentInput.length() == 0) currentInput = "0";
  updateDisplay();
}

void executePendingOperation() {
  double currentValue = getCurrentValue();
  double result = storedValue;

  switch (currentOp) {
    case '+':
      result = storedValue + currentValue;
      break;
    case '-':
      result = storedValue - currentValue;
      break;
    case '*':
      result = storedValue * currentValue;
      break;
    case '/':
      if (fabs(currentValue) < 0.0000001) {
        errorState = true;
        updateDisplay();
        return;
      }
      result = storedValue / currentValue;
      break;
    default:
      result = currentValue;
      break;
  }

  currentInput = formatNumber(result);
  storedValue = result;
  newInput = true;
  updateDisplay();
}

void handleOperator(char op) {
  if (errorState) {
    resetCalculator();
    return;
  }

  if (!newInput && currentOp != 0) {
    executePendingOperation();
    if (errorState) return;
  } else {
    storedValue = getCurrentValue();
  }

  currentOp = op;
  newInput = true;
  updateDisplay();
}

void handleEquals() {
  if (errorState) {
    resetCalculator();
    return;
  }

  if (currentOp != 0) {
    executePendingOperation();
    currentOp = 0;
    updateDisplay();
  }
}

void handleSqrt() {
  if (errorState) {
    resetCalculator();
    return;
  }

  double value = getCurrentValue();

  if (value < 0) {
    errorState = true;
    updateDisplay();
    return;
  }

  double result = sqrt(value);
  currentInput = formatNumber(result);

  if (currentOp == 0) {
    storedValue = result;
  }

  newInput = true;
  updateDisplay();
}

void processButton(const char* label) {
  if (strcmp(label, "0") == 0 || strcmp(label, "1") == 0 ||
      strcmp(label, "2") == 0 || strcmp(label, "3") == 0 ||
      strcmp(label, "4") == 0 || strcmp(label, "5") == 0 ||
      strcmp(label, "6") == 0 || strcmp(label, "7") == 0 ||
      strcmp(label, "8") == 0 || strcmp(label, "9") == 0 ||
      strcmp(label, ".") == 0) {
    inputDigit(label);
  }
  else if (strcmp(label, "+") == 0) handleOperator('+');
  else if (strcmp(label, "-") == 0) handleOperator('-');
  else if (strcmp(label, "*") == 0) handleOperator('*');
  else if (strcmp(label, "/") == 0) handleOperator('/');
  else if (strcmp(label, "=") == 0) handleEquals();
  else if (strcmp(label, "C") == 0) resetCalculator();
  else if (strcmp(label, "SQRT") == 0) handleSqrt();
}

void setup() {
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);

  drawUI();
}

void loop() {
  if (myTouch.dataAvailable()) {
    myTouch.read();

    int rawX = myTouch.getX();
    int rawY = myTouch.getY();

    if (rawX != -1 && rawY != -1) {
      int x = mapTouchX(rawX);
      int y = mapTouchY(rawY);

      delay(5);

      int btn = getPressedButton(x, y);
      if (btn >= 0) {
        processButton(buttons[btn].label);

        while (myTouch.dataAvailable()) {
          myTouch.read();
          delay(5);
        }

        delay(180);
      }
    }
  }
}