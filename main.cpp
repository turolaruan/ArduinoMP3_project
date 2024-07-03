// C++ code
//

#include <LiquidCrystal.h>

// Define a porta 7 para o buzzer
int buzzer = 7;

// Define a porta 6 para o LED vermelho
int led_red = 6;

// Define a porta 13 para o LED verde
int led_green = 13;

// Declara variáveis para os botões
int button_left;
int button_up;
int button_right; 
int button_down;

// Marcador de tempo para debouncing dos botões
unsigned long lastDebounceTime = 0;

// Delay mínimo para debouncing dos botões
unsigned long debounceDelay = 300;

// Marcador de tempo para tocagem da nota
unsigned long lastNoteTime = 0;

// Tempo de tocagem da nota
unsigned long noteDelay = 350;

// Declara variável para armazenar a música selecionada
int selected_song;

// Declara variável para armazenar o menu selecionado
// (0 = seleção de música)
// (1 = tela de reprodução)
int menu = 0;

// Variável para deterinar se a música está pausada ou não
int paused = 0;

// Array de strings que representam as notas das músicas
const char* songs[5][72] = {
    {
      "C", "C", "G", "G", "A", "A", "G", "F", 
      "F", "E", "E", "D", "D", "C", "G", "G", 
      "F", "F", "E", "E", "D", "C", "G", "C",
      "C", "C", "G", "G", "A", "A", "G", "F", 
      "F", "E", "E", "D", "D", "C", "G", "G", 
      "F", "F", "E", "E", "D", "C", "G", "C",
      "C", "C", "G", "G", "A", "A", "G", "F", 
      "F", "E", "E", "D", "D", "C", "G", "G", 
      "F", "F", "E", "E", "D", "C", "G", "C"
	},

    {
      "E", "D", "C", "D", "E", "E", "E", "D", 
      "D", "D", "E", "G", "G", "E", "D", "C", 
      "D", "E", "E", "D", "D", "E", "D", "C",
      "E", "D", "C", "D", "E", "E", "E", "D", 
      "D", "D", "E", "G", "G", "E", "D", "C", 
      "D", "E", "E", "D", "D", "E", "D", "C",
      "E", "D", "C", "D", "E", "E", "E", "D", 
      "D", "D", "E", "G", "G", "E", "D", "C", 
      "D", "E", "E", "D", "D", "E", "D", "C"
    },
      
    {
      "C", "C", "D", "C", "F", "E", "C", "C", 
      "D", "C", "G", "F", "C", "C", "C", "A", 
      "F", "E", "D", "B", "B", "A", "F", "G",
      "C", "C", "D", "C", "F", "E", "C", "C", 
      "D", "C", "G", "F", "C", "C", "C", "A", 
      "F", "E", "D", "B", "B", "A", "F", "G",
      "C", "C", "D", "C", "F", "E", "C", "C", 
      "D", "C", "G", "F", "C", "C", "C", "A", 
      "F", "E", "D", "B", "B", "A", "F", "G"
    },
      
    {
      "E", "E", "E", "E", "E", "E", "E", "G", 
      "C", "D", "E", "F", "F", "F", "F", "F", 
      "E", "E", "E", "E", "D", "D", "E", "D",
      "E", "E", "E", "E", "E", "E", "E", "G", 
      "C", "D", "E", "F", "F", "F", "F", "F", 
      "E", "E", "E", "E", "D", "D", "E", "D",
      "E", "E", "E", "E", "E", "E", "E", "G", 
      "C", "D", "E", "F", "F", "F", "F", "F", 
      "E", "E", "E", "E", "D", "D", "E", "D"
    },
      
    {
      "E", "E", "F", "G", "G", "F", "E", "D", 
      "C", "C", "D", "E", "D", "C", "C", "D", 
      "E", "F", "E", "D", "C", "D", "E", "C",
      "E", "E", "F", "G", "G", "F", "E", "D", 
      "C", "C", "D", "E", "D", "C", "C", "D", 
      "E", "F", "E", "D", "C", "D", "E", "C",
      "E", "E", "F", "G", "G", "F", "E", "D", 
      "C", "C", "D", "E", "D", "C", "C", "D", 
      "E", "F", "E", "D", "C", "D", "E", "C"
    }
};

// Array com os nomes das músicas
String music_menu[5] = {"Music 01", "Music 02", "Music 03", "Music 04" ,"Music 05"};

// Variáveis para armazenar a próxima nota a ser reproduzida
String note_to_play = "";

// Contador de notas tocadas da música
int note_counter;

// Variável para controlar a posição da seta no menu
volatile byte arrow_pointer = LOW;

// Variável que marca a linha a ser exibida no topo do menu
int selected_menu = 0;

// Inicializa o LCD com os pinos especificados
LiquidCrystal lcd_1(12, 11, 5, 4, 3, 2);

void setup()
{
  lcd_1.begin(16, 2); // Configura o LCD 16x2
  
  // Configura os pinos dos botões como INPUT_PULLUP
  pinMode(10, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  
  // Configura os pinos do buzzer e LEDs como OUTPUT
  pinMode(buzzer, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
}

void loop()
{
  
  // Lê o estado dos botões
  button_left = digitalRead(10);
  button_up = digitalRead(9);
  button_right = digitalRead(8);
  button_down = digitalRead(A0);
  
  // Somente registra um input do botão caso o último input
  // tenha sido executado há mais tempo do que o delay
  // de debouncing
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Caso o algum botão seja apertado em intervalo permitido
    if (button_left == 0 || button_right == 0 || button_up == 0 || button_down == 0) {
  		// Atualiza o tempo do último input do botão
      	lastDebounceTime = millis();
  
  		// Se o botão esquerdo for pressionado (botão stop)
        if (button_left == 0) {
          // E se estiver no menu de música, volte ao menu principal
          // e interrompe a música
          if (menu == 1) {
            menu = 0;
            // Reseta as variáveis de reprodução
            note_to_play = "";
            note_counter = 0;
          }
        }

        // Se o botão direito for pressionado (botão play/pause)
        if (button_right == 0) {
          // Se estiver no menu de seleção
          if (menu == 0) {
            // Atualiza a música selecionada com base na seta e inicia a reprodução
            selected_song = selected_menu + arrow_pointer;
            menu = 1; // Muda para o modo de reprodução
            paused = 0; // Define como reproduzindo (não pausado)
            note_counter = 0; // Reseta o contador de notas

          // Se já estiver no modo de reprodução
          } else if (menu == 1) {
            // Se a música estiver sendo reproduzida
            if (paused == 0) {
              paused = 1; // Pausa a música
            // Se a música estiver pausada
            } else if (paused == 1) {
              paused = 0; // Retoma a reprodução
            }
          }
        }

        // Se o botão para cima for pressionado (botão up)
        if (button_up == 0) {
          // Se estiver no menu de seleção
          if (menu == 0) {
            // Se a seta estiver embaixo
            if (arrow_pointer == 1) {
              arrow_pointer = 0; // Move a seta para cima
            // Caso contrário, se a música selecionada não for a primeira
            } else if (selected_menu > 0) {
              selected_menu--; // Sobe a seleção de música
            }
          }
        }

        // Se o botão para baixo for pressionado (botão down)
        if (button_down == 0) {
          // Se estiver no menu de seleção
          if (menu == 0) {
            // Se a seta estiver no topo
            if (arrow_pointer == 0) {
              arrow_pointer = 1; // Move a seta para baixo
            // Caso contrário, se a música selecionada não for a última
            } else if (selected_menu < 3) {
              selected_menu++; // Desce a seleção de música
            }
          }
        }
    }
  }
  
  // Se estiver no menu de seleção
  if (menu == 0) {
    
    // Desliga o buzzer
    noTone(buzzer);
    
    // Posiciona o cursor no local da seta e imprime "->"
    lcd_1.setCursor(0, arrow_pointer);
    lcd_1.print("->  ");
    
    // Limpa o local oposto à seta
    lcd_1.setCursor(0, !arrow_pointer);
    lcd_1.print("    ");
    
    // Imprime as músicas a serem exibidas no menu
    lcd_1.setCursor(4, 0);
    lcd_1.print(music_menu[selected_menu]);
    lcd_1.print("  ");
    lcd_1.setCursor(4, 1);
    lcd_1.print(music_menu[selected_menu + 1]);
    lcd_1.print("  ");
    
    // Desativa os LEDs vermelho e verde
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, LOW);
  
  // Se estiver no modo de reprodução
  } else if (menu == 1) {
    
    // Limpa os primeiros caracteres de ambas as linhas
    lcd_1.setCursor(0, 0);
    lcd_1.print("  ");
    lcd_1.setCursor(0, 1);
    lcd_1.print("  ");
    lcd_1.setCursor(2, 0);
    
    // Se a música estiver sendo reproduzida (não pausada)
    if (paused == 0) {
      lcd_1.print("Reproduzindo");
      
      // Acende o LED verde (indicando reprodução)
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);
      
      // Define a nota do array a ser tocada
      note_to_play = songs[selected_song][note_counter];
      
      if ((millis() - lastNoteTime) > noteDelay) {
        
        // Interrompe a nota anterior
        noTone(buzzer);
        
        // Traduz a nota atual em sua respectiva frequência e a toca no buzzer
        if (note_to_play == "C") {
          tone(buzzer, 262);
        } else if (note_to_play == "D") {
          tone(buzzer, 294);
        } else if (note_to_play == "E") {
          tone(buzzer, 330);
        } else if (note_to_play == "F") {
          tone(buzzer, 349);
        } else if (note_to_play == "G") {
          tone(buzzer, 392);
        } else if (note_to_play == "A") {
          tone(buzzer, 440);
        } else if (note_to_play == "B") {
          tone(buzzer, 494);
        // Se a nota for "P" (pausa), aguarda 350ms sem tocar nada
        } else if (note_to_play == "P") { 
        }
      
      // Se as notas acabarem (fim do array)
        if (note_counter >= sizeof(songs[selected_song])/sizeof(songs[selected_song][0])) {
          note_counter = 0; // Reseta o contador de notas (replay)
        // Caso contrário, incrementa o contador de notas
        } else {
          note_counter++;
        }
        
        lastNoteTime = millis();
      }
      
    // Se a música estiver pausada
    } else if (paused == 1) {
      
      // Interrompe o buzzer
      noTone(buzzer);
      
      lcd_1.print(" Pausado || ");
      
      // Acende o LED vermelho (indicando pausa)
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
    }
    
    // Imprime o nome da música que está sendo reproduzida/pausada
    lcd_1.setCursor(4, 1);
    lcd_1.print(music_menu[selected_song]);
  }
}