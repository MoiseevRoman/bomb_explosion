#define F_CPU 1000000UL
#include <xc.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned int razr1 = 0, razr2 = 0;
int z=30;
void segchar (unsigned char seg)
{
	switch (seg)
	{
		case 1: PORTB = 0b01111001; break;
		case 2: PORTB = 0b00100100; break;
		case 3: PORTB = 0b00110000; break;
		case 4: PORTB = 0b00011001; break;
		case 5: PORTB = 0b00010010; break;
		case 6: PORTB = 0b00000010; break;
		case 7: PORTB = 0b01111000; break;
		case 8: PORTB = 0b00000000; break;
		case 9: PORTB = 0b00010000; break;
		case 0: PORTB = 0b01000000; break;
	}
}
void start (void)
{
	TCCR1B &= ~(1<<CS12);
	TCCR1B |= (1<<CS11) | (1<<CS10); // деление частоты на 64 = 15625
	TIMSK |= (1<<OCIE1A); // разрешаем прерывание при совпадении
	OCR1AH = 0b00111101;
	OCR1AL = 0b00001001; // регистр сравнения 15625
	TCNT1 = 0;
	TCCR1B |= (1<<WGM12); // сброс при совпадении
}

void display(unsigned int a)
{
	int time_ = 3;		// ms
	razr1 = a/10;       
	razr2 = a%10;
	
	segchar(razr1);
	PORTA |= (1<<0);
	_delay_ms(time_);
	PORTA &= ~(1<<0);
	
	segchar(razr2);
	//PORTB |= (1<<7); //пьезоизлучатель
	PORTA |= (1<<1);
	_delay_ms(time_);
	PORTA &= ~(1<<1);
}
ISR (TIMER1_COMPA_vect)
{
	z--;
	if (z<0) z=0;
	/*if (z == 0)    // условие для пьезоизлучателя
	{
		PORTB |= (1<<7);
	}
	else
	{
		PORTB &= ~(1<<7);
	}*/ 
}

void boost(void)
{
	OCR1AH = 0b00100000;
	OCR1AL = 0b10000100;
}
int main(void)
{
	z = 51;
	DDRA = 0b00000011;
	DDRB = 0xFF;
	DDRD = 0x00;
	PORTA = 0;
	PORTB = 0;
	PORTD = 0;
	sei();
	while (1)
	{
		display(z);
		if (PIND & (1<<0)) // проверка кнопки "старт"
		{
			z = 30;
			start();
			_delay_ms(200);
		}
		if (PIND & (1<<1))
		{
			boost();
		}
		if (PIND & (1<<2))
		{
			
		}
		if (PIND & (1<<3))
		{
			z = 0;
		}
		if (PIND & (1<<4))
		{
			
		}
	}
}