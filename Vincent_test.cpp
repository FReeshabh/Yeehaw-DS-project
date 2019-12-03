//BIG PROJECT Part 1
//Tyler Nee, Vincent Hew, Rishabh Tewari
//game working correctly except naming issue
//Vincent version 1.7.8

#include <iostream>
#include <malloc.h>
#include <time.h>
#include <string.h>
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

//Headers
//create player functions
player *createPlayer(int position);
player *generatePlayers(int playerCount);
void assign_role(player *current_player, int total_playerCount);
string name_display(int position);
//functions for roll and resolve the dices
void initializeDice(die *dice[]);
void rollDice(player *currPlayer);
void clearDice(die *dice[]);
void resolveDice(player *currPlayer, int reroll, int total_playerCount);
//each dice value action
void kaboom(player *currPlayer);
void resolveArrows(player *sheriff); 
void shoot(player *currPlayer, int diceVal, int total_playerCount);
void giveBeer(player *currPlayer, int total_playerCount);
void gatling(player *currPlayer);
//victory check functions
void sixFeetUnder(player *deceased); 
int checkVictoryConditions(player *currPlayer);
//display function
void display(player *first_player);

//Variables 
player *first_player = NULL;            //first player as root
int deputy_count = 0;                   //availabe deputy count
int outlaws_count = 0;                  //available outlaw count
int renegades_count = 0;                //available renegades count
bool role_available[4];                 //check which roles still need to be assign
bool player_dead_status[8];             //check which player is dead
bool name_check[8];                     //naming system to check which name are used
player *sheriff;                        //pointer always point to sheriff
int arrowsRemaining = 9;                //the number of remaining arrow tokens
die *dice[MAX];                         //keep the dice value
int winCondition = GAME_CONTINUE;       //win condition = 4
int initial_playerCount = 4;
string name_list[8] = {"Vincent","Tyler","Rishabh","John","Rosa","Monica","Lisa","Albert"};
string name_join[8];

int main() {
    srand(time(0));
	initializeDice(dice);
    int round_count = 1;
    initial_playerCount = (rand() % (8 - 4 + 1)) + 4;
    player *current;
    cout << "Player initial amount: " << initial_playerCount << endl;
    first_player = generatePlayers(initial_playerCount);
    display(first_player);
    //finished generating players in the game
    //game start
    /*
    cout << endl << "****Game Start****" << endl;
    current = first_player;
    while(winCondition == GAME_CONTINUE) {
        cout << "****Round: " << round_count << endl;
        for(int i = 0; i < initial_playerCount; i++) {
            resolveDice(current, 0, initial_playerCount);
            current = current->left;
            cout << endl;
            display(first_player);
            cout << endl;
            if(winCondition != GAME_CONTINUE) {
                return 0;
            }
        }
        cout << "****End of round: " << round_count << endl;
        cout << endl;
        round_count++;
    }
    //end of game
    */
    return 0;
}

// NAME : resolveArrows
// INPUT PARAMETERS: currPlayer
// OUTPUT: none 
// PURPOSE: triggered when there are no more arrow token remains and take 1 hp aways from every players. 
void resolveArrows(player *currPlayer) {
	cout << "No more arrow token!" << endl;
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
}

// NAME : giveBeer
// INPUT PARAMETERS: currPlayer, total_playerCount
// OUTPUT: none 
// PURPOSE: Heal random target 1 hp when it triggered. 
void giveBeer(player *currPlayer, int total_playerCount) {
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

// NAME : kaboom
// INPUT PARAMETERS: currPlayer
// OUTPUT: none 
// PURPOSE: take 1 hp away from the current player when it triggered. 
void kaboom(player *currPlayer) {
    cout << "OH NO! Dynamite exploded!" << endl;
    for(int i = 0; i < MAX; i++) {
        dice[i]->locked = true;
    }
    currPlayer->hp--;
    if(currPlayer->hp <= 0) {
        sixFeetUnder(currPlayer);
    }
}

// NAME : checkVictoryConditions
// INPUT PARAMETERS: currPlayer
// OUTPUT: it returns the current game status: OUTLAW, RENEGADE, SHERIFF, or GAME_CONTINUE
// PURPOSE: Determine who win the game or the game continue. 
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
        return GAME_CONTINUE;
    }
}

// NAME : gatling
// INPUT PARAMETERS: currPlayer
// OUTPUT: none
// PURPOSE: shoot every player once except the player who triggered. 
void gatling(player *currPlayer) {
    cout << "It's high noon. BANG!" << endl;
    player *enemy;
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

// NAME : sixFeetUnder
// INPUT PARAMETERS: deceased
// OUTPUT: output messages
// PURPOSE: reveal the player role when they died. 
void sixFeetUnder(player *deceased) {
	if(deceased->hp < 0) {
		deceased->hp = 0;
	}
	cout << "**Player " << deceased->tag << " has died." << endl;
	cout << "**They were a";
	switch(deceased->role) {
		case 0: cout << " sheriff.";
		break;
		case 1: cout << " deputy.";
		break;
		case 2: cout << "n outlaw.";
		break;
		case 3: cout << " renegade.";
		break;
	}
    cout << endl;
	arrowsRemaining += deceased->arrowsHeld;
	deceased->arrowsHeld = 0;
	player_dead_status[deceased->tag] = true;
}

// NAME : rollDice
// INPUT PARAMETERS: currPlayer
// OUTPUT: output messages
// PURPOSE: roll the dice and keep in dice array. 
void rollDice(player *currPlayer) {
    cout << "current player: " << currPlayer->tag << "\trole: " << currPlayer->role << " is rolling the dice." << endl;
    cout << "Current dice result:\t";
    for(int i = 0; i < MAX; i++) {
        dice[i]->val = (rand()%(5 - 0 + 1) + 0);
        cout << dice[i]->val << "\t";
    }
    cout << endl;
}

// NAME : resolveDice
// INPUT PARAMETERS: currPlayer, reroll_count, total_playerCount
// OUTPUT: none
// PURPOSE: resolve the dice for current player
void resolveDice(player *currPlayer, int reroll_count, int total_playerCount) {
    int gats = 0;       //Holds Amount of Gatling Die
    int dyna = 0;       //Holds Amount of Dynamite Die
    int bullet_1 = 0;
    int bullet_2 = 0;
    int beer = 0;
    int reroll_option;
    bool reroll_record = false;
    rollDice(currPlayer);
    for(int i = 0; i < MAX; i++) {
        reroll_option = (rand() % (2 - 1 + 1)) + 1;
        if(reroll_count < 2 && dice[i]->val != DYNAMITE && reroll_option == 1) {
            if(dice[i]->val == ARROW) {
                currPlayer->arrowsHeld++;
                arrowsRemaining--;
                if(arrowsRemaining == 0) {
				    resolveArrows(currPlayer);
                    winCondition = checkVictoryConditions(currPlayer);
                    if(winCondition != GAME_CONTINUE) {
                        return;
                    }
			    }
            }
            cout << "Rerolling the "<< i+1 <<"th dice for the " << reroll_count+1 << " times." << endl;
            dice[i]->val = (rand() % (5 - 0 + 1)) + 0;
            reroll_record = true;
            reroll_count++;
            i--;
            continue;
        }
        else if(reroll_option == 2) {
            reroll_count = 0;
        }
        switch (dice[i]->val) {
            case DYNAMITE:
            {
                dice[i]->locked = true;
                dyna++;
                if(dyna == 3) {
                    kaboom(currPlayer);
                    winCondition = checkVictoryConditions(currPlayer);
                    if(winCondition != GAME_CONTINUE) {
                        return;
                    }
                }
                break;
            }
            case ARROW:
            {
                currPlayer->arrowsHeld++;
                arrowsRemaining--;
                if(arrowsRemaining == 0) {
				    resolveArrows(currPlayer);
                    winCondition = checkVictoryConditions(currPlayer);
                    if(winCondition != GAME_CONTINUE) {
                        return;
                    }
			    }
                break;
            }
            case GATLING:
            {
                gats++;
                if(gats == 3) {
                    gatling(currPlayer);
                    winCondition = checkVictoryConditions(currPlayer);
                    if(winCondition != GAME_CONTINUE) {
                        return;
                    }
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
    if(reroll_record == true) {
        cout << "Updated dice result:\t";
        for(int i = 0; i < MAX; i++) {
                cout << dice[i]->val << "\t";
        }
        cout << endl;
    }
    for(int i = 0; i < bullet_1; i++) {
        shoot(currPlayer, 1, total_playerCount);
        winCondition = checkVictoryConditions(currPlayer);
        if(winCondition != GAME_CONTINUE) {
            return;
        }
    }
    for(int i = 0; i < bullet_2; i++) {
        shoot(currPlayer, 2, total_playerCount);
        winCondition = checkVictoryConditions(currPlayer);
        if(winCondition != GAME_CONTINUE) {
            return;
        }
    }
    for(int i = 0; i < beer; i++) {
        giveBeer(currPlayer, total_playerCount);
    }
    clearDice(dice);
}

// NAME : shoot
// INPUT PARAMETERS: currPlayer, diceVal, total_playerCount
// OUTPUT: none
// PURPOSE: shoot either left or right of range one and two when it triggered
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
            cout << "Player "<< target->tag << " got shot." << endl;
        }
        else {
            while(player_dead_status[current->left->tag] != false) {
                current = current->left;
            }
            target = current->left;
            cout << "Player "<< target->tag << " got shot." << endl;
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
                cout << "Player "<< target->tag << " got shot." << endl;
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
                cout << "Player "<< target->tag << " got shot." << endl;
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

// NAME : createPlayer
// INPUT PARAMETERS: position
// OUTPUT: newPlayer
// PURPOSE: create new player node
player *createPlayer(int position) {
    player *newPlayer = (player*)malloc(sizeof(player));
    newPlayer->hp = 9;
    newPlayer->maxhp = 9;
    newPlayer->arrowsHeld = 0;
    newPlayer->tag = position;
    newPlayer->left = NULL;
    newPlayer->right = NULL;
    return newPlayer;
}

// NAME : name_assign
// INPUT PARAMETERS: currPlayer
// OUTPUT: none
// PURPOSE: give random player name to each player
string name_display(int position) {
    int index_namelist = (rand() % (7 - 0 + 1)) + 0;
    while(name_check[index_namelist] != false) {
        index_namelist = (rand() % (7 - 0 + 1)) + 0;
    }
    name_check[index_namelist] = true;
    name_join[position] = 
    return name_list[index_namelist];
}

// NAME : generatePlayers
// INPUT PARAMETERS: playerCount
// OUTPUT: first_player
// PURPOSE: generate players in the game
player *generatePlayers(int playerCount) {
    player *new_player, *prev_player;

    cout << "Creating root player:" << endl;

    first_player = createPlayer(0);
    assign_role(first_player, playerCount);
    prev_player = first_player;
    for(int i = 1; i < playerCount; i++) {

        cout << "*** Creating player:*** " << i << endl;

        new_player = createPlayer(i);
        assign_role(new_player, playerCount);
        new_player->right = prev_player;
        prev_player->left = new_player;
        prev_player = new_player;

        cout << "*** end of creating player *** " << i << endl;
    }
    prev_player->left = first_player;
    first_player->right = prev_player;
    return first_player;
}

// NAME : assign_role
// INPUT PARAMETERS: current_player, total_playerCount
// OUTPUT: none
// PURPOSE: assign different random roles for each player
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

// NAME : display
// INPUT PARAMETERS: first_player
// OUTPUT: none
// PURPOSE: display the current player list status
void display(player *first_player) {
    player *current;
    current = first_player;
    cout << "***Current Player List Status***" << endl;
    int count = 0;
    do {
        cout << "Player " << current->tag << " Role: " << current->role;
        cout <<"\thp:" << current->hp << "/" << current->maxhp << endl;
        current = current->left;
        count = count + 1;
    }while(current->tag != 0 || count < initial_playerCount);
}

// NAME : initializeDice
// INPUT PARAMETERS: dice[]
// OUTPUT: none
// PURPOSE: initialize the dice
void initializeDice(die *dice[]) {
	for(int i = 0; i < MAX; i++) {
		die *newDie = (die*)malloc(sizeof(die));
		dice[i] = newDie;
		dice[i]->locked = false;
		dice[i]->val = 0;
	}
}

// NAME : clearDice
// INPUT PARAMETERS: dice[]
// OUTPUT: none
// PURPOSE: clear the old dice values
void clearDice(die *dice[]) {
	for(int i = 0; i < MAX; i++) {
		dice[i]->locked = false;
		dice[i]->val = 0;
	}
}

/*
//FOR Part 2 resolve dice
// NAME : resolveDice
// INPUT PARAMETERS: currPlayer, reroll_count, total_playerCount
// OUTPUT: none
// PURPOSE: resolve the dice for current player
void resolveDice(player *currPlayer, int reroll_count, int total_playerCount) {
    int gats = 0;       //Holds Amount of Gatling Die
    int dyna = 0;       //Holds Amount of Dynamite Die
    int bullet_1 = 0;
    int bullet_2 = 0;
    int beer = 0;
    int reroll_option;
    bool reroll_record = false;
    rollDice(currPlayer);
    for(int i= 0; i < MAX; i++) {
        if(dice[i]->val == ARROW) {
            currPlayer->arrowsHeld++;
            arrowsRemaining--;
            if(arrowsRemaining == 0) {
			    resolveArrows(currPlayer);
                winCondition = checkVictoryConditions(currPlayer);
                if(winCondition != GAME_CONTINUE) {
                    return;
                }
		    }
        }
        else if(dice[i]->val == DYNAMITE) {
            dice[i]->locked = true;
            dyna++;
            if(dyna == 3) {
                kaboom(currPlayer);
                winCondition = checkVictoryConditions(currPlayer);
                if(winCondition != GAME_CONTINUE) {
                    return;
                }
            }
        }
    }
    while(reroll_count < 2) {
        for(int i = 0; i < MAX; i++) {
            reroll_option = (rand() % 2) + 1;
            //rerolling
            if(reroll_option == 1 && dice[i]->locked == false && dyna < 3) {
                dice[i]->val = (rand() % (5 - 0 + 1)) + 0;
                if(dice[i]->val == ARROW) {
                    currPlayer->arrowsHeld++;
                    arrowsRemaining--;
                    if(arrowsRemaining == 0) {
                        resolveArrows(currPlayer);
                        winCondition = checkVictoryConditions(currPlayer);
                        if(winCondition != GAME_CONTINUE) {
                            return;
                        }
                    }
                }
                else if(dice[i]->val == DYNAMITE) {
                    dice[i]->locked = true;
                    dyna++;
                    if(dyna == 3) {
                        kaboom(currPlayer);
                        winCondition = checkVictoryConditions(currPlayer);
                        if(winCondition != GAME_CONTINUE) {
                            return;
                        }
                    }
                }
                reroll_record = true;
            }
            //dont want to reroll
            else {
                dice[i]->locked = true;
            }
            if(reroll_count == 1) {
                dice[i]->locked = true;
            }
        }
        reroll_count++;
    }
    for(int i = 0; i < MAX; i++) {
        switch (dice[i]->val) {
            case GATLING:
            {
                gats++;
                if(gats == 3) {
                    gatling(currPlayer);
                    winCondition = checkVictoryConditions(currPlayer);
                    if(winCondition != GAME_CONTINUE) {
                        return;
                    }
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
    if(reroll_record == true) {
        cout << "Updated dice result:\t";
        for(int i = 0; i < MAX; i++) {
                cout << dice[i]->val << "\t";
        }
        cout << endl;
    }
    for(int i = 0; i < bullet_1; i++) {
        shoot(currPlayer, 1, total_playerCount);
        winCondition = checkVictoryConditions(currPlayer);
        if(winCondition != GAME_CONTINUE) {
            return;
        }
    }
    for(int i = 0; i < bullet_2; i++) {
        shoot(currPlayer, 2, total_playerCount);
        winCondition = checkVictoryConditions(currPlayer);
        if(winCondition != GAME_CONTINUE) {
            return;
        }
    }
    for(int i = 0; i < beer; i++) {
        giveBeer(currPlayer, total_playerCount);
    }
    clearDice(dice);
}
*/