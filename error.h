#pragma once

/*! \brief report the error_at during tokenizing and parsing*/
void error_at(char *cur_code, char *fmt, ...);

void report(char *fmt, ...);