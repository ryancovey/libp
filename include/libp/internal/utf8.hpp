#ifndef LIBP_INTERNAL_UTF8_HPP_GUARD
#define LIBP_INTERNAL_UTF8_HPP_GUARD

namespace libp { inline namespace internal { namespace utf8 {
                                                                                          // LaTeX:
    static const char * blackboard_N = reinterpret_cast<const char *>(u8"\u2115");        // \mathbb{N}
    static const char * blackboard_R = reinterpret_cast<const char *>(u8"\u211D");        // \mathbb{R}
    static const char * blackboard_Z = reinterpret_cast<const char *>(u8"\u2124");        // \mathbb{Z}
    static const char * blackboard_U = reinterpret_cast<const char *>(u8"\U0001D54C");    // \mathbb{U}
    static const char * emptyset =     reinterpret_cast<const char *>(u8"\u2205");        // \varnothing
    static const char * times =        reinterpret_cast<const char *>(u8"\u00D7");        // \times

}}}

#endif

