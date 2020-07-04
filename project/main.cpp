#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shader.h"
#include "Object.h"
#include "Animation.h"
#include <GL/glut.h>
#include <irrklang/irrKlang.h>
#include"Tray.h";
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <vector>
#define GL_BGR_EXT 0x80E0
#define BMP_Header_Length 54

using namespace std;

//plane
struct Plane {
	glm::vec3 _Position;
	glm::vec3 _Normal;
};
Plane pl1, pl2, pl3, pl4, pl5, pl6,pl7;

//balls
glm::vec3 veloc1(2, 2, -2);              //initial velocity of balls
glm::vec3 veloc2(-2, 2, 0);
glm::vec3 ArrayVel[2];                     //holds velocity of balls
glm::vec3 ArrayPos[2];                     //position of balls
glm::vec3 OldPos[2];                       //old position of balls
glm::vec3 tmp;
int NrOfBalls=2;                            //sets the number of balls

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
unsigned int loadTexture(char const* path);
void renderQuad();
void renderCube();
void renderSphere();
void idle();

//Collision
int TestIntersionPlane(const Plane& plane, const glm::vec3& position, const glm::vec3& direction, double& lamda, glm::vec3& pNormal);//五个参数分别是平面的结构，camera或ball的当前位置，camera或ball的运动方向，碰撞距离，碰撞法线
int FindBallCol(glm::vec3& point, double& TimePoint, double Time2, int& BallNr1, int& BallNr2);

// Game
bool bOver = false;  //true 游戏结束
bool bWin = false;
int winCount = 0;
int candle_num = 0;
bool isopen = false;
// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;


// camera
Camera camera(glm::vec3(0.0f, 1.0f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//动画
Door Adoor(glm::vec3(-1.3f, 0.0f, 4.15f), 25, 2);
vector<Coin> Acoins;
AnimateModel Acandle( glm::vec3(-0.8f, 0.95f, -0.2F), 10, 1);



// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 0.4f, -3.6f);
float light_str = 0.1f;
glm::vec3 lightDir(0.0f, 4.0f, -3.6f);
// music
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();


int main()
{
	//select mode
	cout << "0 for game mode, 1 for roam mode" << endl;
	int m;
	cin >> m;
	if (!m)
	{
		camera.Mode = GAME;
	}
	else
	{
		camera.Mode = ROAM;
	}


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	//新增
	glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);        //  设置深度测试类型

    // build and compile our shader zprogram
    // ------------------------------------
	Shader shader("shaders/shadow_mapping.vert", "shaders/shadow_mapping.frag");
    Shader ourShader("shaders/model_loading.vert", "shaders/model_loading.frag");
    Shader lightCubeShader("shaders/light.vert", "shaders/light.frag");
    Shader simpleDepthShader("shaders/shadow_mapping_depth.vert", "shaders/shadow_mapping_depth.frag");
	Shader ballShader("shaders/light.vert", "shaders/light.frag");

	//play music
	SoundEngine->play2D("BGM_H.mp3", GL_TRUE);

	// load model
	// template
	//Model my_model("textures/steve.obj");

	//外景
	Model house("textures/house/house.obj");

	//结构
	Model fireplace("textures/fireplace/fireplace.obj");
	Model win("textures/window/window.obj");
	
	//家具
	Model bookshelf("textures/shelf/bookshelf.obj");
	Model cabinet("textures/Cabinet/cabinet.obj");
	Model chair("textures/chair/chair.obj");
	Model chest("textures/chest/chest.obj");
	Model diningtable("textures/diningtable/diningtable.obj");

	//小物件
	//Model sword("textures/sword/sword_dirty.obj");
	Model book("textures/Scroll/ScrollBookCandle.obj");

	//动画
	Adoor.Load("textures/door/door ");
	Acandle.Load("textures/candle/candle ");

	Coin tmpcoin(glm::vec3(3.5, 1, 0), 10, 1);  //OK
	tmpcoin.Load("textures/coin/coin ");
	Acoins.push_back(tmpcoin);
	tmpcoin.Assign(glm::vec3(0, 1.1, 0), 30);  //ok
	Acoins.push_back(tmpcoin);
	tmpcoin.Assign(glm::vec3(-3.3, 1.0, 0), 90);
	Acoins.push_back(tmpcoin);
	tmpcoin.Assign(glm::vec3(2.8, 1.6, -3.3));
	Acoins.push_back(tmpcoin);
	tmpcoin.Assign(glm::vec3(3.5, 2.8, 3));
	Acoins.push_back(tmpcoin);

	// game text
	Model gameover("textures/game/gameover.obj");
	Model youwin("textures/game/youwin.obj");
	//Game
	Ghost ghost("textures/ghost/ghost.obj");
	ghost.initialize(0.4f, 0.0f, 3.0f, 0.0);

	ghost.Output("outputtext.obj");
	
    unsigned int plantexture=loadTexture("textures/wall.jpg");
	unsigned int walltexture = loadTexture("textures/stonewall.jpg");
	unsigned int ceilingtexture = loadTexture("textures/wood2.jpg");

    renderQuad();
    renderCube();
	renderSphere();

    //阴影贴图
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);//为渲染的深度贴图创建帧缓存对象

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024; //创建一个2D纹理，提供给帧缓存的深度缓冲使用
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);//把生成的深度纹理作为帧缓冲的深度缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);//我们需要的只是在从光的透视图下渲染场景的时候深度信息，所以颜色缓冲没有用。然而，不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。我们通过将调用glDrawBuffer和glReadBuffer把读和绘制缓冲设置为GL_NONE来做这件事。

	ArrayPos[0] = glm::vec3(2, 2, 2);
	ArrayPos[1] = glm::vec3(-2, 2, 2);

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);
    // render loop
    // -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------

		//Game check
		if (bOver || bWin)
		{
			//Display Text
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Message Position
			glm::vec3 staticPosition(0.0f, 0.0f, 4.0f);
			glm::vec3 staticFront(0.0f, 0.0f, -1.0f);
			glm::vec3 staticUp(0.0f, 1.0f, 0.0f);

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::lookAt(staticPosition, staticPosition + staticFront, staticUp);


			ourShader.use();
			ourShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 4.0f));
			ourShader.setFloat("shininess", 1.0f);
			ourShader.setVec3("dirlight.direction", -1.0f, -1.0f, -1.0f);
			ourShader.setVec3("dirlight.ambient", 0.4f, 0.2f, 0.2f);
			ourShader.setVec3("dirlight.diffuse", 0.1f, 0.1f, 0.1f);
			ourShader.setVec3("dirlight.specular", 0.1f, 0.1f, 0.1f);

			ourShader.setMat4("projection", projection);
			ourShader.setMat4("view", view);



			// Draw Text
			if (bOver)
			{
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
				model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));	// it's a bit too big for our scene, so scale it down
				ourShader.setMat4("model", model);
				gameover.Draw(ourShader);

			}

			if (bWin)
			{
				model = glm::translate(model, glm::vec3(0.4f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
				model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));	// it's a bit too big for our scene, so scale it down
				ourShader.setMat4("model", model);
				youwin.Draw(ourShader);
			}

		}
		// Normal Scene Render
		else {

			glClearColor(0.4f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 1. render depth of scene to texture (from light's perspective)
			// --------------------------------------------------------------
			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			float near_plane = 1.0f, far_plane = 7.5f;
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;
			// render scene from light's point of view
			simpleDepthShader.use();
			simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.2f, -0.15f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.125f, 0.125f, 0.125f));	// it's a bit too big for our scene, so scale it down
			simpleDepthShader.setMat4("model", model);
			house.Draw(simpleDepthShader);
			//BigGround
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, plantexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(32.0f, 32.0f, 32.0f));
			simpleDepthShader.setMat4("model", model);
			renderQuad();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// reset viewport
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			view = glm::translate(view, glm::vec3(0.0f, -1.0f, 0.0f));

			shader.use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			// set light uniforms
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightDir);
			shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.2f, -0.15f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.125f, 0.125f, 0.125f));	// it's a bit too big for our scene, so scale it down
			shader.setMat4("model", model);
			house.Draw(shader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, plantexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(32.0f, 32.0f, 32.0f));
			shader.setMat4("model", model);
			renderQuad();

			ourShader.use();
			ourShader.setVec3("viewPos", camera.Position);
			ourShader.setFloat("shininess", 1.0f);
			//定向光
			ourShader.setVec3("dirlight.direction", -1.0f, -1.0f, -1.0f);
			ourShader.setVec3("dirlight.ambient", 0.4f, 0.2f, 0.2f);
			ourShader.setVec3("dirlight.diffuse", 0.1f, 0.1f, 0.1f);
			ourShader.setVec3("dirlight.specular", 0.1f, 0.1f, 0.1f);
			//点光源
			ourShader.setVec3("pointlight.position", lightPos);
			ourShader.setVec3("pointlight.ambient", 0.2f, 0.2f, 0.05f);
			ourShader.setVec3("pointlight.diffuse", light_str, light_str, light_str);
			ourShader.setVec3("pointlight.specular", 0.3f, 0.3f, 0.3f);
			ourShader.setFloat("pointligh.constant", 1.0f);
			ourShader.setFloat("pointligh.linear", 0.09);
			ourShader.setFloat("pointligh.quadratic", 0.032);

			//绘制模型
			// pass projection matrix to shader (note that in this case it could change every frame
			ourShader.setMat4("projection", projection);
			ourShader.setMat4("view", view);

			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			ourShader.setMat4("model", model);
			//my_model.Draw(ourShader);


			//GHOST
			if (camera.Mode == GAME)
			{
				//debug
				bOver = ghost.Update(deltaTime);
				model = glm::mat4(1.0f);
				model = glm::translate(model, ghost.Position); // translate it down so it's at the center of the scene
				model = glm::rotate(model, glm::radians(ghost.Rotate[0]), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(ghost.Rotate[1]), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));	// it's a bit too big for our scene, so scale it down
				ourShader.setMat4("model", model);
				//cout << ghost.Position[0] <<" " <<ghost.Position[1] <<" " <<ghost.Position[2] << endl;
				//cout << ghost.Rotate[0] << " " << ghost.Rotate[1] << " " << ghost.Rotate[2] << endl;
				ghost.Draw(ourShader);
			}



		

			////结构
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.033f, 0.033f, 0.03f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ourShader.setMat4("model", model);
			fireplace.Draw(ourShader);


			////两边对称的窗户
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(3.95f, 1.5f, 2.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			win.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(3.95f, 1.5f, -2.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			win.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.95f, 1.5f, 2.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			win.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.95f, 1.5f, -2.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			win.Draw(ourShader);

			//家具
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(3.0f, -0.3f, -4.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ourShader.setMat4("model", model);
			bookshelf.Draw(ourShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.3f, 0.0f, 0.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.028f, 0.028f, 0.028f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			cabinet.Draw(ourShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ourShader.setMat4("model", model);
			chair.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.5f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			chair.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(3.5f, 0.0f, 0.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			chest.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.0012f, 0.0012f, 0.0012f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			diningtable.Draw(ourShader);




			////小物件
			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(1.5f, 1.2f, 3.9f)); // translate it down so it's at the center of the scene
			//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
			//model = glm::rotate(model, glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			//ourShader.setMat4("model", model);
			////sword.Draw(ourShader);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.7f, 0.85f, 0.5F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			book.Draw(ourShader);
		
			
			////动画
			////蜡烛
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-0.8f, 0.95f, -0.2F)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("model", model);
			Acandle.Draw(ourShader);
			////金币
			if (camera.Mode == GAME)
			{
				for (int i = 0; i < Acoins.size(); i++)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, Acoins[i].Position); // translate it down so it's at the center of the scene
					model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
					model = glm::rotate(model, glm::radians(Acoins[i].Rotate), glm::vec3(0.0f, 1.0f, 0.0f));
					ourShader.setMat4("model", model);
					Acoins[i].Draw(ourShader);
				}
			}

			//门
			model = glm::mat4(1.0f);
			model = glm::translate(model, Adoor.Position); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.025f, 0.024f, 0.025f));	// it's a bit too big for our scene, so scale it down
			pl6._Position = glm::vec3(0, 0, 4);
			pl6._Normal = glm::vec3(0, 0, -1);
			pl7._Position = glm::vec3(0, 0, 5);
			pl7._Normal = glm::vec3(0, 0, 1);
			ourShader.setMat4("model", model);
			Adoor.Draw(ourShader);


			//Room Box
			//ground
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ceilingtexture);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.1f));
			pl5._Position = glm::vec3(0, 0, 0);
			pl5._Normal = glm::vec3(0, 1, 0);

			ourShader.setMat4("model", model);
			renderQuad();

			////wall
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, walltexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.75f, -4.0F));
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(8.0f, 3.5f, 8.0f));
			pl1._Position = glm::vec3(0, 0, -3);
			pl1._Normal = glm::vec3(0, 0, 1);
			ourShader.setMat4("model", model);
			renderQuad();

			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 1.75f, 4.0F));
			//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(8.0f, 3.5f, 8.0f));
			//ourShader.setMat4("model", model);
			////renderQuad();

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(4.0f, 1.75f, 0.0F));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(8.0f, 3.5f, 8.0f));
			pl3._Position = glm::vec3(3.5, 0, 0);
			pl3._Normal = glm::vec3(-1, 0, 0);
			ourShader.setMat4("model", model);
			renderQuad();

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-4.0f, 1.75f, 0.0F));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(8.0f, 3.5f, 8.0f));
			pl2._Position = glm::vec3(-3, 0, 0);
			pl2._Normal = glm::vec3(1, 0, 0);
			ourShader.setMat4("model", model);
			renderQuad();



			////ceiling
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ceilingtexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 3.4f, 0.2F));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.2f));
			ourShader.setMat4("model", model);
			pl4._Position = glm::vec3(0, 3, 0);
			pl4._Normal = glm::vec3(0, -1, 0);
			renderQuad();

			lightCubeShader.use();
			lightCubeShader.setMat4("projection", projection);
			lightCubeShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.06f)); // a smaller cube
			lightCubeShader.setMat4("model", model);
			renderCube();

			//balls
			if (camera.Mode == ROAM) {
				ballShader.use();
				ballShader.setMat4("projection", projection);
				ballShader.setMat4("view", view);
				model = glm::mat4(1.0f);
				model = glm::translate(model, ArrayPos[0]);
				model = glm::scale(model, glm::vec3(0.1f));
				ballShader.setMat4("model", model);
				renderSphere();

				ballShader.use();
				ballShader.setMat4("projection", projection);
				ballShader.setMat4("view", view);
				model = glm::mat4(1.0f);
				model = glm::translate(model, ArrayPos[1]);
				model = glm::scale(model, glm::vec3(0.1f));
				ballShader.setMat4("model", model);
				renderSphere();

				double rt, rt2, rt4, lamda = 10000;
				glm::vec3 norm, uveloc;
				glm::vec3 normal, point, time;
				double RestTime, BallTime;
				glm::vec3 Pos2;
				int BallNr = 0, dummy = 0, BallColNr1, BallColNr2;
				RestTime = 0.001;
				lamda = 10000;

				for (int i = 0; i < NrOfBalls; i++) {
					tmp.x = ArrayVel[i].x * RestTime;
					tmp.y = ArrayVel[i].y * RestTime;
					tmp.z = ArrayVel[i].z * RestTime;
					ArrayPos[i] = ArrayPos[i] + tmp;
				}

				//While timestep not over
				while (RestTime > ZERO)
				{
					lamda = 10000;   //initialize to very large value

					//For all the balls find closest intersection between balls and planes/cylinders
					for (int i = 0; i < NrOfBalls; i++)
					{
						//compute new position and distance
						OldPos[i] = ArrayPos[i];
						uveloc = ArrayVel[i];
						uveloc = glm::normalize(uveloc);
						tmp.x = ArrayVel[i].x * RestTime;
						tmp.y = ArrayVel[i].y * RestTime;
						tmp.z = ArrayVel[i].z * RestTime;
						ArrayPos[i] = ArrayPos[i] + tmp;
						rt2 = glm::distance(OldPos[i], ArrayPos[i]);
						if (TestIntersionPlane(pl1, OldPos[i], uveloc, rt, norm))
						{
							rt4 = rt * RestTime / rt2;

							if (rt4 <= lamda)
							{

								if (rt4 <= RestTime + ZERO) {

									if (!((rt <= ZERO) && (glm::dot(uveloc, norm) > ZERO)))
									{
										normal = norm;
										tmp.x = uveloc.x * rt;
										tmp.y = uveloc.y * rt;
										tmp.z = uveloc.z * rt;
										point = OldPos[i] + tmp;
										lamda = rt4;
										BallNr = i;
									}
								}
							}
						}
						if (TestIntersionPlane(pl2, OldPos[i], uveloc, rt, norm))
						{
							rt4 = rt * RestTime / rt2;
							if (rt4 <= lamda)
							{

								if (rt4 <= RestTime + ZERO) {
									if (!((rt <= ZERO) && (glm::dot(uveloc, norm) > ZERO)))
									{
										normal = norm;
										tmp.x = uveloc.x * rt;
										tmp.y = uveloc.y * rt;
										tmp.z = uveloc.z * rt;
										point = OldPos[i] + tmp;
										lamda = rt4;
										BallNr = i;
									}
								}
							}
						}
						if (TestIntersionPlane(pl3, OldPos[i], uveloc, rt, norm))
						{   
							rt4 = rt * RestTime / rt2;

							if (rt4 <= lamda)
							{

								if (rt4 <= RestTime + ZERO) {

									if (!((rt <= ZERO) && (glm::dot(uveloc, norm) > ZERO)))
									{
										normal = norm;
										tmp.x = uveloc.x * rt;
										tmp.y = uveloc.y * rt;
										tmp.z = uveloc.z * rt;
										point = OldPos[i] + tmp;
										lamda = rt4;
										BallNr = i;
									}
								}
							}
						}
						if (TestIntersionPlane(pl4, OldPos[i], uveloc, rt, norm))
						{
							rt4 = rt * RestTime / rt2;

							if (rt4 <= lamda)
							{

								if (rt4 <= RestTime + ZERO) {

									if (!((rt <= ZERO) && (glm::dot(uveloc, norm) > ZERO)))
									{
										normal = norm;
										tmp.x = uveloc.x * rt;
										tmp.y = uveloc.y * rt;
										tmp.z = uveloc.z * rt;
										point = OldPos[i] + tmp;
										lamda = rt4;
										BallNr = i;
									}
								}
							}
						}
						if (TestIntersionPlane(pl5, OldPos[i], uveloc, rt, norm))
						{
							rt4 = rt * RestTime / rt2;
							if (rt4 <= lamda)
							{

								if (rt4 <= RestTime + ZERO) {
									if (!((rt <= ZERO) && (glm::dot(uveloc, norm) > ZERO)))
									{
										normal = norm;
										tmp.x = uveloc.x * rt;
										tmp.y = uveloc.y * rt;
										tmp.z = uveloc.z * rt;
										point = OldPos[i] + tmp;
										lamda = rt4;
										BallNr = i;
									}
								}
							}
						}
						if (TestIntersionPlane(pl6, OldPos[i], uveloc, rt, norm))
						{
							rt4 = rt * RestTime / rt2;

							if (rt4 <= lamda)
							{

								if (rt4 <= RestTime + ZERO) {

									if (!((rt <= ZERO) && (glm::dot(uveloc, norm) > ZERO)))
									{
										normal = norm;
										tmp.x = uveloc.x * rt;
										tmp.y = uveloc.y * rt;
										tmp.z = uveloc.z * rt;
										point = OldPos[i] + tmp;
										lamda = rt4;
										BallNr = i;
									}
								}
							}
						}
					}
					//End of tests 
					//If test occured move simulation for the correct timestep
					//and compute response for the colliding ball
					if (lamda != 10000)
					{
						RestTime -= lamda;

						for (int j = 0; j < NrOfBalls; j++) {
							tmp.x = ArrayVel[j].x * lamda;
							tmp.y = ArrayVel[j].y * lamda;
							tmp.z = ArrayVel[j].z * lamda;
							ArrayPos[j] = OldPos[j] + tmp;
						}
						rt2 = glm::length(ArrayVel[BallNr]);//返回速度向量的模
						ArrayVel[BallNr] = glm::normalize(ArrayVel[BallNr]);
						ArrayVel[BallNr] = glm::normalize((normal*(2 * glm::dot(normal, -ArrayVel[BallNr]))) + ArrayVel[BallNr]);
						ArrayVel[BallNr].y *= rt2;
						ArrayVel[BallNr].z *= rt2;
					}
					else RestTime = 0;

				}
			}
			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
		}
				glfwSwapBuffers(window);
				glfwPollEvents();

			}

			// optional: de-allocate all resources once they've outlived their purpose:
			// ------------------------------------------------------------------------
			//glDeleteVertexArrays(1, &VAO);
			//glDeleteBuffers(1, &VBO);

			// glfw: terminate, clearing all previously allocated GLFW resources.
			// ------------------------------------------------------------------
			glfwTerminate();
			return 0;
		}


void grab(void)
{
	FILE* pDummyFile;  //指向另一bmp文件，用于复制它的文件头和信息头数据
	FILE* pWritingFile;  //指向要保存截图的bmp文件
	GLubyte* pPixelData;    //指向新的空的内存，用于保存截图bmp文件数据
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;   //BMP文件数据总长度
	time_t now = time(0);
	char* dt = ctime(&now);
	char pic_name[100] = { "" };
	memcpy(pic_name, dt, strlen(dt) - 1);
	strcat(pic_name, ".bmp");
	dt = pic_name + 4;
	i = 0;
	j = 0;
	while (dt[i] != '\0')
	{
		if (dt[i] != ':')  //只有在不是:的情况下目标才会移动赋值
		{
			dt[j++] = dt[i];
		}
		i++;  //源一直移动
	}
	dt[j] = '\0';


	// 计算像素数据的实际长度
	i = 1200 * 3;   // 得到每一行的像素数据长度
	while (i % 4 != 0)      // 补充数据，直到i是的倍数
		++i;
	PixelDataLength = i * 1200;  //补齐后的总位数

	// 分配内存和打开文件
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);

	pDummyFile = fopen("Monet.bmp", "rb");//只读形式打开
	if (pDummyFile == 0)
		exit(0);



	pWritingFile = fopen(dt, "wb"); //只写形式打开
	if (pWritingFile == 0)
		exit(0);




	//把读入的bmp文件的文件头和信息头数据复制，并修改宽高数据
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);  //读取文件头和信息头，占据54字节
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET); //移动到0X0012处，指向图像宽度所在内存
	i = 1200;
	j = 1200;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 读取当前画板上图像的像素数据
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //设置4位对齐方式
	glReadPixels(0, 0, 1200, 1200, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

	// 写入像素数据
	fseek(pWritingFile, 0, SEEK_END);
	//把完整的BMP文件数据写入pWritingFile
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// 释放内存和关闭文件
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	glm::vec3 uveloc1 = camera.Front;
	glm::vec3 op = camera.Position;
	glm::vec3 uveloc2 = -camera.Right;
	glm::vec3 uveloc3 = camera.Right;
	glm::vec3 uveloc4 = -camera.Front;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

		glm::vec3 norm1;
		double rt1;
		glm::vec3 norm2;
		double rt2;
		glm::vec3 norm3;
		double rt3;
		glm::vec3 norm7;
		double rt7;
		double min=10000;
		if (!isopen&&TestIntersionPlane(pl7, op, uveloc1, rt7, norm7)) {
			if (rt7 / 8 > deltaTime) {
				camera.ProcessKeyboard(FORWARD, deltaTime);
			}
		}
		else {
			for (int i = 1; i <= 3; i++) {
				switch (i) {
				case 1:TestIntersionPlane(pl1, op, uveloc1, rt1, norm1); break;
				case 2:TestIntersionPlane(pl2, op, uveloc1, rt2, norm2); break;
				case 3:TestIntersionPlane(pl3, op, uveloc1, rt3, norm3); break;
				}
			}
			if (rt1 > ZERO&&rt1 <= min) min = rt1;
			if (rt2 > ZERO&&rt2 <= min) min = rt2;
			if (rt3 > ZERO&&rt3 <= min) min = rt3;
			if (min / 8 > deltaTime) {
				camera.ProcessKeyboard(FORWARD, deltaTime);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glm::vec3 norm1;
		double rt1;
		glm::vec3 norm2;
		double rt2;
		glm::vec3 norm3;
		double rt3;
		glm::vec3 norm7;
		double rt7;
		double min = 10000;
		if (!isopen&&TestIntersionPlane(pl7, op, uveloc4, rt7, norm7)) {
			if (rt7 / 8 > deltaTime) {
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			}
		}
		else {
			for (int i = 1; i <= 3; i++) {
				switch (i) {
				case 1:TestIntersionPlane(pl1, op, uveloc4, rt1, norm1); break;
				case 2:TestIntersionPlane(pl2, op, uveloc4, rt2, norm2); break;
				case 3:TestIntersionPlane(pl3, op, uveloc4, rt3, norm3); break;
				}
			}
			if (rt1 > ZERO&&rt1 <= min) min = rt1;
			if (rt2 > ZERO&&rt2 <= min) min = rt2;
			if (rt3 > ZERO&&rt3 <= min) min = rt3;
			if (min / 8 > deltaTime) {
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 norm1;
		double rt1;
		glm::vec3 norm2;
		double rt2;
		glm::vec3 norm3;
		double rt3;
		glm::vec3 norm7;
		double rt7;
		double min = 10000;
		if (!isopen&&TestIntersionPlane(pl7, op, uveloc2, rt7, norm7)) {
			if (rt7 / 8 > deltaTime) {
				camera.ProcessKeyboard(LEFT, deltaTime);
			}
		}
		else {
			for (int i = 1; i <= 3; i++) {
				switch (i) {
				case 1:TestIntersionPlane(pl1, op, uveloc2, rt1, norm1); break;
				case 2:TestIntersionPlane(pl2, op, uveloc2, rt2, norm2); break;
				case 3:TestIntersionPlane(pl3, op, uveloc2, rt3, norm3); break;
				}
			}
			if (rt1 > ZERO&&rt1 <= min) min = rt1;
			if (rt2 > ZERO&&rt2 <= min) min = rt2;
			if (rt3 > ZERO&&rt3 <= min) min = rt3;
			if (min / 8 > deltaTime) {
				camera.ProcessKeyboard(LEFT, deltaTime);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 norm1;
		double rt1;
		glm::vec3 norm2;
		double rt2;
		glm::vec3 norm3;
		double rt3;
		glm::vec3 norm7;
		double rt7;
		double min = 10000;
		if (!isopen&&TestIntersionPlane(pl7, op, uveloc3, rt7, norm7)) {
			if (rt7 / 8 > deltaTime) {
				camera.ProcessKeyboard(RIGHT, deltaTime);
			}
		}
		else {
			for (int i = 1; i <= 3; i++) {
				switch (i) {
				case 1:TestIntersionPlane(pl1, op, uveloc3, rt1, norm1); break;
				case 2:TestIntersionPlane(pl2, op, uveloc3, rt2, norm2); break;
				case 3:TestIntersionPlane(pl3, op, uveloc3, rt3, norm3); break;
				}
			}
			if (rt1 > ZERO&&rt1 <= min) min = rt1;
			if (rt2 > ZERO&&rt2 <= min) min = rt2;
			if (rt3 > ZERO&&rt3 <= min) min = rt3;
			if (min / 8 > deltaTime) {
				camera.ProcessKeyboard(RIGHT, deltaTime);
			}
		}
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPos.x -= 0.03;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPos.x += 0.03;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPos.y += 0.03;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPos.y -= 0.03;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		lightPos.z += 0.03;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		lightPos.z -= 0.03;

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		light_str += 0.1;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		light_str -= 0.1;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		lightDir.z += 0.03;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightDir.z -= 0.03;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		ArrayVel[0] = veloc2;
		ArrayVel[1] = veloc1;
	}
	//截图
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		grab();

	
	//触发按钮
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Adoor.Update(camera.Position);
		
		for (int i = 0; i < Acoins.size(); i++)
		{
			Acoins[i].Update(camera.Position, camera.Front);
			if (Acoins[i].action)
			{
				if (Acoins[i].firstAction)
				{
					winCount++;
				}
			}
			
		}
		if (winCount == Acoins.size())
		{
			bWin = true;
		}
		isopen = true;
	}
}

void idle() {
	
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// process mouse button pressed
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
	if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS)
		camera.ProcessMouseMovement(xoffset, yoffset,LEFTBUTTON);
    else camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO,quadEBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
                 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   4.0f, 4.0f,   // 右上
                 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,   // 右下
                -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
                -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 4.0f    // 左上
        };

        unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glGenBuffers(1, &quadEBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}


//与平面的碰撞检测
int TestIntersionPlane(const Plane& plane, const glm::vec3& position, const glm::vec3& direction, double& lamda, glm::vec3& pNormal)//五个参数分别是平面的结构，camera或ball的当前位置，camera或ball的运动方向，碰撞距离，碰撞法线
{

	double DotProduct = glm::dot(direction, plane._Normal);
	double l2;

	//判断射线是否平行于平面
	if ((DotProduct < ZERO) && (DotProduct > -ZERO))
		return 0;

	l2 = glm::dot(plane._Normal, (plane._Position - position)) / DotProduct; //计算碰撞距离

	if (l2 < -ZERO)
		return 0;

	pNormal = plane._Normal;
	lamda = l2;
	return 1;

}

//球体间的碰撞检测
int FindBallCol(glm::vec3& point, double& TimePoint, double Time2, int& BallNr1, int& BallNr2)
{
	glm::vec3 RelativeV;//相对速度
	TRay rays;
	double MyTime = 0.0, Add = Time2 / 150.0, Timedummy = 10000;
	glm::vec3 posi;

	//Test all balls against eachother in 150 small steps
	for (int i = 0; i < NrOfBalls - 1; i++)
	{
		for (int j = i + 1; j < NrOfBalls; j++)
		{
			RelativeV = ArrayVel[i] - ArrayVel[j];
			RelativeV = glm::normalize(RelativeV);
			rays = TRay(OldPos[i], RelativeV);
			MyTime = 0.0;

			if ((rays.dist(OldPos[j])) > 2) continue;

			while (MyTime < Time2)
			{
				MyTime += Add;
				tmp.x = RelativeV.x * MyTime;
				tmp.y = RelativeV.y * MyTime;
				tmp.z = RelativeV.z * MyTime;
				posi = OldPos[i] + tmp;
				if (glm::distance(posi, OldPos[j]) <= 2) {
					point = posi;
					if (Timedummy > (MyTime - Add)) Timedummy = MyTime - Add;
					BallNr1 = i;
					BallNr2 = j;
					break;
				}

			}
		}

	}

	if (Timedummy != 10000) {
		TimePoint = Timedummy;
		return 1;
	}

	return 0;
}