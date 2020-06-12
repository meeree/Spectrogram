#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <stack>
#include <random>

struct Graphics 
{
    std::vector<float> mesh;
    size_t num_verts;
    GLuint vao;
    GLint width, height;
    GLuint shader_program;
    GLuint p_mat_loc, v_mat_loc, m_mat_loc;
    GLFWwindow* window;
	bool done;

    GLuint texture;
	size_t tex_w, tex_h;
    GLuint sampler_loc;
	std::vector<float> tex_data; 

    Graphics () = default;

    GLuint LoadInShader(GLenum const &shaderType, char const *fname) 
	{
       std::vector<char> buffer;
       std::ifstream in;
       in.open(fname, std::ios::binary);

       if (in.is_open()) {
          in.seekg(0, std::ios::end);
          size_t const &length = in.tellg();

          in.seekg(0, std::ios::beg);

          buffer.resize(length + 1);
          in.read(&buffer[0], length);
          in.close();
          buffer[length] = '\0';
       } else {
          std::cerr << "Unable to open " << fname << std::endl;
          exit(-1);
       }

       GLchar const *src = &buffer[0];

       GLuint shader = glCreateShader(shaderType);
       glShaderSource(shader, 1, &src, NULL);
       glCompileShader(shader);
       GLint test;
       glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

       if (!test) {
          std::cerr << "Shader compilation failed with this message:" << std::endl;
          std::vector<char> compilationLog(512);
          glGetShaderInfoLog(shader, compilationLog.size(), NULL, &compilationLog[0]);
          std::cerr << &compilationLog[0] << std::endl;
          glfwTerminate();
          exit(-1);
       }

       return shader;
    }

    void InitShaders (char const* vert_loc, char const* frag_loc)
    {   
        shader_program = glCreateProgram();

        GLuint vertShader = LoadInShader(GL_VERTEX_SHADER, vert_loc);
        GLuint fragShader = LoadInShader(GL_FRAGMENT_SHADER, frag_loc);

        glAttachShader(shader_program, vertShader);
        glAttachShader(shader_program, fragShader);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        glLinkProgram(shader_program);
    }

    void Startup (GLfloat const& width_, GLfloat const& height_, char const* vert_loc, char const* frag_loc, const char* title="Untitled Window")
    {   
		done = false;
        width = width_; 
        height = height_;

        if(!glfwInit()) 
		{
            std::cerr<<"failed to initialize GLFW"<<std::endl;
            exit(EXIT_SUCCESS);
        }

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        window = glfwCreateWindow(width_, height_, title, NULL, NULL);
        if(!window) 
		{
            std::cerr<<"failed to initialize window"<<std::endl;
            exit(EXIT_SUCCESS);
        }
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        if(glewInit() != 0) 
		{
            std::cerr<<"failed to initialize GLEW"<<std::endl;
            exit(EXIT_SUCCESS);
        }

        InitShaders(vert_loc, frag_loc);
    }

	void Shutdown () 
	{   
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(shader_program);
	}
};


void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{  
	//Get graphics pointer (assumed to bound to GLFW "window pointer" associated with var. window)
	Graphics* graphics{(Graphics*)(glfwGetWindowUserPointer(window))};
	assert(graphics);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        graphics->done = true;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		glm::mat4x4 vMat = glm::lookAt(glm::vec3(0.0, 0.0, -7), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
		glUniformMatrix4fv(graphics->v_mat_loc, 1, GL_FALSE, glm::value_ptr(vMat));
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		glm::mat4x4 vMat = glm::lookAt(glm::vec3(3.0, 3.0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
		glUniformMatrix4fv(graphics->v_mat_loc, 1, GL_FALSE, glm::value_ptr(vMat));
	}
}

bool Render (double const& t, Graphics& graphics)
{
	if(graphics.done) 
		return false; 

    GLfloat const color [4] {0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0.0f, color);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUniform1f(2, t);

	//Push texture 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, graphics.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, graphics.tex_w, graphics.tex_w, 0, GL_RED, GL_FLOAT, graphics.tex_data.data()); 
    glUniform1i(graphics.sampler_loc, 0);

	//Shouldn't be necessary but included these lines anyways
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D); 

    glm::mat4x4 m_mat(1.0f);
    glUniformMatrix4fv(graphics.m_mat_loc, 1, GL_FALSE, glm::value_ptr(m_mat));
    glDrawArrays(GL_TRIANGLES, 0, graphics.num_verts);

    glfwPollEvents();
    glfwSwapBuffers(graphics.window);

	return true;
}

int main ()
{   
	Graphics graphics;
    graphics.Startup(1920, 1080, "./Shaders/vert.glsl", "./Shaders/frag.glsl", "Optimization Stuff"); 

    graphics.mesh = std::vector<float>
    {
        -1, 0,  1,  0, 1, 0,  0, 1, 
        -1, 0, -1,  0, 1, 0,  0, 0,
         1, 0,  1,  0, 1, 0,  1, 1,

        -1, 0, -1,  0, 1, 0,  0, 0, 
         1, 0, -1,  0, 1, 0,  1, 0, 
         1, 0,  1,  0, 1, 0,  1, 1 
    };
    graphics.num_verts = 6;

	//RENDERING AND GRAPHICS STUFF BELOW

    glUseProgram(graphics.shader_program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	//Setup buffer and attributes
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
            graphics.mesh.size() * sizeof(glm::vec3),
            graphics.mesh.data(),
            GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), NULL); //Position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid const*)(3*sizeof(GLfloat))); //Normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid const*)(6*sizeof(GLfloat))); //UV coords
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

	//Set GLFW window pointer 
	glfwSetWindowUserPointer(graphics.window, &graphics);

	//Generate texture
    glGenTextures(1, &graphics.texture);
	graphics.tex_w = 400;
	graphics.tex_h = 400; 
	graphics.tex_data.resize(graphics.tex_h * graphics.tex_w);
    for(size_t i = 0; i < graphics.tex_w; ++i)
    {
        for(size_t j = 0; j < graphics.tex_h; ++j)
        {
            float x = 2 * i / (float)graphics.tex_w - 1;
            float y = 2 * j / (float)graphics.tex_h - 1;
            graphics.tex_data[i * graphics.tex_h + j] = fabs(y - sin(10*x));
        }
    }

	//Set location of uniforms in shader
    graphics.p_mat_loc = glGetUniformLocation(graphics.shader_program, "pMat");
    graphics.v_mat_loc = glGetUniformLocation(graphics.shader_program, "vMat");
    graphics.m_mat_loc = glGetUniformLocation(graphics.shader_program, "mMat");
	graphics.sampler_loc = glGetUniformLocation(graphics.shader_program, "sampler");

    // Init matrix uniforms 
	glm::mat4x4 pMat = glm::perspective(glm::radians(45.0f),(GLfloat)graphics.width/graphics.height, 0.1f, 100.0f); 
    glm::mat4x4 vMat = glm::lookAt(glm::vec3(0.0, 10.0, -5.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
    glUniformMatrix4fv(graphics.p_mat_loc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(graphics.v_mat_loc, 1, GL_FALSE, glm::value_ptr(vMat));

	//Set key callback
    glfwSetKeyCallback(graphics.window, KeyCallback);

	//Blending for transparency
    glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

    //These lines prevent seg fault on glTexImage2D
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	//Main loop
	for(float t = glfwGetTime(); ; t = glfwGetTime())
	{
		if(!Render(t, graphics))
			break;
	}

	graphics.Shutdown();
}
