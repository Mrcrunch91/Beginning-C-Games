//Blackjack
//Plays a simple version of the casino game of blackjack; for 1 - 7 players

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include "Card.h"

using namespace std;






class House: public GenericPlayer {
public:

	House(const string &name = "House") :
			GenericPlayer(name) {
	}

	virtual ~House() {
	}

	//indicates whether house is hitting - will always hit on 16 or less
	bool IsHitting() const {
		return (GetTotal() <= 16);
	}

	//flips over first card
	void FlipFirstCard() {
		if (!(m_Cards.empty())) {
			m_Cards[0]->Flip();
		} else {
			cout << "No card to flip!\n";
		}
	}
};

class Deck: public Hand {
public:

	Deck() {
		m_Cards.reserve(52);
		Populate();
	}

	virtual ~Deck() {
	}

	//create a standard deck of 52 cards
	void Populate() {
		Clear();
		//create standard deck
		for (int s = Card::CLUBS; s <= Card::SPADES; ++s) {
			for (int r = Card::ACE; r <= Card::KING; ++r) {
				Add(
						new Card(static_cast<Card::rank>(r),
								static_cast<Card::suit>(s))
				);
			}
		}
	}

	//shuffle cards
	void Shuffle() {
		random_shuffle(m_Cards.begin(), m_Cards.end());
	}

	//deal one card to a hand
	void Deal(Hand &aHand) {
		if (!m_Cards.empty()) {
			aHand.Add(m_Cards.back());
			m_Cards.pop_back();
		} else {
			cout << "Out of cards. Unable to deal.";
		}
	}

	//give additional cards to a generic player
	void AdditionalCards(GenericPlayer &aGenericPlayer) {
		cout << endl;
		//continue to deal a card as long as generic player isn't busted and
		//wants another hit
		while (!(aGenericPlayer.IsBusted()) && aGenericPlayer.IsHitting()) {
			Deal(aGenericPlayer);
			cout << aGenericPlayer << endl;

			if (aGenericPlayer.IsBusted()) {
				aGenericPlayer.Bust();
			}
		}
	}
};

class Game {

public:

	Game(const vector<string>& names){

		//create a vector of players from a vector of names
		vector<string>::const_iterator pName;

		for (pName = names.begin(); pName != names.end(); ++pName) {
			m_Players.push_back(Player(*pName));
		}

		//seed the random number generator
		srand(static_cast<unsigned int>(time(0)));
		m_Deck.Populate();
		m_Deck.Shuffle();
	}

	~Game() {
	}

	//plays the game of blackjack
	void Play() {
		//deal initial 2 cards to everyone
		vector<Player>::iterator pPlayer;
		for (int i = 0; i < 2; ++i) {
			for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
					++pPlayer) {
				m_Deck.Deal(*pPlayer);
			}
			m_Deck.Deal(m_House);
		}

		//hide house's first card
		m_House.FlipFirstCard();

		//display everyone's hand
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
				++pPlayer) {
			cout << *pPlayer << endl;
		}
		cout << m_House << endl;

		//deal additional cards to players
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
				++pPlayer) {
			m_Deck.AdditionalCards(*pPlayer);
		}

		//reveal house's first card
		m_House.FlipFirstCard();
		cout << endl << m_House;

		//deal additional cards to house
		m_Deck.AdditionalCards(m_House);

		if (m_House.IsBusted()) {
			//everyone still playing wins
			for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
					++pPlayer) {
				if (!(pPlayer->IsBusted())) {
					pPlayer->Win();
				}
			}
		} else {
			//compare each player still playing to house
			for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
					++pPlayer) {
				if (!(pPlayer->IsBusted())) {
					if (pPlayer->GetTotal() > m_House.GetTotal()) {
						pPlayer->Win();
					} else if (pPlayer->GetTotal() < m_House.GetTotal()) {
						pPlayer->Lose();
					} else {
						pPlayer->Push();
					}
				}
			}

		}

		//remove everyone's cards
		for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
				++pPlayer) {
			pPlayer->Clear();
		}
		m_House.Clear();
	}

private:
	Deck m_Deck;
	House m_House;
	vector<Player> m_Players;
};

//function prototypes
ostream& operator<<(ostream &os, const Card &aCard);
ostream& operator<<(ostream &os, const GenericPlayer &aGenericPlayer);

int main() {

	cout << "\t\tWelcome to Blackjack!\n\n";

	int numPlayers = 0;
	while (numPlayers < 1 || numPlayers > 7) {
		cout << "How many players? (1 - 7): ";
		cin >> numPlayers;
	}

	vector<string> names;
	string name;
	for (int i = 0; i < numPlayers; ++i) {
		cout << "Enter player name: ";
		cin >> name;
		names.push_back(name);
	}
	cout << endl;

	//the game loop
	Game aGame(names);
	char again = 'y';
	while (again != 'n' && again != 'N') {
		aGame.Play();
		cout << "\nDo you want to play again? (Y/N): ";
		cin >> again;
	}

	return 0;
}

//overloads << operator so Card object can be sent to cout
ostream& operator<<(ostream &os, const Card &aCard) {
	const string RANKS[] = { "0", "A", "2", "3", "4", "5", "6", "7", "8", "9",
			"10", "J", "Q", "K" };
	const string SUITS[] = { "c", "d", "h", "s" };

	if (aCard.m_IsFaceUp) {
		os << RANKS[aCard.m_Rank] << SUITS[aCard.m_Suit];
	} else {
		os << "XX";
	}

	return os;
}

//overloads << operator so a GenericPlayer object can be sent to cout
ostream& operator<<(ostream &os, const GenericPlayer &aGenericPlayer) {
	os << aGenericPlayer.m_Name << ":\t";

	vector<Card*>::const_iterator pCard;
	if (!aGenericPlayer.m_Cards.empty()) {
		for (pCard = aGenericPlayer.m_Cards.begin();
				pCard != aGenericPlayer.m_Cards.end(); ++pCard) {
			os << *(*pCard) << "\t";
		}

		if (aGenericPlayer.GetTotal() != 0) {
			cout << "(" << aGenericPlayer.GetTotal() << ")";
		}
	} else {
		os << "<empty>";
	}

	return os;
}
