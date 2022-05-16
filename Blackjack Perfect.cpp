#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <chrono>
#include <string>

using namespace std;

int Generate_Deck(vector<double> &deck) {
    deck.clear();
    for (int i = 0; i < 40; i++) {
        deck.push_back((i%10)+2);
    }
    for (int i = 0; i < 4; i++) deck.push_back(10.1);
    for (int i = 0; i < 4; i++) deck.push_back(10.2);
    for (int i = 0; i < 4; i++) deck.push_back(10.3);
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    shuffle(begin(deck), end(deck), rng);
    return (rand()%5+15);
}

int Deal_Hand(vector<double> &deck, vector<double> &dealer, vector<double> &player, int &money) {
    dealer.clear(); player.clear();
    dealer.push_back(deck.back());  deck.pop_back();
    dealer.push_back(deck.back());  deck.pop_back();
    player.push_back(deck.back());  deck.pop_back();
    player.push_back(deck.back());  deck.pop_back();

    int bet = 0;
    if (money < 50) {
        cout << "Out of money" << endl;
        exit(0);
    }
    redoBet:
    //cout << "How much would you like to bet (minimum $50)? You have $" << money << "!" << endl;
    //cin >> bet;
    bet = 50;
    if (bet < 50 || bet > money) goto redoBet;
    money -= bet;
    return bet;
}

void Deal_Card(vector<double> &deck, vector<double> &hand) {
    hand.push_back(deck.back());  deck.pop_back();
}

string MakeLetter(double card) {
    if (card == 1 || card == 11) return "A";
    else if (card == 10.1) return "J";
    else if (card == 10.2) return "Q";
    else if (card == 10.3) return "K";
    else return to_string((int)card);
}
void Print_Hands(bool showDealer, vector<double> &dealer, vector<double> &player) {
    if (showDealer) {
        cout << "Dealer:";
        for (double i : dealer) cout << '\t' << MakeLetter(i);
        cout << endl;
    } else cout << "Dealer:" << '\t' << MakeLetter(dealer[0]) << endl;
    cout << "Player:";
    for (double i : player) cout << '\t' << MakeLetter(i);
    cout << "\t\t TOTAL: " << (int)accumulate(player.begin(), player.end(), 0) << endl;
    cout << "1:Hit   2:Stand   3:Double Down   4:Split   0:End Game" << endl;
}

bool Check_Bust(vector<double> &hand) {
    int i = 0;
    if ((int)accumulate(hand.begin(), hand.end(), 0) > 21) {
        if (count(hand.begin(), hand.end(), 11)) {
            while (hand[i++] != 11);
            hand[--i] = 1;
            return false;
        }
        return true;
    }
    return false;
}

int Perfect_Strategy(vector<double> hand, int dealer, bool canSplit, int &errorCode) {
    vector<vector<int>> twinTable {
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}, //1 1
        {4, 4, 4, 4, 4, 4, 1, 1, 1, 1}, //2 2 
        {4, 4, 4, 4, 4, 4, 1, 1, 1, 1}, //3 3
        {1, 1, 1, 4, 4, 1, 1, 1, 1, 1}, //4 4
        {3, 3, 3, 3, 3, 3, 3, 3, 1, 1}, //5 5
        {4, 4, 4, 4, 4, 1, 1, 1, 1, 1}, //6 6
        {4, 4, 4, 4, 4, 4, 1, 1, 1, 1}, //7 7
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}, //8 8
        {4, 4, 4, 4, 4, 2, 4, 4, 2, 2}, //9 9
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //10 10
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4}  //A A
    };
    vector<vector<int>> aceTable {
        {1, 1, 1, 3, 3, 1, 1, 1, 1, 1}, //A 2
        {1, 1, 1, 3, 3, 1, 1, 1, 1, 1}, //A 3
        {1, 1, 3, 3, 3, 1, 1, 1, 1, 1}, //A 4
        {1, 1, 3, 3, 3, 1, 1, 1, 1, 1}, //A 5
        {1, 3, 3, 3, 3, 1, 1, 1, 1, 1}, //A 6
        {2, 3, 3, 3, 3, 2, 2, 1, 1, 1}, //A 7
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //A 8
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //A 9
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}  //A 10
    };
    vector<vector<int>> totalTable {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //5
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //6
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //7
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //8
        {1, 3, 3, 3, 3, 1, 1, 1, 1, 1}, //9
        {3, 3, 3, 3, 3, 3, 3, 3, 1, 1}, //10
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 1}, //11
        {1, 1, 2, 2, 2, 1, 1, 1, 1, 1}, //12
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 1}, //13
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 1}, //14
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 1}, //15
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 1}, //16
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //17
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //18
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //19
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, //20
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}  //21
    };

    if (errorCode == 3) {
        errorCode = 0;
        return 1;
    }

    if (hand.size() == 2 && (hand[0] == hand[1] || ((int)hand[0] == 1 && (int)hand[1] == 11)) && canSplit) {
        return twinTable[(int)hand[0]-1][dealer-2];
    }
    int tot = 0;
    if (count(hand.begin(), hand.end(), 11)) {
        for(int i : hand) {
            if (i != 11) tot += i;
        }
        return aceTable[tot-2][dealer-2];
    }
    return totalTable[(int)accumulate(hand.begin(), hand.end(), 0)-5][dealer-2];
}

int main() {
    int PlayerWins = 0, DealerWins = 0, TotalGames = 0, gamesWanted = 100000;
    bool canSplit, manual;
    cout << "0: Automatic Play,    1: Manual Play" << endl;
    cin >> manual;
    if (manual == false) {
        cout << "How many games do you want to play?" << endl;
        cin >> gamesWanted;
    }
    vector<double> deck, dealer, player;
    int money = 500000, bet = 0, input = 5, savedCard = 0, decksplit = 0, errorCode = 0, errors = 0;
    decksplit = Generate_Deck(deck);
    while(TotalGames < gamesWanted && input != 0) {
        input = 5;
        bet = Deal_Hand(deck, dealer, player, money);
        canSplit = true;
        splitHand:
        while (input != 2 && Check_Bust(player) == false && input != 0) {
            if (manual) Print_Hands(false, dealer, player);  //////// Here is where hands are printed
            SkipPrint:
            if (manual == false) {
                input = Perfect_Strategy(player, dealer[0], canSplit, errorCode);
                if (input == 0) {
                    errors++;
                    input = 2;
                }
            }
            if (manual) cin >> input;
            switch(input) {
                case(1):
                    Deal_Card(deck, player);
                    break;
                case(2):
                    break;
                case(3):
                    if (money - bet >= 0 && player.size() == 2) {
                        if (manual) cout << "New Bet is $" << bet*2 << endl;
                        money -= bet;
                        bet *= 2;
                        Deal_Card(deck, player);
                        input = 2;
                    } else {
                        if (manual) cout << "Unable to Double Down" << endl;
                        errorCode = 3;
                        goto SkipPrint;
                    }
                    break;
                case(4):
                    if ((player[0] == player[1] || ((int)player[0] == 1 && (int)player[1] == 11)) && canSplit && player.size() == 2) {
                        canSplit == false;
                        savedCard = player.back(); player.pop_back();
                        if ((int)player[0] == 1) player[0] = 11;
                        Deal_Card(deck, player);
                        money -= bet;
                    } else {
                        cout << "Cannot Split Hand" << endl;
                        goto SkipPrint;
                    }
                    break;
                default:
                    cout << "Invalid Input" << endl;
                    break;

            }
        }
        if (Check_Bust(player) == true) {
            if (manual) Print_Hands(true, dealer, player); //Another PrintHands
            if (manual) cout << "DEALER WINS" << endl;
            DealerWins++;
            goto EndGame;
        }
        while (Check_Bust(dealer) == false && (int)accumulate(dealer.begin(), dealer.end(), 0) < 17) {
            Deal_Card(deck, dealer);
        }
        if (manual && savedCard == 0) Print_Hands(true, dealer, player); //PrintHands 2
        if (Check_Bust(dealer) == true) {
            if (manual) cout << "PLAYER WINS   Player recieves: $" << bet*2 << endl;
            money += bet*2;
            PlayerWins++;
        } else if ((int)accumulate(dealer.begin(), dealer.end(), 0) > (int)accumulate(player.begin(), player.end(), 0)) {
            if (manual) cout << "DEALER WINS" << endl;
            DealerWins++;
        } else if ((int)accumulate(dealer.begin(), dealer.end(), 0) == (int)accumulate(player.begin(), player.end(), 0)) {
            if (manual) cout << "TIE GAME      Player recieves: $" << bet << endl;
            money += bet;
        } else {
            if (manual) cout << "PLAYER WINS   Player recieves: $" << bet*2 << endl;
            money += bet*2;
            PlayerWins++;
        }
        EndGame:
        if (savedCard != 0) {
            player.clear();
            player.push_back(savedCard);
            Deal_Card(deck, player);
            savedCard = 0;
            input = 5;
            goto splitHand;
        }
        if (deck.size() <= decksplit) {
            decksplit = Generate_Deck(deck);
            if (manual) cout << "-----DECK HAS BEEN RESET-----" << endl;
        }
        TotalGames++;
        if (manual) cout << endl << "=============================NEW GAME===============================" << endl;
    }

    cout << "There were a total of " << errors << " errors" << endl;
    cout << "There were " << TotalGames << " Games in total!" << endl;
    cout << "Player Won " << PlayerWins << " Games!" << endl;
    cout << "Dealer Won " << DealerWins << " Games!" << endl;
    cout << "The Player has $" << money << endl;
}