workspace 'RefCanvas'
    configurations {'Debug', 'Release'}
    location "workspace"
    targetdir "bin"

project 'RefCanvas'
    kind "ConsoleApp"
    language "C++"

    files
    {
        "src/**.**"
    }

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"
    filter {}

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
    filter {}

    includedirs {
        'src',
        'deps/glfw/include',
        'deps/glew/include',
        'deps/glm',
        'deps/stb',
    }

    filter {"system:windows"}
    
        libdirs {
            'deps/glew/lib',
            'deps/glfw/lib-vc2017'
        }

        defines { "GLFW_STATIC", "GLEW_STATIC" }

        links { "glew32s","glew32", "Opengl32", "glfw3" }
        
        defines {'_CRT_SECURE_NO_WARNINGS'}

    filter {}

    filter {'system:not windows'}
    
        libdirs
        {
            'deps/glew/lib',
            'deps/glfw/lib-vc2017'
        }


        defines { "GLFW_STATIC", "GLEW_STATIC" }
        links { "glfw3","GLEW" , "GL",}



