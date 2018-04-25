#include <pch.h>

#if PROJECT_ONE
    const Project::Type Project::defaultProject = Project::Type::ONE;
#elif PROJECT_TWO
    const Project::Type Project::defaultProject = Project::Type::TWO;
#elif PROJECT_THREE
    const Project::Type Project::defaultProject = Project::Type::THREE;
#endif