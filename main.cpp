#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

std::vector<float> createCircleVertices(float radius, int segments) {
    std::vector<float> vertices;

    // The Center Point (0,0)
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; i++) {
        float angle = i * 2.0f * 3.14159f / segments;
        vertices.push_back(cos(angle) * radius); // X
        vertices.push_back(sin(angle) * radius); // Y
        vertices.push_back(0.0f);                // Z
    }
    return vertices;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec2 playerPos = glm::vec2(0.0f, 300.0f);
glm::vec2 playerSize = glm::vec2(25.0f, 175.0f);
float playerSpeed = 350.0f;

int playerScore = 0;
int enemyScore = 0;

glm::vec2 ballPos = glm::vec2(600.0f, 300.0f);
glm::vec2 ballVelocity = glm::vec2(300.0f, 300.0f);
glm::vec2 ballSize = glm::vec2(25.0f, 25.0f);
glm::vec3 ballSize2 = glm::vec3(ballSize, 1.0f);
float ballSpeed = 400.0f;

glm::vec2 enemyPos = glm::vec2(800.0f, 300.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "   gl_Position = projection * model * vec4(aPos, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float velocity = playerSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) playerPos.y += velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) playerPos.y -= velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) enemyPos.y -= velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) enemyPos.y += velocity;


    // Borders (Player is 50 wide, so 25 from center to edge)
    float halfWidth = playerSize.x / 2.0f;
    float halfHeight = playerSize.y / 2.0f;

    if (enemyPos.y < halfHeight)  enemyPos.y = halfHeight;
    if (enemyPos.y > 600.0f - halfHeight)  enemyPos.y = 600.0f - halfHeight;
    if (playerPos.y < halfHeight) playerPos.y = halfHeight;
    if (playerPos.y > 600.0f - halfHeight) playerPos.y = 600.0f - halfHeight;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 1. Compile Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // 2. Compile Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // 3. Link into a Program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 4. Clean up individual shaders (they are linked now, no longer needed)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
       -0.5f,  0.5f, 0.0f   // top left
   };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    std::vector<float> circleVertices = createCircleVertices(0.5f, 32);

    unsigned int circleVAO, circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), &circleVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 2. Set Projection (Same for both)
        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // 3. DRAW PLAYER
        glm::mat4 playerModel = glm::mat4(1.0f);
        playerModel = glm::translate(playerModel, glm::vec3(playerPos, 0.0f));
        playerModel = glm::scale(playerModel, glm::vec3(playerSize, 1.0f));
        // Send PLAYER matrix
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(playerModel));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 4. DRAW ENEMY
        glm::mat4 enemyModel = glm::mat4(1.0f);
        enemyModel = glm::translate(enemyModel, glm::vec3(enemyPos, 0.0f));
        enemyModel = glm::scale(enemyModel, glm::vec3(playerSize, 1.0f));
        // Send ENEMY matrix (Crucial: use enemyModel, not playerModel!)
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(enemyModel));
        // VAO is already bound, but we call draw again with the new matrix active
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Ball
        glm::mat4 ballModel = glm::mat4(1.0f);
        ballModel = glm::translate(ballModel, glm::vec3(ballPos, 0.0f)); // Center of screen
        ballModel = glm::scale(ballModel, ballSize2);       // Size
        ballPos += ballVelocity * deltaTime;

        // 2. Bounce off Top and Bottom walls
        if (ballPos.y <= 0.0f || ballPos.y + ballSize.y >= 600.0f) {
            ballVelocity.y *= -1.0f;
        }

        // 3. Simple AABB Collision (Ball vs Player Paddle)
        if (ballPos.x <= playerPos.x + playerSize.x &&
            ballPos.y + ballSize.y >= playerPos.y - playerSize.y/2.0f &&
            ballPos.y <= playerPos.y + playerSize.y/2.0f)
        {
            ballVelocity.x *= -1.0f; // Bounce back
            ballPos.x = playerPos.x + playerSize.x + 1.0f; // Prevent getting stuck
        }

        // 4. Simple AABB Collision (Ball vs Enemy Paddle)
        if (ballPos.x + ballSize.x >= enemyPos.x &&
            ballPos.y + ballSize.y >= enemyPos.y - playerSize.y/2.0f &&
            ballPos.y <= enemyPos.y + playerSize.y/2.0f)
        {
            ballVelocity.x *= -1.0f;
            ballPos.x = enemyPos.x - ballSize.x - 1.0f;
        }

        // 5. Scoring (If ball goes past the left or right edge)
        if (ballPos.x < 0.0f) {
            enemyScore++;
            ballPos = glm::vec2(400.0f, 300.0f); // Reset ball to center
            std::cout << "SCORE UPDATED | Player: " << playerScore << " - Enemy: " << enemyScore << std::endl;
        }
        if (ballPos.x > 800.0f) {
            playerScore++;
            ballPos = glm::vec2(400.0f, 300.0f); // Reset ball to center
        }

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(ballModel));

        glBindVertexArray(circleVAO);
        // We draw 'segments + 2' vertices (center + perimeter points + closing point)
        glDrawArrays(GL_TRIANGLE_FAN, 0, 34);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}