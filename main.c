#define F_CPU 1000000UL
#include <xc.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <time.h>
#include <stdlib.h>
unsigned int razr1 = 0, razr2 = 0;
int z;
unsigned char flag = 0;
unsigned int res, i;
int arr[4] = {1, 2, 3, 4};
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

int random_port(int array[])
{
	srand(time(NULL));
	for (int i = 0; i < 13; i++)
	{
		// Генерируем случайно два индекса элементов
		int ind1 = rand() % 4;
		int ind2 = rand() % 4;
		// и меняем местами элементы с этими индексами
		int temp = array[ind1];
		array[ind1] = array[ind2];
		array[ind2] = temp;
	}
	return *array;
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
	for(i=0; i<30; i++)
	{
		PORTB |= (1<<7);
		_delay_ms(1);
		PORTB &= ~(1<<7);
		_delay_ms(1);
	}

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

void win(void)
{
	res = z;
}

void lose(void)
{
	res = 0;
}
int main(void)
{
	z = 51;
	DDRA = 0b00000011;
	DDRB = 0xFF;
	DDRD = 0b01000000;
	PORTA = 0;
	PORTA |= (1<<2);
	PORTB = 0;
	PORTD = 0;
	sei();
	while (1)
	{
		display(z);
		if (PIND & (1<<0)) // проверка кнопки "старт"
		{
			z = 15;
			start();
			flag = 1;
			random_port(arr);
			PORTD |= (1<<6);
			_delay_ms(100);	
		}
		if (flag == 1)
		{
			if (PIND & (1<<arr[0]))// & (DDRD & (0<<arr[0])))
			{
				DDRD |= (1<<arr[0]);
				PORTD |= (1<<arr[0]);
				boost();
			}
			if (PIND & (1<<arr[1]))// & (DDRD & (0<<arr[1])))
			{
				DDRD |= (1<<arr[1]);
				PORTD |= (1<<arr[1]);
			}
			if (PIND & (1<<arr[2]))// & (DDRD & (0<<arr[2])))
			{
				DDRD |= (1<<arr[2]);
				PORTD |= (1<<arr[2]);
				z = 0;
			}
			if (PIND & (1<<arr[3]))// & (DDRD & (0<<arr[3])))
			{
				DDRD |= (1<<arr[3]);
				PORTD |= (1<<arr[3]);
				win();
				break;
			}
		}
		if(z == 0)
		{
			lose();
			break;
		}
	}
	TCCR1B = (0<<CS11);
	TCCR1B = (0<<CS12);
	while(1)
	{
		display(res);
		_delay_ms(50);
	}
}