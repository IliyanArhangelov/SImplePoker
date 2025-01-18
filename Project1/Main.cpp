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
	unsigned money = SARTING_CHIPS * CHIP_VALUE;
	unsigned handValue = 0;
	unsigned moneyInPot = 0;
	bool active = true;
};

struct Pot
{
	unsigned money = 0;
	unsigned currentHighBet = 0;
	unsigned currentPoorestPlayer = CHIP_VALUE * SARTING_CHIPS;
	unsigned activePlayersCount = 0;
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

void updatePoorestActivePlayer(const Player players[], unsigned playersCount, Pot& pot)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].money < pot.currentPoorestPlayer)
		{
			pot.currentPoorestPlayer = players[i].money;
		}
	}
}

void call(Player& player, Pot& pot)
{
	unsigned moneyToPot = pot.currentHighBet - player.moneyInPot;
	player.moneyInPot += moneyToPot;
	pot.money += moneyToPot;
	player.money -= moneyToPot;
}

void raise(Player& player, Pot& pot, const unsigned chipsToRaise)
{
	unsigned moneyToRaise = chipsToRaise * CHIP_VALUE;
	pot.currentHighBet += moneyToRaise;

	call(player, pot);
}

unsigned askRaise(Player& player, Pot& pot)
{
	unsigned chipsToRaise = 0;
	unsigned maxChips = pot.currentPoorestPlayer / CHIP_VALUE;
	std::cout << "Raise by how many chips? (1 chip = 10 money)\n";
	std::cout << "Max allowed raise is by " << maxChips << " chips: ";
	std::cin >> chipsToRaise;

	if (chipsToRaise > pot.currentPoorestPlayer / CHIP_VALUE || !chipsToRaise)
	{
		std::cout << "Try again. Max allowed raise is by " << maxChips << " chips: ";
		return askRaise(player, pot);
	}
	return chipsToRaise;
}

unsigned playTurn(Player& player, Pot& pot)
{
	if (!player.active)
	{
		return 0;
	}

	char input = 0;
	std::cout << player.name << ", raise, fold or calls? (r/f/c): ";
	std::cin >> input;

	//fold
	if (input == 'f' || input == 'F')
	{
		player.active = false;
		pot.activePlayersCount--;
		return 0;
	}

	//calls
	if (input == 'c' || input == 'C')
	{
		call(player, pot);
		return 1;
	}

	//raise
	if (input == 'r' || input == 'R')
	{
		unsigned chipsToRaise = askRaise(player, pot);
		raise(player, pot, chipsToRaise);
		return 2;
	}

	std::cout << "\nWrong input!!!\n\n";
	return playTurn(player, pot);
}

void playTurns(Player players[], const unsigned playersCount, Pot& pot)
{
	int countOfCalls = 0;
	bool isStart = true;
	for (size_t i = 0; countOfCalls < pot.activePlayersCount - 1; i++)
	{
		unsigned playerIndex = i % playersCount;
		std::cout << "\n";
		std::cout << pot.money << "lv, High bet: " << pot.currentHighBet << " Money in pot: " << players[playerIndex].moneyInPot << "\n";
		

		unsigned action = playTurn(players[playerIndex], pot);
		if (action == 1)
		{
			countOfCalls++;
		}
		else if (action == 2)
		{
			if (isStart)
			{
				pot.activePlayersCount--;
				isStart = false;
			}
			countOfCalls = 0;
		}
		updatePoorestActivePlayer(players, playersCount, pot);
	}
}

unsigned winningHandValue(const Player players[], const unsigned playersCount)
{
	unsigned winningHand = 0;

	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].handValue > winningHand && players[i].active)
		{
			winningHand = players[i].handValue;
		}
	}
	return winningHand;
}

unsigned countWinners(const Player players[], const unsigned playersCount, const unsigned winningHand)
{
	unsigned countOfWinners = 0;

	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].handValue == winningHand && players[i].active)
		{
			countOfWinners++;
		}
	}
	return winningHand;
}

void awardWinner(Player players[], const unsigned playersCount, Pot& pot, const unsigned winningHand)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].handValue == winningHand && players[i].active)
		{
			players[i].money += pot.money;
			pot.money = 0;
			pot.currentHighBet = 0;
			pot.currentPoorestPlayer = CHIP_VALUE * SARTING_CHIPS;

			std::cout << "The winner is " << players[i].name;
			return;
		}
	}
}

void win(Player players[], const unsigned playersCount, Pot& pot)
{
	unsigned winningHand = winningHandValue(players, playersCount);
	unsigned countOfWinners = countWinners(players, playersCount, winningHand);
	if (countOfWinners == 1)
	{
		awardWinner(players, playersCount, pot, winningHand);
	}
}

void playRound(Player players[], const unsigned playersCount, Pot& pot)
{
	
	pot.activePlayersCount++;
	for (size_t i = 0; i < playersCount; i++)
	{
		players[i].money -= CHIP_VALUE;
	}
	
	pot.money += CHIP_VALUE * playersCount;
	pot.currentPoorestPlayer = players[0].money;

	playTurns(players, playersCount, pot);
	win(players, playersCount, pot);
}

int main()
{
	unsigned playersCount = 4;
	
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

	Pot pot;
	pot.activePlayersCount = playersCount;

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
		std::cout << players[i].money << " " << players[i].handValue << "\n\n";
	}


	playRound(players, playersCount, pot);

	for (size_t i = 0; i < playersCount; i++)
	{
		std::cout << "\n" << players[i].name << ": \n";
		for (size_t j = 0; j < HAND_SIZE; j++)
		{
			std::cout << players[i].cards[j].face << players[i].cards[j].symbol << " " << players[i].cards[j].value << "   ";
		}
		std::cout << players[i].money << " " << players[i].handValue << "\n\n";
	}

	return 0;
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