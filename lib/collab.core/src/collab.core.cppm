export module collab.core;

export import :error;
export import :semver;
export import :log;
export import :term;
export import :field;
export import :field_reflect;
export import :meta;
export import :type_def;

export namespace collab::core {
    inline constexpr semver version{1, 0, 0};
}
