#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "adam_stark_audio_file.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <stack>
#include <random>
#include <fstream>
#include <math.h>

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

    GLuint colorscheme = 0, num_colorschemes = 2; 

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
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        graphics->colorscheme = (graphics->colorscheme + 1) % graphics->num_colorschemes;
        glUniform1i(glGetUniformLocation(graphics->shader_program, "colorscheme"), graphics->colorscheme);
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

    glm::mat4x4 m_mat = glm::rotate(glm::mat4x4(1.0f), (float)-M_PI_2, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(graphics.m_mat_loc, 1, GL_FALSE, glm::value_ptr(m_mat));
    glDrawArrays(GL_TRIANGLES, 0, graphics.num_verts);

    glfwPollEvents();
    glfwSwapBuffers(graphics.window);

	return true;
}

float STFT (std::vector<float> const& input_signal,
		int const n_samples,
		int const chunk_size,
		int const offset,
		float const freq)
{
	float real_prt = 0;
	float img_prt = 0;
	for(int m = 0; m < chunk_size; m++)
	{
		real_prt += input_signal[offset + m] * cos(2 * M_PI * m * freq / n_samples);
		img_prt += input_signal[offset + m] * sin(2 * M_PI * m * freq / n_samples);
	}


	#ifdef DEBUG
	std::cout << "Real part: " << real_prt << std::endl;
	std::cout << "Imaginary part: " << img_prt << std::endl;
	#endif

	return real_prt * real_prt + img_prt * img_prt;
}

unsigned int reverse_binary(unsigned int num, int N)
{
	float d = static_cast<int>(log2(N) + 0.5);
	int ret = 0;
	for(int i=0;i<d;i++)
	{
		ret = (num % 2)+2*ret;
		num = num/2;
	}

	return ret;
}

void fft(std::vector<float>& data_real)
{
	using namespace std;
	int temp_len = data_real.size();
    int len = int(pow(2, ceil(log2(data_real.size()))) + 0.5);
	data_real.resize(len);

	vector<float> data_img(data_real.size());
	for(unsigned int i = 0; i<data_real.size();i++)
	{
		if(reverse_binary(i, data_real.size()) > i)
		{
			float temp = data_real.at(i);
			int rev_i = reverse_binary(i, data_real.size());
			data_real.at(i) = data_real.at(rev_i);
			data_real.at(rev_i) = temp;
		}		
	}

	float wn_re;
	float wn_im;

	unsigned int N = data_real.size();

	for(unsigned int bstep=2; bstep <= N; bstep *= 2)
	{
		for(unsigned int j = 0; j < bstep / 2; j++)
		{
			wn_re = cos(2 * M_PI * j / bstep);
			wn_im = -sin(2 * M_PI * j / bstep);
			
			for(unsigned int hi = j; hi < N; hi += bstep)
			{
				float t1 = wn_re * data_real[hi + bstep/2]
					 - wn_im * data_img[hi + bstep/2];

				float t2 = wn_re * data_img[hi + bstep/2]
					 + wn_im * data_real[hi + bstep/2];


				data_real[hi + bstep / 2] = data_real[hi] - t1;
				data_img[hi + bstep / 2] = data_img[hi] - t2;

				data_real[hi] = data_real[hi] + t1;
				data_img[hi] = data_img[hi] + t2;
			}
		}
	}
	for(unsigned int i = 0; i < data_real.size(); i++)
		data_real[i] = sqrt(data_real[i]*data_real[i] + data_img[i]*data_img[i]);
}

void GenerateTexture (Graphics& graphics) 
{
    // Load in WAV file
    AudioFile<float> audio_file;
    audio_file.load("./voice.wav");
    int channel = 0;
    int num_samples = audio_file.getNumSamplesPerChannel();
    auto& data = audio_file.samples[channel];

    int const window_size = 2048;
    int const window_shift = window_size / 6;

    audio_file.printSummary();

    // Time axis:
    // We use the whole file by calculating so that we have the exact amount of texels necessary. 
    // We want (tex_w-1) * window_shift + window_size = N.
    graphics.tex_w = (data.size() - window_size) / window_shift + 1; 

    graphics.tex_h = window_size;  // Frequency axis
    graphics.tex_data.resize(graphics.tex_h * graphics.tex_w);
    for(size_t i = 0; i < graphics.tex_w; ++i)
    {
        auto chunk = std::vector<float>(data.begin() + window_shift*i, data.begin() + window_shift*i + window_size);
        fft(chunk);
        for(size_t j = 0; j < graphics.tex_h; ++j)
            graphics.tex_data[j * graphics.tex_w + i] = 10 * log(chunk[j]) / log(10);
    }
}

int main ()
{   
    Graphics graphics;
    graphics.Startup(1920, 1080, "./Shaders/vert.glsl", "./Shaders/frag.glsl", "Optimization Stuff"); 

    graphics.mesh = std::vector<float>
    {
        -1,  1, 0,  0, 1, 0,  0, 1, 
        -1, -1, 0,  0, 1, 0,  0, 0,
         1,  1, 0,  0, 1, 0,  1, 1,

        -1, -1, 0,  0, 1, 0,  0, 0, 
         1, -1, 0,  0, 1, 0,  1, 0, 
         1,  1, 0,  0, 1, 0,  1, 1 
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
    GenerateTexture(graphics);

	//Set location of uniforms in shade
    graphics.p_mat_loc = glGetUniformLocation(graphics.shader_program, "pMat");
    graphics.v_mat_loc = glGetUniformLocation(graphics.shader_program, "vMat");
    graphics.m_mat_loc = glGetUniformLocation(graphics.shader_program, "mMat");
	graphics.sampler_loc = glGetUniformLocation(graphics.shader_program, "sampler");

    // Init matrix uniforms 
	glm::mat4x4 pMat = glm::perspective(glm::radians(45.0f),(GLfloat)graphics.width/graphics.height, 0.1f, 100.0f); 
    glm::mat4x4 vMat = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
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

