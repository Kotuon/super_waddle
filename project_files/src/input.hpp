
#ifndef INPUT_HPP
#define INPUT_HPP
#pragma once

struct GLFWwindow;

class Input {
public:
    void Update();

    static void CloseWindowCallback( GLFWwindow* Window );

    static Input& Instance();

private:
    Input();
};

#endif
