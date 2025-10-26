#pragma once
#include "mplib/mplib.h"
#include "glad/glad.h"

void mgl_update_attrib(MPMesh mesh, unsigned int attrib_index, size_t size, void* data){
    glad_glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_ids[attrib_index]);
    glad_glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
}
