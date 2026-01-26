#pragma once

namespace ParameterIDs
{
    // Parameter identifiers - must match exactly in C++ and TypeScript
    inline constexpr const char* drive    = "drive";
    inline constexpr const char* tone     = "tone";
    inline constexpr const char* output   = "output";
    inline constexpr const char* bypass   = "bypass";

    // State versioning for safe preset/session recall
    inline constexpr int kStateVersion = 1;
}
