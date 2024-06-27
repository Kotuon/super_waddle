
#ifndef EDITOR_HPP
#define EDITOR_HPP
#pragma once

// std includes
#include <functional>
#include <vector>

class Editor {
public:
    bool Initialize();
    void Update();

    void Render();

    template < typename TCallback >
    inline void AddDisplayMenuCallback( TCallback&& Callback ) {
        display_menu_callbacks.insert( display_menu_callbacks.begin(), Callback );
    }

    static Editor& Instance();

private:
    Editor();

    void Display_Dockspace();
    void Display_Menu_Bar();

    std::vector< std::function< void() > > display_menu_callbacks;

    bool is_open;
};

#endif
