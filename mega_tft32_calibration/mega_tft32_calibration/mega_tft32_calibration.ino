#include <UTFT.h>
#include <URTouch.h>

// Inicializa o display (Modo Paralelo 16-bits)
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);

// Inicializa o Touch com a pinagem que provou funcionar no teste serial
URTouch myTouch(6, 5, 4, 3, 2);

extern uint8_t SmallFont[];
extern uint8_t BigFont[];

void setup() {
  // Inicializa o LCD
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();

  // Inicializa o Touch
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);

  // Desenha a interface inicial
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("Leitor Coordenadas", CENTER, 10);
  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_GRAY);
  myGLCD.print("Toque para testar a precisao", CENTER, 35);
  
  myGLCD.setColor(VGA_BLUE);
  myGLCD.drawRect(0, 50, 319, 239);
}

void loop() {
  long xRaw, yRaw;
  int xReal, yReal;

  if (myTouch.dataAvailable()) {
    myTouch.read();
    xRaw = myTouch.getX();
    yRaw = myTouch.getY();

    // Filtro básico para garantir que há um toque real
    if (xRaw != -1 && yRaw != -1) {
      
      // --- MAPEAMENTO PERSONALIZADO COM BASE NO SEU MONITOR SERIAL ---
      // Sintaxe: map(valor_lido, minimo_lido, maximo_lido, minimo_tela, maximo_tela)
      xReal = map(xRaw, 24, 302, 0, 319);
      yReal = map(yRaw, 10, 227, 0, 239);

      // Garante que nenhum cálculo ultrapasse as bordas da tela (0-319 e 0-239)
      xReal = constrain(xReal, 0, 319);
      yReal = constrain(yReal, 0, 239);

      // --- SOLUÇÃO DO CONFLITO DE TIMING ---
      delay(5); 
      
      myGLCD.setFont(BigFont);
      
      // Apaga a região dos números anteriores
      myGLCD.setColor(VGA_BLACK);
      myGLCD.fillRect(20, 100, 300, 150);
      
      // Desenha o valor de X Corrigido
      myGLCD.setColor(VGA_GREEN);
      myGLCD.print("X = ", 50, 100);
      myGLCD.printNumI(xReal, 110, 100);
      
      // Desenha o valor de Y Corrigido
      myGLCD.setColor(VGA_RED);
      myGLCD.print("Y = ", 50, 130);
      myGLCD.printNumI(yReal, 110, 130);
      
      // Desenha o ponto indicador (Agora usando as coordenadas reais mapeadas)
      // Limitado para desenhar apenas abaixo da linha azul do título (y > 55)
      if (yReal > 55 && yReal < 235 && xReal > 5 && xReal < 315) {
        myGLCD.setColor(VGA_YELLOW);
        myGLCD.drawPixel(xReal, yReal);
        myGLCD.drawPixel(xReal+1, yReal);
        myGLCD.drawPixel(xReal, yReal+1);
        myGLCD.drawPixel(xReal+1, yReal+1);
      }
      
      delay(150); 
    }
  }
}