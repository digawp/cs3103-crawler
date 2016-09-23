// @author: Diga Widyaprana
// @matric: A0114171W

struct Url {
    std::string base;
    std::string path;
    std::string full() { return base + path; };
};
