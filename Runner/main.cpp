#include <iostream>
#include <raylib.h>
#include <string>

struct AnimationData{
    int frame;
    float updateTime;
    float runningTime;
};

struct Entity{
    Texture2D texture;
    Rectangle objectRect;
    Vector2 position;
    int acceleration;
    Vector2 velocity;
    Vector2 texture_sheet;
    bool get_points;

    AnimationData animation;

    Entity(Texture2D &n_texture, Vector2 n_position, Vector2 texture_par, int new_speed, Vector2 n_velocity){
        texture = n_texture;
        position = n_position;
        acceleration = new_speed;
        objectRect.x = position.x;
        objectRect.y = position.y;
        texture_sheet = texture_par;
        if(texture_par.x <= 0){
            texture_sheet.x = 1;
        }
        if(texture_par.y <= 0){
            texture_sheet.y = 1;
        }
        objectRect.width = texture.width/texture_sheet.x;
        objectRect.height = texture.height/texture_sheet.y;
        velocity = n_velocity;
        get_points = false;
    }
    Entity(){};
    void updateAnimation(float delta){
       
        animation.runningTime += delta;

        if(animation.runningTime >= animation.updateTime)
        {   
            objectRect.x = animation.frame * objectRect.width;
            animation.frame++;
            animation.runningTime = 0.f;
        }
        if(animation.frame > texture_sheet.x - 1){
            animation.frame = 0;
        }
    }
};

int main(){
    
    SetTargetFPS(60);
    int window_x = 640, window_y = 320;
    InitWindow(window_x,window_y,"HELLO WORLD");

    //Move variables
    float speed = 32.0;
    int dir_y = 0;
    int jump_power = 940;

    //Gravity
    float gravity = 1800.0;


    int nebulaSpeed = 200;

    bool collision = false;

    //Scores
    int points = 0;
    std::string points_string = std::to_string(points);

    //Load textures
    Texture2D t_player = LoadTexture("textures/scarfy.png");
    Texture2D t_nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D t_background = LoadTexture("textures/far-buildings.png");
    Texture2D t_foreground = LoadTexture("textures/foreground.png");
    Texture2D t_midground = LoadTexture("textures/back-buildings.png");

    //Initialize player
    Entity player(t_player,{0.f, 0.f},{6.f, 1.f},speed,{0.f,0.f});
    player.position.x = window_x/2 - player.objectRect.width/2;
    player.position.y = window_y/2;
    player.animation = {0, 1.f/12.f, 0.f};

    const int OBSTACLES_SIZE = 3;
    Entity obstacles[OBSTACLES_SIZE];

    //Initialize obstacles
    for(int i = 0; i < OBSTACLES_SIZE; i++){
        obstacles[i].texture = t_nebula;
        obstacles[i].position = {0.f, 0.f};
        obstacles[i].texture_sheet = {8.f, 8.f};
        obstacles[i].objectRect = {0.f, 0.f, obstacles[i].texture.width/obstacles[i].texture_sheet.x, 
        obstacles[i].texture.height/obstacles[i].texture_sheet.y};

        obstacles[i].acceleration = nebulaSpeed;
        obstacles[i].velocity = {0.f, 0.f};

        obstacles[i].position.x = window_x + (i * (window_x/2));
        obstacles[i].position.y = window_y - obstacles[i].objectRect.height;

        obstacles[i].animation = {0, 1.f/12.f, 0.f};
    }

    bool in_air = false;

    //Backgrounds
    float bgX = {0.f};
    int bgAccel = 32;
    float fgX = {0.f};
    float mgX = {0.f};
    int mgAccel = 40;
    int fgAccel = 64;

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);

        float delta = GetFrameTime();

        //Draw scrolling background
        bgX -= bgAccel * delta;
        mgX -= mgAccel * delta;
        fgX -= fgAccel * delta;

        //x3 cuz it's scaled x3
        if(bgX + t_background.width * 3 <= 0){
            bgX = 0;
        }
        if(mgX + t_midground.width * 3 <= 0){
            mgX = 0;
        }
        if(fgX + t_foreground.width * 3 <= 0){
            fgX = 0;
        }
        Vector2 bgPos = {bgX, 0.f};
        Vector2 bgPos2 = {bgX + t_background.width * 3, 0.f};
        Vector2 mgPos = {mgX, 0.f};
        Vector2 mgPos2 = {mgX + t_midground.width * 3, 0.f};
        Vector2 fgPos = {fgX, 0.f};
        Vector2 fgPos2 = {fgX + t_foreground.width * 3, 0.f};

        DrawTextureEx(t_background, bgPos, 0.f, 3.f, WHITE);
        DrawTextureEx(t_background, bgPos2, 0.f, 3.f, WHITE);
        DrawTextureEx(t_midground, mgPos, 0.f, 3.f, WHITE);
        DrawTextureEx(t_midground, mgPos2, 0.f, 3.f, WHITE);
        DrawTextureEx(t_foreground, fgPos, 0.f, 3.f, WHITE);
        DrawTextureEx(t_foreground, fgPos2, 0.f, 3.f, WHITE);

        if((player.position.y) > window_y - player.objectRect.height)
        {
            player.velocity.y = 0;
            if(in_air){
                player.position.y = window_y - player.objectRect.height;
            }
            in_air = false;
        }
        else{
        player.velocity.y += gravity * delta;
        }

        if(IsKeyDown(KEY_SPACE) && !in_air){
            player.velocity.y -= jump_power;
            in_air = true;
        }
        if(IsKeyDown(KEY_W)){
            dir_y -= 1.0;
        }
        if(IsKeyDown(KEY_S)){
            dir_y += 1.0;
        }

        for(int i = 0; i < OBSTACLES_SIZE; i++){
            obstacles[i].position.x -= obstacles[i].acceleration * delta;
            if(obstacles[i].position.x + obstacles[i].objectRect.width < player.position.x && !obstacles[i].get_points && !collision){
                points += 20;
                points_string = std::to_string(points);
                obstacles[i].get_points = true;
            }
            obstacles[i].updateAnimation(delta);
            if(obstacles[i].position.x + obstacles[i].objectRect.width < 0){
                obstacles[i].position.x = window_x + window_x/2;//obstacles[OBSTACLES_SIZE-1].position.x + window_x/2;
                obstacles[i].get_points = false;
            }
        }

        player.position.y += player.velocity.y * delta;

        for(Entity ent : obstacles){
            
            float padding = 64.f;
            float pad = 24.f;

            Rectangle obstacleHurtBox{ent.position.x + padding, ent.position.y + padding,
            ent.objectRect.width - padding, ent.objectRect.height - padding};

            Rectangle playerHurtBox{player.position.x + 2*pad, player.position.y + 2*pad,
            player.objectRect.width - pad, player.objectRect.height- pad};

            if(CheckCollisionRecs(obstacleHurtBox, playerHurtBox)){
                collision = true;
            }


        }

        if(!in_air){
            player.updateAnimation(delta);
        }

        //Render entities
        for(int i = 0; i < OBSTACLES_SIZE; i++){
            DrawTextureRec(t_nebula, obstacles[i].objectRect, obstacles[i].position, WHITE);
        }
        

        const char* str = points_string.c_str();

        if(!collision){
            DrawTextureRec(t_player, player.objectRect, player.position, WHITE);
        }
        else{
            std::string end_text = "YOUR FINAL SCORE: " + std::to_string(points);
            const char* ending =  end_text.c_str();
            DrawText(ending, window_x/2, window_y/2, 24, PINK);
        }

        DrawText(str,0,0,16,PINK);

        EndDrawing();
    }
    UnloadTexture(t_player);
    UnloadTexture(t_nebula);
    UnloadTexture(t_background);
    UnloadTexture(t_midground);
    UnloadTexture(t_foreground);
    return 0;
}