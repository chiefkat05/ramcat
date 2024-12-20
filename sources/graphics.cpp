#include "../headers/graphics.h"

std::string readFileToString(std::string path)
{
    std::ifstream shader(path);

    if (!shader.is_open())
    {
        std::cerr << "\n\tvertex shader at " << path << " failed to open!\n";
    }

    std::string output, line;
    while (std::getline(shader, line))
    {
        output += line + "\n";

        if (shader.eof())
            break;
    }

    shader.close();
    return output;
}

void shader::use()
{
    glUseProgram(ID);
}
shader::shader(const char *vPath, const char *fPath)
{
    int success;
    char infolog[512];

    std::string vShaderStr = readFileToString(vPath);
    std::string fShaderStr = readFileToString(fPath);
    const char *vShaderSource = vShaderStr.c_str();
    const char *fShaderSource = fShaderStr.c_str();

    unsigned int vertexShader;
    unsigned int fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
        std::cerr << "\n\tvertex shader failed to compile with error: " << infolog << "\n";
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
        std::cerr << "\n\tfragment shader failed to compile with error: " << infolog << "\n";
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infolog);
        std::cerr << "\n\tshader program failed to link with error: " << infolog << "\n";
    }

    use();
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void shader::setUniformVec4(const char *name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}
void shader::setUniformVec3(const char *name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void shader::setUniformVec2(const char *name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}
void shader::setUniformFloat(const char *name, float x)
{
    glUniform1f(glGetUniformLocation(ID, name), x);
}
void shader::setUniformInt(const char *name, int x)
{
    glUniform1i(glGetUniformLocation(ID, name), x);
}
void shader::setUniformBool(const char *name, bool x)
{
    glUniform1i(glGetUniformLocation(ID, name), x);
}
void shader::setUniformMat4(const char *name, glm::mat4 matrix)
{
    unsigned int transLoc = glGetUniformLocation(ID, name);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}