%module(package="libcellml") specificationrule

#define LIBCELLML_EXPORT

%{
#include "libcellml/referencerules.h"
%}

%include "libcellml/referencerules.h"
