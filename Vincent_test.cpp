#include <iostream>
using namespace std;

typedef struct die
{
    int val;
    bool locked;
}die;

typedef struct player
{
    struct player *left;
    struct player *right;
    struct die *dice[5];
    int hp;
    int maxhp;
    int arrowsHeld;
    int role;
    int ability;
}player;

void gatling(struct player *player)
{
    struct player *enemy;
    enemy = player;
    while(player != enemy->right)
    {
        enemy = enemy->right;
        (enemy->hp)--;
    }
}







int main()
{

}