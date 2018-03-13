#define HEADER
#ifdef HEADER
    #define FOR(i,j,n) for (int i=(j);i<(n);i++)
    #define REP(i,n) for (int i=0;i<(n);i++)
    #define REPN(i,n) for (int i=(n);i>=0;i--)
    #define I(n) scanf("%d", &(n))
    #define pb(n) push_back((n))
    #define mp(i,j) make_pair((i),(j))
    #define pow2(x) (x)*(x)
    #include <bits/stdc++.h>
    using namespace std;

    //------------------------------typedef集
    typedef vector<int> vi;
    typedef pair<int,int> pi;
    typedef vector<pi> vpi;
    typedef vector<vi> vvi;
    typedef vector<vpi> vvpi;
    typedef vector<vvi> vvvi;
    typedef long long ll;
#endif

struct Entity{
    int unitId;
    int team;
    string unitType; // UNIT, HERO, TOWER, can also be GROOT from wood1
    int x;
    int y;
    int attackRange;
    int health;
    int maxHealth;
    int shield; // useful in bronze
    int attackDamage;
    int movementSpeed;
    int stunDuration; // useful in bronze
    int goldValue;
    int countDown1; // all countDown and mana variables are useful starting in bronze
    int countDown2;
    int countDown3;
    int mana;
    int maxMana;
    int manaRegeneration;
    string heroType; // DEADPOOL, VALKYRIE, DOCTOR_STRANGE, HULK, IRONMAN
    int isVisible; // 0 if it isn't
    int itemsOwned; // useful from wood1
};
struct Item{
    string itemName; // contains keywords such as BRONZE, SILVER and BLADE, BOOTS connected by "_" to help you sort easier
    int itemCost; // BRONZE items have lowest cost, the most expensive items are LEGENDARY
    int damage; // keyword BLADE is present if the most important item stat is damage
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int moveSpeed; // keyword BOOTS is present if the most important item stat is moveSpeed
    int manaRegeneration;
    int isPotion; // 0 if it's not instantly consumed

    bool operator<( const Item& right ) const {
        return itemCost == right.itemCost ? itemName < right.itemName : itemCost < right.itemCost;
    }


};
int dist2 (Entity const& s, Entity const& t){
    return (s.x - t.x) * (s.x - t.x) + (s.y - t.y) * (s.y - t.y);
}
const int spawn[2][2] = {{ 0, 540 } , { 1920, 540 } };
const int tower[2][2] = {{ 100, 540 } , { 1820, 540} };
int turn_count_incl = 0;
int totower( Entity const& my, int myTeam){
    return pow2(my.x - tower[myTeam^1][0]) + pow2(my.y - tower[myTeam^1][1]);
}
pi movetarget (Entity const& my, Entity const& tgt, int myTeam){
    int vecx = tgt.x - spawn[myTeam][0];
    int vecy = tgt.y - spawn[myTeam][1];
    // double dist = sqrt( dist2(my,tgt));
    double dist = sqrt( pow2(vecx) + pow2(vecy) );
    double ex = (double) vecx / dist;
    double ey = (double) vecy / dist;

    double aggr = 1.0 + max(0.0,((double)500 - my.health) / 500) - max(0.0, (double)(turn_count_incl - 100) / 150);
    if (aggr > 1.0) aggr = 1.0;
    if (aggr < 0.0) aggr = 0.0;

    double resx = tgt.x - ex * ( my.attackRange + tgt.attackRange * aggr ) ;
    double resy = tgt.y - ey * ( my.attackRange + tgt.attackRange * aggr ) ;


    cerr << tgt.x << " " << tgt.y << "  " << resx << " " << resy << endl;

    if (resx < 0 || 1920 < resx){
        resx = spawn[myTeam][0];
        resy = spawn[myTeam][1];
    }

    return make_pair( (int) resx, (int) resy );
}

const string heros[5] = { "IRONMAN", "DOCTOR_STRANGE", "DEADPOOL", "HULK", "VALKYRIE" };

string buyitem( Entity const& my, vector<Item> const& itemlist, int money, double damage,
                double health, double mana, double moveSpeed, double manaRegeneration, int last_thre = 1000){

    double reteval = 0;
    string ret = "none";
    int healthbuff = my.maxHealth - my.health;
    int manabuff = my.maxMana - my.mana - 3*my.manaRegeneration;

    for (auto it: itemlist){
        if (it.itemCost > money) break;
        double eval = (damage*7.2*it.damage + health*0.7*(min(it.health, healthbuff + it.maxHealth)) + mana*0.5*(min(it.mana, manabuff + it.maxMana)) +
                        moveSpeed*3.6*it.moveSpeed + manaRegeneration*50.0*it.manaRegeneration) ;

        if (my.itemsOwned == 2 && it.itemCost < last_thre && !it.isPotion) {
            eval /= 2;
        } else if ((my.itemsOwned == 3 && it.itemCost < last_thre && !it.isPotion) || my.itemsOwned == 4){
            eval = 0.0;
        }
        if (eval > reteval){
            reteval = eval;
            ret = it.itemName;
        }
    }
    return ret;
}

vi burning_radius(201), burning_range(501);

void init_burning(){
    REP(i,201) {
        REP(j,201){
            if ( pow2(i-100) + pow2(j-100) <= pow2(100) ){
                burning_radius[i] = j;
                break;
            }
        }
    }
    REP(i,501) {
        REP(j,501){
            if ( pow2(i-250) + pow2(j-250) <= pow2(250) ){
                burning_range[i] = j;
                break;
            }
        }
    }
}

int burning_point[2130][960];

int main()
{
    init_burning();
    int myTeam;
    cin >> myTeam; cin.ignore();
    int bushAndSpawnPointCount; // usefrul from wood1, represents the number of bushes and the number of places where neutral units can spawn
    cin >> bushAndSpawnPointCount; cin.ignore();
    for (int i = 0; i < bushAndSpawnPointCount; i++) {
        string entityType; // BUSH, from wood1 it can also be SPAWN
        int x;
        int y;
        int radius;
        cin >> entityType >> x >> y >> radius; cin.ignore();
    }
    int itemCount; // useful from wood2
    cin >> itemCount; cin.ignore();
    vector<Item> itemlist(itemCount);
    for (int i = 0; i < itemCount; i++) {
        string itemName; // contains keywords such as BRONZE, SILVER and BLADE, BOOTS connected by "_" to help you sort easier
        int itemCost; // BRONZE items have lowest cost, the most expensive items are LEGENDARY
        int damage; // keyword BLADE is present if the most important item stat is damage
        int health;
        int maxHealth;
        int mana;
        int maxMana;
        int moveSpeed; // keyword BOOTS is present if the most important item stat is moveSpeed
        int manaRegeneration;
        int isPotion; // 0 if it's not instantly consumed
        cin >> itemName >> itemCost >> damage >> health >> maxHealth >> mana >> maxMana >> moveSpeed >> manaRegeneration >> isPotion; cin.ignore();
        itemlist[i] = Item{ itemName , itemCost , damage , health , maxHealth , mana , maxMana , moveSpeed , manaRegeneration , isPotion };
    }
    sort(itemlist.begin(),itemlist.end());

    int _hero = 0;
    // game loop
    while (1) {
        memset(burning_point,0,sizeof(burning_point));
        turn_count_incl++;
        cerr << "TURN : " << turn_count_incl << endl;

        int gold;
        cin >> gold; cin.ignore();
        int enemyGold;
        cin >> enemyGold; cin.ignore();
        int roundType; // a positive value will show the number of heroes that await a command
        cin >> roundType; cin.ignore();
        int entityCount;
        cin >> entityCount; cin.ignore();

        vector<Entity> entitylist(entityCount);
        vector <Entity> myherolist;
        int myherocount = 0;
        for (int i = 0; i < entityCount; i++) {
            int unitId;
            int team;
            string unitType; // UNIT, HERO, TOWER, can also be GROOT from wood1
            int x;
            int y;
            int attackRange;
            int health;
            int maxHealth;
            int shield; // useful in bronze
            int attackDamage;
            int movementSpeed;
            int stunDuration; // useful in bronze
            int goldValue;
            int countDown1; // all countDown and mana variables are useful starting in bronze
            int countDown2;
            int countDown3;
            int mana;
            int maxMana;
            int manaRegeneration;
            string heroType; // DEADPOOL, VALKYRIE, DOCTOR_STRANGE, HULK, IRONMAN
            int isVisible; // 0 if it isn't
            int itemsOwned; // useful from wood1
            cin >> unitId >> team >> unitType >> x >> y >> attackRange >> health >> maxHealth >> shield >> attackDamage >> movementSpeed >> stunDuration >> goldValue >> countDown1 >> countDown2 >> countDown3 >> mana >> maxMana >> manaRegeneration >> heroType >> isVisible >> itemsOwned; cin.ignore();

            //if (myTeam) team ^= 1;

            entitylist[i] = Entity{ unitId , team , unitType , x , y , attackRange , health , maxHealth , shield , attackDamage , movementSpeed , stunDuration , goldValue , countDown1 , countDown2 , countDown3 , mana , maxMana , manaRegeneration , heroType , isVisible , itemsOwned };
            //cerr << team << " " << unitType << endl;
            if (team == myTeam && unitType == "HERO") {
                myherolist.pb(entitylist[i]);
                myherocount++;
                //cerr << entitylist[i].heroType << endl;
            }

            // Write an action using cout. DON'T FORGET THE "<< endl"
            // To debug: cerr << "Debug messages..." << endl;


            // If roundType has a negative value then you need to output a Hero name, such as "DEADPOOL" or "VALKYRIE".
            // Else you need to output roundType number of any valid action, such as "WAIT" or "ATTACK unitId"
        }// entity_input

        vi nearest(_hero);
        vi nearest_length(_hero,4249000);
        vvi enemy_hero(_hero);
        int enemy_tower_idx;

        int index = -1;
        for (auto myhero : myherolist){
            index++;

            int entityidx = -1;
            for (auto someentity : entitylist){
                entityidx++;
                if (someentity.team != (1^myTeam)) continue;
                //if (someentity.unitType != "HERO") continue;
                //cerr << dist2(myhero, someentity) << endl;
                if ( dist2(myhero, someentity) < nearest_length[index] ) {
                    nearest[index] = entityidx;
                    nearest_length[index] = dist2( myhero, someentity);
                }
                if (someentity.unitType == "HERO" ){
                    enemy_hero[index].pb(entityidx);
                }
                if (someentity.unitType == "TOWER" ){
                    enemy_tower_idx = entityidx;
                }
            }
        }

        Entity& enemy_tower = entitylist[enemy_tower_idx];
        int tower_nearest_id = -1; //unitId
        int tower_nearest_length = pow2(401);
        int entityidx = -1;
        for (auto someentity : entitylist){
            entityidx++;
            if (someentity.team != myTeam) continue;
            if ( dist2(someentity, enemy_tower) < tower_nearest_length ){
                tower_nearest_id = someentity.unitId;
                tower_nearest_length = dist2(someentity, enemy_tower);
            }
        }

        //cerr << index << endl;
        if (roundType <= 0 && _hero < 2){
            cout << heros[_hero] << endl;
            _hero++;
        } else {
            for (int i = 0; i <= index; i++){

                string itemtobuy;
                int nearest_of_nearest = 4249000;
                Entity& hero_i = myherolist[i];

                for (auto someentity : entitylist){
                    if (someentity.team == entitylist[nearest[i]].team ) continue;
                    nearest_of_nearest = min(nearest_of_nearest, dist2(someentity, entitylist[nearest[i]]) );
                }

                if ( nearest_of_nearest > pow2(500) && hero_i.heroType == heros[0]){ //Ironman
                    itemtobuy = buyitem( hero_i, itemlist, gold, 0,0,1,0,3);

                    if (itemtobuy != "none"){
                        cout << "BUY " << itemtobuy << endl;
                        continue; // i = herocount;
                    }
                }

                int nearest_enemy_idx = -1;
                int nearest_enemy_length = 4249000;
                for (int entityidx: enemy_hero[i]){
                    int enedist = dist2(hero_i, entitylist[entityidx]);
                    if (enedist < nearest_enemy_length){
                        nearest_enemy_idx = entityidx;
                        nearest_enemy_length = enedist;
                    }
                }
                Entity& nearest_enemy_hero = entitylist[nearest_enemy_idx];
                cerr << hero_i.heroType << " nearest_hero dist:"<<  sqrt (nearest_enemy_length ) << " attackR: " << nearest_enemy_hero.attackRange << endl;

                int tower_attack = 0;

                if (nearest_enemy_idx < 0){
                    if (nearest_of_nearest == nearest_length[i] || (totower(hero_i, myTeam) < pow2(400) && tower_nearest_id == hero_i.unitId) ){
                        pi saki = movetarget(hero_i, entitylist[nearest[i]] , myTeam);
                        if (hero_i.heroType == heros[0] && hero_i.mana >= 16 && hero_i.countDown1 == 0 &&
                            pow2(hero_i.x - saki.first) + pow2(hero_i.y - saki.second) < pow2(200)){
                                cout << "BLINK " << saki.first << " " << saki.second << " " << endl;
                            } else {
                                cout << "MOVE " << saki.first << " " << saki.second << " " << endl;
                            }
                    } else {
                        tower_attack = 100;
                        goto NOT_EVADED;
                    }
                } else {
                    if ( ( 100 < hero_i.x && hero_i.x < 1820) &&
                         ( nearest_of_nearest == nearest_length[i] || (!index && nearest_of_nearest == nearest_length[1^i])
                         || sqrt (dist2(hero_i, nearest_enemy_hero)) < sqrt(pow2(nearest_enemy_hero.attackRange)) + 0.9 * nearest_enemy_hero.movementSpeed
                         || (totower(hero_i, myTeam) < pow2(400) && tower_nearest_id == hero_i.unitId) ) ) {
                        pi saki = movetarget(hero_i, entitylist[nearest[i]] , myTeam);
                        if ((hero_i.x - saki.first) * (2*myTeam - 1) < -50) {
                            if (hero_i.heroType == heros[0] && hero_i.mana >= 16 && hero_i.countDown1 == 0 &&
                                pow2(hero_i.x - saki.first) + pow2(hero_i.y - saki.second) < pow2(200)){
                                    cout << "BLINK " << saki.first << " " << saki.second << " " << endl;
                                } else {
                                    cout << "MOVE " << saki.first << " " << saki.second << " " << endl;
                                }
                        } else {
                            goto NOT_EVADED;
                        }
                    } else {
                        NOT_EVADED:
                        if (hero_i.heroType == heros[0]) { //Ironman
                            int magx = -1, magy = -1;
                            int castdist2 = pow2(magx - hero_i.x) + pow2(magy - hero_i.y);

                            if ( !tower_attack && hero_i.mana >= 160 && hero_i.countDown2 == 0 && dist2(hero_i, nearest_enemy_hero )<= pow2(850) ){ //safety: 900 -> 800
                                magx = nearest_enemy_hero.x;
                                magy = nearest_enemy_hero.y;
                                int vecx = magx -  hero_i.x;
                                int vecy = magy -  hero_i.y;
                                double ex = (double) vecx / sqrt(castdist2);
                                double ey = (double) vecy / sqrt(castdist2);

                                double k = 880;
                                magx = (int)(hero_i.x + ex * k) ;
                                magy = (int)(hero_i.y + ey * k) ;

                                if (magx < 0){
                                    k = - hero_i.x / ex;
                                } else if (magx > 1920 ){
                                    k = (1920 - hero_i.x) / ex;
                                }
                                magx = (int)(hero_i.x + ex * k) ;
                                magy = (int)(hero_i.y + ey * k) ;

                                if (magy < 0){
                                    k = - hero_i.y / ey;
                                } else if (magy > 750 ){
                                    k = (750 - hero_i.y) / ey;
                                }
                                magx = (int)(hero_i.x + ex * k) ;
                                magy = (int)(hero_i.y + ey * k) ;

                                cout << "FIREBALL " << magx << " " << magy << endl;
                                continue;
                            }


                            magx = -1; magy = -1;
                            for (auto someentity: entitylist){
                                if (someentity.team == (1^myTeam) && someentity.unitType == "HERO") {
                                    if(magx<0){
                                        magx = someentity.x; magy = someentity.y;
                                    } else {
                                        magx = (magx + someentity.x) / 2;
                                        magy = (magy + someentity.y) / 2;
                                    }
                                }
                            }

                            if (tower_attack){
                                magx = enemy_tower.x;
                                magy = enemy_tower.y;
                            }
                            castdist2 = pow2(magx - hero_i.x) + pow2(magy - hero_i.y);
                            if (castdist2 > pow2(250)){
                                int vecx = magx -  hero_i.x;
                                int vecy = magy -  hero_i.y;
                                double ex = (double) vecx / sqrt(castdist2);
                                double ey = (double) vecy / sqrt(castdist2);

                                magx = (int)(hero_i.x + ex * 248) ;
                                magy = (int)(hero_i.y + ey * 248) ;
                            }

                            int burning_threshold = 10 - (hero_i.manaRegeneration/3);
                            int importance = 0;
                            for (auto someentity: entitylist){
                                if (someentity.team != (1^myTeam) ) continue;
                                if (someentity.unitType == "TOWER" && pow2(magx - someentity.x) + pow2(magy - someentity.y) <= pow2(100) ) {
                                    importance += tower_attack + 5;
                                } else if (someentity.unitType == "HERO" && pow2(magx - someentity.x) + pow2(magy - someentity.y) <= pow2(100)) {
                                        importance += 10;
                                } else if (someentity.unitType == "UNIT" && someentity.health <= hero_i.manaRegeneration*5+30 && pow2(magx - someentity.x) + pow2(magy - someentity.y) <= pow2(100)){
                                        importance += 5;
                                } else if (someentity.unitType == "UNIT" && pow2(magx - someentity.x) + pow2(magy - someentity.y) <= pow2(100)){
                                        importance += 1;
                                }
                            }

                            if (hero_i.mana > 50 && hero_i.countDown3 == 0 && importance >= burning_threshold) {
                                cout << "BURNING " << magx << " " << magy << endl;
                                continue; //i_loop
                            }


                            itemtobuy = buyitem( hero_i, itemlist, gold, 0, max(0.0, ((double)800-hero_i.health)/80) ,1,0,3);
                            if (itemtobuy != "none"){
                                cout << "BUY " << itemtobuy << endl;
    /*                        } else if (totower(hero_i, myTeam) > pow2(400)){
                                cout << "ATTACK " << entitylist[nearest[i]].unitId << endl;
                            } else {
                                cout << "ATTACK_NEAREST UNIT" << endl;
                            }
    */
                            } else {
                                int attackHP = 10000000;
                                int attackidx = -1;
                                entityidx = -1;
                                for (auto someentity: entitylist){
                                    entityidx++;
                                    if (someentity.unitType == "GROOT" || (!tower_attack && someentity.unitType == "TOWER")
                                        || someentity.team == myTeam ) continue;

                                    int modhealth = someentity.health + max(0, dist2(hero_i,someentity) - pow2(hero_i.attackRange));

                                    if (0 < modhealth && modhealth < attackHP ){
                                        attackHP = modhealth;
                                        attackidx = entityidx;
                                    }
                                }

                                entitylist[attackidx].health -= hero_i.attackDamage;
                                cout << "ATTACK " << entitylist[attackidx].unitId << endl;
                            }

                        } else { //DOCTOR_STRANGE

                            itemtobuy = buyitem( hero_i, itemlist, gold, 1,max(0.0, ((double)800-hero_i.health)/80),0,0,0);

                            int magx = -1, magy = -1;
                            for (auto someentity: myherolist){
                                if(magx<0){
                                    magx = someentity.x; magy = someentity.y;
                                } else {
                                    magx = (magx + someentity.x) / 2;
                                    magy = (magy + someentity.y) / 2;
                                }
                            }

                            int castdist2 = pow2(magx - hero_i.x) + pow2(magy - hero_i.y);
                            if (castdist2 > pow2(250)){
                                int vecx = magx -  hero_i.x;
                                int vecy = magy -  hero_i.y;
                                double ex = (double) vecx / sqrt(castdist2);
                                double ey = (double) vecy / sqrt(castdist2);

                                magx = (int)(hero_i.x + ex * 248) ;
                                magy = (int)(hero_i.y + ey * 248) ;
                            }

                            for (auto someentity: myherolist){
                                if (pow2(magx - someentity.x) + pow2(magy - someentity.y) > pow2(100) ) {
                                    cerr << "AOEHEAL from: " << myherolist[i].x << " " << myherolist[i].x << " to: " << magx << " " << magy << endl;
                                    cerr << "try to dist: " << sqrt(pow2(magx - someentity.x) + pow2(magy - someentity.y)) << endl;
                                    goto HEAL_FAILED_DR;
                                }
                            }

                            if ((hero_i.maxHealth - hero_i.health > hero_i.mana/5 || (index > 0 && myherolist[1^i].maxHealth - hero_i.health > hero_i.mana/5)) &&
                                hero_i.mana > 100 && hero_i.countDown1 == 0) {
                                cout << "AOEHEAL " << magx << " " << magy << endl;
                                continue; //i_loop
                            }

                            HEAL_FAILED_DR:

                            if (index == 0 && itemtobuy != "none"){
                                cout << "BUY " << itemtobuy << endl;
    /*                        } else if (totower(hero_i, myTeam) > pow2(400)){
                                cout << "ATTACK " << entitylist[nearest[i]].unitId << endl;
                            } else {
                                cout << "ATTACK_NEAREST UNIT" << endl;
                            }
    */
                            } else {
                                int attackHP = 10000000;
                                int attackidx = -1;
                                entityidx = -1;
                                for (auto someentity: entitylist){
                                    entityidx++;
                                    if (someentity.unitType == "GROOT" || (!tower_attack && someentity.unitType == "TOWER")
                                        || someentity.team == myTeam ) continue;

                                    int modhealth = someentity.health + max(0, dist2(hero_i,someentity) - pow2(hero_i.attackRange));

                                    if (0 < modhealth && modhealth < attackHP ){
                                        attackHP = modhealth;
                                        attackidx = entityidx;
                                    }
                                }

                                entitylist[attackidx].health -= hero_i.attackDamage;
                                cout << "ATTACK " << entitylist[attackidx].unitId << endl;
                            } //move select if

                        } //heroType
                    } // is_evasion
                }//hikikomori
            } // i_loop
        } //roundType
    } // while(1)
} // main
