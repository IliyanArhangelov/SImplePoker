#include <iostream>
//#include <cstdlib>
//#include <ctime>

const unsigned NAME_SIZE = 20;
const unsigned HAND_SIZE = 3;
const unsigned CHIP_VALUE = 10;
const unsigned SARTING_CHIPS = 100;
const unsigned CHARITY = 5 * CHIP_VALUE;
const char FACES[] = {'7', '8', '9', 'T', 'J', 'K', 'Q', 'A'};
const unsigned FACES_COUNT = 8;
const char SYMBOLS[] = {'S', 'H', 'D', 'C'};
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
	char face = FACES[0];
	char symbol = SYMBOLS[0];
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
	bool multipleWinners = false;
	bool playGame = true;
};

void printPlayersStats(const Player players[], const unsigned playersCount);
void printDebug(Player players[], const unsigned playersCount, Pot& pot);
void updateActivePlayersCount(Player players[], const unsigned playersCount, Pot& pot);
void setup(Player players[], const unsigned playersCount, Pot& pot, Card deck[]);
void multipleWinners(Player players[], const unsigned playersCount, Pot& pot, Card deck[]);
void win(Player players[], const unsigned playersCount, Pot& pot, Card deck[]);
void takeBlind(Player players[], const unsigned playersCount, Pot& pot, Card deck[]);
void printPlayerStatus(const Player player, const Pot pot);

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
			cards[i + j].face = FACES[i / SYMBOLS_COUNT];
			cards[i + j].symbol = SYMBOLS[j];
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

void bubbleSortCards(Card* cards, unsigned size)
{
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
}

void updatePlayerHandValue(Player &player)
{
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

	while (chipsToRaise > pot.currentPoorestPlayer / CHIP_VALUE || !chipsToRaise)
	{
		std::cout << "Try again. Max allowed raise is by " << maxChips << " chips: ";
		std::cin >> chipsToRaise;
	}
	return chipsToRaise;
}

void awarding(Player& player, Pot& pot)
{
	player.money += pot.money;
	pot.money = 0;
	pot.currentHighBet = 0;

	std::cout << "The winner is " << player.name;
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

unsigned findIndexOfLastPlayer(Player players[])
{
	for (size_t i = 0;; i++)
	{
		if (players[i].active)
		{
			return i;
		}
	}
}

void playTurns(Player players[], const unsigned playersCount, Pot& pot)
{
	int countOfCalls = 0;
	bool isStart = true;
	updatePoorestActivePlayer(players, playersCount, pot);
	updateActivePlayersCount(players, playersCount, pot);

	for (size_t i = 0; countOfCalls < pot.activePlayersCount + isStart - 1; i++)
	{
		if (pot.activePlayersCount == 1)
		{
			return;
		}
		unsigned playerIndex = i % playersCount;
		printPlayersStats(players, playersCount);
		printPlayerStatus(players[playerIndex], pot);


		unsigned action = playTurn(players[playerIndex], pot);
		if (action == 1)
		{
			countOfCalls++;
		}
		else if (action == 2)
		{
			isStart = false;
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
	return countOfWinners;
}

void awardWinner(Player players[], const unsigned playersCount, Pot& pot, const unsigned winningHand)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].handValue == winningHand && players[i].active)
		{
			awarding(players[i], pot);
		}
	}
}

void activatePlayers(Player players[], const unsigned playersCount, Pot& pot)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		players[i].active = !!players[i].money;
	}
	updateActivePlayersCount(players, playersCount, pot);
}

void deactivateLosers(Player players[], const unsigned playersCount, Pot& pot, const unsigned winningHand)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].handValue < winningHand)
		{
			players[i].active = false;
			pot.activePlayersCount--;
		}
		players[i].moneyInPot = 0;
	}
}

void askRejoin(Player& player, Pot& pot, const unsigned rejoinCost)
{
	if (rejoinCost > player.money)
	{
		std::cout << player.name << ", not enough money to rejoin :(\n";
		return;
	}

	char input = 0;
	std::cout << player.name << ", rejoin for " << rejoinCost << "? (y / n) : ";
	std::cin >> input;
	if (input == 'y' || input == 'Y')
	{
		player.money -= rejoinCost;
		pot.money += rejoinCost;
		player.active = true;
		pot.activePlayersCount++;
		return;
	}
	if (input == 'n' || input == 'N')
	{
		return;
	}

	std::cout << "Wrong input!";
	askRejoin(player, pot, rejoinCost);
}

void feedThePoor(Player players[], const unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		players[i].money += (players[i].money == 0 && players[i].active) * CHARITY;
	}
}

void updateActivePlayersCount(Player players[], const unsigned playersCount, Pot& pot)
{
	pot.activePlayersCount = 0;
	for (size_t i = 0; i < playersCount; i++)
	{
		pot.activePlayersCount += players[i].active;
	}
}

void printPlayersStats(const Player players[], const unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		std::cout << players[i].name << ": ";

		if (players[i].active)
		{
			std::cout << "Active;  ";
		}
		else
		{
			std::cout << "Unctive;  ";
		}
		std::cout << "Money: " << players[i].money << ";  Money in pot: " << players[i].moneyInPot << "\n\n";
		
	}
}

void printPlayerStatus(const Player player, const Pot pot)
{
	std::cout << player.name << ":\nHand: ";
	for (size_t j = 0; j < HAND_SIZE; j++)
	{
		std::cout << player.cards[j].face << player.cards[j].symbol << "   ";
	}
	std::cout << "\nHand value: " << player.handValue << "\n";
	std::cout <<"Money in pot:" << pot.money << "\nHigh bet: " << pot.currentHighBet << "\nYour money in pot: "
		<< player.moneyInPot << "\nActive players: " << pot.activePlayersCount << "\n";
}

void printDebug(Player players[], const unsigned playersCount, Pot& pot)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		std::cout << players[i].name << ": \n";
		for (size_t j = 0; j < HAND_SIZE; j++)
		{
			std::cout << players[i].cards[j].face << players[i].cards[j].symbol << " " << players[i].cards[j].value << "   ";
		}
		std::cout << players[i].money << " " << players[i].handValue << "\n\n";
	}
}

void playEquals(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	shuffleDeck(deck);
	dealCards(deck, players, playersCount);

	updatePlayerHandValue(players, playersCount);
}

void playGame(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	activatePlayers(players, playersCount, pot);
	setup(players, playersCount, pot, deck);
	updatePoorestActivePlayer(players, playersCount, pot);
	takeBlind(players, playersCount, pot, deck);
	do
	{
		updatePoorestActivePlayer(players, playersCount, pot);
		playEquals(players, playersCount, pot, deck);
		playTurns(players, playersCount, pot);
		win(players, playersCount, pot, deck);
		if (pot.multipleWinners)
		{
			multipleWinners(players, playersCount, pot, deck);
		}

	} while (pot.multipleWinners);

	for (size_t i = 0; i < playersCount; i++)
	{
		std::cout << "\n" << players[i].name << ": \n";
		for (size_t j = 0; j < HAND_SIZE; j++)
		{
			std::cout << players[i].cards[j].face << players[i].cards[j].symbol << " " << players[i].cards[j].value << "   ";
		}
		std::cout << players[i].money << " " << players[i].handValue << "\n\n";
	}

	char input = 0;
	std::cout << "\n\n\nPlay again? (y/n): ";
	std::cin >> input;
	while (input != 'y' && input != 'Y' && input != 'n' && input != 'N')
	{
		std::cout << "\nWrong input !!!\nPlay again? (y/n): ";
		std::cin >> input;
	}

	if (input == 'n' || input == 'N')
	{
		pot.playGame = false;
	}

}

void multipleWinners(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	pot.currentHighBet = 0;
	unsigned rejoinCost = (pot.money / 20) * 10;
	for (size_t i = 0; i < playersCount; i++)
	{
		if (!players[i].active)
		{
			askRejoin(players[i], pot, rejoinCost);
		}
	}

	feedThePoor(players, playersCount);
}

void win(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	unsigned winningHand = winningHandValue(players, playersCount);
	unsigned countOfWinners = countWinners(players, playersCount, winningHand);
	deactivateLosers(players, playersCount, pot, winningHand);

	if (countOfWinners == 1)
	{
		awardWinner(players, playersCount, pot, winningHand);
		pot.multipleWinners = false;
		return;
	}

	pot.multipleWinners = true;
}

void takeBlind(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	for (size_t i = 0; i < playersCount; i++)
	{
		players[i].moneyInPot += CHIP_VALUE * players[i].active;
		players[i].money -= CHIP_VALUE * players[i].active;
	}
	
	pot.money += CHIP_VALUE * pot.activePlayersCount;
}

void setup(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	pot.activePlayersCount = 0;
	activatePlayers(players, playersCount, pot);
	updateActivePlayersCount(players, playersCount, pot);
}

int main()
{
	unsigned playersCount = 4;
	
	Player* players = new Player[playersCount];
	namePlayers(players, playersCount);

	Card* deck = new Card[DECK_SIZE];
	fillDeck(deck);

	Pot pot;
	activatePlayers(players, playersCount, pot);

	while (pot.playGame)
	{
		playGame(players, playersCount, pot, deck);
	}

	return 0;
}