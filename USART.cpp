#include <Arduino.h>

#define F_CPU 16000000UL
#define BAUD 9600UL
#define TEILER F_CPU/16/BAUD-1
void usart_init(void); // initialize
void usart_putc(char c); // send character
void usart_puts(char *s); // send string (character array)
void checkOffset(uint16_t *value, int upper, int lower); // Checks offset of variable

volatile uint16_t freq = 1000;
volatile uint16_t duty = 30;

int main() {
	// Ein String
	char str[7];
	usart_init();

	sei();

	while (1) {
		// Int to ASCII mit radix 10
		itoa(freq, str, 10);
		// Sende str seriell durch UDR0
		usart_puts(str);
		// Sende Escape Sequenz TAB via UDR0
		usart_putc('\t');
		// Int to ASCII mit radix 10
		itoa(duty, str, 10);
		// Sende str seriell durch UDR0
		usart_puts(str);
		// Sende Escape Sequenz NEXT_LINE via UDR0
		usart_putc('\n');
		// Warte 1 Sekunde
		_delay_ms(1000);
	}
}

// Initialisierung
void usart_init(void) {
	// Teiler der Baudrate in register setzen
	UBRR0 = TEILER;
	// Sender einschalten
	UCSR0B |= (1 << TXEN0);
	// Empfänger einschalten
	UCSR0B |= (1 << RXEN0);
	// Interrupt beim Empfangen
	UCSR0B |= (1 << RXCIE0);
}

void usart_putc(char c) {
	// Warte bis UDR0 leer ist
	while (!(UCSR0A & (1 << UDRE0)))
		;
	// Schreibe char in UDR0 und starte damit das senden, des Registers
	UDR0 = c;
}

// iteriere über alle folgenden charakter des pointers s bis s == '\0' (null termination)
// call putc für pointer dann inkrementiere pointer
// Sendet im Prinzip einen String seriell durch UDR0 Register
void usart_puts(char *s) {
	while (*s) // *s != '\0'
	{
		usart_putc(*s);
		s++;
	}
}

// ISR für das Empfangen
ISR(USART_RX_vect) {
	char read = (char) UDR0;
	switch (read) {
	case 'w':
		freq += 100;
		break;
	case 's':
		freq -= 100;
		break;
	case 'd':
		duty += 5;
		break;
	case 'a':
		duty -= 5;
		break;
	default:
		break;
	}
	checkOffset((uint16_t*) &freq, 2500, 250);
	checkOffset((uint16_t*) &duty, 2500, 250);
}

void checkOffset(uint16_t *value, int upper, int lower) {
	if (value > upper) {
		value = upper;
	} else if (value < lower) {
		value = lower;
	}
}

// Macht das Geiche nur mit einem Index statt pointer
/*
 void usart_puts (char* s)
 {
 uint8_t i = 0;
 while (s[i] != '\0')
 {
 usart_putc(s[i]);
 i++;
 }
 }
 */
