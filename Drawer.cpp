// Include headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <math.h>

#include <stdlib.h>
#include <string.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "controls.hpp"
#include "text2D.hpp"

using namespace glm;

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Trajectory drawer", NULL, NULL);
    if( window == NULL )
    {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // White background
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "../TransformVertexShader.vertexshader", "../TextureFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > colors;

    //Прикостыливаем землю
    float GrSize = 10000;
    vertices.push_back( glm::vec3(GrSize, 0.0f,GrSize));
    vertices.push_back( glm::vec3 ((-1*GrSize), 0.0f, (-1*GrSize)));
    vertices.push_back( glm::vec3((-1*GrSize), 0.0f,GrSize));

    vertices.push_back( glm::vec3 {GrSize  , 0.0f,GrSize});
    vertices.push_back( glm::vec3 {GrSize , 0.0f,(-1*GrSize)});
    vertices.push_back( glm::vec3 {(-1*GrSize) , 0.0f, (-1*GrSize)});

    colors.push_back( glm::vec3 {0.05f, 0.5f, 0.014f});
    colors.push_back( glm::vec3 {0.05f, 0.5f, 0.014f});
    colors.push_back( glm::vec3 {0.05f, 0.5f, 0.014f});
    colors.push_back( glm::vec3 {0.05f, 0.5f, 0.014f});
    colors.push_back( glm::vec3 {0.05f, 0.5f, 0.014f});
    colors.push_back( glm::vec3 {0.05f, 0.5f, 0.014f});

    //Number of current vertex
    unsigned int i;

    FILE * file = fopen("../Model.txt", "r");
    if( file == NULL )
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    std::vector<float> times;
    std::vector<float> temps;
    std::vector<float> Pres;
    glm::vec3 vertex;
    glm::vec3 color;
    float time;
    float T;
    float P;

    while(1)
    {

        int res = fscanf(file, "%f x = %f y = %f z = %f T = %f P =%f\n", &time, &vertex.x, &vertex.z, &vertex.y, &T, &P);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        vertices.push_back(vertex);
        times.push_back(time);
        temps.push_back(T);
        Pres.push_back(P);
        //Yes, every vertex is written twice because i have not learned indexing
        vertices.push_back(vertex);
        times.push_back(time);
        temps.push_back(T);
        Pres.push_back(P);
    }

    //need max and min temperature to transform temperature into color (max = red; min =blue)
    std::vector<float>::iterator result = std::min_element(temps.begin(), temps.end());
    float Tmin = *result;
    result = std::max_element(temps.begin(), temps.end());
    float Tmax = *result;
    //temperature -> color
    for (i = 1; i < temps.size(); i++)
    {
        color.x = float((temps[i]-Tmin)/(Tmax - Tmin));
        color.y = 0;
        color.z = 1 - float((temps[i]-Tmin)/(Tmax - Tmin));

        colors.push_back(color);
    }



    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

    // Initialize my text library with black letters
    initText2D( "../TextB.dds" );

    printf("Tmin = %f Tmax = %f\n", Tmin, Tmax);

    char text[256];

    unsigned int lasti = 7;
    do
    {
        lasti = i;
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs(vertices);
        i = getcurrentnumber();

        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
/*
        if (i != lasti)
        {
            printf("x = %f  z = %f  h = %f T = %f P = %f\n", vertices[i].x, vertices[i].z, vertices[i].y, temps[i], Pres[i]);
            printf("R = %f G = %f B  = %f\n",colors[i].x, colors[i].y, colors[i].z);
        }
*/
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,                                // Атрибут. Здесь необязательно указывать 1, но главное, чтобы это значение совпадало с layout в шейдере..
            3,                                // Размер
            GL_FLOAT,                         // Тип
            GL_FALSE,                         // Нормализован?
            0,                                // Шаг
            (void*)0                          // Смещение
        );

        // Draw the ground
        glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2*3 indices starting at 0 -> 2 triangles
        // Draw the trajectory
        glDrawArrays(GL_LINES,7,vertices.size() - 7);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        //Print data
        sprintf(text, "x=%f  z=%f  h=%f\n", vertices[i].x, vertices[i].z, vertices[i].y);
        printText2D(text, 2, 10, 20);

        sprintf(text, "T = %f P = %f\n",temps[i], Pres[i]);
        printText2D(text, 30, 30, 20);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    //glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(programID);
    //glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);

    cleanupText2D();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

