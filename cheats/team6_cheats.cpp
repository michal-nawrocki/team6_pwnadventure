#include <dlfcn.h>
#include <set>
#include <map>
#include <functional>
#include <string.h>
#include <vector>
#include "libGameLogic.h"

/*
    Cheat globals
*/
ClientWorld* world;
IPlayer* iplayer;
Player* player;
Actor* actor;
void (*realSetJumpState)(bool) = (void (*)(bool))dlsym(RTLD_NEXT,"_ZN6Player12SetJumpStateEb");
void (*realPerformChat)(const std::string&) = (void (*)(const std::string&))dlsym(RTLD_NEXT,"_ZN6Player11PerformChatERKSs");

Vector3 frozen_pos;

int actorStep = 0;
Actor* goldenEggActors[11];
int arrCount = 0;

bool cheat_is_player_set = false;
bool cheat_fly = false;
bool cheat_run = false;
bool cheat_frozen = false;
bool cheat_health = false;


/* 
    Helper functions 
 */
void WriteInChatBox(const char* msg){
    /*
        Print string in Player's chat box
    */
    iplayer->GetLocalPlayer()->OnChatMessage("Team6_Cheats", true, msg);  
}

/*
    Override functions 
 */
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
        WriteInChatBox("CHEATS: Activated FLY");
        cheat_fly = !cheat_fly;
    }

    if(strcmp(msg, "/run") == 0){
        WriteInChatBox("CHEATS: Activated RUN");
        cheat_run = !cheat_run;
    }
    if(strcmp(msg, "/health") == 0){
        WriteInChatBox("CHEATS: Activated HEALTH");
        cheat_health = !cheat_health;
    }

    if(strcmp(msg, "/get_pos") == 0){
        Vector3 pos = player->GetPosition();
        WriteInChatBox("CHEATS: Get position:");

        char buffer[100];
        sprintf(buffer, "Player pos: %.2f / %.2f / %.2f", pos.x, pos.y, pos.z);
        
        WriteInChatBox(buffer);
    }

    if(strncmp("/teleport ", msg, 10) == 0){
        printf("CHEATS: Activated TELEPORT");

        float* x;
        float* y;
        float* z;
        sscanf(msg+10, "%f %f %f", x, y, z);

        char buffer[100];
        sprintf(buffer, "Teleporting to new pos: %f %f %f", *x, *y, *z) ;
        WriteInChatBox(buffer);
        player->SetPosition(Vector3(*x, *y, *z));
    }

    if(strcmp(msg, "/bearFlag") == 0){
        // Simply activate cheat to tele to chest, and start bear waves
        // Activate again to disable freeze, and "jump" out of chest

        // Move player to Bear Chest
        Vector3 curr_pos = Vector3(-7894, 64482, 2663);
        char buffer[100];
        sprintf(buffer, "Teleporting to new pos: %f %f %f", curr_pos.x, curr_pos.y, curr_pos.z);
        WriteInChatBox(buffer);
        player->SetPosition(Vector3(curr_pos.x, curr_pos.y, curr_pos.z));

        // Freeze player inside Bear Chest
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

          // if blueprint has Golden egg go to it.
          if(actorBlueprintName.find("GoldenEgg") != std::string::npos || actorBlueprintName.find("BallmerPeak") != std::string::npos){
            printf("%s\n", actorBlueprint);
            goldenEggActors[arrCount] = actor;
            arrCount++;

          }

          actorID++;

        }
        arrCount = 0;

    }

    if(strcmp(msg, "/tpEgg") == 0){
        WriteInChatBox("CHEATS: Activated TPEGG");
        // std::set<ActorRef<IActor>> m_actors;
        // class IActor;
        // class Actor;
        // actor class has     Vector3 GetPosition();

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
        /* 
            Utitilty cheat just to list the available commands,
            if they specify a command afterwards it describes the required input.
        */
        
        if(strlen(msg) == 5){
            WriteInChatBox("Available Commands:\n/fly, /run, /health,\n/get_pos, /teleport, /bearFlag,\n/findEggs, /tpEgg, /help,\n/locate, /help <command>");
        } else {
            
            if(strcmp(msg, "/help fly") == 0){
                WriteInChatBox("Command Usage: /fly.\nToggles the ability to fly in the direction you are looking.");
            }
            
            if(strcmp(msg, "/help run") == 0){
                WriteInChatBox("Command Usage: /run.\nToggles the ability to run super fast.");
            }
            
            if(strcmp(msg, "/help health") == 0){
                WriteInChatBox("Command Usage: /health.\nToggles a health and mana cheat to improve regeneration and maximum values.");
            }
            
            if(strcmp(msg, "/help get_pos") == 0){
                WriteInChatBox("Command Usage: /get_pos.\nReturns your current player position.");
            }
            
            if(strcmp(msg, "/help teleport") == 0){
                WriteInChatBox("Command Usage: /teleport x y z.\nInput 3 float values to update your position with those values.");
            }
            
            if(strcmp(msg, "/help bearFlag") == 0){
                WriteInChatBox("Command Usage: /bearFlag.\nToggles the frozen state at the chest for the bearFlag.");
            }
            
            if(strcmp(msg, "/help findEggs") == 0){
                WriteInChatBox("Command Usage: /findEggs.\nUpdates an array to store all the goldenEgg objects.");
            }
            
            if(strcmp(msg, "/help tpEgg") == 0){
                WriteInChatBox("Command Usage: /tpEgg.\nTeleports you to the next goldenEgg in the array, stepping through each time you call this command.");
            }
            
        }        
    }
    
    if(strncmp(msg, "/locate", 7) == 0){
        /*
            Utitilty cheat just to list the currently known locations by co-ordinates.
            Without input it lists their names and inputting a name will only display the position.
         */
        
        if(strlen(msg) == 7){
            WriteInChatBox("Available locations:\nBallmerPeak, BearChestAbove, BearChestBelow");
        } else {
            
            if(strcmp(msg, "/locate BallmerPeak") == 0){
                WriteInChatBox(" (X Y Z) -6791.0 -11655.0 10528.0");
            }
            
            if(strcmp(msg, "/locate BearChestAbove") == 0){
                WriteInChatBox(" (X Y Z) -7894.0 64482.0 2663.0");
            }
            
            if(strcmp(msg, "/locate BearChestBelow") == 0){
                WriteInChatBox(" (X Y Z) -7894.21 64499.97 2605.77");
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
