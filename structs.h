// @author: Diga Widyaprana
// @matric: A0114171W

#ifndef STRUCTS_H_
#define STRUCTS_H_

struct Url {
    std::string base;
    std::string path;
    std::string full() { return base + path; };
};
#endif // STRUCTS_H_
