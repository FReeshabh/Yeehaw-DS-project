//BIG PROJECT Part 2
//Tyler Nee, Vincent Hew, Rishabh Tewari
//game working correctly except naming issue
//using vincent_test version 1.7.7

#include <iostream>
#include <time.h>
#include <cstring>
#define MAX 5
#define SHERIFF  0
#define DEPUTY   1
#define OUTLAW   2
#define RENEGADE 3
#define GAME_CONTINUE 4
#define BEER 0
#define SHOOT_1 1
#define SHOOT_2 2
#define GATLING 3
#define DYNAMITE 4
#define ARROW 5
using namespace std;

typedef struct die{
    int val;
    bool locked;
}die;

typedef struct player{
    int hp;
    int maxhp;
    int arrowsHeld;
    int role;
    int ability;
	int tag;
    player *left;
    player *right;
}player;

void resolveDice(player *currPlayer, int reroll); //master function
//Supporting Functions
void kaboom(player *currPlayer);
void resolveArrows(player *sheriff); //done dice val 5
void sixFeetUnder(player *deceased); //done
//Supporting Function
int checkVictoryConditions(player *currPlayer);
//**
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
int getBehaviorModifier(player *currPlayer, int diceValue); //When you call random, the func will instead return a value that is checked against random, depending on the current player's role and the value of the dice
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

int main() {
    srand(time(0));
	initializeDice(dice);
    int initial_playerCount = (rand() % (8 - 4 + 1)) + 4;
    cout << "Player initial amount: " << initial_playerCount << endl;
    first_player = generatePlayers(initial_playerCount);
    display(first_player);
    
/*
    for(int i = 0; i < 10; i++) {
		sheriff->arrowsHeld++;
		arrowsRemaining--;
		if(arrowsRemaining == 0)
			resolveArrows(sheriff);
        display(first_player);
		
    }
*/  
    for(int i = 0; i < 14; i++) {
		gatling(first_player);
        display(first_player);
		if(checkVictoryConditions(first_player) != GAME_CONTINUE) {
            return 0;
        }
    }

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

void resolveArrows(player *currPlayer) {
	cout << "RESOLVING ARROWS" << endl;
	player *current = currPlayer;
	do {
		current->hp -= current->arrowsHeld;
		cout << "Player " << current->tag << " sustained " << current->arrowsHeld << " damage " <<endl;
         	arrowsRemaining += current->arrowsHeld;
		 if(current->hp <= 0) {
			 sixFeetUnder(current);
		}
        current->arrowsHeld = 0;
        current = current->left;
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
    if(currPlayer->hp <= 0) {
        sixFeetUnder(currPlayer);
    }
}

int checkVictoryConditions(player *currPlayer) {
    int criminals_total = 0;
    int criminals_outlaw = 0;
    int criminals_renegade = 0;
    int alive_deputy = 0;
    int alive_playerCount = 0;
    player *current = currPlayer;
	do{
        if(player_dead_status[current->tag] == false) {
            alive_playerCount++;
            if(current->role == 2)
                criminals_outlaw++;
            else if(current->role == 3)
                criminals_renegade++;
            else if(current->role == 1)
                alive_deputy++;
        }
        current = current->left;
    }while(current != currPlayer);
    criminals_total = criminals_outlaw+criminals_renegade;
    if(alive_playerCount == 0) {
        cout << "THE OUTLAW WINS" << endl;
        return OUTLAW;
    }
    else if(criminals_total == 0) {
        cout << "THE LAW WINS" << endl;
        return SHERIFF;
    }
    else if(alive_playerCount == 1 && criminals_renegade == 1) {
        cout << "THE RENEGADE WINS" << endl;
        return RENEGADE;
    }
    else if(sheriff->hp == 0) {
        cout << "THE OUTLAWS WINS" << endl;
        return OUTLAW;
    }
    else if((criminals_renegade+alive_deputy) == alive_playerCount) {
        cout << "THE OUTLAW WINS" << endl;
        return OUTLAW;
    }
    else {
        cout << "GAME CONTINUE" << endl;
        return GAME_CONTINUE;
    }
}

void gatling(player *currPlayer) {
    player *enemy;
	cout << "Gatling Effect Triggered" << endl;
	arrowsRemaining = currPlayer->arrowsHeld + arrowsRemaining;
	currPlayer->arrowsHeld = 0;
    enemy = currPlayer->left;
    while(enemy != currPlayer) {
        if(player_dead_status[enemy->tag] != false) {
            enemy = enemy->left;
            continue;
        }
        (enemy->hp)--;
        if(enemy->hp <= 0) {
            sixFeetUnder(enemy);
        }
        enemy = enemy->left;
    }
}


void sixFeetUnder(player *deceased) {
	if(deceased->hp <= 0) {
		deceased->hp = 0;
	}
	cout << "Player " << deceased->tag << " has died." << endl;
	cout << "Player " << deceased->tag << " was a ";
	switch(deceased->role) {
		case 0: cout << " sheriff.";
		break;
		case 1: cout << " deputy.";
		break;
		case 2: cout << "an outlaw.";
		break;
		case 3: cout << " renegade.";
		break;
	}
	cout << endl << "They dropped " << deceased->arrowsHeld << " arrows."<< endl;
	arrowsRemaining += deceased->arrowsHeld;
	deceased->arrowsHeld = 0;
	player_dead_status[deceased->tag] = true;
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
    int gats = 0;       //Holds Amount of Gatling Die
    int dyna = 0;       //Holds Amount of Dynamite Die
    int bullet_1 = 0;
    int bullet_2 = 0;
    int beer = 0;
    int reroll_option;
    rollDice(currPlayer);
    for(int i = 0; i < MAX; i++) {
        switch (dice[i]->val) {
            case DYNAMITE:
            {
                dice[i]->locked = true;
                dyna++;
                if(dyna == 3) {
                    kaboom(currPlayer);
                    checkVictoryConditions(currPlayer);
                }
                break;
            }
            case ARROW:
            {
                currPlayer->arrowsHeld++;
                arrowsRemaining--;
                if(arrowsRemaining == 0) {
				    resolveArrows(currPlayer);
                    checkVictoryConditions(currPlayer);
			    }
                break;
            }
            case GATLING:
            {
                gats++;
                if(gats == 3) {
                    gatling(currPlayer);
                    checkVictoryConditions(currPlayer);
                }
                break;
            }
            case BEER:
            {
                beer++;
                break;
            }
            case SHOOT_1:
            {
                bullet_1++;
                break;
            }
            case SHOOT_2:
            {
                bullet_2++;
                break;
            }
        }
    }
    /*
    if(reroll <= 2) {
        reroll_option = (rand() % (2 - 1 + 1)) + 1;
        if() {

        }
    }
    */
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
        sixFeetUnder(target);
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
		sheriff = current_player;
        role_available[0] = true;
    }
    else if(role_position == 1) {
        if(total_playerCount == 5 || total_playerCount == 6) {
            current_player->role = role_position;
            deputy_count++;
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
            renegades_count++;
            role_available[3] = true;
        }
    }
}
//displayer player in the game
void display(player *first_player) {
    player *current;
    current = first_player;
    cout << "***Current Player List Status***" << endl;
    cout << "Player " << current->tag << " Role: " << current->role;
    cout <<"\thp:" << current->hp << "/" << current->maxhp << endl;
    current = current->left;
    while(current->tag != 0) {
        cout << "Player " << current->tag << " Role: " << current->role;
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
int getBehaviorModifier(player *currPlayer, int dieValue)
{
	int be = 0; 
	//Creates a number based off of role and die type that a random 1d6 checks against. 
	//Higher values make it less likely that a die will be locked.
	//0 guarantees a lock in, 7 guarantees a reroll attempt.
	switch(currPlayer->role)
	{
		case SHERIFF:
		cout << "Sheriff's Resolve"<<endl;
		//The sheriff will prioritize healing himself. He won't attack targets that have healed him in the last two turns.
		//Will always trigger gatling when available.
		switch(dieValue)
		{
			case BEER:
			if(currPlayer->hp < currPlayer->maxhp)
				be = 0;
			else
				be = 7;
			break;
			case SHOOT_1:
			be = 3;
			break;
			case SHOOT_2:
			be = 3;
			break;
			case GATLING:
			//Priority
			be = 0;
			break;
			case DYNAMITE:
			//Forces a lockin.
			be = 0;
			break;
		}
		break;
		case DEPUTY:
		cout << "Deputy's Reasoning"<<endl;
		//Deputies will prioritize healing the sheriff, and themselves if they fall low.
		//If they have to shoot, the sheriff will never be targeted.
		switch(dieValue)
		{
			case BEER:
			//Prioritized
			if(sheriff->hp < sheriff->maxhp || currPlayer->hp < currPlayer->maxhp)
				be = 0;
			else
			//No Use
				be = 7;
			break;
			case SHOOT_1:
			be = 3;
			break;
			case SHOOT_2:
			be = 3;
			break;
			case GATLING:
			be = 0;
			break;
			case DYNAMITE:
			//Forces a lockin.
			be = 0;
			break;
		}
		break;
		case OUTLAW:
		cout << "Outlaw's Revenge"<<endl;
		//Outlaws will target the sheriff whenever possible. They will heal themselves when low.
		switch(dieValue)
		{
			case BEER:
			if(currPlayer->hp < (currPlayer->hp/2))
				be = 0;
			else
				be = 2;
			break;
			//Shoot cases need to determine available targets. Outlaws will always lock if the sheriff can be hit.
			//They will only occasionally lock if the sheriff is not a valid target.
			case SHOOT_1:
			be = 3;
			break;
			case SHOOT_2:
			be = 3;
			break;
			//Outlaws play on a team usually larger than the Law's. They will avoid gatling under most circumstances.
			//They will attempt to use the gatling if they have too many arrows, the sheriff is not a valid target, or they are outnumbered.
			case GATLING:
			be = 7;
			//Check dead Lawmen against dead Outlaws.
			//Check if sheriff is not 1 or 2 spaces away.
			if(currPlayer->arrowsHeld > 3)
				be -= 3;
			break;
			case DYNAMITE:
			//Forces a lockin.
			be = 0;
			break;
		}
		break;
		case RENEGADE:
		cout << "Renegade's Rage"<<endl;
		//Renegades prioritize healing themselves. They will start healing the sheriff if he falls below half health.
		//Will always trigger gatling when available. Will always attack the lowest hp target on his target list.
		switch(dieValue)
		{
			case BEER:
			//Prioritized
			if(currPlayer->hp < currPlayer->maxhp)
				be = 0;
			else if(sheriff->hp < (sheriff->maxhp/2))
				be = 0;
			else
				be = 7;
			break;
			case SHOOT_1:
			//Check for lowest target
			be = 4;
			break;
			case SHOOT_2:
			//Check for lowest target
			be = 4;
			break;
			case GATLING:
			//Prioritized
			be = 0;
			break;
			case DYNAMITE:
			//Forces a lockin.
			be = 0;
			break;
		}
		break;
	}
	if(dieValue == ARROW)
		be = 7;
	//Arrows have no utility and will always be rerolled.
	return be;
}