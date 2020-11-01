#include <dlfcn.h>
#include <set>
#include <map>
#include <functional>
#include <string.h>
#include <vector>
#include "libGameLogic.h"

// Globals for cheats
ClientWorld* world;
IPlayer* iplayer;
Player* player;
Actor* actor;
void (*realSetJumpState)(bool) = (void (*)(bool))dlsym(RTLD_NEXT,"_ZN6Player12SetJumpStateEb");
Vector3 frozen_pos;

int actorStep = 0;
Actor* goldenEggActors[11];
int arrCount = 0;

bool cheat_is_player_set = false;
bool cheat_fly = false;
bool cheat_run = false;
bool cheat_frozen = false;
bool cheat_health = false;

void World::Tick(float f){
    if(!cheat_is_player_set){
        world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));
        iplayer = world->m_activePlayer.m_object;
        player = ((Player*)(iplayer));
        actor = ((Actor*)(iplayer));
    }

    if(cheat_fly){
        player->m_jumpSpeed = 200;
        player->m_jumpHoldTime = 99999;

        Rotation rot = player->GetLookRotation();
        Vector3 vel = player->GetVelocity();

        player->SetVelocity(Vector3(vel.x+(rot.roll/180*20000), vel.y+(rot.pitch/90*20000), vel.z));
    }else{
        player->m_jumpSpeed = 420;
        player->m_jumpHoldTime = 0.2f;
    }

    if(cheat_run){
        player->m_walkingSpeed = 99999;
    }else{
        player->m_walkingSpeed = 200;
    }

    if(cheat_frozen){
        player->SetPosition(frozen_pos);
        player->SetVelocity(Vector3(0, 0, 400));

    }

    if(cheat_health){
      // float m_manaRegenTimer;
      // float m_healthRegenCooldown;
      // float m_healthRegenTimer;
      player->m_manaRegenTimer = 0.0f;
      player->m_healthRegenCooldown = 0.0f;
      player->m_healthRegenTimer = 0.0f;
      player->m_health = 6000;
      player->m_mana = 6000;
    }
}

void Player::Chat(const char* msg){
    printf("Player typed: %s\n", msg);

    if(strcmp(msg, "/fly") == 0){
        printf("CHEATS: Activated FLY\n");
        cheat_fly = !cheat_fly;
    }

    if(strcmp(msg, "/run") == 0){
        printf("CHEATS: Activated RUN\n");
        cheat_run = !cheat_run;
    }
    if(strcmp(msg, "/health") == 0){
        printf("CHEATS: Activated HEALTH\n");
        cheat_health = !cheat_health;
    }

    if(strcmp(msg, "/get_pos") == 0){
        Vector3 pos = player->GetPosition();
        printf("CHEATS: Get position:\n");
        printf("Player pos: %.2f / %.2f / %.2f\n", pos.x, pos.y, pos.z);
    }

    if(strncmp("/teleport ", msg, 10) == 0){
        printf("CHEATS: Activated TELEPORT\n");

        float* x;
        float* y;
        float* z;
        sscanf(msg+10, "%f %f %f", x, y, z);

        printf("Teleporting to new pos: %f %f %f\n", *x, *y, *z);
        player->SetPosition(Vector3(*x, *y, *z));
    }

    if(strcmp(msg, "/bearFlag") == 0){
        //Simply activate cheat to tele to chest, and start bear waves
        //Activate again to disable freeze, and "jump" out of chest

        //Move player to Bear Chest
        Vector3 curr_pos = Vector3(-7894, 64482, 2663);
        printf("Teleporting to new pos: %f %f %f\n", curr_pos.x, curr_pos.y, curr_pos.z);
        player->SetPosition(Vector3(curr_pos.x, curr_pos.y, curr_pos.z));

        //Freeze player inside Bear Chest
        cheat_frozen = !cheat_frozen;
        Vector3 pos = player->GetPosition();
        frozen_pos = Vector3(-7894.21, 64499.97, 2605.77);
        player->SetPosition(frozen_pos);
    }

    if(strcmp(msg, "/findEggs") == 0){

        std::set<ActorRef<IActor>> actors = world->m_actors;
        std::set<ActorRef<IActor>>::iterator actorID = actors.begin();

        for(int i = 0; i < actors.size(); i++){

          Actor* actor = (Actor*)(actorID->m_object);
          const char* actorBlueprint = actor->m_blueprintName;
          std::string actorBlueprintName = actorBlueprint;

          if(actorBlueprintName.find("GoldenEgg") != std::string::npos || actorBlueprintName.find("BallmerPeak") != std::string::npos){ // if blueprint has Golden egg go to it.
            printf("%s\n", actorBlueprint);
            goldenEggActors[arrCount] = actor;
            arrCount++;

          }

          actorID++;

        }
        arrCount = 0;

    }

    if(strcmp(msg, "/tpEgg") == 0){
        printf("CHEATS: Activated TPEGG\n");
        //std::set<ActorRef<IActor>> m_actors;
        // class IActor;
        // class Actor;
        //actor class has     Vector3 GetPosition();

        if( actorStep < sizeof(goldenEggActors)){

          Actor* actor = goldenEggActors[arrCount];
          printf("inside: %s", actor->m_blueprintName);
          Vector3 position = actor->GetPosition();

          player->SetPosition(Vector3(position.x, position.y, position.z+180));

          actorStep++;
          arrCount++;
        }else{
          actorStep = 0;
        }

      }
      
    if(strncmp(msg, "/help", 5) == 0){
        //Utitilty cheat just to list the available commands, if they specify a command afterwards it describes the required input
        
        if(strlen(msg) == 5){
            printf("Available Commands: /fly, /run, /health, /get_pos, /teleport, /bearFlag, /findEggs, /tpEgg, /help");
        } else {
            
            if(strcmp(command, "/help fly") == 0){
                printf("Command Usage: /fly. Toggles the ability to fly in the direction you are looking.");
            }
            
            if(strcmp(command, "/help run") == 0){
                printf("Command Usage: /run. Toggles the ability to run super fast.");
            }
            
            if(strcmp(command, "/help health") == 0){
                printf("Command Usage: /health. Toggles a health and mana cheat to improve regeneration and maximum values.");
            }
            
            if(strcmp(command, "/help get_pos") == 0){
                printf("Command Usage: /get_pos. Returns your current player position.");
            }
            
            if(strcmp(command, "/help teleport") == 0){
                printf("Command Usage: /teleport x y z. Input 3 float values to update your position with those values.");
            }
            
            if(strcmp(command, "/help bearFlag") == 0){
                printf("Command Usage: /bearFlag. Toggles the frozen state at the chest for the bearFlag.");
            }
            
            if(strcmp(command, "/help findEggs") == 0){
                printf("Command Usage: /findEggs. Updates an array to store all the goldenEgg objects.");
            }
            
            if(strcmp(command, "/help tpEgg") == 0){
                printf("Command Usage: /tpEgg. Teleports you to the next goldenEgg in the array, stepping through each time you call this command.");
            }
            
        }        
    }
    
    if(strncmp(msg, "/locate", 7) == 0){
        //Utitilty cheat just to list the currently known locations by co-ordinates. Without input it lists their names and inputting a name will only display the position.
        
        if(strlen(msg) == 7){
            printf("Available locations: BallmerPeak, BearChestAbove, BearChestBelow");
        } else {
            
            if(strcmp(command, "/locate BallmerPeak") == 0){
                printf(" (X Y Z) -6791.0 -11655.0 10528.0");
            }
            
            if(strcmp(command, "/locate BearChestAbove") == 0){
                printf(" (X Y Z) -7894.0 64482.0 2663.0");
            }
            
            if(strcmp(command, "/locate BearChestBelow") == 0){
                printf(" (X Y Z) -7894.21 64499.97 2605.77");
            }
            
        }        
    }

}

bool Player::CanJump(){
    return 1;
}

void Player::SetJumpState(bool b){
    if(!cheat_fly){
        realSetJumpState(b);
    }else{
        return;
    }
}




// // This method will be called everytime the user jumps (presses space).
// void Player::SetJumpState(bool b) {

//     // load a pointer to the GameWorld object:
//     ClientWorld* world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));

//     // we know these names from the libGameLogic.h file.
//     IPlayer* iplayer = world->m_activePlayer.m_object;
//     Player* player = ((Player*)(iplayer));
//     Actor* actor = ((Actor*)(iplayer));

//     // print some data to the console that was used to start the program
//     printf("[LO] %f \n", world->m_timeUntilNextNetTick);
//     printf ("Speed %f\n", player->m_walkingSpeed);

//     // set the player jump to very high values.
//     //
//     player->m_jumpSpeed=5000;
//     player->m_jumpHoldTime=60;

//     // We now need to call the orginal Set Jump State method, otherwise the server
//     // (and other players) will not see us jump. Here is a pointer we will set to
//     // that function:
//     void (*realSetJumpState)(bool);

//     //To find the address of the real function we need to find its  orginal address
//     //For this we need the "mangled" name, which we found open the libGameLogic.so
//     //file in IDA looking at the exprots and right clicking and deselecting "demangle"
//     realSetJumpState =(void (*)(bool))dlsym(RTLD_NEXT,"_ZN6Player12SetJumpStateEb");

//     printf("realSetJumpState is at: %p\n", realSetJumpState);
//     realSetJumpState(b);
// }
