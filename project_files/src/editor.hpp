
#ifndef EDITOR_HPP
#define EDITOR_HPP
#pragma once

class Editor {
public:
    bool Initialize();
    void Update();

    void Render();

    static Editor& Instance();

private:
    Editor();

    void Display_Dockspace();
    void Display_Menu_Bar();

    bool is_open;
};

#endif
