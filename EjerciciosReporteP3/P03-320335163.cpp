//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<map>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
#include<gtc\quaternion.hpp>
#include<gtx\quaternion.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
// Ejemplo: en tu archivo principal antes de incluir GLM

//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
std::map<std::string, Mesh*> figures;

//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
	figures["cubo"] = cubo;
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
	figures["piramideTriangular"] = obj1;
}
/*
Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
	figures["cilindro"] = cilindro;
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
	figures["cono"] = cono;
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
	figures["piramideCuadrangular"] = piramide;
}

MeshColor* CrearTetraedo() {


	GLfloat vertices_tetaedro[] = {
		// Cara 1 (Rojo)
	-0.5f, -0.2887f,  0.0f,   1.0f, 0.0f, 0.0f, // 0
	 0.5f, -0.2887f,  0.0f,   1.0f, 0.0f, 0.0f, // 1
	 0.0f,  0.5773f, -0.2887f,1.0f, 0.0f, 0.0f, // 2

	 // Cara 2 (Verde)
	  0.5f, -0.2887f,  0.0f,   0.0f, 1.0f, 0.0f, // 1
	  0.0f, -0.2887f, -0.866f, 0.0f, 1.0f, 0.0f, // 3
	  0.0f,  0.5773f, -0.2887f,0.0f, 1.0f, 0.0f, // 2

	  // Cara 3 (Azul)
	   0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 1.0f, // 3
	  -0.5f, -0.2887f,  0.0f,   0.0f, 0.0f, 1.0f, // 0
	   0.0f,  0.5773f, -0.2887f,0.0f, 0.0f, 1.0f, // 2

	   // Cara 4 (Amarillo)
		0.0f, -0.2887f, -0.866f, 1.0f, 1.0f, 0.0f, // 3
		0.5f, -0.2887f,  0.0f,   1.0f, 1.0f, 0.0f, // 1
	   -0.5f, -0.2887f,  0.0f,   1.0f, 1.0f, 0.0f  // 0
	};
	MeshColor* obj1 = new MeshColor();
	obj1->CreateMeshColor(vertices_tetaedro, 72);
	return obj1;
	
}

MeshColor* CrearBlackTetraedo() {


	GLfloat vertices_tetaedro[] = {
		
	-0.5f, -0.2887f,  0.0f,   0.0f, 0.0f, 0.0f,
	 0.5f, -0.2887f,  0.0f,   0.0f, 0.0f, 0.0f,
	 0.0f,  0.5773f, -0.2887f,0.0f, 0.0f, 0.0f,

	 0.5f, -0.2887f,  0.0f,   0.0f, 0.0f, 0.0f,
	 0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,
	 0.0f,  0.5773f, -0.2887f,0.0f, 0.0f, 0.0f,

	 0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,
	-0.5f, -0.2887f,  0.0f,   0.0f, 0.0f, 0.0f,
	 0.0f,  0.5773f, -0.2887f,0.0f, 0.0f, 0.0f,

	 0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.2887f,  0.0f,   0.0f, 0.0f, 0.0f,
	-0.5f, -0.2887f,  0.0f,   0.0f,
	};
	MeshColor* obj1 = new MeshColor();
	obj1->CreateMeshColor(vertices_tetaedro, 72);
	return obj1;

}

MeshColor* CrearTetraedoDown() {
	GLfloat vertices_tetaedro[] = {
		// Cara 1 (Rojo)
		-0.5f,  0.5773f,  0.0f,   1.0f, 0.0f, 0.0f, // 0
		 0.5f,  0.5773f,  0.0f,   1.0f, 0.0f, 0.0f, // 1
		 0.0f, -0.2887f, -0.2887f,1.0f, 0.0f, 0.0f, // 2 

		 // Cara 2 (Verde)
		 0.5f,  0.5773f,  0.0f,   0.0f, 1.0f, 0.0f, // 1 
		 0.0f,  0.5773f, -0.866f, 0.0f, 1.0f, 0.0f, // 3 
		 0.0f, -0.2887f, -0.2887f,0.0f, 1.0f, 0.0f, // 2 

		 // Cara 3 (Azul)
		 0.0f,  0.5773f, -0.866f, 0.0f, 0.0f, 1.0f, // 3
		-0.5f,  0.5773f,  0.0f,   0.0f, 0.0f, 1.0f, // 0 
		 0.0f, -0.2887f, -0.2887f,0.0f, 0.0f, 1.0f, // 2 

		 // Cara 4 (Amarillo)
		 0.0f,  0.5773f, -0.866f, 1.0f, 1.0f, 0.0f, // 3
		 0.5f,  0.5773f,  0.0f,   1.0f, 1.0f, 0.0f, // 1 
		-0.5f,  0.5773f,  0.0f,   1.0f, 1.0f, 0.0f  // 0 
	};
	MeshColor* obj1 = new MeshColor();
	obj1->CreateMeshColor(vertices_tetaedro, 72);
	return obj1;
}

MeshColor* CreateContorno() {
	float lineVertices[] = {
		
		// (1,2)
		-0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,
		0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,

		// (2,3)
		0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,
		0.0f,  0.5773f, -0.2887f, 0.0f, 0.0f, 0.0f,

		// (3,4)
		0.0f,  0.5773f, -0.2887f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,

		// (4,5)
		0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,

		// (5,6)
		0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,
		0.0f,  0.5773f, -0.2887f, 0.0f, 0.0f, 0.0f,

		// (6,7)
		0.0f,  0.5773f, -0.2887f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,

		// (7,8)
		0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,

		// (8,9)
		-0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,
		0.0f,  0.5773f, -0.2887f, 0.0f, 0.0f, 0.0f,

		// (9,10)
		0.0f,  0.5773f, -0.2887f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,

	// (10,11)
		0.0f, -0.2887f, -0.866f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,

		// (11,12)
		0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.2887f,  0.0f,  0.0f, 0.0f, 0.0f
				 
	};
	MeshColor* obj1 = new MeshColor();
	obj1->CreateMeshColor(lineVertices, 132);
	return obj1;
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

void drawfigure(glm::mat4& projection, Window& mainWindow, Camera& camera, glm::mat4 &model, MeshColor* tet) {
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
    // Cambia la declaración de tet para que no sea const

	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	
	shaderList[1].useShader();
	uniformModel = shaderList[1].getModelLocation();
	uniformProjection = shaderList[1].getProjectLocation();
	uniformView = shaderList[1].getViewLocation();
	uniformColor = shaderList[1].getColorLocation();

	//model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
	//model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
	//se programe cambio entre proyección ortogonal y perspectiva
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
	tet->RenderMeshColor();

}

void drawlines(glm::mat4& projection, Window& mainWindow, Camera& camera, glm::mat4& model, MeshColor* tet) {
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	// Cambia la declaración de tet para que no sea const

	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	shaderList[1].useShader();
	uniformModel = shaderList[1].getModelLocation();
	uniformProjection = shaderList[1].getProjectLocation();
	uniformView = shaderList[1].getViewLocation();
	uniformColor = shaderList[1].getColorLocation();

	//model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
	//model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
	//se programe cambio entre proyección ortogonal y perspectiva
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
	tet->RenderMeshColorLine();

}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa
	MeshColor* tet = CrearTetraedo();
	MeshColor* tetdown = CrearTetraedoDown();
	MeshColor* blacktet = CrearBlackTetraedo();
	MeshColor* contorno = CreateContorno();
	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(5, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);
	//por convencion up es z+,arriba

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader
	glm::quat rotx = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
	glm::quat roty = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, 1, 0));
	glm::quat rotz = glm::angleAxis(glm::radians(00.0f), glm::vec3(0, 0, 1));
	glm::quat rot = rotx * roty * rotz;
	glm::mat4 RotationM = glm::mat4(1.0f);
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad

		float scale = 0.5f;
		glm::vec3 universal_scale =glm::vec3(scale, scale, scale);
		glm::vec3 universal_translate = glm::vec3(0.0, 0.0f, -3.0f);
		float gap = 0.08f*scale;
		
		//enormous pyramid
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::translate(model, glm::vec3(0.0f, 0.298f, -0.04f));
		model = glm::scale(model, glm::vec3(1.5f,1.52f,1.5f));
		drawfigure(projection, mainWindow, camera, model, blacktet);
		//lines
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::translate(model, glm::vec3(0.0f, 0.298f, -0.04f));
		model = glm::scale(model, glm::vec3(1.52f, 1.6f, 1.5f));
		//drawlines(projection, mainWindow, camera, model, contorno);
		//first layer 
		//up front
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate)-glm::vec3(0.0f,0.0f,0.029f));
		model = glm::scale(model, universal_scale);
		drawfigure(projection, mainWindow, camera, model,tet);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate) - glm::vec3(0.0f, 0.0f, 0.029f));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(1.0f+2*gap, 0.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model,tet);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate) - glm::vec3(0.0f, 0.0f, 0.029f));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-1.0f-2*gap, 0.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model,tet);
		//up lateral derecha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, -0.95f-gap));
		drawfigure(projection, mainWindow, camera, model,tet);
		//up lateral izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.95f - gap));
		drawfigure(projection, mainWindow, camera, model, tet);
		//down front
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.49f+gap, 0.1f+gap, -0.01f));
		model = glm::rotate(model, glm::radians((GLfloat)-37.0f), glm::vec3(1.0f, 0.0f,0.0f));
		drawfigure(projection, mainWindow, camera, model, tetdown);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.5f - gap, 0.1f + gap, -0.01f));
		model = glm::rotate(model, glm::radians((GLfloat)-37.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model, tetdown);

		//down lateral derecha

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.87f + gap,  -0.18f+ gap, -0.53f-gap-0.025f));
		model = glm::rotate(model, glm::radians(18.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		rotx= glm::angleAxis(glm::radians(-1.0f), glm::vec3(1, 0, 0));
		roty = glm::angleAxis(glm::radians(7.0f), glm::vec3(0, 1, 0));
		rotz = glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 0, 1));
		rot = rotx*roty*rotz;
		RotationM = glm::toMat4(rot);

		model = model*glm::toMat4(rot);
		drawfigure(projection, mainWindow, camera, model, tetdown);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.33f + gap, -0.18f + gap, -1.49f - gap));
		model = glm::rotate(model, glm::radians(18.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		rotx = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
		roty = glm::angleAxis(glm::radians(7.0f), glm::vec3(0, 1, 0));
		rotz = glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 0, 1));
		rot = rotx * roty * rotz;
		RotationM = glm::toMat4(rot);

		model = model * glm::toMat4(rot);
		drawfigure(projection, mainWindow, camera, model, tetdown);

		//down lateral izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.93f + gap, -0.16f + gap, -0.51f - gap-0.05f));
		model = glm::rotate(model, glm::radians(18.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		rotx = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
		roty = glm::angleAxis(glm::radians(-7.0f), glm::vec3(0, 1, 0));
		rotz = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0, 0, 1));
		rot = rotx * roty * rotz;
		RotationM = glm::toMat4(rot);
		model = model * glm::toMat4(rot);
		drawfigure(projection, mainWindow, camera, model, tetdown);


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.42f + gap, -0.16f + gap, -1.5f - gap+0.01f));
		model = glm::rotate(model, glm::radians(18.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		rotx = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
		roty = glm::angleAxis(glm::radians(-7.0f), glm::vec3(0, 1, 0));
		rotz = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0, 0, 1));
		rot = rotx * roty * rotz;
		RotationM = glm::toMat4(rot);

		model = model * glm::toMat4(rot);
		drawfigure(projection, mainWindow, camera, model, tetdown);


		//up back
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.01f,0.0f,-1.75f-4*gap-0.05f));
		drawfigure(projection, mainWindow, camera, model, tet);

		//second layer
		float back_layer = -0.3f;

		//up front
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.5f+gap, 0.866f+2*gap, back_layer-0.08f));
		drawfigure(projection, mainWindow, camera, model, tet);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.5f-gap, 0.866f + 2*gap, back_layer-0.08f));
		drawfigure(projection, mainWindow, camera, model, tet);


		//down front
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.0, 1.0f + 2*gap, back_layer-0.025f));
		model = glm::rotate(model, glm::radians((GLfloat)-37.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model, tetdown);

		//up lateral derecha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.0f, 0.95f, -1.55f-back_layer-gap));
		drawfigure(projection, mainWindow, camera, model, tet);

		//up lateral izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.00f, 0.95f, -1.55f - back_layer - gap));
		drawfigure(projection, mainWindow, camera, model, tet);

		//down lateral derecha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.35f + gap, -0.18f + gap+0.95f, -0.85f - gap-0.01f));
		model = glm::rotate(model, glm::radians(18.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		rotx = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
		roty = glm::angleAxis(glm::radians(7.0f), glm::vec3(0, 1, 0));
		rotz = glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 0, 1));
		rot = rotx * roty * rotz;
		RotationM = glm::toMat4(rot);

		model = model * glm::toMat4(rot);
		drawfigure(projection, mainWindow, camera, model, tetdown);
		//down lateral izquierda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.43f + gap, -0.16f + gap+0.95f, -0.84f - gap-0.02f));
		model = glm::rotate(model, glm::radians(18.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		rotx = glm::angleAxis(glm::radians(0.0f), glm::vec3(1, 0, 0));
		roty = glm::angleAxis(glm::radians(-7.0f), glm::vec3(0, 1, 0));
		rotz = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0, 0, 1));
		rot = rotx * roty * rotz;
		RotationM = glm::toMat4(rot);

		model = model * glm::toMat4(rot);
		drawfigure(projection, mainWindow, camera, model, tetdown);


		//third layer

		//up head
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.0f, 1.76f+3*gap, 2*back_layer-gap-0.052f));
		drawfigure(projection, mainWindow, camera, model, tet);

		//base

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.8f, 0.005f, -0.5f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model, tet);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(-0.3f, 0.005f, -0.5f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model, tet);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(universal_translate));
		model = glm::scale(model, universal_scale);
		model = glm::translate(model, glm::vec3(0.25f, 0.005f, -1.5f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		drawfigure(projection, mainWindow, camera, model, tet);

		glUseProgram(0);
		mainWindow.swapBuffers();
		
	}
	return 0;
}
