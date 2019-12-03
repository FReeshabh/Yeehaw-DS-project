//BIG PROJECT Part 2
//Tyler Nee, Vincent Hew, Rishabh Tewari
//game working correctly except naming issue
//using vincent_test part 1 version 1.7.8
//part 2 version 1.8.1

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
#define MAX_head 8
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
    string player_name;
    player *left;
    player *right;
}player;

typedef struct favor{
	int values[8][8];
} favor;

//Headers
//create player functions
player *createPlayer(int position);
player *generatePlayers(int playerCount);
void assign_role(player *current_player, int total_playerCount);
void name_assign(player *currPlayer);
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
//part 2 header
//When you call random, the func will instead return a value that is checked against random,
//depending on the current player's role and the value of the dice
int getBehaviorModifier(player *currPlayer, int diceValue); 
//graph
void generate_graph();
void print_graph();
int role_reveal(int position);
//decision
player *targeting(player *currPlayer, int dieValue);
void updateFavor(player *actor, player *target, int option);



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
int initial_playerCount;                //initial player amount
favor *favorGraph = NULL;


int main() {
    srand(time(0));
	initializeDice(dice);
    int round_count = 1;
    initial_playerCount = (rand() % (8 - 4 + 1)) + 4;
    player *current;
    cout << "Player initial amount: " << initial_playerCount << endl;
    first_player = generatePlayers(initial_playerCount);
	generate_graph();
    display(first_player);
    //finished generating players in the game
    //game start
    cout << endl << ">>>>Game Start<<<<" << endl;
    current = first_player;
	
   while(winCondition == GAME_CONTINUE) {
        cout << ">>>>Round: " << round_count << endl;
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
        cout << ">>>>End of round: " << round_count << endl;
        cout << endl;
        round_count++;
    }

    //end of game
    //graph testing
    //cout << endl << ">>ERROR: graph testing"<<endl;
	/*print_graph();
    cout << endl;
    cout << endl;
    updateFavor(first_player, sheriff, 1);
    print_graph();*/
	
	//Targeting Testing:
	/*updateFavor(first_player, first_player->left, 1);
	cout << "Favor updated...\n";
	cout << targeting(first_player->left, 1)->tag << endl;
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
		cout << ">>Player " << current->tag << " sustained " << current->arrowsHeld << " damage " <<endl;
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
		cout << ">>Recipient " << recipient->tag << " was healed" << endl;
        recipient->hp++;
	}
	else {
		cout << ">>Recipient " << recipient->tag << " already maxed out" << endl;
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
	cout << ">>Player " << deceased->tag << " has died." << endl;
	cout << ">>They were a";
	switch(deceased->role) {
		case 0: cout << " sheriff.";
		break;
		case 1: cout << " deputy.";
        deputy_count--;
		break;
		case 2: cout << "n outlaw.";
        outlaws_count--;
		break;
		case 3: cout << " renegade.";
        renegades_count--;
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
        reroll_option = (rand() % 2) + 1;
        if(reroll_count < 2 && dice[i]->val != DYNAMITE && reroll_option < 1) {
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
        else
		{
			cout << "Die " << i+1 << " was locked in at " << dice[i]->val<<endl;
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
	player *target = targeting(currPlayer, diceVal);
	cout << "Player " << target->tag << " was shot by Player " << currPlayer->tag <<endl;
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
    int initial_playerCout = (rand() % (8 - 4 + 1)) + 4;
    newPlayer->hp = 9;
    newPlayer->maxhp = 9;
    newPlayer->arrowsHeld = 0;
    newPlayer->tag = position;
    //newPlayer->player_name;
    newPlayer->left = NULL;
    newPlayer->right = NULL;
    //name_assign(newPlayer);
    return newPlayer;
}

// NAME : name_assign
// INPUT PARAMETERS: currPlayer
// OUTPUT: none
// PURPOSE: give random player name to each player
void name_assign(player *currPlayer) {
    cout << "get in" << endl;
    string name_list[8] = {"Vincent","Tyler","Rishabh","John","Rosa","Monica","Lisa","Albert"};
    int index_namelist = (rand() % (7 - 0 + 1)) + 0;
    cout << "nameList available check: " << endl;
    for(int i = 0; i < 8; i++) {
        cout << "list pos (" << i << ") status: ";
        if (name_check[index_namelist] != false) {
            cout << "Used" << endl;
        }
        else if(name_check[index_namelist] == false) {
            cout << "Available" << endl;
        }
    }
    while(name_check[index_namelist] != false) {
        index_namelist = (rand() % (7 - 0 + 1)) + 0;
        cout << "finding another" << endl;
    }
    name_check[index_namelist] = true;
    cout << "**New status: " << endl;
    cout << "list pos (" << index_namelist << ") status: ";
    if (name_check[index_namelist] != false) {
            cout << "Used" << endl;
    }
    else if(name_check[index_namelist] == false) {
        cout << "Available" << endl;
    }
    cout << "assigning the name: " << name_list[index_namelist] << endl;
    cout << "target spot: " << currPlayer->player_name << endl;
    currPlayer->player_name = name_list[index_namelist];
    cout << "Assigned for player tag: ";
    cout << currPlayer->tag << endl;
    cout << " as ";
    cout << currPlayer->player_name << endl;
    cout << "leaving" << endl;
}

// NAME : generatePlayers
// INPUT PARAMETERS: playerCount
// OUTPUT: first_player
// PURPOSE: generate players in the game
player *generatePlayers(int playerCount) {
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
    cout << ">>>Current Player List Status<<<" << endl;
    do {
        cout << "Player " << current->tag << " Role: " << current->role;
        cout <<"\thp:" << current->hp << "/" << current->maxhp << endl;
        current = current->left;
    }while(current->tag != 0);
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
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

// NAME : role_reveal
// INPUT PARAMETERS: position
// OUTPUT: integer
// PURPOSE: Reveal the role of the player
int role_reveal(int position) {
    player *current = first_player;
    while(current->tag != position) {
        current = current->left;
    }
    return current->role;
}

player *validRightTarget(player *currPlayer)
{
	player *assign = currPlayer->right;
	while(player_dead_status[assign->tag] == true) 
	{
		assign = assign->right;
	}
	return assign;
}
player *validLeftTarget(player *currPlayer)
{
	player *assign = currPlayer->left;
	while(player_dead_status[assign->tag] == true) 
	{
		assign = assign->left;
	}
	return assign;
}
//Use for Shoot 1
player *findNemesis1(player *currPlayer)
{
	int favorLeft = favorGraph->values[currPlayer->tag][validLeftTarget(currPlayer)->tag];
	int favorRight = favorGraph->values[currPlayer->tag][validRightTarget(currPlayer)->tag];
	if(favorLeft < favorRight)
		return validLeftTarget(currPlayer);
	else
		return validRightTarget(currPlayer);
}
//Use for Shoot 2
player *findNemesis2(player *currPlayer)
{
	int favorLeft = favorGraph->values[currPlayer->tag][validLeftTarget(validLeftTarget(currPlayer))->tag];
	int favorRight = favorGraph->values[currPlayer->tag][validRightTarget(validRightTarget(currPlayer))->tag];
	if(favorLeft < favorRight)
		return validLeftTarget(validLeftTarget(currPlayer));
	else
		return validRightTarget(validRightTarget(currPlayer));
}
player *findLowestAlly(player *currPlayer)
{
	//Check players with favor greater than 65.
	player *allies[4];
	player *check = first_player;
	int index = 0;
	for(int i = 0; i < initial_playerCount; i++)
	{
		if(favorGraph->values[currPlayer->tag][i] > 65 && currPlayer->tag != i)
		{
			allies[index] = check;
		}
		check = check->left;
	}
	//Find the lowest player in that group. //Sheriff is prioritized-
	//-by calculating their health as half its current value.
	//When Outlaws or Renegades call this function,
	//their favor for the sheriff will always be zero,
	//and sheriff will not be passed to the allies array.
	int minHP = 11;
	int curMin;
	for(int i = 0; i < 4; i++)
	{
		curMin = allies[i]->hp;
		if(allies[i]->role == SHERIFF)
			curMin = curMin/2;
		if(curMin < minHP)
		{
			minHP = curMin;
			//Check is repurposed to hold the lowest.
			check = allies[i];
		}
	}
	//Return them as a target.
	return check;
}

// NAME : generate_graph
// INPUT PARAMETERS: none
// OUTPUT: none
// PURPOSE: generate the graph
void generate_graph() {
	favorGraph = (favor *)malloc(sizeof(favor));
    player *current = first_player;
	player *target = first_player;
	for(int i = 0; i < initial_playerCount; i++)
	{
		for(int j = 0; j < initial_playerCount; j++)
		{
			switch(current->role)
			{
				case SHERIFF:
				{
					if(deputy_count == 0)
						favorGraph->values[i][j] = 0;
					else
						favorGraph->values[i][j] = 50;
				}
				break;
				case DEPUTY:
				{
					if(deputy_count < 2)
						favorGraph->values[i][j] = 0;
					if(deputy_count == 2)
						favorGraph->values[i][j] = 50;
					if(target->role == SHERIFF)
						favorGraph->values[i][j] = 100;
				}
				break;
				case OUTLAW:
				{
					if(target->role == SHERIFF)
						favorGraph->values[i][j] = 0;
					else
						favorGraph->values[i][j] = 50;
				}
				break;
				case RENEGADE:
				{
					favorGraph->values[i][j] = 0;
				}
				break;
			}
			if(current->tag == target->tag)
				favorGraph->values[i][j] = 100;
			target = target->left;
		}
		current = current->left;
	}
}

//graph testing function
void print_graph() 
{
	for(int i = 0; i < initial_playerCount; i++)
	{
		cout << "Player " << i <<": ";
		for(int j = 0; j < initial_playerCount; j++)
		{
			cout << favorGraph->values[i][j] << "\t";
		}
		cout << endl;
	}
}

int getBehaviorModifier(player *currPlayer, int dieValue)
{
	int be = 0; 
	//Creates a number based off of role and die type that a random 1d6 checks against. 
	//Higher values make it less likely that a die will be locked.
	//0 guarantees a lock in, 7 guarantees a reroll attempt.
	player *validTargets[4];
	validTargets[0] = validRightTarget(currPlayer);
	validTargets[2] = validRightTarget(validRightTarget(currPlayer));
	validTargets[1] = validLeftTarget(currPlayer);
	validTargets[3] = validLeftTarget(validLeftTarget(currPlayer));
	//Creates a list of valid targets.
	int sheriffTargetable;
	//Sees if Sheriff is in targeting range. 0 if not, 1 or 2 if in range, respective to distance.
	int lowestHP = 11;
	int lowestPlayerTargetable;
	//Returns the index of the lowest HP target that isn't the sheriff. Used for Renegade Targeting.
	for(int i = 0; i < 4; i++)
	{
		if(validTargets[i]->role == SHERIFF)
		{
			sheriffTargetable = (i/2) + 1;
		}
		if(validTargets[i]->hp < lowestHP && validTargets[i]->role != SHERIFF)
		{
			lowestHP = validTargets[i]->hp;
			lowestPlayerTargetable = i;
		}
	}
	switch(currPlayer->role)
	{
		case SHERIFF:
		{
			cout << "Sheriff's Resolve"<<endl;
			//The sheriff will prioritize healing himself. He won't attack targets that have healed him recently.
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
			}
		}
		break;
		case DEPUTY:
		{
			cout << "Deputy's Reasoning"<<endl;
			//Deputies will prioritize healing the sheriff, and themselves if they fall low.
			//If they have to shoot, the sheriff will never be targeted.
			switch(dieValue)
			{
				case BEER:
				//Prioritized
				if(sheriff->hp < sheriff->maxhp || currPlayer->hp < (currPlayer->maxhp / 3))
					be = 0;
				else
				//No Use, may need to check for favored players
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
			}
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
				if(sheriffTargetable == 1)
					be = 0;
				else
					be = 2;
			break;
			case SHOOT_2:
				if(sheriffTargetable == 2)
					be = 0;
				else
					be = 3;
			break;
			//Outlaws play on a team usually larger than the Law's. They will avoid gatling under most circumstances.
			//They are more likely to use the gatling if they have too many arrows, the sheriff is not a valid target, or they are outnumbered.
			case GATLING:
				be = 7;
				//Check team sized;
				if(outlaws_count <= deputy_count + 1)
					be -= 3;
				//Check if sheriff is not 1 or 2 spaces away.
				if(sheriffTargetable == 0)
					be -= 1;
				if(currPlayer->arrowsHeld > 3)
					be -= 3;
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
				else if(sheriff->hp < (sheriff->maxhp/2)) //AND there are less dead Outlaws than Starting.
					//Will Heal Sheriff to keep the game going. 
					be = 0;
				else
					be = 7;
				break;
				case SHOOT_1:
				//Check for lowest target, still a likely lockin. Outprioritized by gatling.
				if(lowestPlayerTargetable / 2 == 0)
					be = 0;
				else
					be = 4;
				break;
				case SHOOT_2:
				//Check for lowest target, still a likely lockin
				if(lowestPlayerTargetable / 2 == 1)
					be = 0;
				else
					be = 4;
				break;
				case GATLING:
				//Prioritized
					be = 0;
				break;
			}
		break;
	}
	if(dieValue == DYNAMITE)
		be = 0;
	if(dieValue == ARROW)
		be = 7;
	//Arrows have no utility and will always be rerolled.
	//Dynamite forces the die to lock.
	return be;
}

player *targeting(player *currPlayer, int dieValue)
{
	cout << "Targeting... \n";
	int hp = currPlayer->hp;
	int maxhp = currPlayer->maxhp;
	switch(dieValue)
	{
		case BEER:
		{
			switch(currPlayer->role)
			{
				case SHERIFF:
				{
					if(hp < maxhp)
						return currPlayer;
					return findLowestAlly(currPlayer);
				}
				break;
				case DEPUTY:
				{
					if(hp < maxhp/2)
						return currPlayer;
					return findLowestAlly(currPlayer);
				}
				break;
				case OUTLAW:
				{
					if(hp < (maxhp/2))
						return currPlayer;
					//Check for favored player.
					return findLowestAlly(currPlayer);
				}
				break;
				case RENEGADE:
				{
					if(outlaws_count == 0 || currPlayer->hp < currPlayer->maxhp/3)
						return currPlayer;
					if(deputy_count + 1 < outlaws_count && sheriff->hp < (sheriff->maxhp / 2))
						return sheriff;
					//Renegades cannot have favor, will always heal self if no alternative present.
					return currPlayer;
				}
				break;
			}
		}
		case SHOOT_1:
		{
			
			switch(currPlayer->role)
			{
				case SHERIFF:
				{
					//Shoot your least favorite player in range.
					return findNemesis1(currPlayer);
				}
				break;
				case DEPUTY:
				{
					//Shoot your least favorite player in range.
					return findNemesis1(currPlayer);
				}
				break;
				case OUTLAW:
				{
					//Shoot your least favorite player in range.
					return findNemesis1(currPlayer);
				}
				break;
				case RENEGADE:
				{
					//Avoid shooting the sheriff while outlaws are still in the game.
					if(validLeftTarget(currPlayer)->tag == sheriff->tag && outlaws_count > 0)
						return validRightTarget(currPlayer);
					if(validRightTarget(currPlayer)->tag == sheriff->tag && outlaws_count > 0)
						return validLeftTarget(currPlayer);
					else
					{
						//Find lowest HP target available.
						if(validLeftTarget(currPlayer)->hp > validRightTarget(currPlayer)->hp)
							return validRightTarget(currPlayer);
						else
							return validLeftTarget(currPlayer);
					}
				}
				break;
			}
		}
		case SHOOT_2:
		{
			switch(currPlayer->role)
			{
				case SHERIFF:
				{
					//Shoot your least favorite player in range.
					return findNemesis2(currPlayer);
				}
				break;
				case DEPUTY:
				{
					//Shoot your least favorite player in range.
					return findNemesis2(currPlayer);
				}
				break;
				case OUTLAW:
				{
					//Shoot your least favorite player in range.
					return findNemesis2(currPlayer);
				}
				break; 
				case RENEGADE:
				{
					//Avoid shooting the sheriff while outlaws are still in the game. Range 2
					if(validLeftTarget(validLeftTarget(currPlayer))->tag == sheriff->tag && outlaws_count > 0)
						return validRightTarget(validRightTarget(currPlayer));
					if(validRightTarget(validRightTarget(currPlayer))->tag == sheriff->tag && outlaws_count > 0)
						return validLeftTarget(validLeftTarget(currPlayer));
					else
					{
						//Find lowest HP target available. Range 2
						if(validLeftTarget(validLeftTarget(currPlayer))->hp > validRightTarget(validRightTarget(currPlayer))->hp)
							return validRightTarget(currPlayer);
						else
							return validLeftTarget(currPlayer);
					}
				}
				break;
			}
		}
	}
	cout << "ERROR: Fallout of targeting function " << endl;
	return NULL;
}

//update graph
//change favor point to the actor based on what action actor did to the target
void updateFavor(player *actor, player *target, int option) {
    //option 0 = heal, 1 = shoot
    player *current = actor->left;
    if(actor == target) {
        cout << "**Error: updateFavor actor == target" << endl;
        return;
    }
    if(option == 0) {
        switch (target->role)
        {
        case SHERIFF:
            while(current != actor) {
                switch (current->role)
                {
                case SHERIFF:
                    if(deputy_count >= 1) {
                        if(favorGraph->values[current->tag][actor->tag] != 100) {
                            favorGraph->values[current->tag][actor->tag] += 12;
                            if(favorGraph->values[current->tag][actor->tag] > 99)
                                favorGraph->values[current->tag][actor->tag] = 99;
                        }

                    }
                    break;
                case DEPUTY:
                    if(deputy_count > 1) {
                        if(favorGraph->values[current->tag][actor->tag] != 100) {
                            favorGraph->values[current->tag][actor->tag] += 12;
                            if(favorGraph->values[current->tag][actor->tag] > 99)
                                favorGraph->values[current->tag][actor->tag] = 99;
                        }
                    }
                    break;
                case OUTLAW:
                    if(favorGraph->values[current->tag][actor->tag] != 0) {
                        favorGraph->values[current->tag][actor->tag] -= 12;
                        if(favorGraph->values[current->tag][actor->tag] < 1) {
                            favorGraph->values[current->tag][actor->tag] = 1;
                        }
                    }

                    break;
                case RENEGADE:
                    //Reegade dont change favor to the actor
                    break;
                }
                current = current->left;
            }
            break;
        case DEPUTY:
            if(favorGraph->values[current->tag][actor->tag] != 100) {
                favorGraph->values[target->tag][actor->tag] += 12;
                if(favorGraph->values[target->tag][actor->tag] > 99) {
                    favorGraph->values[target->tag][actor->tag] = 99;
                }
            }
            break;
        case OUTLAW:
            if(favorGraph->values[current->tag][actor->tag] != 100) {
                favorGraph->values[target->tag][actor->tag] += 12;
                if(favorGraph->values[target->tag][actor->tag] > 99) {
                    favorGraph->values[target->tag][actor->tag] = 99;
                }
            }
            break;
        case RENEGADE:
            //renegade dont change
            break;
        default:
            cout << "**Error: updateFavor option 0 switch default" << endl;
            break;
        }
    }
    else if(option == 1) {
        switch (target->role)
        {
        case SHERIFF:
            while(current != actor) {
                switch (current->role)
                {
                case SHERIFF:
                    if(favorGraph->values[current->tag][actor->tag] != 0) {
                        favorGraph->values[current->tag][actor->tag] -= 15;
                        if(favorGraph->values[current->tag][actor->tag] < 1) {
                            favorGraph->values[current->tag][actor->tag] = 1;
                        }
                    }
                    break;
                case DEPUTY:
                    if(favorGraph->values[current->tag][actor->tag] != 0) {
                        favorGraph->values[current->tag][actor->tag] -= 15;
                        if(favorGraph->values[current->tag][actor->tag] < 1) {
                            favorGraph->values[current->tag][actor->tag] = 1;
                        }
                    }
                    break;
                case OUTLAW:
                    if (favorGraph->values[current->tag][actor->tag] != 100) {
                        favorGraph->values[current->tag][actor->tag] += 15;
                        if(favorGraph->values[current->tag][actor->tag] > 99) {
                            favorGraph->values[current->tag][actor->tag] = 99;
                        }
                    }
                    break;
                case RENEGADE:
                    //renegage dont change favor
                    break;
                }
                current = current->left;
            }
            break;
        case DEPUTY:
            if(favorGraph->values[current->tag][actor->tag] != 0) {
                favorGraph->values[target->tag][actor->tag] -= 15;
                if(favorGraph->values[target->tag][actor->tag] < 1) {
                    favorGraph->values[target->tag][actor->tag] = 1;
                }
            }
            break;
        case OUTLAW:
            if(favorGraph->values[current->tag][actor->tag] != 0) {
                favorGraph->values[target->tag][actor->tag] -= 15;
                if(favorGraph->values[target->tag][actor->tag] < 1) {
                    favorGraph->values[target->tag][actor->tag] = 1;
                }
            }
            break;
        case RENEGADE:
            //renegade dont change favor point
            break;
        default:
            cout << "**Error: updateFavor option 1 switch default" << endl;
            break;
        }
    }
}

