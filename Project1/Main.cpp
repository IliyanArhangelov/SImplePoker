#include <iostream>
#include <cstdlib>
#include <ctime>


const char faces[] = {'7', '8', '9', 'T', 'J', 'K', 'Q', 'A'};
const unsigned FACESCOUNT = 8;
const char symbols[] = {'S', 'H', 'D', 'C'};
const unsigned SYMBOLSCOUNT = 4;

const unsigned DECKSIZE = FACESCOUNT * SYMBOLSCOUNT;

struct Card
{
	char face = faces[0];
	char symbol = symbols[0];
	
	unsigned value = 0;

	void updateValue()
	{
		if (face == 'A')
		{
			value = 11;
			return;
		}

		value = face - '0';

		if (value > 10)
		{
			value = 10;
		}
	}
};

struct Deck
{
	Card cards[DECKSIZE];
	
	void fillDeck()
	{
		for (size_t i = 0; i < DECKSIZE; i += SYMBOLSCOUNT)
		{
			for (size_t j = 0; j < SYMBOLSCOUNT; j++)
			{
				cards[i + j].face = faces[i / SYMBOLSCOUNT];
				cards[i + j].symbol = symbols[j];
				cards[i + j].updateValue();
			}
		}
	}

	void shuffleDeck()
	{
		std::srand(std::time(0));
		
		for (size_t i = 0; i < DECKSIZE; i++)
		{
			int random = rand() % DECKSIZE;

			Card temp = cards[i];
			cards[i] = cards[random];
			cards[random] = temp;
		}
	}
	
};

int main()
{
	Deck deck;
	deck.fillDeck();
	for (size_t i = 0; i < DECKSIZE; i++)
	{
		std::cout << deck.cards[i].face << deck.cards[i].symbol << " " << deck.cards[i].value << "\n";
	}
	deck.shuffleDeck();
	for (size_t i = 0; i < DECKSIZE; i++)
	{
		std::cout << deck.cards[i].face << deck.cards[i].symbol << " " << deck.cards[i].value << "\n";
	}
	std::srand(std::time(0));
	/*
	Card c1 = { 'A', 'S', 11 };
	Card c2 = c1;
	c1 = { 'A', 'C', 11 };
	Card temp = c1;
	c1 = c2;
	c2 = temp;
	std::cout << c2.face << c1.symbol << c2.value;*/
}