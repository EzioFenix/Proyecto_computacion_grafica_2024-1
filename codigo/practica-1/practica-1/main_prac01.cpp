/*---------------------------------------------------------*/
/* ----------------  Práctica Animacion por cuadros clave -----------*/
/*-----------------    2024-1   ---------------------------*/
/*------------- Alumno: Barrera Peña Víctor Miguel   ---------------*/
/*------------- No. Cuenta   315346219               ---------------*/
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 30.0f, 90.0f));
float MovementSpeed = 0.5f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);

float myVar = 0.0;

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

float rojo = 0.0f,
verde = 0.0f,
azul = 0.0f;
glm::vec3 cambio= glm::vec3(rojo,verde,azul);

int incremento = 0;

bool activarColor = false;

// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
movAuto_y=0.0f,
orienta = 0.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;

//movimiento del carro

int tipoRecorrido = 0;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f,
giroBrazoDer = 0.0f,
giroBrazoIzq = 0.0f,
giroPiernaIzq = 0.0f,

giroCabeza = 0.0,
volar = 0.0;

float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotInc = 0.0f,
giroMonitoInc = 0.0f,
brazoDerInc = 0.0,
brazoIzqInc = 0.0,
piernaIzqInc = 0.0,
giroCabezaInc = 0.0,
volarInc = 0.0;




#define MAX_FRAMES 19
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float giroBrazoDer;
	float giroBrazoIzq;
	float giroPiernaIzq;
	float giroCabeza;
	float volar;


}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 2;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	KeyFrame[FrameIndex].giroBrazoDer = giroBrazoDer;
	KeyFrame[FrameIndex].giroBrazoIzq = giroBrazoIzq;

	KeyFrame[FrameIndex].giroPiernaIzq = giroPiernaIzq;

	KeyFrame[FrameIndex].giroCabeza = giroCabeza;
	KeyFrame[FrameIndex].volar = volar;


	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;

	giroBrazoDer = KeyFrame[0].giroBrazoDer;
	giroBrazoIzq = KeyFrame[0].giroBrazoIzq;

	giroPiernaIzq = KeyFrame[0].giroPiernaIzq;

	giroCabeza = KeyFrame[0].giroCabeza;
	volar = KeyFrame[0].volar;

}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

	brazoDerInc = (KeyFrame[playIndex + 1].giroBrazoDer - KeyFrame[playIndex].giroBrazoDer) / i_max_steps;
	brazoIzqInc = (KeyFrame[playIndex + 1].giroBrazoIzq - KeyFrame[playIndex].giroBrazoIzq) / i_max_steps;

	piernaIzqInc = (KeyFrame[playIndex + 1].giroPiernaIzq - KeyFrame[playIndex].giroPiernaIzq) / i_max_steps;

	giroCabezaInc = (KeyFrame[playIndex + 1].giroCabeza - KeyFrame[playIndex].giroCabeza) / i_max_steps;
	volarInc = (KeyFrame[playIndex + 1].volar - KeyFrame[playIndex].volar) / i_max_steps;


}

void animate(void)
{
	lightPosition.x = 300.0f * sin(myVar);
	lightPosition.x = 300.0f * cos(myVar);
	myVar += 0.01;
	
	
	if (incremento == 1000)
	{
		incremento = 0;
	}

	if (activarColor)
	{
		if (incremento==0)
		{
			cambio = glm::vec3(0.1f, 0.0f, 0.0f);
		}
		if ( incremento== 40)
		{
			cambio = glm::vec3(0.0f, 0.1f, 0.0f);
		}
		if (incremento==80)
		{
			cambio = glm::vec3(0.f, 0.0f, 0.1f);
		}
		if ( incremento == 120)
		{
			cambio = glm::vec3(0.3f, 0.3f, 0.0f);
		}
		if (incremento == 160)
		{
			incremento = 0;
			cambio = glm::vec3(0.0f, 0.3f, 0.3f);
		}
	}
	incremento += 1;
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			giroBrazoDer += brazoDerInc;
			giroBrazoIzq += brazoIzqInc;

			giroPiernaIzq += piernaIzqInc;
			giroCabeza += giroCabezaInc;
			volar += volarInc;


			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		if (tipoRecorrido == 0)
		{
			orienta = 0.0f;
			movAuto_z += 3.0f;

			if (200.0<=movAuto_z )
			{
				tipoRecorrido = 5;
			}
		}
		if (tipoRecorrido == 1)
		{
			orienta = 270.0;
			movAuto_x -= 3.0f;

			if (movAuto_x<=-250.0)
			{
				tipoRecorrido = 2;
				
			}
		}
		if (tipoRecorrido == 2)
		{
			movAuto_z -= 3.0f;
			orienta = 180.0f;

			if (movAuto_z<=-200.0)
			{
				tipoRecorrido = 3;

			}
		}
		if (tipoRecorrido == 3)
		{
			movAuto_x += 3.0f;
			orienta = 90.0f;

			if (0<=movAuto_x)
			{
				tipoRecorrido = 6;

			}
		}
		if (tipoRecorrido == 4)
		{
			movAuto_z += 3.0f;
			orienta = 0.0f;

			if (0 <= movAuto_z)
			{
				animacion = false;
				tipoRecorrido = 0;

			}
		}
		if (tipoRecorrido==5)
		{
			orienta = 212.005;
			movAuto_z -= 1.0f *3;
			movAuto_x -= 0.625f* 3;
			//orienta=
			if (movAuto_x<=-250.0f && movAuto_z<= -200.0f)
			{
				//animacion = false;
				tipoRecorrido = 3;

			}
		}
		if (tipoRecorrido == 6)
		{
			orienta = 36.69;
			movAuto_z += 1.0f * 3;
			movAuto_x += 0.65f * 3;
			//orienta=
			if (65<=movAuto_x && movAuto_z >= -100.0f)
			{
				//animacion = false;
				tipoRecorrido = 7;

			}
		}
		if (tipoRecorrido == 7)
		{
			orienta = -36.69;
			movAuto_z += 1.0f * 3;
			movAuto_x -= 0.65f * 3;
			//orienta=
			if (0 <= movAuto_x && movAuto_z >= 0.0f)
			{
				//animacion = false;
				animacion = false;
				tipoRecorrido = 0;

			}
		}
		
		
	}
}

void getResolution()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC 20241", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model botaDer("resources/objects/Personaje/bota.obj");
	Model piernaDer("resources/objects/Personaje/piernader.obj");
	Model piernaIzq("resources/objects/Personaje/piernader.obj");
	Model torso("resources/objects/Personaje/torso.obj");
	Model brazoDer("resources/objects/Personaje/brazoder.obj");
	Model brazoIzq("resources/objects/Personaje/brazoizq.obj");
	Model cabeza("resources/objects/Personaje/cabeza.obj");
	//Model carro("resources/objects/lambo/carroceria.obj");
	Model llanta("resources/objects/lambo/Wheel.obj");
	//Model tapete("resources/objects/tapete/10404_Circular_Throw_Rug_v1_max2010_iterations-2.obj");
	Model museo("resources/objects/museo/museo.obj");

	//nuevos modelos
	Model mariposa("resources/objects/mariposa/uploads_files_2618445_butterfly_free.obj");
	Model tigre_amarillo("resources/objects/tigre/Tigre_amarillo.obj");
	Model tigre_blanco("resources/objects/tigre/Tigre_blanco.obj");
	Model lobo("resources/objects/lobo/lobo.obj");
	Model elefante("resources/objects/elefante/elefante.obj");


	


	ModelAnim animacionPersonaje("resources/objects/Personaje1/PersonajeBrazo.dae");
	animacionPersonaje.initShaders(animShader.ID);



	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].giroBrazoDer = 0;
		KeyFrame[i].giroBrazoIzq = 0;
		KeyFrame[i].giroPiernaIzq = 0;
		KeyFrame[i].giroCabeza = 0;
		KeyFrame[i].volar = 0;
	}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	KeyFrame[0].posX = 0.0f;
	KeyFrame[0].posY = 0.0f;
	KeyFrame[0].posZ = 0.0f;
	KeyFrame[0].rotRodIzq = 0.0f;
	KeyFrame[0].giroMonito = 0.0f;
	KeyFrame[0].giroBrazoDer = 0.0f;

	KeyFrame[1].posX = 0.0f;
	KeyFrame[1].posY = 25.0f;
	KeyFrame[1].posZ = 0.0f;
	KeyFrame[1].rotRodIzq = 30.0f;
	KeyFrame[1].giroMonito = 0.0f;
	KeyFrame[1].giroBrazoDer = -90.0f;

	KeyFrame[2].posX = 40.0f;
	KeyFrame[2].posY = 0.0f;
	KeyFrame[2].posZ = 0.0f;
	KeyFrame[2].rotRodIzq = -30.0f;
	KeyFrame[2].giroMonito = 90.0f;
	KeyFrame[2].giroBrazoDer = 0.0f;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		//staticShader.setVec3("dirLight.ambient", ambientColor);
		//staticShader.setVec3("dirLight.diffuse", diffuseColor);
		//staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setVec3("dirLight.ambient", glm::vec3(1.0f,1.0f,1.0f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", cambio);
		staticShader.setVec3("pointLight[1].diffuse", cambio);
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 0.1f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.0032f);


		staticShader.setVec3("pointLight[2].position", glm::vec3(-160.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 2.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 0.1f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.0032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);


		model = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.2f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		//animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		//ninja.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		

		// tapete
		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 100.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotación de 90 grados alrededor del eje -x
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		tapete.Draw(staticShader);*/

		// tapete
		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 100.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotación de 90 grados alrededor del eje -x
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		tapete.Draw(staticShader);*/



		//piso
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		model = glm::translate(model, glm::vec3(0.0f, 7.05f, 0.0f));
		//model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotación de 90 grados alrededor del eje -x
		staticShader.setMat4("model", model);
		museo.Draw(staticShader);

		//mariposa
		model = glm::translate(model, glm::vec3(0.0f,1.3f, 10.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación de 90 grados alrededor del eje -x
		staticShader.setMat4("model", model);
		mariposa.Draw(staticShader);

		//Tigre amarillo
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación de 90 grados alrededor del eje -x
		model = glm::translate(model, glm::vec3(-15.0f, -1.0f, -15.0f));
		staticShader.setMat4("model", model);
		tigre_amarillo.Draw(staticShader);
		
		//Tigre blanco
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -15.0f));
		staticShader.setMat4("model", model);
		tigre_blanco.Draw(staticShader);

		//lobo
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -15.0f));
		model = glm::scale(model, glm::vec3(4.0f));
		staticShader.setMat4("model", model);
		lobo.Draw(staticShader);
		model = glm::scale(model, glm::vec3(0.25f));

		//elefante
		model = glm::translate(model, glm::vec3(+15.0f, 0.0f, -15.0f));
		staticShader.setMat4("model", model);
		elefante.Draw(staticShader);




		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		model = glm::translate(model, glm::vec3(posX, posY + volar, posZ));
		tmp = model = glm::rotate(model, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		torso.Draw(staticShader);*/

		//Pierna Der
		/*model = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaDer.Draw(staticShader);*/

		//Pie Der
		/*model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		botaDer.Draw(staticShader);*/

		//Pierna Izq
		/*model = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(-giroPiernaIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaIzq.Draw(staticShader);*/

		//Pie Iz
		/*model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		botaDer.Draw(staticShader);	//Izq trase*/

		//Brazo derecho
		/*model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(giroBrazoDer), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		brazoDer.Draw(staticShader);*/

		//Brazo izquierdo
		/*model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(giroBrazoIzq), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		brazoIzq.Draw(staticShader);*/

		//Cabeza
		/*model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giroCabeza), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", model);
		cabeza.Draw(staticShader);*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Caja Transparente --- Siguiente Práctica
		// -------------------------------------------------------------------------------------------------------------------------
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	//if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	//	lightPosition.x--;

	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		giroBrazoDer++;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		giroBrazoDer--;

	// brazo izquierdo
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		giroBrazoIzq++;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		giroBrazoIzq--;

	// pierna izquierdo
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		giroPiernaIzq++;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		giroPiernaIzq--;

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		giroCabeza++;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		giroCabeza--;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		volar++;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		volar--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		movAuto_z = 0.0;
		movAuto_y = 0.0;
		animacion = false;
		tipoRecorrido = 0;
	}

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		if (activarColor == false)	
		{
			cambio = glm::vec3(1.0f, 1.0f, 1.0f);
			activarColor = true;
		}
	}
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}