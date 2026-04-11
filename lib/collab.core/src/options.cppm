export module collab.core:options;

export namespace collab::core {

struct options {
    const char* desc         = "";
    bool        required     = false;
    const char* display_name = "";
    bool        hidden       = false;
};

}  // namespace collab::core
