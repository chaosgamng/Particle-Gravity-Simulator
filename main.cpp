#include <exception>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <thread>

class Particle {
public:
    int state;
    float x;
    float y;
    float theta;
    float accel;
    int mass;

    Particle() {
        state = 0;
        mass = 100;
        x = 0;
        y = 0;
        theta = 45;
        accel = 0.0;

    }

};

typedef struct {
    int x;
    int y;

} Mouse;


/// keeping the game running
static bool running = true;

/// for game options
static bool merging = true;
static bool setmap = true;
static bool move_to_mouse = false;
static bool pull = false;

/// variables I need for the game
static int scale = 2;
static int width = 640;
static int height = 480;
static SDL_Event e;
static Mouse mouse;

/// constants for gravity calculations
static double g = 0.667;

static int maxparticles = 128000;

/// game map (make it what max particles is)
static Particle particles[128000];


void gravitySim(int split) {
    int parts = maxparticles / 4;
    while(running == true){
        /// logic
        if(move_to_mouse == false){


        /// making everything go to each particle
            for(int i = (parts * split); i < (parts * (split+1)); i++){
                for(int x = 0; x < maxparticles; x++){
                    if(running == true && particles[i].state == 1 && particles[x].state == 1){
                        /// distance between particles
                        float r = (sqrt(pow((particles[i].x - particles[x].x), 2) + pow((particles[i].y - particles[x].y), 2)));
                        /// create gravity variable
                        float grav = 0.0;
                        /// see how far away and if we should apply force to the particle
                        if(r > 2 && r < (width / 4)){
                            float mass = particles[i].mass * particles[x].mass;

                            if(r < 40){
                                r = 40;
                            }

                            /// force formula for gravity
                            grav = ((g * mass) / (r * r));

                            /// how far away the particle is on x and y axis
                            float diffy = particles[i].y - particles[x].y;
                            float diffx = particles[i].x - particles[x].x;

                            /// using the difference to find our angle
                            particles[i].theta = atan2(diffy, diffx);

                            /// giving the particle acceleration
                            particles[i].accel = grav / particles[i].mass;
                        }
                        /// always have to apply acceleration to particle
                        particles[i].x += -(particles[i].accel * cos(particles[i].theta));
                        particles[i].y += -(particles[i].accel * sin(particles[i].theta));

                        r = (sqrt(pow((particles[i].x - particles[x].x), 2) + pow((particles[i].y - particles[x].y), 2)));
                        /// checks to see if the distance from the other particle is less than the mass
                        if(merging == true && i != x && r < 5){
                            if(particles[i].mass >= particles[x].mass){
                                particles[x].state = 0;
                                particles[i].mass += 1;
                            } else {
                                particles[i].state = 0;
                                particles[x].mass += 1;
                                break;
                            }

                        }
                        if(merging == false && i != x && r < 2){
                            /// finding the overlap in our x and y cords
                            float overlapx = (particles[i].x - particles[x].x);
                            float overlapy = (particles[i].y - particles[x].y);

                            /// applying the overlap to get the particles away from each other
                            particles[i].x += overlapx;
                            particles[i].y += overlapy;
                        }

                        /// resetting the acceleration
                        particles[i].accel = 0;
                    }
                }
            }
        } else {



            /// this is when we are making everything go to the mouse




            parts = maxparticles / 3 + 1;
            if(split == 3){
                for(int x = 0; x < maxparticles; x++){
                    if(running == true && particles[x].state == 1){
                        /// distance between particles
                        SDL_GetMouseState(&mouse.x, &mouse.y);
                        mouse.x = mouse.x / scale;
                        mouse.y = mouse.y / scale;
                        float r = (sqrt(pow((particles[x].x - mouse.x ), 2) + pow((particles[x].y - mouse.y), 2)));
                        /// create gravity variable
                        float grav = 0.0;
                        /// see how far away and if we should apply force to the particle
                        if(r > 2 && r < (width / 4)){
                            float mass = 50 * particles[x].mass;

                            if(r < 40 && pull == true){
                                r = 40;
                            }
                            /// force formula for gravity
                            grav = ((g * mass) / (r * r));

                            /// how far away the particle is on x and y axis
                            float diffy = particles[x].y - mouse.y;
                            float diffx = particles[x].x - mouse.x;

                            /// using the difference to find our angle
                            particles[x].theta = atan2(diffy, diffx);

                            /// giving the particle acceleration
                            particles[x].accel = grav;
                        }
                        /// always have to apply acceleration to particle
                        if(pull == true){
                            /// pull close
                            particles[x].x += -(particles[x].accel * cos(particles[x].theta));
                            particles[x].y += -(particles[x].accel * sin(particles[x].theta));
                        } else {
                            /// push away
                            particles[x].x += (particles[x].accel * cos(particles[x].theta));
                            particles[x].y += (particles[x].accel * sin(particles[x].theta));
                        }

                        /// resetting the acceleration
                        particles[x].accel = 0;
                    }
                }
            } else {
                for(int i = (parts * split); i < (parts * (split+1)); i++){
                    for(int x = 0; x < maxparticles; x++){
                        if(running == true && particles[i].state == 1 && particles[x].state == 1){
                            float r = (sqrt(pow((particles[i].x - particles[x].x), 2) + pow((particles[i].y - particles[x].y), 2)));

                            /// checks to see if the distance from the other particle is less than the mass
                            if(merging == true && i != x && r < 2){
                                if(particles[i].mass >= particles[x].mass){
                                    particles[x].state = 0;
                                    particles[i].mass += 1;
                                } else {
                                    particles[i].state = 0;
                                    particles[x].mass += 1;
                                    break;
                                }
                            }
                            if(merging == false && i != x){
                                /// finding the overlap in our x and y cords
                                float overlapx = (particles[i].x - particles[x].x);
                                float overlapy = (particles[i].y - particles[x].y);

                                /// applying the overlap to get the particles away from each other
                                particles[i].x += overlapx;
                                particles[i].y += overlapy;
                            }
                        }
                    }
                }
            }
        }
    }
}


int main(int argc , char* argv[] ) {

    /// for all the particle collisions
    std::thread gravlogic1(gravitySim, 0);
    std::thread gravlogic2(gravitySim, 1);
    std::thread gravlogic3(gravitySim, 2);
    std::thread gravlogic4(gravitySim, 3);


    /// making all the threads independent
    gravlogic1.detach();
    gravlogic2.detach();
    gravlogic3.detach();
    gravlogic4.detach();


    /// creating the window and setting everything up
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Init( SDL_INIT_EVERYTHING );
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, scale, scale);

    /// variables used for fps
    Uint32 start_time, frame_time;
    float fps;

    /// setting the game map up and all particles up
    for(int i =0; i < maxparticles; i++){
        if(setmap == true && rand() % 100 <= 90) {
            particles[i].state = 1;
            particles[i].x = rand() % height;
            particles[i].y = rand() % width;
        } else {
            particles[i].state = 0;
        }
    }
    /// run loop
    while(running){
        start_time = SDL_GetTicks();
        /// finding where we can place another particle
        int first = 0;
        int total = 0;
        int highestmass = 0;
        for(int i = maxparticles-1; i >= 0; i--){
                if(particles[i].state == 1){
                    total += 1;
                } else {
                    first = i;
                }
                if(particles[i].mass > highestmass){
                    highestmass = particles[i].mass;
                }
        }

        /// checking for keyboard / mouse events
        if(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                running = false;
                if(gravlogic1.joinable()){
                    gravlogic1.join();
                }
                if(gravlogic2.joinable()){
                    gravlogic2.join();
                }
                if(gravlogic3.joinable()){
                    gravlogic3.join();
                }
                if(gravlogic4.joinable()){
                    gravlogic4.join();
                }
            /// checks if mouse is moving and pressed or pressed
            } else if (e.type == SDL_MOUSEBUTTONUP){
                SDL_GetMouseState(&mouse.x, &mouse.y);
                mouse.x = mouse.x / scale;
                mouse.y = mouse.y / scale;
                if(mouse.x > 0 || mouse.x < width || mouse.y < height || mouse.y > 0){
                    if(total < maxparticles && particles[first].state == 0){
                        particles[first].state = 1;
                        particles[first].x = mouse.x;
                        particles[first].y = mouse.y;
                    }
                }
            } else if( e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == 27){
                    running = false;
                    if(gravlogic1.joinable()){
                        gravlogic1.join();
                    }
                    if(gravlogic2.joinable()){
                        gravlogic2.join();
                    }
                    if(gravlogic3.joinable()){
                        gravlogic3.join();
                    }
                    if(gravlogic4.joinable()){
                        gravlogic4.join();
                    }
                }
            }
        }



        /// clearing the map
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);
        /// drawing the pixels from the map
        for(int i =0; i < maxparticles; i++){
            if(particles[i].state == 1){
                if(particles[i].mass > 255){
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, particles[i].mass, 255, 255);
                }

                SDL_RenderDrawPoint(renderer, particles[i].x, particles[i].y);
            }
        }
        SDL_RenderPresent(renderer);

        /// getting fps and printing it
        frame_time = SDL_GetTicks()-start_time;
        fps = (frame_time > 0) ? 1000.0f / frame_time : 0.0f;
        if(fps > 60){
            std::cout << fps << std::endl;
        }
    }
    if(gravlogic1.joinable()){
        gravlogic1.join();
    }
    if(gravlogic2.joinable()){
        gravlogic2.join();
    }
    if(gravlogic3.joinable()){
        gravlogic3.join();
    }
    if(gravlogic4.joinable()){
        gravlogic4.join();
    }

    return 0;
}
