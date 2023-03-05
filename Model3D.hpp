#ifndef Model3D_hpp
#define Model3D_hpp

#include "Mesh.hpp"

#include "tiny_obj_loader.h"
#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>

namespace gps {

    class Model3D
    {

    public:
        ~Model3D();

		void LoadModel(std::string fileName);

		void LoadModel(std::string fileName, std::string basePath);

		void Draw(gps::Shader shaderProgram);

        const Vertex &getColisiTest() const;

        void setColisiTest(const Vertex &colisiTest);

        void setX(float x);

        float getX() const;

        float getY() const;

        void setY(float y);

        float getZ() const;

        void setZ(float z);

        auto CalculateAxisAlignedBox();

        glm::vec3 CalculateAxismin();

        glm::vec3 CalculateAxisMax();

    private:
		// Component meshes - group of objects
        std::vector<gps::Mesh> meshes;
		// Associated textures
        std::vector<gps::Texture> loadedTextures;

		// Does the parsing of the .obj file and fills in the data structure
		void ReadOBJ(std::string fileName, std::string basePath);

		// Retrieves a texture associated with the object - by its name and type
		gps::Texture LoadTexture(std::string path, std::string type);

		// Reads the pixel data from an image file and loads it into the video memory
		GLuint ReadTextureFromFile(const char* file_name);

        //colisiune
        float x;
        float y;
        float z;

//        struct AABB
//        {
//        };

    };
}

#endif /* Model3D_hpp */
