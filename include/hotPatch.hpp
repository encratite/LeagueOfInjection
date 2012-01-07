#pragma once

#include <string>

bool hotPatchFunction(std::string const & module, std::string const & function, void * functionPointer, void * & oldAddress);
