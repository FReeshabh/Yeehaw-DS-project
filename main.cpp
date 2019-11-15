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
    die *dice[MAX];
}player;

void resolveDice(player *currPlayer); //master function
//Supporting Functions
void giveBeer(player *currPlayer); //done dice val 0
void shoot(player *currPlayer, int diceVal, int total_player_count); //vince dice val 1/2 
void gatling(player *currPlayer); //vince dice val 3
void kaboom(player *currPlayer); //done dice val 4
void resolveArrows(player *sheriff); //done dice val 5
int listPlayers(player *sheriff); //done
void sixFeetUnder(player *deceased); //done
//Supporting Function
int checkVictoryConditions(player *sheriff); //done

player *sheriff;
int arrowsRemaining;

int main()
{
    srand(time(NULL));
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
            cout << " DEAD" <<endl;
        }
        else
        {
            playerCount++;
        }
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
    for(int i = 0; i < MAX; i++)
    {
        currPlayer->dice[i]->locked = true;
    }
    currPlayer->hp--;
}

int checkVictoryConditions(player *sheriff)
{
    int outlaws = 0;
    player *current = sheriff;
    do{
        if(current->role == 2 || current->role == 3)
        {
            cout << "Outlaw Detected\n";
            outlaws++;
        }
    }while(current != sheriff);

    if(outlaws == 0)
    {
        cout << "The Sheriff has driven crime from the town.\n";
        return 0;
    }

    if(sheriff->hp == 0 && listPlayers(sheriff) == 1)
    {
        cout << "The Renegade is the last man standing. \n";
        return 1;
    }
    else
    {
        cout << "The Outlaws have killed the Sheriff. \n";
        return 2;
    }   
}
void gatling(player *currPlayer)
{
    player *enemy;
    enemy = currPlayer;
    while(currPlayer != enemy->right)
    {
        enemy = enemy->right;
        (enemy->hp)--;
    }
}


void sixFeetUnder(player *deceased)
{
    deceased->left->right = deceased->right;
    deceased->right->left = deceased->left;
    cout << "Player of Role " << deceased->role << "has died."<<endl;
    checkVictoryConditions(sheriff);
}

void shoot(player *currPlayer, int diceVal, int total_player_count)
{
    int random_target = (rand() % (2 - 1 + 1)) + 1;
    int currPlayer_count = 1;
    if(diceVal == 1)
    {
        while(currPlayer->right->hp == 0)
        {
            currPlayer = currPlayer->right;
        }
        if(random_target == 1)
        {
            currPlayer->right->hp--;
            return;
        }
        else
        {
            currPlayer->left->hp--;
            return;
        }
    }
    else if(diceVal == 2)
    {
        while(currPlayer->right->right->hp == 0)
        {
            currPlayer = currPlayer->right;
            currPlayer_count++;
            if(currPlayer_count == total_player_count)
            {
                currPlayer = currPlayer->right;
                currPlayer_count++;
            }
        }
        if(random_target == 1)
        {
            currPlayer->right->right->hp--;
            return;
        }
        else
        {
            currPlayer->left->left->hp--;
            return;
        }
    }
}