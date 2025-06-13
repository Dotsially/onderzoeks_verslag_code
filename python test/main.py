from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from pyglm import glm
import numpy as np
import time

class RenderData:
    def __init__(self, vao, vbo, ebo):
        self.vao = vao
        self.vbo = vbo
        self.ebo = ebo

class ProfilingData:
    count : int = 0 
    def __init__(self, min, average, max):
        self.min = min
        self.average = average
        self.max = max

class Shader:
    program_id : int = 0
    def __init__(self, vertex_path, fragment_path):
        with open(vertex_path, 'r') as vertex_file:
            vertex_string = vertex_file.read()
        with open(fragment_path, 'r') as fragment_file:
            fragment_string = fragment_file.read()

        vertex_shader = glCreateShader(GL_VERTEX_SHADER)
        glShaderSource(vertex_shader, [vertex_string.encode('UTF-8')])
        glCompileShader(vertex_shader)
        
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER)
        glShaderSource(fragment_shader, [fragment_string.encode('UTF-8')])
        glCompileShader(fragment_shader)

        self.program_id = glCreateProgram()
        glAttachShader(self.program_id, vertex_shader)
        glAttachShader(self.program_id, fragment_shader)
        glLinkProgram(self.program_id)

        glDeleteShader(vertex_shader)
        glDeleteShader(fragment_shader)
    
    def use(self):
        glUseProgram(self.program_id)

class Camera:
    def __init__(self):
        self.view = glm.translate(glm.mat4(1.0), glm.vec3(0, 0, -3))
        self.projection = glm.perspective(glm.radians(90.0), 1280.0/720.0, 0.1, 100.0)
    
    def get_view_projection_matrix(self):
        return self.projection * self.view 

profiling_data = ProfilingData(1, 0, 0)
render_data = RenderData(0, 0, 0)


def initialize_glut(width, height, title):
    glutInit()
    glutInitDisplayMode(GLUT_RGBA)
    glutInitWindowSize(width, height)
    glutInitWindowPosition(0,0)
    glutInitContextVersion(4, 6)
    window = glutCreateWindow(title.encode('utf-8'))
    return window

def render():
    global render_data, profiling_data
    start_time = time.perf_counter()  # Start timing

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glClearColor(0.2, 0.2, 0.2, 1.0)

    transform = glm.mat4(1.0)
    transform = glm.rotate(transform, glm.radians(time.time()*100 % 360), glm.vec3(0, 1, 0))
    glUniformMatrix4fv(1, 1, GL_FALSE, glm.value_ptr(transform))

    glBindVertexArray(render_data.vao)
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, None)

    glutSwapBuffers()

    end_time = time.perf_counter()  # End timing
    elapsed_ms = (end_time - start_time) * 1000  # Convert to ms

    if elapsed_ms < profiling_data.min:
        profiling_data.min = elapsed_ms
    if elapsed_ms > profiling_data.max:
        profiling_data.max = elapsed_ms

    profiling_data.average[profiling_data.count] = elapsed_ms
    profiling_data.count += 1
    profiling_data.count %= 10

    profiling_sum = 0
    for i in profiling_data.average:
        profiling_sum += i
    profiling_average = profiling_sum/10

    print(f"Render Time: {elapsed_ms:.3f} ms \n Min: {profiling_data.min:.3f} ms\n Max: {profiling_data.max:.3f} ms\n Average: {profiling_average:.3f} ms")

def main():
    global render_data, profiling_data

    window_pointer = initialize_glut(1280, 720, "Cube Test")

    glEnable(GL_DEPTH_TEST)


    camera : Camera = Camera()

    cube_shader : Shader = Shader('cube_vertex.glsl', 'cube_fragment.glsl')
    cube_shader.use()

    viewProjectionMatrix = camera.get_view_projection_matrix()
    glUniformMatrix4fv(0, 1, GL_FALSE, glm.value_ptr(viewProjectionMatrix))
    
    cube_vertices = np.zeros((8,3), dtype=np.float32)
    cube_vertices[...] = (-1,-1,+1),(-1,+1,+1),(+1,+1,+1),(+1,-1,+1),(-1,-1,-1),(-1,1,-1),(+1,+1,-1),(+1,-1,-1)
    cube_indices = np.zeros(24, dtype=np.uint32)
    cube_indices[...] = 0, 1, 2, 0, 2, 3, 4, 5, 1, 4, 1, 0, 7, 6, 5, 7, 5, 4, 3, 2, 6, 3, 6, 7

    vao = glGenVertexArrays(1)
    glBindVertexArray(vao)

    vbo = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, cube_vertices.nbytes, cube_vertices, GL_STATIC_DRAW)

    ebo = glGenBuffers(1)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_indices.nbytes, cube_indices, GL_STATIC_DRAW)

    glVertexAttribPointer(0, 3, GL_FLOAT, False, 0, None)
    glEnableVertexAttribArray(0)

    render_data.vao = vao
    render_data.vbo = vbo
    render_data.ebo = ebo

    average_buffer = np.zeros(10, dtype=np.float32)
    profiling_data.average = average_buffer

    glutDisplayFunc(render)
    glutIdleFunc(render)
    glutMainLoop()

if __name__ == '__main__':
    main()