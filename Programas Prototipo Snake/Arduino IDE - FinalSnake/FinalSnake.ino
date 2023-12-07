//Incluir Librerias
#include <LedControl.h>

//Iniciamos la estructura de Snake
typedef struct Snake Snake;
struct Snake{
  int head[2];     //La (fila, columna) de la cabeza de serpiente
  int body[40][2]; //La matriz que contiene las coordenadas (fila, columna)
  int len;         //La longitud de la serpiente 
  int dir[2];      //Dirección para mover la serpiente
};}

//Iniciamos la estructura de Apple
typedef struct Apple Apple;
struct Apple{
  int rPos; //La fila de la manzana.
  int cPos; //La columna de la manzana.
};

//Matrix de 8x8 MAX72XX
const int DIN = 12; //Pin DIN MAX72xx
const int CS = 11; //Pin CS MAX72xx
const int CLK = 10; //Pin CLK MAX72xx
LedControl lc = LedControl(DIN, CLK, CS, 1); //Inicializar parametros

//Declarar pines de los botónes
const int buttonUpPin = A0; //Pin Botón Arriba
const int buttonDownPin = A1; //Pin Botón Abajo
const int buttonLeftPin = A2; //Pin Botón Aizquierda
const int buttonRightPin = A3; //Pin Botón derecha

byte pic[8] = {0,0,0,0,0,0,0,0}; //Las 8 filas de la Matriz LED

Snake snake = {{1,5},{{0,5}, {1,5}}, 2, {1,0}}; //Inicializa un objeto serpiente (Snake)
Apple apple = {(int)random(0,8),(int)random(0,8)}; //Inicializa un objeto Manzana (Apple)

//Variables para manejar el tiempo de juego
float oldTime = 0; //seguimiento del tiempo anterior en el juego.
float timer = 0; //Seguimiento del tiempo transcurrido desde la última actualización en el juego.
float updateRate = 3; //Actualización deseada para el juego cada 3 segundos.

int i,j; //Variables enteras.

void setup() {
  
  lc.shutdown(0, false);  //El MAX72XX está en modo de ahorro de energía al inicio, tenemos que hacer una llamada de atención
  lc.setIntensity(0, 8);//Establece el brillo en valores medios.
  lc.clearDisplay(0); //Borrar la pantalla. 
  
  // Establecer pines de botones como ENTRADAS
  pinMode(buttonUpPin, INPUT); //Botón Arriba como Entrada
  pinMode(buttonDownPin, INPUT); //Botón Abajo como Entrada
  pinMode(buttonLeftPin, INPUT); //Botón Izquierda como Entrada
  pinMode(buttonRightPin, INPUT); //Botón Derecha como Entrada
}//Fin void setup

void loop() {
  float deltaTime = calculateDeltaTime(); //Devuelve la diferencia de tiempo
  timer += deltaTime; //Realizar un seguimiento del tiempo acumulado.

  //Check For Inputs
  //Dependiendo de qué botón esté presionado y la dirección actual de la serpiente.
  if (digitalRead(buttonUpPin) == HIGH && snake.dir[1] == 0) { //Verifican si el botón está siendo presionado.
    snake.dir[0] = 0; //se actualiza la dirección de movimiento de la serpiente.
    snake.dir[1] = -1;
  } else if (digitalRead(buttonDownPin) == HIGH && snake.dir[1] == 0) {
    snake.dir[0] = 0;
    snake.dir[1] = 1;
  } else if (digitalRead(buttonLeftPin) == HIGH && snake.dir[0] == 0) {
    snake.dir[0] = -1;
    snake.dir[1] = 0;
  } else if (digitalRead(buttonRightPin) == HIGH && snake.dir[0] == 0) {
    snake.dir[0] = 1;
    snake.dir[1] = 0;
  }

  // Actualizar
  if (timer > 1000 / updateRate) { //Controla la frecuencia de actualización del juego.
    timer = 0; //Variable que acumula el tiempo transcurrido desde la última actualización.
    Update(); //Actualiza
  }//Fin if

  // Render
  Render(); //Mostrar el estado actual del juego.
}//Fin void loop

float calculateDeltaTime() { //Calcula el tiempo transcurrido desde la última ejecución del bucle principal.
  float currentTime = millis(); //Obtener el tiempo actual en milisegundos
  float dt = currentTime - oldTime; //Tiempo actual y el tiempo almacenado
  oldTime = currentTime; //Tiempo actual y devuelve la diferencia de tiempo.
  return dt; //Retorno el el tiempo actual y tiempo almacenado
}//Fin float

void reset() { //Metodo para Resetear
  for (int j = 0; j < 8; j++) {
    pic[j] = 0; //Representa la pantalla LED, estableciendo todas las filas en 0.
  } //Fin for
} //Fin void reset

void Update() { //Metodo Update
  reset(); // Reset (Clear) the 8x8 LED matrix

  int newHead[2] = {snake.head[0] + snake.dir[0], snake.head[1] + snake.dir[1]};

  // Handle Borders
  if (newHead[0] == 8) {
    newHead[0] = 0;
  } else if (newHead[0] == -1) {
    newHead[0] = 7;
  } else if (newHead[1] == 8) {
    newHead[1] = 0;
  } else if (newHead[1] == -1) {
    newHead[1] = 7;
  }

  // Check If The Snake hits itself
  for (j = 0; j < snake.len; j++) {
    if (snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1]) {
      // Pause the game for 1 sec then Reset it
      delay(1000);
      snake = {{1, 5}, {{0, 5}, {1, 5}}, 2, {1, 0}}; // Reinitialize the snake object
      apple = {(int)random(0, 8), (int)random(0, 8)}; // Reinitialize an apple object
      return;
    }
  }

  // Check if The snake ate the apple
  if (newHead[0] == apple.rPos && newHead[1] == apple.cPos) {
    snake.len = snake.len + 1;
    apple.rPos = (int)random(0, 8);
    apple.cPos = (int)random(0, 8);
  } else {
    removeFirst(); // Shifting the array to the left
  }

  snake.body[snake.len - 1][0] = newHead[0];
  snake.body[snake.len - 1][1] = newHead[1];

  snake.head[0] = newHead[0];
  snake.head[1] = newHead[1];

  // Update the pic Array to Display(snake and apple)
  for (j = 0; j < snake.len; j++) {
    pic[snake.body[j][0]] |= 128 >> snake.body[j][1];
  }
  pic[apple.rPos] |= 128 >> apple.cPos;
}

void Render() {
  for (i = 0; i < 8; i++) {
    lc.setRow(0, i, pic[i]);
  }
}

void removeFirst() {
  for (j = 1; j < snake.len; j++) {
    snake.body[j - 1][0] = snake.body[j][0];
    snake.body[j - 1][1] = snake.body[j][1];
  }
}
