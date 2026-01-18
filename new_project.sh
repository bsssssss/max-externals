#!/usr/bin/env bash

SOURCE_DIR="./source"

# Prompt for a category
read -p "category: " category
category_path="$SOURCE_DIR/$category"

if [ -d $category_path ]; then
    echo "$category_path already exists, but that's okay"
else 
    mkdir $category_path
    echo "created $category_path"
fi


# Prompt for project name

read -p "project name: " _project_name
project_name=$(echo "$_project_name"| sed 's/[^a-zA-Z0-9_]/_/g')
project_path="$category_path/$project_name"

if [ -d $project_path ]; then
    echo "error: project $project_name already exists."
    exit 1
else 
    mkdir $project_path
    echo "created $project_path"
fi

echo "creating cmake file from template..."
cat > "$project_path/CMakeLists.txt" << "EOF"
include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################

include_directories( 
	"${MAX_SDK_INCLUDES}"
	"${MAX_SDK_MSP_INCLUDES}"
	"${MAX_SDK_JIT_INCLUDES}"
)

file(GLOB PROJECT_SRC
	"*.h"
	"*.c"
	"*.cpp"
)
add_library( 
	${PROJECT_NAME} 
	MODULE
	${PROJECT_SRC}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
EOF

echo "creating c file from template..."
cat > "$project_path/$project_name.c" << EOF
/**
        @file ${project_name} - A max object
        @ingroup ${category}
*/

#include "ext.h"      // standard Max include, always required
#include "ext_obex.h" // required for new style Max object

// structure de l'objet max
typedef struct _${project_name}
{
    t_object obj; // L'objet lui meme (doit etre en premier)
    void    *m_outlet1;
} t_${project_name};

// déclaration des fonctions
void *${project_name}_new(void);
void  ${project_name}_bang(t_${project_name} *x);

// pointeur vers la classe
static t_class *s_${project_name}_class;

// fonction d'initialisation (quand on ouvre max)
void ext_main(void *r)
{
    t_class *c;
    c = class_new("${project_name}",
                  (method)${project_name}_new,
                  (method)NULL,
                  sizeof(t_${project_name}),
                  0L,
                  0);

    class_addmethod(c, (method)${project_name}_bang, "bang", 0);

    class_register(CLASS_BOX, c);
    s_${project_name}_class = c;
}

// routine d'instanciation
void *${project_name}_new()
{
    t_${project_name} *x = (t_${project_name} *)object_alloc(s_${project_name}_class);
    x->m_outlet1    = outlet_new((t_object *)x, NULL);
    object_post((t_object *)x, "Bang me");

    return x;
}

// faire quelque chose avec les messages
void ${project_name}_bang(t_${project_name} *x)
{
    outlet_anything(x->m_outlet1, gensym("Hello Max !"), 0, NULL);
}
EOF
