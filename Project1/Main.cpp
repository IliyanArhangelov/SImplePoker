/**
* 
* Solution to course project # 10
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2024/2025
*
* @author Iliyan Arhangelov
* @idnumber 0MI0600492 @compiler VC
*
* <предназначение на файла>
*
*/

#include <iostream>
#include <fstream>

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

const char CLEAR_SCREEN[] = "\033[H\033[J";
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


void printHand(const Player player);
void printPlayersStats(const Player players[], const unsigned playersCount);
void updateActivePlayersCount(Player players[], const unsigned playersCount, Pot& pot);
void setup(Player players[], const unsigned playersCount, Pot& pot, Card deck[]);
void multipleWinners(Player players[], const unsigned playersCount, Pot& pot);
void win(Player players[], const unsigned playersCount, Pot& pot);
void takeBlind(Player players[], const unsigned playersCount, Pot& pot);
void printPlayerStatus(const Player player, const Pot pot);

//swaps 2 cards
void swapCards(Card& c1, Card& c2)
{
	Card swap = c1;
	c1 = c2;
	c2 = swap;
} 

//every player takes the next cards from the deck
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

//sets the strength of every single card
void updateCardValue(Card& card)
{
	if (card.face == 'A')
	{
		card.value = 11;
		return;
	}

	card.value = card.face - '0';

	//non number cards have a value of 10
	if (card.value > 10)
	{
		card.value = 10;
	}
}

//fills the deck with cards
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
	std::srand(std::time(0)); //random number depending of the machine time

	//goes trough every card and swaps it with a random card
	for (size_t i = 0; i < DECK_SIZE; i++)
	{
		int random = rand() % DECK_SIZE;
		swapCards(cards[i], cards[random]);
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

//every player is given a name
void namePlayers(Player players[], static unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		copyStr(DEFAULT_NAME, players[i].name);
		players[i].name[DEFAULT_NAME_LEN - 2] = '0' + i + 1;
		players[i].name[DEFAULT_NAME_LEN - 2] = '0' + i + 1;
	}
	
}

//sorts cards by strength in your hand
void bubbleSortCards(Card* cards, unsigned size)
{
	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size- i - 1; j++) {
			if (cards[j].value > cards[j + 1].value) {
				swapCards(cards[j], cards[j + 1]);
			}
		}
	}

	//7 Spades is aways first
	if (isSevenOfSpades(cards[1]))
	{
		swapCards(cards[1], cards[0]);
	}
}

//updates the hand value of a single player
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

//updates the hand value of every player
void updatePlayerHandValue(Player* players, unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		updatePlayerHandValue(players[i]);
	}
}

//updates how much money the poorest player has
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

//takes the requied amount from the player and gives it to the pot, depending on the last raise
void call(Player& player, Pot& pot)
{
	unsigned moneyToPot = pot.currentHighBet - player.moneyInPot;
	player.moneyInPot += moneyToPot;
	pot.money += moneyToPot;
	player.money -= moneyToPot;
}

//raising the bet
void raise(Player& player, Pot& pot, const unsigned chipsToRaise)
{
	unsigned moneyToRaise = chipsToRaise * CHIP_VALUE;
	pot.currentHighBet += moneyToRaise;

	//then the action is the same as calling to a previous high bet
	call(player, pot);
}

//ask amount of chips to raise
unsigned askRaise(Player& player, Pot& pot)
{
	unsigned chipsToRaise = 0;
	unsigned maxChips = pot.currentPoorestPlayer / CHIP_VALUE;
	std::cout << "Raise by how many chips? (1 chip = 10 money)\n";
	std::cout << "Max allowed raise is by " << maxChips << " chips: ";
	std::cin >> chipsToRaise;

	//prevents invalid input
	while (chipsToRaise > pot.currentPoorestPlayer / CHIP_VALUE)
	{
		std::cout << "Try again. Max allowed raise is by " << maxChips << " chips: ";
		std::cin >> chipsToRaise;
	}
	return chipsToRaise;
}

//awards the winner
void awarding(Player& player, Pot& pot)
{
	//takes the money from the pot and gives it to the player
	player.money += pot.money;
	pot.money = 0;
	pot.currentHighBet = 0;

	//prints winning message
	std::cout << CLEAR_SCREEN << "The winner is " << player.name << "\nWinning hand: ";
	printHand(player);
	std::cout << "\nHand value: " << player.handValue << "\n";
}

//makes a player take a turn
unsigned playTurn(Player& player, Pot& pot)
{
	if (!player.active)
	{
		return 0;
	}

	char input = 0;
	std::cout << player.name << ", raise, fold or call? (r/f/c): ";
	std::cin >> std::ws >> input;
	std::cin.ignore(100, '\n');

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
		if (!chipsToRaise) // raising by zero is same as calling
		{
			return 1;
		}
		raise(player, pot, chipsToRaise);
		return 2;
	}

	//prevents wrong input
	std::cout << "\nWrong input!!!\n\n";
	return playTurn(player, pot);
}

//finds the index of the last active player
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

	//cycle stops when enough people have folded or called
	for (size_t i = 0; countOfCalls < pot.activePlayersCount + isStart - 1; i++)
	{
		//when everyone exept one player folds
		if (pot.activePlayersCount == 1)
		{
			return;
		}
		unsigned playerIndex = i % playersCount;
		printPlayersStats(players, playersCount);
		printPlayerStatus(players[playerIndex], pot);


		unsigned action = playTurn(players[playerIndex], pot);
		//when calls
		if (action == 1)
		{
			countOfCalls++;
		}
		//when raises
		else if (action == 2)
		{
			isStart = false;
			countOfCalls = 0; //new bet, so the calls should start again from 0
		}
		updatePoorestActivePlayer(players, playersCount, pot);
	}
}

//finds the hand with the most value among the active players
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

//counts how many people have winning hand, so we know when a tie comes
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

//finds single active player with winning hand and gives him the award
void awardWinner(Player players[], const unsigned playersCount, Pot& pot, const unsigned winningHand)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].handValue == winningHand && players[i].active)
		{
			awarding(players[i], pot);
			return;
		}
	}
}

//activates the players that have money
void activatePlayers(Player players[], const unsigned playersCount, Pot& pot)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		players[i].active = !!players[i].money; //unactive only when money = 0
	}
	updateActivePlayersCount(players, playersCount, pot);
}

//deactivates people that have losing hand
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

//asks losers to rejoin for a TIE
void askRejoin(Player& player, Pot& pot, const unsigned rejoinCost)
{
	if (rejoinCost > player.money) //if not enough money, no rejoin
	{
		std::cout << player.name << ", not enough money to rejoin :(\n";
		return;
	}

	char input = 0;
	std::cout << player.name << ", rejoin for " << rejoinCost << "? (y / n) : ";
	std::cin >> std::ws >> input;
	std::cin.ignore(100, '\n');

	if (input == 'y' || input == 'Y')
	{
		//takes the money from the player and gives it to the pot
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

	//prevents wrong input
	std::cout << "Wrong input!";
	askRejoin(player, pot, rejoinCost);
}

//gives active players with 0 money +50 for the tie
void feedThePoor(Player players[], const unsigned playersCount)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		if (players[i].active)
		{
			players[i].money += CHARITY;
		} 
	}
}

//updates pot.activePlayersCount by iterating trough the players
void updateActivePlayersCount(Player players[], const unsigned playersCount, Pot& pot)
{
	pot.activePlayersCount = 0;
	for (size_t i = 0; i < playersCount; i++)
	{
		pot.activePlayersCount += players[i].active;
	}
}

//prints the cards in players hand
void printHand(const Player player)
{
	for (size_t j = 0; j < HAND_SIZE; j++)
	{
		std::cout << player.cards[j].face << player.cards[j].symbol << " " << player.cards[j].value << "   ";
	}
}

//prints information about the players
void printPlayersStats(const Player players[], const unsigned playersCount)
{
	std::cout << CLEAR_SCREEN;
	for (size_t i = 0; i < playersCount; i++)
	{
		std::cout << players[i].name << ": ";

		if (players[i].active)
		{
			std::cout << "Active;  ";
		}
		else
		{
			std::cout << "Unactive;  ";
		}
		std::cout << "Money: " << players[i].money << ";  Money in pot: " << players[i].moneyInPot << "\n";
		
	}
	std::cout << "\n";
}

//prints information about the current playing player
void printPlayerStatus(const Player player, const Pot pot)
{
	std::cout << player.name << ":\nHand: ";
	printHand(player);
	std::cout << "\nHand value: " << player.handValue << "\n\n";
	std::cout <<"Money in pot: " << pot.money << "\nHigh bet: " << pot.currentHighBet << "\nYour money in pot: "
		<< player.moneyInPot << "\n\nActive players: " << pot.activePlayersCount << "\n";
}

//combines the operations with the cards
void dealer(Player players[], const unsigned playersCount, Card deck[])
{
	shuffleDeck(deck);
	dealCards(deck, players, playersCount);
	updatePlayerHandValue(players, playersCount);
}

//ask to load a previous game from the save file
bool askLoadPreviousGame()
{
	char input = 0;

	std::cout << "Continue previous game? (y/n): ";
	std::cin >> std::ws >> input; //inconsistent without std::ws
	std::cin.ignore(100, '\n'); //so the input is only the first inputed character

	if (input == 'y' || input == 'Y')
	{
		return true;
	}
	if (input == 'n' || input == 'N')
	{
		return false;
	}

	//prevents wrong input
	std::cout << "Wrong input!\n\n";
	return askLoadPreviousGame();
}

//asks the player to continue playing the current game
void askContinueGame(Pot& pot)
{
	char input = 0;
	std::cout << "\n\n\nPlay again? (y/n): ";
	std::cin >> std::ws >> input;
	std::cin.ignore(100, '\n');

	while (input != 'y' && input != 'Y' && input != 'n' && input != 'N')
	{
		std::cout << "\nWrong input !!!\nPlay again? (y/n): ";
		std::cin >> std::ws >> input; //inconsistent without std::ws
		std::cin.ignore(100, '\n');
	}

	//stops the game 
	if (input == 'n' || input == 'N')
	{
		pot.playGame = false;
	}

	//continues by default
}

void playGame(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	setup(players, playersCount, pot, deck);
	do //turns are being played untill there is only one winner
	{
		updatePoorestActivePlayer(players, playersCount, pot);
		dealer(players, playersCount, deck);
		playTurns(players, playersCount, pot);
		win(players, playersCount, pot);
		if (pot.multipleWinners)
		{
			printPlayersStats(players, playersCount);
			std::cout << "IT'S A TIE!\n\n";
			multipleWinners(players, playersCount, pot);
		}

	} while (pot.multipleWinners);

	askContinueGame(pot);
}


void multipleWinners(Player players[], const unsigned playersCount, Pot& pot)
{
	pot.currentHighBet = 0;
	unsigned rejoinCost = (pot.money / (CHIP_VALUE * 2)) * CHIP_VALUE; //so we are sure the about can be payed with chips 
	for (size_t i = 0; i < playersCount; i++)
	{
		if (!players[i].active)
		{
			askRejoin(players[i], pot, rejoinCost);
		}
	}
	feedThePoor(players, playersCount);
}

//responsible for winning after the players have stopped taking turns
void win(Player players[], const unsigned playersCount, Pot& pot)
{
	unsigned winningHand = winningHandValue(players, playersCount);
	unsigned countOfWinners = countWinners(players, playersCount, winningHand);
	deactivateLosers(players, playersCount, pot, winningHand);

	if (countOfWinners == 1) // only one winner
	{
		awardWinner(players, playersCount, pot, winningHand);
		pot.multipleWinners = false;
		return;
	}

	pot.multipleWinners = true;
}

//takes a blind from every player at the start of the game
void takeBlind(Player players[], const unsigned playersCount, Pot& pot)
{
	for (size_t i = 0; i < playersCount; i++)
	{
		players[i].moneyInPot += CHIP_VALUE * players[i].active;
		players[i].money -= CHIP_VALUE * players[i].active;
		pot.currentHighBet = CHIP_VALUE;
	}
	
	pot.money += CHIP_VALUE * pot.activePlayersCount;
}

//sets the game contidions
void setup(Player players[], const unsigned playersCount, Pot& pot, Card deck[])
{
	fillDeck(deck);
	activatePlayers(players, playersCount, pot);
	updateActivePlayersCount(players, playersCount, pot);
	updatePoorestActivePlayer(players, playersCount, pot);
	takeBlind(players, playersCount, pot);
}

//saves the information of the game to the save file
void saveGame(const Player players[], const unsigned playersCount)
{
	std::ofstream saveFile("SAVE.txt", std::ios::trunc);
	saveFile << playersCount; //first line is the player count

	for (size_t i = 0; i < playersCount; i++) //every next line is the money in every player's bank
	{
		saveFile << '\n' << players[i].money;
	}
	saveFile.close();
}

int main()
{
	unsigned playersCount = 0;
	bool loadSave = false;
	std::ifstream saveFile("SAVE.txt");
	saveFile >> playersCount;

	if (playersCount) //would be false if save doesn't exist or if empty
	{
		loadSave = askLoadPreviousGame();
		playersCount *= loadSave; //if loadSave is false, player count becomes zero
	}
	while ((playersCount < 2 || playersCount > 9) && !loadSave) //when the player refuses to load previous
	{
		std::cout << CLEAR_SCREEN << "How many players will play? (2 - 9): ";
		std::cin >> playersCount;
	}

	Player* players = new Player[playersCount];
	namePlayers(players, playersCount);
	for (size_t i = 0; i < playersCount && loadSave; i++) //gives the players their money from the previous game
	{
		saveFile >> players[i].money;
	}
	saveFile.close();

	Card* deck = new Card[DECK_SIZE];
	Pot pot;
	while (pot.playGame) //plays until the user refuses to continue
	{
		playGame(players, playersCount, pot, deck);
	}
	//ending the game
	saveGame(players, playersCount);
	delete[] players;
	delete[] deck;
	return 0;
}