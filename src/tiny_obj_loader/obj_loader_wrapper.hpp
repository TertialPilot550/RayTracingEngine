#include "../lead.hpp"
#include "tiny_obj_loader.h"

class OBJModel {
    public:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    void convert_to_native_object() {

        

        // TODO






    }

    void display() {

        // 1. Print overall summary
        std::cout << "=== MODEL SUMMARY ===" << std::endl;
        std::cout << "Total Vertices: " << (attrib.vertices.size() / 3) << std::endl;
        std::cout << "Total Normals:  " << (attrib.normals.size() / 3) << std::endl;
        std::cout << "Total UVs:      " << (attrib.texcoords.size() / 2) << std::endl;
        std::cout << "Total Shapes:   " << shapes.size() << std::endl;

        // 2. Print a small sample of vertices (first 5)
        std::cout << "\n=== VERTEX SAMPLE (First 5) ===" << std::endl;
        size_t num_v_to_print = std::min(size_t(5), attrib.vertices.size() / 3);
        for (size_t i = 0; i < num_v_to_print; ++i) {
            std::cout << "V[" << i << "]: " 
                    << attrib.vertices[3 * i + 0] << ", "
                    << attrib.vertices[3 * i + 1] << ", "
                    << attrib.vertices[3 * i + 2] << std::endl;
        }

        // 3. Print face data for the first shape (first 5 faces)
        if (!shapes.empty()) {
            std::cout << "\n=== FACE SAMPLE (Shape 0, First 5 Faces) ===" << std::endl;
            auto& mesh = shapes[0].mesh;
            size_t index_offset = 0;
            size_t num_f_to_print = std::min(size_t(5), mesh.num_face_vertices.size());

            for (size_t f = 0; f < num_f_to_print; f++) {
                size_t fv = size_t(mesh.num_face_vertices[f]);
                std::cout << "F[" << f << "] Vertices: ";
                
                for (size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = mesh.indices[index_offset + v];
                    std::cout << idx.vertex_index << " ";
                }
                std::cout << std::endl;
                index_offset += fv;
            }
        }
        std::cout << "=====================" << std::endl;
    }
};

bool load_model(OBJModel& m, const char* filepath) {

    bool ret = tinyobj::LoadObj(&m.attrib, &m.shapes, &m.materials, &m.warn, &m.err, filepath);

    // 3. Check for errors or warnings
    if (!m.warn.empty()) {
        std::cout << "Warning: " << m.warn << std::endl;
    }
    if (!m.err.empty()) {
        std::cerr << "Error: " << m.err << std::endl;
        return false;
    }
    if (!ret) {
        return false; // Loading failed
    }

    return true;
}