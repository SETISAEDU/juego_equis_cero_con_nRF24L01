 #include <SPI.h> // Libreria de SPI para la comunicacion con el modulo nRF24L01
//Librerias para el control del modulo nRF
#include <nRF24L01.h> 
#include <RF24.h>
//Librerias para el control de pantalla UTFT y Touch
#include <UTFT.h>
#include <Touch_AS.h>

// Declaracion de pines a usar para el control del touch
#define DOUT 2  /* Data out pin */
#define DIN  3  /* Data in pin */
#define DCS  5  /* Chip select */
#define DCLK 6  /* Clock pin  */

#define buzzer 13

// Declaracion de variables de valores maximos y minimos del panel touch
#define XMIN 0    
#define XMAX 4095 
#define YMIN 0    
#define YMAX 4095


/* Se declaran los objetos para el control de LCD, Touch y el nRF24L01          */
Touch_AS Touch(DCS, DCLK, DIN, DOUT); //Iniciacion del touch con sus pines de control
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);//RS, WR, CS, RESET
RF24 radio(9, 10);//Declaracion de los pines de control CE y CSN para el modulo, se define "radio"

//Declaracion de las variables externas contenidas en los ficheros de las imagenes
extern unsigned short equis[3600];
extern unsigned short cero[3600];
extern unsigned short logo[45000];
extern unsigned short mortal_kombat[76800];

extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

//Se crean las 6 direcciones posibles para los Pipes; 
//Las direcciones es un numero de 40bits para este caso se usa uint_64_t de 64bits
const uint64_t direccion[] = {0x7878787878LL, 0xB3B4B5B6F1LL};
                             // En esta parte LL significa LongLong para asegurar el dato de 64bits

//Declaracion de variables globales
bool player = false;
bool X_vector[9]= {0};
bool zero_vector[9] = {0}; 
bool busy_vector[9] = {0};

void setup() {
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  radio.begin(); //Inicio del modulo nRF24L01+
  radio.setPALevel(RF24_PA_LOW);  // Se configura a baja potencia
  radio.setChannel(100); // Apertura del canal especifico
  
  //Apertura de las lineas de comunicacion 
  radio.openWritingPipe(direccion[1]);  //
  radio.openReadingPipe(0, direccion[0]);

  radio.startListening(); //Iniciamos en modo de escucha
  
  //Calibracion del panel touch
  Touch.SetCal(XMIN, XMAX, YMIN, YMAX, 320, 240); // Raw xmin, xmax, ymin, ymax, width, height
    
  //Configuracion de la pantalla LCD
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.fillScr(VGA_WHITE);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRect(1, 1, 319, 50);
  myGLCD.setBackColor(VGA_TRANSPARENT);
  myGLCD.setColor(255, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("-----JUEGO-----", CENTER, 5);
  myGLCD.print("EQUIS - CERO", CENTER, 25);
  myGLCD.drawBitmap(10, 51, 300, 150, logo);  //imagen 300x150
  delay(4000);

  //Inicializacion del juego
  sys_reset();
}

void loop(){

  //Calculo del vector de cuadriculas llenas con equis o cero
  for(int i=0; i<9; i++){
        busy_vector[i] = X_vector[i] | zero_vector[i];
        }

  radio.startListening(); //Se reconfigura el nrf24l01+ en modo escucha

  
  if (player == 1){ //Si el jugador activo es cero

    //Impresion de contorno rojo, este indica que es el jugador cero esta activo
    myGLCD.setColor(VGA_RED);           
    myGLCD.fillRect(0, 0, 319, 2);      //Marco superior
    myGLCD.fillRect(0, 236, 319, 239);  //Marco inferior
    myGLCD.fillRect(0, 0, 2, 239);      //Marco izquierda
    myGLCD.fillRect(316, 0, 319, 239);  //Marco derecha
    
    if (Touch.Pressed()){ // Verificacion si se ha hecho presion en el panel touch
      int output_vector = 0;
      radio.stopListening(); //Se detiene el modo escucha para poder escribir
      
      //Se lee el valor actual presionado en el panel touch
      int X_Coord = Touch.X(); //Lectura del valor en X
      int Y_Coord = Touch.Y(); //Lectura del valor en Y
      delay(10);

      /* ------------------------------------ Fila 1 ------------------------------------ */
      if ((Y_Coord > 0) && (Y_Coord < 79)){   
        /* Fila 1, columna 1 */
        if (((X_Coord > 0) && (X_Coord < 106) && busy_vector[0] == 0)) //Coordenadas segmento 1
        {
            player = 0;
            zero_vector[0] = 1;
        }
  
        /* Fila 1, columna 2 */
        else if ((X_Coord > 106) && (X_Coord < 211) && busy_vector[1] == 0){ //Coordenadas segmento 2
            player = 0;
            zero_vector[1] = 1;
          }
  
       /* Fila 1, columna 3 */
        else if ((X_Coord > 211) && (X_Coord < 319) && busy_vector[2] == 0){ //Coordenadas segmento 3
            player = 0;
            zero_vector[2] = 1;
          }
      }
      /* ------------------------------------ Fila 2 ------------------------------------ */
      else if ((Y_Coord > 80) && (Y_Coord < 159)) 
      {
        /* Fila 2, columna 1 */
        if ((X_Coord > 0) && (X_Coord < 106) && busy_vector[3] == 0){ //Coordenadas segmento 4
            player = 0;
            zero_vector[3] = 1;
          }
      
        /* Fila 2, columna 2 */
        else if ((X_Coord > 106) && (X_Coord < 211) && busy_vector[4] == 0){ //Coordenadas segmento 5
            player = 0;
            zero_vector[4] = 1;
          }
        /* Fila 2, columna 3 */
        else if ((X_Coord > 211) && (X_Coord < 319) && busy_vector[5] == 0){ //Coordenadas segmento 6
            player = 0;
            zero_vector[5] = 1;
        }
      }
  
      /* ------------------------------------ Fila 3 ------------------------------------ */
      else if ((Y_Coord > 160) && (Y_Coord < 239)) //Coordenadas segmento_02
      {
        /* Fila 3, columna 1 */
        if((X_Coord > 0) && (X_Coord < 106) && busy_vector[6] == 0){ //Coordenadas segmento 7
            player = 0;
            zero_vector[6] = 1;
        }
      /* Fila 3, columna 2 */
      else if ((X_Coord > 106) && (X_Coord < 211) && busy_vector[7] == 0){ //Coordenadas segmento 8
            player = 0;
            zero_vector[7] = 1;
        }
        /* Fila 3, columna 3 */
      else if (((X_Coord > 211) && (X_Coord < 319)&& busy_vector[8] == 0) ){ //Coordenadas segmento 9
            player = 0;
            zero_vector[8] = 1;
        }
      }
      
      //Si se ha presionado el panel tactil en las coordenadas disponibles se envia por el nrf24
      if (player == 0){
        //Inicialmente se concatena el vector de ceros la variable de salida
        output_vector = (zero_vector[8]<<8)+(zero_vector[7]<<7)+(zero_vector[6]<<6)+(zero_vector[5]<<5)+(zero_vector[4]<<4)+(zero_vector[3]<<3)+(zero_vector[2]<<2)+(zero_vector[1]<<1)+(zero_vector[0]);
        output_vector = output_vector & 0x01FF; //Se aplica una mascara para evitar enviar datos invalidos, se envian los 9 bits menos significativos (0b0000000XXXXXXXXX)
        radio.write(&output_vector,sizeof(output_vector)); //Funcion de transmision de datos
      }
      delay(50);
    }
  }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
  else if (player == 0){ //Si el jugador activo es equis

    //Impresion de contorno negro, este indica que es el jugador equis esta activo
    myGLCD.setColor(VGA_BLACK);
    myGLCD.fillRect(0, 0, 319, 2);      //Marco superior
    myGLCD.fillRect(0, 236, 319, 239);  //Marco inferior
    myGLCD.fillRect(0, 0, 2, 239);      //Marco izquierda
    myGLCD.fillRect(316, 0, 319, 239);  //Marco derecha
    
    if (radio.available()){ // Comprobacion de datos recibidos desde nrf24
      int input_vector = 0;
      radio.read( &input_vector, sizeof(input_vector)); //Funcion de lectura de datos
      delay(10);
      
      /* ------------------------------------ Fila 1 ------------------------------------ */
      /* Fila 1, columna 1 */
      if (input_vector & 1){
          player = 1;
          X_vector[0] = 1;
          }
      /* Fila 1, columna 2 */
      if (input_vector & 2){ 
          player = 1;
          X_vector[1] = 1;
          }
     /* Fila 1, columna 3 */
     if (input_vector & 4){ 
          player = 1;
          X_vector[2] = 1;
          }
     /* ------------------------------------ Fila 2 ------------------------------------ */
     /* Fila 2, columna 1 */
     if (input_vector & 8){
        player = 1;
        X_vector[3] = 1;
        }
            
     /* Fila 2, columna 2 */
     if (input_vector & 16){
        player = 1;
        X_vector[4] = 1;
        }
     /* Fila 2, columna 3 */
     if (input_vector & 32){ 
        player = 1;
        X_vector[5] = 1;
        }
  
     /* ------------------------------------ Fila 3 ------------------------------------ */
     /* Fila 3, columna 1 */
     if (input_vector & 64){ 
        player = 1;
        X_vector[6] = 1;
        }
    /* Fila 3, columna 2 */
    if (input_vector & 128){ 
        player = 1;
        X_vector[7] = 1;
        }
      /* Fila 3, columna 3 */
    if (input_vector & 256){ 
       player = 1;
       X_vector[8] = 1;
      }
        delay(50);  
      }
    
    //Calculo del vector de cuadriculas llenas con equis o cero
    for(int i=0; i<9; i++){
      busy_vector[i] = X_vector[i] | zero_vector[i];
      }

    //Se escriben equis y ceros en pantalla UTFT
    refresh(X_vector, 1);
    refresh(zero_vector, 0);
    
    //Se realizan pruebas de ganador o empate
    test(X_vector, player);
    test(zero_vector, player);
    }
}

  /*Rutina de refrescamiento de escritura equis y ceros en pantalla UTFT*/
  void refresh(bool _vector[], bool _player){
    if (_vector[0] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(23, 10, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(23, 10, 60, 60, cero);
      }
    }
    if (_vector[1] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(128, 10, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(128, 10, 60, 60, cero);
      }
    }
    if (_vector[2] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(235, 10, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(235, 10, 60, 60, cero);
      }
    }

    if (_vector[3] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(23, 89, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(23, 89, 60, 60, cero);
      }
    }
    if (_vector[4] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(128, 89, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(128, 89, 60, 60, cero);
      }
    }
    if (_vector[5] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(235, 89, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(235, 89, 60, 60, cero);
      }
    }
    if (_vector[6] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(23, 170, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(23, 170, 60, 60, cero);
      }
    }
    if (_vector[7] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(128, 170, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(128, 170, 60, 60, cero);
      }
    }
    if (_vector[8] == 1){
      if (_player == 1){
        myGLCD.drawBitmap(235, 170, 60, 60, equis);
      }
      else{
        myGLCD.drawBitmap(235, 170, 60, 60, cero);
      }
    }
  }

/*Reinicializacion de partidas, reinicia vectores y dibuja cuadriculas*/
void sys_reset(){
  for(int i = 0; i < 10; i++){
    X_vector[i] = 0;
    zero_vector[i] = 0;
  }

  // Reinicia fondo
  myGLCD.clrScr();
  myGLCD.fillScr(VGA_WHITE);
  delay(50);

  //Impresion de tabla
  myGLCD.setColor(VGA_BLACK);
  myGLCD.fillRect(103, 0, 108, 239);
  myGLCD.fillRect(209, 0, 214, 239);
  myGLCD.fillRect(0, 78, 319, 82);
  myGLCD.fillRect(0, 158, 319, 162);
  delay(50);
  
  //Impresion de contorno negro
  myGLCD.setColor(VGA_BLACK);
  myGLCD.fillRect(0, 0, 319, 2);      //Marco superior
  myGLCD.fillRect(0, 236, 319, 239);  //Marco inferior
  myGLCD.fillRect(0, 0, 2, 239);      //Marco izquierda
  myGLCD.fillRect(316, 0, 319, 239);  //Marco derecha
}

/*Rutina de prueba de ganador y empate*/
void test(bool _vector[], bool _player){

  //Combinaciones posibles para ganar
  if ((_vector[0]  && _vector[1] && _vector[2] ) || (_vector[3] && _vector[4] && _vector[5]) || (_vector[6] && _vector[7] && _vector[8]) || (_vector[0] && _vector[3] && _vector[6]) || (_vector[1] && _vector[4] && _vector[7]) || (_vector[2] && _vector[5] && _vector[8]) || (_vector[4] && _vector[0] && _vector[8]) || (_vector[4] && _vector[2] && _vector[6]))
  {
    myGLCD.setBackColor(VGA_TRANSPARENT);
    myGLCD.setFont(BigFont);
    myGLCD.drawBitmap(0, 0, 320, 240, mortal_kombat);  //imagen 320x240
    if (_player == 1){
      //Si el jugador activo es equis, imprime mensaje de perdedor
      myGLCD.setColor(0, 0, 0);
      myGLCD.print("YOU LOST", 96, 105, 0);
      myGLCD.setColor(255, 255, 255);
      myGLCD.print("YOU LOST", 98, 107,0);
      delay(3000);
      }
    else{
      //Si el jugador activo es cero, imprime mensaje de ganador
      myGLCD.setColor(0, 0, 0);
      myGLCD.print("YOU WIN", 104, 105,0);
      myGLCD.setColor(255, 255, 255);
      myGLCD.print("YOU WIN", 106, 107,0);
      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);
      delay(2000);
    }
    sys_reset();
  }
  //Si todas las cuadriculas estan llenas y no hay ganador, es un empate
  else if (busy_vector[0] && busy_vector[1] && busy_vector[2] && busy_vector[3] && busy_vector[4] && busy_vector[5] && busy_vector[6] && busy_vector[7] && busy_vector[8])
  {
    //Imprime mensaje de empate
    myGLCD.setBackColor(VGA_TRANSPARENT);
    myGLCD.setFont(BigFont);
    myGLCD.drawBitmap(0, 0, 320, 240, mortal_kombat);  //imagen 320x240
    myGLCD.setColor(0, 0, 0);
    myGLCD.print("TIE", 136, 105,0);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("TIE", 138, 105,0);
    delay(3000);
    sys_reset();       
  }
}
