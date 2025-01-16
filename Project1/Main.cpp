#include <iostream>
//#include <cstdlib>
//#include <ctime>

const unsigned NAME_SIZE = 20;
const unsigned HAND_SIZE = 3;
const unsigned CHIP_VALUE = 10;
const unsigned SARTING_CHIPS = 100;

const char faces[] = {'7', '8', '9', 'T', 'J', 'K', 'Q', 'A'};
const unsigned FACES_COUNT = 8;
const char symbols[] = {'S', 'H', 'D', 'C'};
const unsigned SYMBOLS_COUNT = 4;

const unsigned DECK_SIZE = FACES_COUNT * SYMBOLS_COUNT;

unsigned strLen(const char* str)
{
	unsigned len = 1;
	while (*str)
	{
		len++;
		str++;
	}
	return len;
}

void copyStr(const char* str, char* copy)
{
	unsigned len = strLen(str);
	for (size_t i = 0; i < len; i++)
	{
		copy[i] = str[i];
	}
}



const char DEFAULT_NAME[] = "PlayerX";
const unsigned DEFAULT_NAME_LEN = 8;

struct Card
{
	char face = faces[0];
	char symbol = symbols[0];
	unsigned value = 0;
};

struct Player
{
	char name[DEFAULT_NAME_LEN];
	Card cards[HAND_SIZE];
	unsigned chips = SARTING_CHIPS;
	unsigned handValue = 0;
	bool active = false;
};

void swapCards(Card& c1, Card& c2)
{
	Card swap = c1;
	c1 = c2;
	c2 = swap;
}

void dealCards(const Card cards[], Player players[], unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		for (size_t j = 0; j < HAND_SIZE; j++)
		{
			players[i].cards[j] = cards[i * HAND_SIZE + j];
		}
	}
}

void updateCardValue(Card& card)
{
	if (card.face == 'A')
	{
		card.value = 11;
		return;
	}

	card.value = card.face - '0';

	if (card.value > 10)
	{
		card.value = 10;
	}
}

void fillDeck(Card cards[])
{
	for (size_t i = 0; i < DECK_SIZE; i += SYMBOLS_COUNT)
	{
		for (size_t j = 0; j < SYMBOLS_COUNT; j++)
		{
			cards[i + j].face = faces[i / SYMBOLS_COUNT];
			cards[i + j].symbol = symbols[j];
			updateCardValue(cards[i + j]);
		}
	}
}

void shuffleDeck(Card cards[])
{
	std::srand(std::time(0));

	for (size_t i = 0; i < DECK_SIZE; i++)
	{
		int random = rand() % DECK_SIZE;

		Card temp = cards[i];
		cards[i] = cards[random];
		cards[random] = temp;
	}
}

bool isSevenOfSpades(Card card)
{
	if (card.face == '7' && card.symbol == 'S')
	{
		return true;
	}
	return false;
}

void namePlayers(Player players[], static unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		copyStr(DEFAULT_NAME, players[i].name);
		players[i].name[DEFAULT_NAME_LEN - 2] = '0' + i + 1;
		players[i].name[DEFAULT_NAME_LEN - 2] = '0' + i + 1;
	}
	
}

void bubbleSortCards(Card* cards, unsigned size) {
	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size- i - 1; j++) {
			if (cards[j].value > cards[j + 1].value) {
				swapCards(cards[j], cards[j + 1]);
			}
		}
	}

	if (isSevenOfSpades(cards[1]))
	{
		swapCards(cards[1], cards[0]);
	}

	for (size_t i = 0; i < size; i++)
	{
		std::cout << cards[i].face << cards[i].symbol << " " << cards[i].value << "     SORTSORTSORT\n";
	}
	std::cout << "\n";

}

void updatePlayerHandValue(Player &player)
{
	//Card* cards = player.cards;
	bubbleSortCards(player.cards, HAND_SIZE);

	//3 of 7
	if (player.cards[2].face == '7')
	{
		player.handValue = 34;
		return;
	}

	//2 of 7
	if (player.cards[1].face == '7')
	{
		player.handValue = 23;
		return;
	}

	//3 of a kind
	if (player.cards[0].face == player.cards[1].face && player.cards[1].face == player.cards[2].face) {
		player.handValue = player.cards[0].value * 3;
		return;
	}

	//3 of a suit
	if (player.cards[0].symbol == player.cards[1].symbol && player.cards[1].symbol == player.cards[2].symbol) {
		player.handValue = player.cards[0].value + player.cards[1].value + player.cards[2].value;
		return;
	}

	//7 Spades
	if (isSevenOfSpades(player.cards[0]))
	{
		//2 of a kind
		if (player.cards[1].face == player.cards[2].face)
		{
			player.handValue = player.cards[1].value * 2 + 11;
			return;
		}
		//2 of a suit
		if (player.cards[1].symbol == player.cards[2].symbol)
		{
			player.handValue = player.cards[1].value + player.cards[2].value + 11;
			return;
		}
		
		//default + 11
		player.handValue = player.cards[2].value + 11;
		return;
	}

	//2 of Ace
	if (player.cards[1].face == 'A')
	{
		player.handValue = 22;
		return;
	}

	//default
	player.handValue = player.cards[2].value;
}

void updatePlayerHandValue(Player* players, unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		updatePlayerHandValue(players[i]);
	}
}

int main()
{
	unsigned playersCount = 9;
	Player* players = new Player[playersCount];
	namePlayers(players, playersCount);

	Card* deck = new Card[DECK_SIZE];
	fillDeck(deck);

	dealCards(deck, players, playersCount);
	/*for (size_t i = 0; i < DECK_SIZE; i++)
	{
		std::cout << deck[i].face << deck[i].symbol << " " << deck[i].value << "\n";
	}
	std::cout << "\n";*/
	shuffleDeck(deck);
	dealCards(deck, players, playersCount);
	for (size_t i = 0; i < DECK_SIZE; i++)
	{
		std::cout << deck[i].face << deck[i].symbol << " " << deck[i].value << "\n";
	}

	updatePlayerHandValue(players, playersCount);

	for (size_t i = 0; i < playersCount; i++)
	{
		std::cout << players[i].name << ": \n";
		for (size_t j = 0; j < HAND_SIZE; j++)
		{
			std::cout << players[i].cards[j].face << players[i].cards[j].symbol << " " << players[i].cards[j].value << "   ";
		}
		std::cout << players[i].chips << " " << players[i].handValue << "\n\n";

		
	}
	
	/*
	std::srand(std::time(0));
	
	Card c1 = { 'A', 'S', 11 };
	Card c2 = c1;
	c1 = { 'A', 'C', 11 };
	Card temp = c1;
	c1 = c2;
	c2 = temp;
	std::cout << c2.face << c1.symbol << c2.value;*/
}