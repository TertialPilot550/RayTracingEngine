#pragma once

#include "../../../../main.hpp"
#include "../../../../../lib/tiny_obj_loader/tiny_obj_loader.h"

/**
 * @brief Object that represents a .obj model loaded from a file
 * @details Contains code converting this loaded model into a
 * collection of primitives.
 */
class OBJModel {
    public:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    /*
     *  struct mesh_t {
     *      std::vector<index_t> indices;
     *      std::vector<unsigned char> num_face_vertices;
     *      std::vector<int> material_ids;
     *      std::vector<unsigned int> smoothing_group_ids;
     *  }; 
     */

    /**
     * @brief Convert a single shape object to a triangle mesh
     */
    void convert_shape_to_native_object(const tinyobj::shape_t& shape, CollisionList& collision_list) {
        const tinyobj::mesh_t& mesh = shape.mesh;
        auto default_material = std::make_shared<Lambertian>(color(0.8, 0.8, 0.8));

        size_t index_offset = 0;

        for (size_t face = 0;
            face < mesh.num_face_vertices.size();
            ++face) {

            const size_t fv = mesh.num_face_vertices[face];

            if (fv < 3) {
                index_offset += fv;
                continue;
            }

            for (size_t i = 1; i + 1 < fv; ++i) {

                const tinyobj::index_t& ia =
                    mesh.indices[index_offset];

                const tinyobj::index_t& ib =
                    mesh.indices[index_offset + i];

                const tinyobj::index_t& ic =
                    mesh.indices[index_offset + i + 1];

                point a;
                a[P] = 1;
                a[X] = attrib.vertices[3 * ia.vertex_index];
                a[Y] = attrib.vertices[3 * ia.vertex_index + 1];
                a[Z] = attrib.vertices[3 * ia.vertex_index + 2];

                point b;
                b[P] = 1;
                b[X] = attrib.vertices[3 * ib.vertex_index];
                b[Y] = attrib.vertices[3 * ib.vertex_index + 1];
                b[Z] = attrib.vertices[3 * ib.vertex_index + 2];

                point c;
                c[P] = 1;
                c[X] = attrib.vertices[3 * ic.vertex_index];
                c[Y] = attrib.vertices[3 * ic.vertex_index + 1];
                c[Z] = attrib.vertices[3 * ic.vertex_index + 2];

                // std::cout
                //     << "Triangle "
                //     << ia.vertex_index << ", "
                //     << ib.vertex_index << ", "
                //     << ic.vertex_index
                //     << "   "
                //     << a << " | "
                //     << b << " | "
                //     << c
                //     << std::endl;

                auto triangle = std::make_shared<Triangle>(
                    a,
                    b,
                    c,
                    make_surface(default_material)
                );

                collision_list.add(triangle);
            }

            index_offset += fv;
        }
    }

    /**
     * @brief Convert all objects to native objects
     */
    CollisionList convert_to_native_object() {
        CollisionList collision_list;

        // Add each shape to the list
        for (const tinyobj::shape_t& shape : shapes) {
            convert_shape_to_native_object(shape, collision_list);
        }

        return collision_list;
    }

    /**
     * @brief Helper to ensure model is properly loaded into the program.
     */
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

inline bool load_model(OBJModel& m, const char* filepath) {

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