//BIG PROJECT Part 1
//Tyler Nee, Vincent Hew, Rishabh Tewari
//Resolve Arrows was left out purposefully; it was causing strange errors when it wiped out more than one player at once.
//Vincent version 1.3.3

#include <iostream>
#include <conio.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#define MAX 5
using namespace std;

typedef struct die{
    int val;
    bool locked;
}die;

typedef struct player{
    int hp;
    int maxhp;
    int arrowsHeld;
    int role; //0 Sheriff, 1 Deputy, 2 Outlaw, 3 Renegade
    int ability;
	int tag;
    player *left;
    player *right;
}player;

void resolveDice(player *currPlayer, int reroll); //master function
//Supporting Functions
void kaboom(player *currPlayer); //No need to revise
void resolveArrows(player *sheriff); //done dice val 5
int listPlayers(player *sheriff); //done
void sixFeetUnder(player *deceased); //done
//Supporting Function
int checkVictoryConditions(player *currPlayer); //done
//** Vincent Revised
void display(player *first_player);
void shoot(player *currPlayer, int diceVal, int total_playerCount);
player *createPlayer(int position);
player *generatePlayers(int playerCount);
void assign_role(player *current_player, int total_playerCount);
void giveBeer(player *currPlayer, int total_playerCount);
void rollDice(player *currPlayer);
void initializeDice(die *dice[]);
void clearDice(die *dice[]);
void gatling(player *currPlayer);
//**
player *first_player = NULL;
int deputy_count = 0;
int outlaws_count = 0;
int renegades_count = 0;
bool role_available[4];
bool player_dead_status[8];
//**    
player *sheriff;
int arrowsRemaining = 9;
die *dice[MAX];
int winCondition = 3;

int main()
{
    srand(time(0));
	initializeDice(dice);
    int initial_playerCout = (rand() % (8 - 4 + 1)) + 4;
    cout << "Player initial amount: " << initial_playerCout << endl;
    first_player = generatePlayers(initial_playerCout);
    display(first_player);
    /*
    for(int i = 0; i < initial_playerCout; i++) {
        rollDice(first_player);
        first_player = first_player->right;
        clearDice(dice);
    }
    */


    /*
    while(winCondition == 3)
    {
		while(currentPlayer->hp == 0)
		{
			currentPlayer = currentPlayer->right;
			sixFeetUnder(currentPlayer->left);
		}
        resolveDice(currentPlayer, 0);
		clearDice(dice);
		winCondition = checkVictoryConditions(currentPlayer);
        currentPlayer = currentPlayer->right;
    }
    */
   return 0;
}

int listPlayers(player *sheriff)
{
    int playerCount = 0;
    player *temp = sheriff;
    do{
        cout << "["<<playerCount<<"] HP: "<<temp->hp<<" Arrows Held: "<<temp->arrowsHeld;
		if(temp->role == 0)
		{
			cout << " SHERIFF ";
		}
		if(temp->role == 2)
		{
			cout << " OUTLAW ";
		}
		if(temp->role == 3)
		{
			cout << " RENEGADE ";
		}
		
        if(temp->hp <= 0)
        {
            cout << " DEAD ";
        }
        else
        {
            playerCount++;
        }
		cout <<endl;
        temp = temp->right;
    } while(temp != sheriff);
    //playerCount returns the amount of live players still in the game.
    return playerCount;
}

void resolveArrows(player *currPlayer) {
	cout << "RESOLVING ARROWS" << endl;
    player *current = currPlayer;
    do{
         current->hp -= current->arrowsHeld;
         arrowsRemaining += current->arrowsHeld;
		 if(current->hp <= 0) {
			 sixFeetUnder(current);
			 cout << "SixFeetUnder ran successfully "<<endl;
		}
         current->arrowsHeld = 0;
         current = current->right;
		 cout << "CurrentPlayer is " << current->tag << endl;
    } while(current != currPlayer);
    cout << "Arrows Resolved...\n";
}

void giveBeer(player *currPlayer, int total_playerCount) {
	cout << "Giving Beer..." << endl;
    int target_position = rand() % total_playerCount;
    player *recipient = currPlayer;
    while(player_dead_status[target_position] != false) {
        target_position = rand() % total_playerCount;
    }
    while(recipient->tag != target_position) {
        recipient = recipient->right;
    }
    if(recipient->hp != recipient->maxhp) {
		cout << "Recipient " << recipient->tag << " was healed" << endl;
        recipient->hp++;
	}
	else {
		cout << "Recipient " << recipient->tag << " already maxed out" << endl;
	}
}

void kaboom(player *currPlayer) {
    cout << "Dynamite Triggered: Rerolls Restricted" << endl;
    for(int i = 0; i < MAX; i++) {
        dice[i]->locked = true;
    }
    currPlayer->hp--;
}

int checkVictoryConditions(player *currPlayer)
{
    int outlaws = 0;
    player *current = currPlayer;
	do{
		//cout << "Current Role is " << current->role <<endl;
        if(current->role == 2 || current->role == 3)
        {
            //cout << "Outlaw Detected\n";
            outlaws++;
        }
        current = current->right;
    }while(current != currPlayer);

    if(outlaws == 0)
    {
        cout << "The Sheriff has driven crime from the town.\nTHE LAW WINS \n";
        return 0;
    }

    if(listPlayers(currPlayer) == 1 && currPlayer->role == 3)
    {
        cout << "The Renegade is the last man standing. \nTHE RENEGADE WINS \n";
        return 1;
    }
    else if(sheriff->hp == 0)
    {
        cout << "The Outlaws have killed the Sheriff. \nTHE OUTLAWS WIN \n";
        return 2;
    }
    else
    {
        //cout << "No Win Condition has been met."<<endl;
        return 3;
    }
    
}

void gatling(player *currPlayer) {
    player *enemy;
	cout << "Gatling Effect Triggered" << endl;
	arrowsRemaining = currPlayer->arrowsHeld + arrowsRemaining;
	currPlayer->arrowsHeld = 0;
    enemy = currPlayer->right;
    while(enemy != currPlayer) {
        if(player_dead_status[enemy->tag] != false) {
            enemy = enemy->right;
            continue;
        }
        (enemy->hp)--;
        if(enemy->hp <= 0) {
            player_dead_status[enemy->tag] = true;
        }
        enemy = enemy->right;
    }
}

void sixFeetUnder(player *deceased)
{
    deceased->left->right = deceased->right;
    deceased->right->left = deceased->left;
    cout << "Player " << deceased->tag << " has died."<<endl;
}

void rollDice(player *currPlayer) {
    cout << "current player: " << currPlayer->tag << "\trole: " << currPlayer->role << " is rolling the dice." << endl;
    for(int i = 0; i < MAX; i++) {
        if(dice[i]->locked == false) {
            dice[i]->val = (rand()%(5 - 0 + 1) + 0);
			cout << "Dice["<<i<<"] rolled as " << dice[i]->val<<endl;
        }
		else {
			cout << "Dice["<<i<<"] is locked as " << dice[i]->val<<endl;
		}
    }
    cout << "current player tag: " << currPlayer->tag << " finished." << endl;
}

void resolveDice(player *currPlayer, int reroll) {
	rollDice(currPlayer);
    int gats = 0;       //Holds Amount of Gatling Die
    int dyna = 0;       //Holds Amount of Dynamite Die
    for(int i = 0; i < MAX; i++) {
        if(dice[i]->val == 4) {
            dice[i]->locked = true;
            dyna++;
        }
        if(dice[i]->val == 5 && dice[i]->locked == false) {
			cout << "Player took an arrow\n";
            currPlayer->arrowsHeld++;
            arrowsRemaining--;
			if(arrowsRemaining == 0) {
				resolveArrows(currPlayer);
			}
        }
    }
	if(currPlayer->hp <= 0) {
		cout << "Player died during own turn.\n";
		sixFeetUnder(currPlayer);
		currPlayer = currPlayer->right;
	}
    if(reroll == 2 || dyna == 3) {
		if(dyna == 3) {
			kaboom(currPlayer);
		}
        cout << "Max Rerolls Reached \n";
        for(int i = 0; i < MAX; i++) {
            switch (dice[i]->val) {
                //case 0: giveBeer(currPlayer);
                //break;
                /*case 1: shoot(currPlayer, dice[i]->val);
                break;
                case 2: shoot(currPlayer, dice[i]->val);
                break;*/
                case 3: 
                    gats++;
                    if(gats == 3)
                    {
                        gatling(currPlayer);
                    }
                break;
            }
        }
    }
    else
    {
        cout << "Selecting dice to keep locked in..."<<endl;
        for(int i = 0; i < MAX; i++)
        {
            if(dice[i]->locked == false)
            {
                if((rand()% 2) == 0)
                {
                    dice[i]->locked = true;
                }
            }
        }
        resolveDice(currPlayer,reroll+1);
    }
}

void shoot(player *currPlayer, int diceVal, int total_playerCount) {
    int random_target = (rand() % (2 - 1 + 1)) + 1;
    int alive_playerCount = 0;
	player *target;
	player *current = currPlayer;
    for(int i = 0; i < total_playerCount; i++) {
        if(player_dead_status[i] == false)
            alive_playerCount++;
    }
    if(diceVal == 1) {
        if(random_target == 1) {
            while(player_dead_status[current->right->tag] != false) {
                current = current->right;
            }
            target = current->right;
        }
        else {
            while(player_dead_status[current->left->tag] != false) {
                current = current->left;
            }
            target = current->left;
        }
	}
    else if(diceVal == 2) {
        if(alive_playerCount == 2 || alive_playerCount == 3) {
            shoot(currPlayer, 1, total_playerCount);
            return;
        }
        else {
            if(random_target == 1) {
                while(player_dead_status[current->right->tag] != false) {
                    current = current->right;
                }
                current = current->right;
                while(player_dead_status[current->right->tag] != false) {
                    current = current->right;
                }
                target = current->right;
            }
            else {
                while(player_dead_status[current->left->tag] != false) {
                    current = current->left;
                }
                current = current->left;
                while(player_dead_status[current->left->tag] != false) {
                    current = current->left;
                }
                target = current->left;
            }
        }
    }
    if(target->tag == currPlayer->tag)
        return;
    (target->hp)--;
	if(target->hp <= 0) {
        player_dead_status[target->tag] = true;
    }

} 

player *createPlayer(int position) {
    player *newPlayer = (player*)malloc(sizeof(player));
    int initial_playerCout = (rand() % (8 - 4 + 1)) + 4;
    newPlayer->hp = 9;
    newPlayer->maxhp = 9;
    newPlayer->arrowsHeld = 0;
    newPlayer->tag = position;
    newPlayer->left = NULL;
    newPlayer->right = NULL;
    return newPlayer;
}

player *generatePlayers(int playerCount)
{
    player *new_player, *prev_player;
    first_player = createPlayer(0);
    assign_role(first_player, playerCount);
    prev_player = first_player;
    for(int i = 1; i < playerCount; i++) {
        new_player = createPlayer(i);
        assign_role(new_player, playerCount);
        new_player->right = prev_player;
        prev_player->left = new_player;
        prev_player = new_player;
    }
    prev_player->left = first_player;
    first_player->right = prev_player;
    return first_player;
}

void assign_role(player *current_player, int total_playerCount) {
    int role_position = (rand() % (3 - 0 + 1)) + 0;
    int i;
    if(role_available[role_position] != false) {
    }
    else if(role_available[role_position] == false) {
    }
    if(total_playerCount == 4) {
        role_available[1] = true;
    }
    while(role_available[role_position] != false) {
        role_position = (rand() % (3 - 0 + 1)) + 0;
    }
    if(role_position == 0) {
        current_player->role = role_position;
        current_player->maxhp = current_player->maxhp+2;
        current_player->hp = current_player->hp+2;
        role_available[0] = true;
    }
    else if(role_position == 1) {
        if(total_playerCount == 5 || total_playerCount == 6) {
            current_player->role = role_position;
            role_available[1] = true;
        }
        else {
            current_player->role = role_position;
            deputy_count++;
            if(deputy_count == 2) {
                role_available[1] = true;
            }
        }
    }
    else if(role_position == 2) {
        if(total_playerCount == 4 || total_playerCount == 5) {
            current_player->role = role_position;
            outlaws_count++;
            if(outlaws_count == 2) {
                role_available[2] = true;
            }
        }
        else {
            current_player->role = role_position;
            outlaws_count++;
            if(outlaws_count == 3) {
                role_available[2] = true;
            }
        }
    }
    else if(role_position == 3) {
        if(total_playerCount == 8) {
            current_player->role = role_position;
            renegades_count++;
            if(renegades_count == 2) {
                role_available[3] = true;
            }
        }
        else {
            current_player->role = role_position;
            role_available[3] = true;
        }
    }
}
//displayer player in the game
void display(player *first_player) {
    player *current;
    current = first_player;
    cout << "current player tag: " << current->tag << "\trole:" << current->role;
    cout <<"\thp:" << current->hp << "/" << current->maxhp << endl;
    current = current->left;
    while(current->tag != 0) {
        cout << "current player tag: " << current->tag << "\trole:" << current->role;
        cout <<"\thp:" << current->hp << "/" << current->maxhp << endl;
        current = current->left;
    }
}

void initializeDice(die *dice[]) {
	for(int i = 0; i < MAX; i++) {
		die *newDie = (die*)malloc(sizeof(die));
		dice[i] = newDie;
		dice[i]->locked = false;
		dice[i]->val = 0;
	}
}

void clearDice(die *dice[]) {
	for(int i = 0; i < MAX; i++) {
		dice[i]->locked = false;
		dice[i]->val = 0;
	}
}