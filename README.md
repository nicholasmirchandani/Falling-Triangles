# Falling-Triangles
A simple 2D OpenGL Simulation made using VS 2017

Recently finished up the OpenGL red book and felt like doing a simple little project.  Will update with video showing it in action.  Should work if loaded up with VS 2017, since dependencies are part of the project, but if all else fails the important ciles are Application.cpp and the 2 shaders.  Only external libs are GLEW, GLFW, and OpenGL.

General idea is that it's a simulation with an arbitrary number of triangles with different colors, directions, and start positions, going off the screen one way or another.  Additionally, space is a slowdown key, while escape is a pause key.

I have more ideas, so I'll get to implementing them to get a better understanding of how to utilize matrix transformations (Scale them down over time and also rotate, potentially based on their horizontal velocities).  Now that I've gone through the dense manual that is the OpenGL red book, and understood the vast majority of it, OpenGL as a language is a lot less intimidating, but I still want to go through https://learnopengl.com/ (because it's much more of a hands on learning, instead of just reading).  Hopefully after their chapters on transformations where they go at least as far as to build a 3D camera I'll be completely comfortable with how transforms work and will pop back and update this.  Lots of polish and tweaking that can be done here too.  This project isn't done just yet :).
