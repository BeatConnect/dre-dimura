/**
 * Preset Definitions for Dre-Dimura Preamps
 * 30 total presets (10 per preamp) with thematic names and optimized values
 */

export interface Preset {
  id: string;
  name: string;
  description: string;
  preamp: 'cathode' | 'filament' | 'steelplate';
  values: {
    drive: number;
    tone: number;
    output: number;
    effects: number[];  // 5 effect mix values (0-1)
  };
}

// ==============================================================================
// CATHODE PRESETS - Warm, Vintage, Tube (Amber Theme)
// ==============================================================================
export const CATHODE_PRESETS: Preset[] = [
  {
    id: 'cath_whiskey_glow',
    name: 'Whiskey Glow',
    description: 'Smooth warmth for intimate vocals and acoustic guitars',
    preamp: 'cathode',
    values: {
      drive: 0.25,
      tone: 0.55,
      output: 0.6,
      effects: [0.3, 0.2, 0.15, 0.1, 0.25]
    }
  },
  {
    id: 'cath_velvet_thunder',
    name: 'Velvet Thunder',
    description: 'Rich, pillowy saturation for full-bodied rhythm tracks',
    preamp: 'cathode',
    values: {
      drive: 0.55,
      tone: 0.45,
      output: 0.55,
      effects: [0.4, 0.35, 0.2, 0.25, 0.45]
    }
  },
  {
    id: 'cath_midnight_radio',
    name: 'Midnight Radio',
    description: 'Lo-fi vintage broadcast character with subtle wobble',
    preamp: 'cathode',
    values: {
      drive: 0.35,
      tone: 0.35,
      output: 0.5,
      effects: [0.2, 0.45, 0.35, 0.4, 0.15]
    }
  },
  {
    id: 'cath_golden_hour',
    name: 'Golden Hour',
    description: 'Sun-drenched warmth perfect for dreamy pads and keys',
    preamp: 'cathode',
    values: {
      drive: 0.2,
      tone: 0.65,
      output: 0.55,
      effects: [0.15, 0.3, 0.25, 0.5, 0.4]
    }
  },
  {
    id: 'cath_worn_leather',
    name: 'Worn Leather',
    description: 'Broken-in tube character for bass and synths',
    preamp: 'cathode',
    values: {
      drive: 0.45,
      tone: 0.3,
      output: 0.65,
      effects: [0.5, 0.2, 0.1, 0.15, 0.35]
    }
  },
  {
    id: 'cath_ember_trail',
    name: 'Ember Trail',
    description: 'Gentle heat that builds with sustained notes',
    preamp: 'cathode',
    values: {
      drive: 0.3,
      tone: 0.5,
      output: 0.5,
      effects: [0.6, 0.15, 0.2, 0.35, 0.3]
    }
  },
  {
    id: 'cath_dusty_vinyl',
    name: 'Dusty Vinyl',
    description: 'Classic record warmth with nostalgic texture',
    preamp: 'cathode',
    values: {
      drive: 0.28,
      tone: 0.4,
      output: 0.55,
      effects: [0.25, 0.55, 0.3, 0.2, 0.2]
    }
  },
  {
    id: 'cath_copper_wire',
    name: 'Copper Wire',
    description: 'Bright yet warm, ideal for clean electric guitars',
    preamp: 'cathode',
    values: {
      drive: 0.18,
      tone: 0.7,
      output: 0.6,
      effects: [0.2, 0.1, 0.25, 0.15, 0.15]
    }
  },
  {
    id: 'cath_fireplace',
    name: 'Fireplace',
    description: 'Cozy, enveloping warmth for ambient textures',
    preamp: 'cathode',
    values: {
      drive: 0.22,
      tone: 0.45,
      output: 0.45,
      effects: [0.35, 0.4, 0.4, 0.55, 0.5]
    }
  },
  {
    id: 'cath_honeycomb',
    name: 'Honeycomb',
    description: 'Sweet, dense harmonics for layered productions',
    preamp: 'cathode',
    values: {
      drive: 0.4,
      tone: 0.55,
      output: 0.55,
      effects: [0.45, 0.25, 0.15, 0.3, 0.55]
    }
  }
];

// ==============================================================================
// FILAMENT PRESETS - Cold, Digital, Precise (Blue Theme)
// ==============================================================================
export const FILAMENT_PRESETS: Preset[] = [
  {
    id: 'fil_zero_kelvin',
    name: 'Zero Kelvin',
    description: 'Pristine clarity with surgical precision',
    preamp: 'filament',
    values: {
      drive: 0.15,
      tone: 0.6,
      output: 0.65,
      effects: [0.1, 0.15, 0.1, 0.2, 0.25]
    }
  },
  {
    id: 'fil_neon_cascade',
    name: 'Neon Cascade',
    description: 'Shimmering digital trails for atmospheric builds',
    preamp: 'filament',
    values: {
      drive: 0.25,
      tone: 0.7,
      output: 0.55,
      effects: [0.2, 0.45, 0.55, 0.35, 0.4]
    }
  },
  {
    id: 'fil_chrome_edge',
    name: 'Chrome Edge',
    description: 'Sharp, modern bite for cutting synth leads',
    preamp: 'filament',
    values: {
      drive: 0.4,
      tone: 0.75,
      output: 0.6,
      effects: [0.35, 0.3, 0.2, 0.25, 0.15]
    }
  },
  {
    id: 'fil_arctic_drift',
    name: 'Arctic Drift',
    description: 'Frozen textures with glacial reverb tails',
    preamp: 'filament',
    values: {
      drive: 0.18,
      tone: 0.5,
      output: 0.5,
      effects: [0.15, 0.25, 0.6, 0.5, 0.45]
    }
  },
  {
    id: 'fil_data_stream',
    name: 'Data Stream',
    description: 'Rhythmic, pulsing character for electronic beats',
    preamp: 'filament',
    values: {
      drive: 0.35,
      tone: 0.55,
      output: 0.6,
      effects: [0.4, 0.2, 0.35, 0.45, 0.2]
    }
  },
  {
    id: 'fil_mirror_lake',
    name: 'Mirror Lake',
    description: 'Crystal-clear reflections with infinite depth',
    preamp: 'filament',
    values: {
      drive: 0.12,
      tone: 0.65,
      output: 0.55,
      effects: [0.1, 0.35, 0.5, 0.4, 0.55]
    }
  },
  {
    id: 'fil_circuit_bend',
    name: 'Circuit Bend',
    description: 'Controlled digital artifacts for experimental work',
    preamp: 'filament',
    values: {
      drive: 0.5,
      tone: 0.45,
      output: 0.5,
      effects: [0.55, 0.15, 0.25, 0.55, 0.3]
    }
  },
  {
    id: 'fil_polar_vortex',
    name: 'Polar Vortex',
    description: 'Swirling, icy modulation for evolving soundscapes',
    preamp: 'filament',
    values: {
      drive: 0.22,
      tone: 0.55,
      output: 0.5,
      effects: [0.25, 0.3, 0.45, 0.6, 0.35]
    }
  },
  {
    id: 'fil_blue_steel',
    name: 'Blue Steel',
    description: 'Cold precision with subtle metallic edge',
    preamp: 'filament',
    values: {
      drive: 0.32,
      tone: 0.6,
      output: 0.6,
      effects: [0.3, 0.25, 0.15, 0.3, 0.2]
    }
  },
  {
    id: 'fil_hologram',
    name: 'Hologram',
    description: 'Translucent, ethereal presence for vocals and pads',
    preamp: 'filament',
    values: {
      drive: 0.15,
      tone: 0.7,
      output: 0.5,
      effects: [0.15, 0.4, 0.4, 0.35, 0.6]
    }
  }
];

// ==============================================================================
// STEEL PLATE PRESETS - Aggressive, Industrial, Raw (Red Theme)
// ==============================================================================
export const STEELPLATE_PRESETS: Preset[] = [
  {
    id: 'steel_iron_fist',
    name: 'Iron Fist',
    description: 'Crushing weight for heavy rhythm guitars',
    preamp: 'steelplate',
    values: {
      drive: 0.65,
      tone: 0.4,
      output: 0.6,
      effects: [0.5, 0.3, 0.45, 0.35, 0.4]
    }
  },
  {
    id: 'steel_rust_belt',
    name: 'Rust Belt',
    description: 'Gritty, decayed industrial character',
    preamp: 'steelplate',
    values: {
      drive: 0.45,
      tone: 0.35,
      output: 0.55,
      effects: [0.35, 0.55, 0.3, 0.2, 0.25]
    }
  },
  {
    id: 'steel_molten_core',
    name: 'Molten Core',
    description: 'Seething intensity for aggressive synth basses',
    preamp: 'steelplate',
    values: {
      drive: 0.7,
      tone: 0.3,
      output: 0.55,
      effects: [0.6, 0.25, 0.35, 0.4, 0.5]
    }
  },
  {
    id: 'steel_shrapnel',
    name: 'Shrapnel',
    description: 'Sharp, explosive transients for drums and percussion',
    preamp: 'steelplate',
    values: {
      drive: 0.5,
      tone: 0.65,
      output: 0.6,
      effects: [0.45, 0.2, 0.55, 0.5, 0.3]
    }
  },
  {
    id: 'steel_forge_fire',
    name: 'Forge Fire',
    description: 'White-hot saturation for screaming leads',
    preamp: 'steelplate',
    values: {
      drive: 0.8,
      tone: 0.55,
      output: 0.5,
      effects: [0.7, 0.15, 0.25, 0.3, 0.55]
    }
  },
  {
    id: 'steel_concrete_jungle',
    name: 'Concrete Jungle',
    description: 'Urban grit with punchy, in-your-face presence',
    preamp: 'steelplate',
    values: {
      drive: 0.55,
      tone: 0.5,
      output: 0.65,
      effects: [0.4, 0.35, 0.4, 0.35, 0.3]
    }
  },
  {
    id: 'steel_voltage_spike',
    name: 'Voltage Spike',
    description: 'Erratic, dangerous energy for chaotic textures',
    preamp: 'steelplate',
    values: {
      drive: 0.6,
      tone: 0.6,
      output: 0.55,
      effects: [0.5, 0.4, 0.5, 0.6, 0.45]
    }
  },
  {
    id: 'steel_war_machine',
    name: 'War Machine',
    description: 'Relentless mechanical aggression',
    preamp: 'steelplate',
    values: {
      drive: 0.75,
      tone: 0.45,
      output: 0.6,
      effects: [0.55, 0.3, 0.6, 0.45, 0.5]
    }
  },
  {
    id: 'steel_blood_orange',
    name: 'Blood Orange',
    description: 'Aggressive warmth with violent undertones',
    preamp: 'steelplate',
    values: {
      drive: 0.5,
      tone: 0.4,
      output: 0.55,
      effects: [0.45, 0.45, 0.25, 0.3, 0.4]
    }
  },
  {
    id: 'steel_demolition',
    name: 'Demolition',
    description: 'Total destruction for the heaviest moments',
    preamp: 'steelplate',
    values: {
      drive: 0.85,
      tone: 0.35,
      output: 0.55,
      effects: [0.75, 0.5, 0.65, 0.55, 0.6]
    }
  }
];

// ==============================================================================
// PRESET LOOKUP FUNCTIONS
// ==============================================================================

export function getPresetsForPreamp(preamp: 'cathode' | 'filament' | 'steelplate'): Preset[] {
  switch (preamp) {
    case 'cathode':
      return CATHODE_PRESETS;
    case 'filament':
      return FILAMENT_PRESETS;
    case 'steelplate':
      return STEELPLATE_PRESETS;
    default:
      return CATHODE_PRESETS;
  }
}

export function getPresetById(id: string): Preset | undefined {
  const allPresets = [...CATHODE_PRESETS, ...FILAMENT_PRESETS, ...STEELPLATE_PRESETS];
  return allPresets.find(preset => preset.id === id);
}

export function getDefaultPresetForPreamp(preamp: 'cathode' | 'filament' | 'steelplate'): Preset {
  return getPresetsForPreamp(preamp)[0];
}
