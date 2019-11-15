#include <iostream>
#include <conio.h>
#include <malloc.h>
#include <time.h>
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
    player *left;
    player *right;
}player;

void resolveDice(player *currPlayer, int reroll); //master function
//Supporting Functions
void giveBeer(player *currPlayer); //done dice val 0
void shoot(player *currPlayer, int diceVal); //vince dice val 1/2 
void gatling(player *currPlayer); //vince dice val 3
void kaboom(player *currPlayer); //done dice val 4
void resolveArrows(player *sheriff); //done dice val 5
int listPlayers(player *sheriff); //done
player *generatePlayers(int playerCount);
void initializeDice(die *dice[]);
void clearDice(die *dice[]);

void sixFeetUnder(player *deceased); //done
//Supporting Function
int checkVictoryConditions(player *currPlayer); //done

player *sheriff;
int arrowsRemaining = 10;
die *dice[MAX];
int winCondition = 3;

int main()
{
    srand(time(0));
	initializeDice(dice);
    cout << "Player Count Locked at 4 + Sheriff \n";
    int playercount = 4;
    generatePlayers(playercount);
    player *currentPlayer = sheriff;
    while(winCondition == 3)
    {
        resolveDice(currentPlayer, 0);
		clearDice(dice);
		checkVictoryConditions(currentPlayer);
        currentPlayer = currentPlayer->right;
    }
}

int listPlayers(player *sheriff)
{
    int playerCount = 0;
    player *temp = sheriff;
    cout << "Sheriff ";
    do{
        cout << "["<<playerCount<<"] HP: "<<temp->hp<<" Arrows Held: "<<temp->arrowsHeld;
        if(temp->hp == 0)
        {
            cout << " DEAD";
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

void resolveArrows(player *sheriff)
{
    player *current = sheriff;
     do{
         current->hp -= current->arrowsHeld;
         arrowsRemaining += current->arrowsHeld;
         current->arrowsHeld = 0;
         current = current->right;
    } while(current != sheriff);
    cout << "Arrows Resolved...\n";
}

void giveBeer(player *currPlayer)
{
    int players = listPlayers(sheriff);
    int howFarLeft = rand() % players;
    player *recipient = currPlayer;
    while(howFarLeft > 0)
    {
        recipient = recipient->left;
    }
    if(recipient->hp != recipient->maxhp)
        recipient->hp++;
}

void kaboom(player *currPlayer)
{
    cout << "Dynamite Triggered: Rerolls Restricted\n";
    for(int i = 0; i < MAX; i++)
    {
        dice[i]->locked = true;
    }
    currPlayer->hp--;
}

int checkVictoryConditions(player *currPlayer)
{
    int outlaws = 0;
    player *current = currPlayer;
	do{
		cout << "Current Role is " << current->role <<endl;
        if(current->role == 2 || current->role == 3)
        {
            cout << "Outlaw Detected\n";
            outlaws++;
        }
        current = current->right;
    }while(current != currPlayer);

    if(outlaws == 0)
    {
        cout << "The Sheriff has driven crime from the town.\n THE LAW WINS \n";
        return 0;
    }

    if(sheriff->hp == 0 && listPlayers(currPlayer) == 1)
    {
        cout << "The Renegade is the last man standing. \n THE RENEGADE WINS \n";
        return 1;
    }
    else if(sheriff->hp == 0)
    {
        cout << "The Outlaws have killed the Sheriff. \n THE OUTLAWS WIN \n";
        return 2;
    }

    else
    {
        cout << "No Win Condition has been met."<<endl;
        return 3;
    }
    
}

void gatling(player *currPlayer)
{
	cout << "Gatling Effect Triggered \n";
    player *enemy;
    enemy = currPlayer->right;
    while(enemy != currPlayer)
    {
        (enemy->hp)--;
        if(enemy->hp == 0)
        {
            sixFeetUnder(enemy);
            winCondition = checkVictoryConditions(currPlayer);
        }
        enemy = enemy->right;
    }
}

void sixFeetUnder(player *deceased)
{
    deceased->left->right = deceased->right;
    deceased->right->left = deceased->left;
    cout << "Player of Role " << deceased->role << "has died."<<endl;
}

void rollDice(player *currPlayer)
{
	cout << "Player of role " << currPlayer->role << " is rolling.\n";
    for(int i = 0; i < MAX; i++)
    {
        if(dice[i]->locked == false)
        {
            dice[i]->val = (rand()%6);
			cout << "Dice["<<i<<"] rolled as " << dice[i]->val<<endl;
        }
		else{
			cout << "Dice["<<i<<"] is locked as " << dice[i]->val<<endl;
		}
    }
}

void resolveDice(player *currPlayer, int reroll)
{
	cout << "Player of role " << currPlayer->role << " is resolving their turn.\n";
	rollDice(currPlayer);
    int gats = 0;
    //Holds Amount of Gatling Die
    int dyna = 0;
    //Holds Amount of Dynamite Die
    for(int i = 0; i < MAX; i++)
    {
        if(dice[i]->val == 4)
        {
            dice[i]->locked = true;
            dyna++;
        }
        if(dice[i]->val == 5)
        {
            currPlayer->arrowsHeld++;
            arrowsRemaining--;
        }
    }
    if(reroll == 2 || dyna == 3)
    {
		if(dyna == 3)
		{
			kaboom(currPlayer);
		}
        cout << "Max Rerolls Reached \n";
        for(int i = 0; i < MAX; i++)
        {
            switch (dice[i]->val)
            {
                case 0: giveBeer(currPlayer);
                break;
                case 1: shoot(currPlayer, dice[i]->val);
                break;
                case 2: shoot(currPlayer, dice[i]->val);
                break;
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

void shoot(player *currPlayer, int diceVal)
{
    int random_target = (rand() % (2 - 1 + 1)) + 1;
    int currPlayer_count = listPlayers(sheriff);
    if(diceVal == 1)
    {
        while(currPlayer->right->hp == 0)
        {
            currPlayer = currPlayer->right;
        }
        if(random_target == 1)
        {
            currPlayer->right->hp--;
            if(currPlayer->right->hp == 0)
            {
                sixFeetUnder(currPlayer->right);
                checkVictoryConditions(currPlayer);
            }
            return;
        }
        else
        {
            currPlayer->left->hp--;
            if(currPlayer->left->hp == 0)
            {
                sixFeetUnder(currPlayer->left);
                checkVictoryConditions(currPlayer);
            }
            return;
        }
    }
    else if(diceVal == 2)
    {
        if(currPlayer_count == 2)
        {
            currPlayer->right->hp--;
            if(currPlayer->right->hp == 0)
            {
                sixFeetUnder(currPlayer->right);
                checkVictoryConditions(currPlayer);
            }
        }
        if(random_target == 1)
        {
            currPlayer->right->right->hp--;
            if(currPlayer->right->right->hp == 0)
            {
                sixFeetUnder(currPlayer->right->right);
                checkVictoryConditions(currPlayer);
            }
            return;
        }
        else
        {
            currPlayer->left->left->hp--;
            if(currPlayer->left->left->hp == 0)
            {
                sixFeetUnder(currPlayer->left->left);
                checkVictoryConditions(currPlayer);
            }
            return;
        }
    }
} 

player *generatePlayers(int playerCount)
{
	player *newSheriff = (player*)malloc(sizeof(player));
    sheriff = newSheriff;
    sheriff->maxhp = 11;
    sheriff->hp = 11;
    sheriff->role = 0;
    sheriff->arrowsHeld = 0;
    player *prevPlayer = sheriff;
    for(int i = 0; i < playerCount; i++)
    {
        player *newPlayer = (player*)malloc(sizeof(player));
        newPlayer->maxhp = 9;
        newPlayer->hp = 9;
        newPlayer->arrowsHeld = 0;
        newPlayer->role = 2;
        newPlayer->left = prevPlayer;
        prevPlayer->right = newPlayer;
		prevPlayer = newPlayer;
    }
    sheriff->left = prevPlayer;
    prevPlayer->right = sheriff;
    prevPlayer->role = 3;
    return prevPlayer->right;
}

void initializeDice(die *dice[])
{
	for(int i = 0; i < MAX; i++)
	{
		die *newDie = (die*)malloc(sizeof(die));
		dice[i] = newDie;
		dice[i]->locked = false;
		dice[i]->val = 0;
	}
}

void clearDice(die *dice[])
{
	for(int i = 0; i < MAX; i++)
	{
		dice[i]->locked = false;
		dice[i]->val = 0;
	}
}
