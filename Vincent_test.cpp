#include <iostream>
using namespace std;
/*
    0 = beer
    1 = 1 bullet
    2 = 2 bullet
    3 = gatling
    4 = bomb
    5 = arrow
*/
typedef struct die
{
    int val;
    bool locked;
}die;

typedef struct player
{
    int hp;
    int maxhp;
    int arrowsHeld;
    int role;
    int ability;
    struct player *left;
    struct player *right;
    struct die *dice[5];
}player;

void gatling(player *player_x)
{
    player *enemy;
    enemy = player_x;
    while(player_x != enemy->right)
    {
        enemy = enemy->right;
        (enemy->hp)--;
    }
}







int main() {

}
