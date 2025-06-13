#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "SDL3/SDL.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>


void ReadShaders(unsigned int* programID){
    std::ifstream vertexFile("resources/cube_vertex.glsl");
    std::ifstream fragmentFile("resources/cube_fragment.glsl");

    std::stringstream vertexBuffer;
    vertexBuffer << vertexFile.rdbuf(); 
    std::string vertexString = vertexBuffer.str();

    std::stringstream fragmentBuffer;
    fragmentBuffer << fragmentFile.rdbuf();
    std::string fragmentString = fragmentBuffer.str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertexSource = vertexString.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);

    const char* fragmentSource = fragmentString.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    *programID = glCreateProgram();
    glAttachShader(*programID, vertexShader);
    glAttachShader(*programID, fragmentShader);
    glLinkProgram(*programID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void Initialize(unsigned int* vao, unsigned int* programID){
    ReadShaders(programID);
    glUseProgram(*programID);

    glm::mat4 view = glm::translate(glm::mat4(1), glm::vec3(0, 0, -3));
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f/720.0f, 0.1f, 100.0f);
    glm::mat4 projectionView = projection * view;
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionView));

    float vertices[] = {-1,-1,+1,-1,+1,+1,+1,+1,+1,+1,-1,+1,-1,-1,-1,-1,1,-1,+1,+1,-1,+1,-1,-1};
    int indices[] = {0, 1, 2, 0, 2, 3, 4, 5, 1, 4, 1, 0, 7, 6, 5, 7, 5, 4, 3, 2, 6, 3, 6, 7};

    glCreateVertexArrays(1, vao);
    glBindVertexArray(*vao);

    unsigned int vbo;
    unsigned int ebo;

    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*3, 0);
    glEnableVertexAttribArray(0);
}

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    
    SDL_Window* window = SDL_CreateWindow("Cube Test", 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    glEnable(GL_DEPTH_TEST);

    unsigned int vao;
    unsigned int programID;
    Initialize(&vao, &programID);

    bool quit = false;
    SDL_Event event;

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double deltaTime = 0.0;

    double min = 1;
    double max = 0;
    double averages[10] = { 0 };
    int count = 0;
    
    while(!quit){
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1.0);

        glm::mat4 transform = glm::rotate(glm::mat4(1), glm::radians(SDL_GetTicks() * 0.1f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(transform));

        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr);


        SDL_GL_SwapWindow(window);

        last = now;
        now = SDL_GetPerformanceCounter();

        deltaTime = (double)((now - last) * 1000) / SDL_GetPerformanceFrequency();
        
        averages[count] = deltaTime;
        count++;
        count = count % 10;
        
        double average = 0;
        for(int i = 0; i < 10; i++){
            average+=averages[i];
        }

        average = average / 10.0;

        if (deltaTime < min){
            min = deltaTime;
        }

        if (deltaTime > max){
            max = deltaTime;
        }

        std::cout
          << "Render Time: " << deltaTime << " ms \n"
          << "Min: " << min << " ms \n"
          << "Average: " << average << " ms \n"
          << "Max: " << max << " ms \n";
    }


    SDL_Quit();

    return 0;
}