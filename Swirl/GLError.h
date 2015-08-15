/* Morten Nobel-Jørgensen GitHub Gist */

#ifndef GLERROR_H
#define GLERROR_H

void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// check_gl_error();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif // GLERROR_H