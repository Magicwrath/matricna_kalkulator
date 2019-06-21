#include "usart.h"
#include <avr/io.h>
#include <util/delay.h>

char keypadScan();

int main(void) {
  unsigned char FSM_state = 0x00;
  char key = 0;
  unsigned char keyPressed = 0;
  char cBroj[2];
  char operator = 0;
  int prviBroj = 0;
  int drugiBroj = 0;

  while(1) {
    switch(FSM_state) {
      //Stanje za hvatanje prvog dvocifrenog broja
    case 0x00:
      key = keypadScan();
      unsigned char i = 0;

      if(!keyPressed && key != 0) {
        usartPutChar(key);
        cBroj[i++] = key; //postfix ++, daje staru vrednost, novu stavlja posle
        keyPressed = 0xff;

        if(i == 2) {
          sscanf(cBroj, "%d", &prviBroj);
          ++FSM_state;
          i = 0;
        }
      }

      if(key == 0)
        keyPressed = 0x00;
      break;

      //Stanje za hvatanje operatora
    case 0x01:
      key = keypadScan();
      switch(key) {
      case 'A':
        operator = '+';
        usartPutChar(operator);
        FSM_state++;
        break;
      case 'B':
        operator = '-';
        usartPutChar(operator);
        FSM_state++;
        break;
      case 'C':
        operator = '*';
        usartPutChar(operator);
        FSM_state++;
        break;
      case 'D':
        operator = '/';
        usartPutChar(operator);
        FSM_state++;
        break;
      }
      break;


      //Stanje za hvatanje drugog dvocifrenog broja
    case 0x02:
      key = keypadScan();

      if(!keyPressed && key != 0) {
        usartPutChar(key);
        cBroj[i++] = key;
        keyPressed = 0xff;

        if(i == 2) {
          sscanf(cBroj, "%d", drugiBroj);
          ++FSM_state;
          i = 0;
        }
      }

      if(key == 0)
        keyPressed = 0x00;

      break;


      //Stanje za izvrsavanje operacije i ispis
    case 0x03:
      char ispis[30];
      int rezultat;

      switch(operator) {
      case '+':
        rezultat = prviBroj + drugiBroj;
        break;
      case '-':
        rezultat = prviBroj - drugiBroj;
        break;
      case '*':
        rezultat = prviBroj * drugiBroj;
        break;
      case '/':
        rezultat = prviBroj / drugiBroj;
        break;
      }

      sprintf(ispis, "=%d", rezultat);
      usartPutString(ispis);
      FSM_state = 0x00;
      break;

    }
  }

  return 0;
}


char keypadScan() {
  unsigned char row, tmp;
  char key = 0;

  MCUCR &= ~0x10; //Ukljucenje PUD = 0
  tmp = UCSR0B;
  UCSR0B &= ~0x18;
  PORTD = 0x0f; //Ukljucenje pull-up otpornika

  for(row = 0x80; row != 0x08; row >>= 1) {
    DDRD = row;
    _delay_ms(2);

    switch(row | ((~PIND)&0x0f)) {
    case 0x88:
      key = '1';
      break;
    case 0x84:
      key = '2';
      break;
    case 0x82:
      key = '3';
      break;
    case 0x81:
      key = 'A';
      break;

    case 0x48:
      key = '4';
      break;
    case 0x44:
      key = '5';
      break;
    case 0x42:
      key = '6';
      break;
    case 0x41:
      key = 'B';
      break;

    case 0x28:
      key = '7';
      break;
    case 0x24:
      key = '8';
      break;
    case 0x22:
      key = '9';
      break;
    case 0x21:
      key = 'C';
      break;

    case 0x18:
      key = '*';
      break;
    case 0x14:
      key = '0';
      break;
    case 0x12:
      key = '#';
      break;
    case 0x11:
      key = 'D';
      break;
    }
  }

  UCSR0B = tmp;
  return key;
}

