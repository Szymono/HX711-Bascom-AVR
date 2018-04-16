

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "Uart/uart.h"

#define F_CPU 8000000UL		//CPU frequency [Hz]
#define UART_BAUD_RATE 9600	//UART buad rate

#define CLK_PIN PC1
#define DATA_PIN PC0
#define LED_PIN PB5

uint32_t warto;
char buf[10];

uint32_t ReadCount(void);
void ULongToStr(uint32_t Var, char *str );

/***************************************************************/
int main()
{
	DDRB |= (1<<LED_PIN);
	DDRC &= ~(1<<DATA_PIN);
	DDRC |= (1<<CLK_PIN);

	sei(); //globalne wl przerwan
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

	//uart_puts("Test");
	while(1)
	{
		warto = ReadCount();
		ULongToStr(warto , buf); //konwersja Long na String
		uart_puts(buf); //wyslanie przez UART String'a z pomiarem
		uart_puts("\n"); //endline
		_delay_ms(1000); //pomiar co 1s
	}
}

/***************************************************************/

uint32_t ReadCount(void)
{
	uint8_t i;
	uint32_t Count;

	PORTC |= (1<<DATA_PIN);
	PORTC &= (1<<CLK_PIN);
	Count = 0;

	while(PINC && (1<<DATA_PIN)){}     //oczekiwanie na stan niski na DATA (while , kiedy stan wysoki)

	for (i=0;i<24;i++)
	{
		PORTC |= (1<<CLK_PIN);
		PORTB |= (1<<LED_PIN);
		_delay_us(1);

		Count = Count << 1;

		PORTC &= ~(1<<CLK_PIN);
		PORTB &= ~(1<<LED_PIN);
		_delay_us(1);

		if(PINC && (1<<DATA_PIN))
		{
			Count++;
		}
	}

	_delay_us(1);
	PORTC  |= (1<<CLK_PIN);
	_delay_us(1);
	PORTC  &= ~(1<<CLK_PIN);

	Count= Count ^ 0x800000;
	return Count;
}

void ULongToStr(uint32_t Var, char *str )
{
	int Len=0; //length of Var

	if(Var < 0)
	{
		str[0]=('-');
		Len = 1;
		Var=Var*(-1);
	}

	uint32_t Varlen = Var;
	while(Varlen>9)
	{
		Len++;
		Varlen /= 10;
	}

	do
	{
		str[Len--] = Var % 10 + '0';
	} while ((Var /= 10) > 0);
}
