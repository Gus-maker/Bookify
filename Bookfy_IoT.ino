#include <Keypad.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64);

const int TAMANHO = 100; // tamamnho da mensagem do Json
String ISBN =""; // lista para capturar os códigos ISBN
int estoque = 0; //variavel que armazena o número de livros da box
int repeat_message = 0; //variavel para controlar o loop de mensagem do pré cadastro do usuário com o usuário
int repeat_message1 = 0; //variavel para controlar o loop da mensagem inicial do programa com o usuário
int led_pin = 13; //pino do led
bool led_status = HIGH; // led ligado
float i = 0; //indice da lista
const byte ROWS = 4; //4 linhas
const byte COLS = 3; //2 colunas
char Key_ISBN[ROWS][COLS] = {
  {'3', '2', '1'},
  {'6', '5', '4'},
  {'9', '8', '7'},
  {'#', '0', '*'}
}; //caracteres da lista
byte rowPins[ROWS] = {2, 3, 4, 5}; // pinos das linhas conectadas do teclao númerico
byte colPins[COLS] = {6, 7, 8}; // pinos das colunas conectadas do teclado númerico
Keypad keypad( makeKeymap(Key_ISBN), rowPins, colPins, sizeof(rowPins), sizeof(colPins) ); //mapeando o teclado númerico

void setup() {
  Serial.begin(9600); //iniciando porta serial


  //inicializa display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x64


  //texto de boot
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);
display.println("Ola sou a box do Bookfy");
display.display();
delay(1000);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);
display.println("Para doar um livro");
display.setCursor(0, 30);
display.println("Insira o ISBN");
display.setCursor(0, 45);
display.println("Ele fica na traseira do seu livro");
display.display();
delay(1000);


  pinMode(led_pin, OUTPUT);

}

void loop() {
//    Serial.print(led_pin);
  char key = keypad.getKey(); //variavel para capturar a tecla digitada
  led_status = LOW; // desliga o led


  if (key != NO_KEY && i < 13 && key != '#' && key != '*') // se houver botão pressionado e o vetor tiver menos de 13 caracteres ele acesse esse if
  {
    //    Serial.println(key);//printa o número
    word(key);
    ISBN += key; // adiciona na string do Código ISBN
    Serial.println(ISBN);
    StaticJsonDocument<TAMANHO> json; //cria o objeto Json
    int sensorValue = key - 48 ; //  - 48 Passa o valor do Keypad para o valor real da tecla pressionada, ao digitar a o valor seria "49", caso o usuário pressionasse "1".
    boolean sensorValue2 = led_status; // Passa o valor do  estado do led que informa se está ligado ou não

    ////formato de leitura no node-red
    json["Key"] = sensorValue; // Tecla e seu valor
    json["Led"] = sensorValue2; // Led e seu estado
    serializeJson(json, Serial);
    Serial.println();
    analogWrite(led_pin, led_status);
    i++;//assim que adicionado o número, aumenta um no indice da lista
  }
  if (key == '#' && i < 13) { // tecla volta
    i--; // retira um do índice para voltar na posição da última tecla
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Volta");
    display.display();
    ISBN.remove(i, 1); // Usa o índice e retira a última tecla pressionada
  }

  if ( i == 13 && key == '*' ) { // If para cadastro
    StaticJsonDocument<TAMANHO> json; //cria o objeto Json
//    String sensorValue = ISBN; // Passa o valor do Keypad para o valor real da tecla pressionada, ao digitar a o valor seria "49", caso o usuário pressionasse "1".
    bool sensorValue2 = led_status; // Passa o valor do  estado do led que informa se está ligado ou não

    ////formato de leitura no node-red
//    json["ISBN"] = ISBN; // código do livro
    json["Led"] = sensorValue2;// Tecla e seu valor
    
    serializeJson(json, Serial);
    Serial.println();
    analogWrite(led_pin, led_status);
    estoque++; //Adiciona um livro ao estoque
    adicionar_ao_estoque(estoque); // Printa a mensagem, sinalizando que o livro foi adicionado
    delay(1000);
    ISBN = ""; // Esvazia o ISBN
    i = 0; // zera o índice
    repeat_message = 0; // zera o repetidor para uma próxima mensagem de pré cadastro
//    serializeJson(json, Serial);
  }

    
  if (i == 13 && key == '#') { // if para apagar o ISBN e reiniciar
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("ISBN apagando,digite novamente");
    display.display();
    ISBN = ""; // esvazia o ISBN
    i = 0; // Volta o índice ao seu ínicio
    repeat_message = 0; // zera o repetidor para uma próxima mensagem de pré cadastro
  }


  while (i == 13 && repeat_message == 0) { // Quando o ISBN está completo, printa a mensagem de pré cadastro para o usuário apagar o ISBN ou finalizar o cadastro
    pre_cadastro(ISBN); // função de pré cadastro
    StaticJsonDocument<TAMANHO> json; //cria o objeto Json

    int sensorValue = key - 48; // Passa o valor do Keypad para o valor real da tecla pressionada, ao digitar a o valor seria "49", caso o usuário pressionasse "1".
    bool sensorValue2 = led_status; //Passa o valor do  estado do led que informa se está ligado ou não

    ////formato de leitura no node-red
    json["Key"] = sensorValue; // Tecla e seu valor
    json["Led"] = sensorValue2;// Led e seu estado

    serializeJson(json, Serial);
    Serial.println();
    analogWrite(led_pin, led_status);
    repeat_message++; //adiciona um porque a imagem já foi repetida, assim sai do while acima
    break;
  }

}

String pre_cadastro(String ISBN) {
  led_status = HIGH;
  digitalWrite(led_pin, led_status); //liga led
  Serial.print(led_pin);
  return ISBN;
}


//Função que cadastra o livro e adiciona ele ao estoque


int adicionar_ao_estoque (int estoque) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Livro cadastrado!");
  display.setCursor(0, 30);
  display.println("Estoque:");
  display.setCursor(0, 40);
  display.println(estoque);
  display.display();
  Serial.println(led_pin);
  led_status = HIGH;
  digitalWrite(led_pin, led_status); //liga led
  Serial.println(led_pin);
  return estoque;
}



//Referência
// Keypad documentação da lib https://playground.arduino.cc/Code/Keypad/
//  Função de remover https://www.arduino.cc/reference/pt/language/variables/data-types/string/functions/remove/
//  Função para transformar váriavel em palavra https://www.arduino.cc/reference/pt/language/variables/conversion/wordcast/
// while https://www.arduino.cc/reference/pt/language/structure/control-structure/while/
// break https://www.arduino.cc/reference/pt/language/structure/control-structure/break/
