#pragma once

namespace ParameterIDs
{
    // Parameter identifiers - must match exactly in C++ and TypeScript

    // Preamp type selection (0=Cathode, 1=Filament, 2=Steel Plate)
    inline constexpr const char* preampType = "preampType";

    // Core controls
    inline constexpr const char* drive     = "drive";
    inline constexpr const char* tone      = "tone";
    inline constexpr const char* output    = "output";

    // Bypass
    inline constexpr const char* bypass    = "bypass";

    // ======================================
    // Effect Parameters (0.0-1.0 Mix/Amount)
    // ======================================

    // Cathode Effects (Warm, Vintage, Tube)
    inline constexpr const char* cath_ember  = "cath_ember";   // Tube saturation
    inline constexpr const char* cath_haze   = "cath_haze";    // Dark plate reverb
    inline constexpr const char* cath_echo   = "cath_echo";    // Tape delay
    inline constexpr const char* cath_drift  = "cath_drift";   // Tape chorus
    inline constexpr const char* cath_velvet = "cath_velvet";  // Smooth low-pass

    // Filament Effects (Cold, Digital, Precise)
    inline constexpr const char* fil_fracture = "fil_fracture"; // Digital clipping
    inline constexpr const char* fil_glisten  = "fil_glisten";  // Shimmer reverb
    inline constexpr const char* fil_cascade  = "fil_cascade";  // Multi-tap delay
    inline constexpr const char* fil_phase    = "fil_phase";    // Through-zero flanger
    inline constexpr const char* fil_prism    = "fil_prism";    // Comb filter

    // Steel Plate Effects (Aggressive, Industrial, Raw)
    inline constexpr const char* steel_scorch = "steel_scorch"; // Hard clipping
    inline constexpr const char* steel_rust   = "steel_rust";   // Gated reverb
    inline constexpr const char* steel_grind  = "steel_grind";  // Bitcrush delay
    inline constexpr const char* steel_shred  = "steel_shred";  // Ring modulation
    inline constexpr const char* steel_snarl  = "steel_snarl";  // Aggressive band-pass

    // State versioning for safe preset/session recall
    inline constexpr int kStateVersion = 3;  // Bumped for new effect params
}
