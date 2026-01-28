import React, { useState, useCallback, useEffect } from 'react';
import { useSliderParam, useToggleParam, applyBatchUpdates } from './hooks/useJuceParam';
import { useAudioLevels } from './hooks/useAudioLevels';
import { useActivation } from './hooks/useActivation';
import { Knob } from './components/Knob';
import { EffectModule } from './components/EffectModule';
import { PresetSelector } from './components/PresetSelector';
import { PreampTooltipTrigger } from './components/PreampTooltip';
import { ActivationScreen } from './components/ActivationScreen';
import { HearthglowBackground } from './components/artwork/HearthglowBackground';
import { NightfallBackground } from './components/artwork/NightfallBackground';
import { SteelplateBackground } from './components/artwork/SteelplateBackground';
import { Preset, getDefaultPresetForPreamp } from './data/presets';

// Effect definitions per preamp
const CATHODE_EFFECTS = [
  { id: 'cath_ember', name: 'Ember' },
  { id: 'cath_haze', name: 'Haze' },
  { id: 'cath_echo', name: 'Echo' },
  { id: 'cath_drift', name: 'Drift' },
  { id: 'cath_velvet', name: 'Velvet' },
];

const FILAMENT_EFFECTS = [
  { id: 'fil_fracture', name: 'Fracture' },
  { id: 'fil_glisten', name: 'Glisten' },
  { id: 'fil_cascade', name: 'Cascade' },
  { id: 'fil_phase', name: 'Phase' },
  { id: 'fil_prism', name: 'Prism' },
];

const STEELPLATE_EFFECTS = [
  { id: 'steel_scorch', name: 'Scorch' },
  { id: 'steel_rust', name: 'Rust' },
  { id: 'steel_grind', name: 'Grind' },
  { id: 'steel_shred', name: 'Shred' },
  { id: 'steel_snarl', name: 'Snarl' },
];

// Preamp configurations
const PREAMPS = [
  {
    id: 'cathode',
    name: 'Cathode',
    subtitle: 'Vintage Tube Warmth',
    theme: 'hearthglow',
  },
  {
    id: 'filament',
    name: 'Filament',
    subtitle: 'Cold Digital Edge',
    theme: 'nightfall',
  },
  {
    id: 'steelplate',
    name: 'Steel Plate',
    subtitle: 'Raw Iron Grit',
    theme: 'steelplate',
  },
];

// Cathode VU Meter - Classic analog style
function CathodeVUMeter({ level }: { level: number }) {
  const percentage = Math.min(100, level * 100);
  return (
    <div className="vu-meter cathode-meter">
      <div className="vu-scale">
        <span>-20</span>
        <span>-10</span>
        <span>0</span>
        <span>+3</span>
      </div>
      <div className="vu-track">
        <div className="vu-fill" style={{ width: `${percentage}%` }} />
      </div>
      <div className="vu-label">Output</div>
    </div>
  );
}

// Filament Oscilloscope Display - Animated waveform visualizer
function FilamentOscilloscope({ level, color }: { level: number; color: number }) {
  // Generate waveform path based on level and color (affects wave shape)
  const generateWavePath = () => {
    const points: string[] = [];
    const width = 180;
    const height = 60;
    const centerY = height / 2;
    const amplitude = level * 22;
    const frequency = 2 + color * 2; // Color affects frequency

    for (let x = 0; x <= width; x += 2) {
      const normalX = x / width;
      // Multiple wave components for complex shape
      const wave1 = Math.sin(normalX * Math.PI * frequency) * amplitude;
      const wave2 = Math.sin(normalX * Math.PI * frequency * 2.3 + 0.5) * amplitude * 0.3;
      const wave3 = Math.cos(normalX * Math.PI * frequency * 0.7) * amplitude * 0.2;
      // Envelope to fade edges
      const envelope = Math.sin(normalX * Math.PI);
      const y = centerY + (wave1 + wave2 + wave3) * envelope;
      points.push(`${x},${y}`);
    }
    return `M ${points.join(' L ')}`;
  };

  return (
    <div className="oscilloscope">
      <div className="oscilloscope-display">
        <svg viewBox="0 0 180 60" preserveAspectRatio="none">
          <defs>
            {/* Main wave gradient */}
            <linearGradient id="fil-wave-gradient" x1="0%" y1="0%" x2="100%" y2="0%">
              <stop offset="0%" stopColor="#3060a0" stopOpacity="0.3" />
              <stop offset="30%" stopColor="#5090d0" />
              <stop offset="50%" stopColor="#70b0f0" />
              <stop offset="70%" stopColor="#5090d0" />
              <stop offset="100%" stopColor="#3060a0" stopOpacity="0.3" />
            </linearGradient>

            {/* Glow gradient */}
            <linearGradient id="fil-glow-gradient" x1="0%" y1="0%" x2="100%" y2="0%">
              <stop offset="0%" stopColor="transparent" />
              <stop offset="50%" stopColor="#80c0ff" stopOpacity="0.4" />
              <stop offset="100%" stopColor="transparent" />
            </linearGradient>

            {/* Grid pattern */}
            <pattern id="fil-grid" patternUnits="userSpaceOnUse" width="15" height="15">
              <path d="M 15 0 L 0 0 0 15" fill="none" stroke="#1a2030" strokeWidth="0.5" />
            </pattern>

            {/* Scan line pattern */}
            <pattern id="fil-scanlines" patternUnits="userSpaceOnUse" width="180" height="2">
              <line x1="0" y1="0" x2="180" y2="0" stroke="#000" strokeWidth="1" opacity="0.15" />
            </pattern>

            {/* Glow filter */}
            <filter id="fil-waveglow" x="-50%" y="-100%" width="200%" height="300%">
              <feGaussianBlur in="SourceGraphic" stdDeviation="2" result="blur" />
              <feMerge>
                <feMergeNode in="blur" />
                <feMergeNode in="blur" />
                <feMergeNode in="SourceGraphic" />
              </feMerge>
            </filter>
          </defs>

          {/* Background grid */}
          <rect width="180" height="60" fill="url(#fil-grid)" />

          {/* Center line */}
          <line x1="0" y1="30" x2="180" y2="30" stroke="#2a3848" strokeWidth="1" />

          {/* Waveform glow layer */}
          <path
            d={generateWavePath()}
            fill="none"
            stroke="url(#fil-glow-gradient)"
            strokeWidth="8"
            opacity="0.5"
          >
            <animate
              attributeName="d"
              dur="0.1s"
              repeatCount="indefinite"
              values={`${generateWavePath()};${generateWavePath()}`}
            />
          </path>

          {/* Main waveform */}
          <path
            d={generateWavePath()}
            fill="none"
            stroke="url(#fil-wave-gradient)"
            strokeWidth="2"
            strokeLinecap="round"
            filter="url(#fil-waveglow)"
          >
            <animate
              attributeName="d"
              dur="0.08s"
              repeatCount="indefinite"
              values={`${generateWavePath()};${generateWavePath()}`}
            />
          </path>

          {/* Bright center line on wave */}
          <path
            d={generateWavePath()}
            fill="none"
            stroke="#a0d0ff"
            strokeWidth="1"
            strokeLinecap="round"
            opacity="0.8"
          />

          {/* Scan lines */}
          <rect width="180" height="60" fill="url(#fil-scanlines)" />

          {/* Edge markers */}
          <g stroke="#4070a0" strokeWidth="1" opacity="0.5">
            <line x1="5" y1="5" x2="15" y2="5" />
            <line x1="5" y1="5" x2="5" y2="15" />
            <line x1="165" y1="5" x2="175" y2="5" />
            <line x1="175" y1="5" x2="175" y2="15" />
            <line x1="5" y1="55" x2="15" y2="55" />
            <line x1="5" y1="45" x2="5" y2="55" />
            <line x1="165" y1="55" x2="175" y2="55" />
            <line x1="175" y1="45" x2="175" y2="55" />
          </g>
        </svg>

        {/* Data particles */}
        <div className="data-particle p1" />
        <div className="data-particle p2" />
        <div className="data-particle p3" />
        <div className="data-particle p4" />

        {/* Scan beam */}
        <div className="scan-beam" />
      </div>
    </div>
  );
}

// Filament Mini Meter - Compact vertical indicator
function FilamentMiniMeter({ level, label }: { level: number; label: string }) {
  return (
    <div className="mini-meter">
      <div className="mini-meter-track">
        <div className="mini-meter-fill" style={{ height: `${level * 100}%` }} />
        <div className="mini-meter-glow" style={{ height: `${level * 100}%` }} />
      </div>
      <span className="mini-meter-label">{label}</span>
    </div>
  );
}

// Filament Slider - Horizontal slider control
function FilamentSlider({
  value,
  onChange,
  onDragStart,
  onDragEnd,
  label
}: {
  value: number;
  onChange: (v: number) => void;
  onDragStart?: () => void;
  onDragEnd?: () => void;
  label: string;
}) {
  const trackRef = React.useRef<HTMLDivElement>(null);
  const isDragging = React.useRef(false);

  const handleMouseDown = (e: React.MouseEvent) => {
    isDragging.current = true;
    onDragStart?.();
    updateValue(e.clientX);

    const handleMouseMove = (moveEvent: MouseEvent) => {
      if (!isDragging.current) return;
      updateValue(moveEvent.clientX);
    };

    const handleMouseUp = () => {
      isDragging.current = false;
      onDragEnd?.();
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleMouseUp);
    };

    window.addEventListener('mousemove', handleMouseMove);
    window.addEventListener('mouseup', handleMouseUp);
  };

  const updateValue = (clientX: number) => {
    if (!trackRef.current) return;
    const rect = trackRef.current.getBoundingClientRect();
    const x = clientX - rect.left;
    const newValue = Math.max(0, Math.min(1, x / rect.width));
    onChange(newValue);
  };

  return (
    <div className="filament-slider">
      <span className="slider-label">{label}</span>
      <div
        className="slider-track"
        ref={trackRef}
        onMouseDown={handleMouseDown}
      >
        <div className="slider-fill" style={{ width: `${value * 100}%` }} />
        <div className="slider-thumb" style={{ left: `${value * 100}%` }} />
        <div className="slider-glow" style={{ width: `${value * 100}%` }} />
      </div>
      <span className="slider-value">{Math.round(value * 100)}%</span>
    </div>
  );
}

// Steel Plate Visualizer - Shader-style spectrum display
function SteelPlateVisualizer({ level }: { level: number }) {
  const bars = 24;

  // Generate pseudo-random but deterministic heights based on level
  const getBarHeight = (index: number, time: number) => {
    const base = level * 0.6;
    const wave1 = Math.sin((index * 0.5) + time * 2) * 0.15;
    const wave2 = Math.sin((index * 0.3) + time * 3.7) * 0.1;
    const wave3 = Math.cos((index * 0.7) + time * 1.3) * 0.08;
    const center = 1 - Math.abs((index - bars / 2) / (bars / 2)) * 0.3;
    return Math.max(0.05, Math.min(1, (base + wave1 + wave2 + wave3) * center));
  };

  return (
    <div className="spectrum-visualizer">
      <div className="spectrum-display">
        <svg viewBox="0 0 200 80" preserveAspectRatio="none">
          <defs>
            {/* Bar gradient */}
            <linearGradient id="sp-bar-gradient" x1="0%" y1="100%" x2="0%" y2="0%">
              <stop offset="0%" stopColor="#303038" />
              <stop offset="40%" stopColor="#505058" />
              <stop offset="70%" stopColor="#808088" />
              <stop offset="85%" stopColor="#a04050" />
              <stop offset="100%" stopColor="#d05060" />
            </linearGradient>

            {/* Glow gradient */}
            <linearGradient id="sp-glow-gradient" x1="0%" y1="100%" x2="0%" y2="0%">
              <stop offset="0%" stopColor="transparent" />
              <stop offset="60%" stopColor="transparent" />
              <stop offset="100%" stopColor="#c04050" stopOpacity="0.6" />
            </linearGradient>

            {/* Reflection gradient */}
            <linearGradient id="sp-reflect-gradient" x1="0%" y1="0%" x2="0%" y2="100%">
              <stop offset="0%" stopColor="#a04050" stopOpacity="0.3" />
              <stop offset="100%" stopColor="transparent" />
            </linearGradient>

            {/* Scan line pattern */}
            <pattern id="sp-scanlines" patternUnits="userSpaceOnUse" width="200" height="2">
              <line x1="0" y1="0" x2="200" y2="0" stroke="#000" strokeWidth="1" opacity="0.3" />
            </pattern>

            {/* Noise filter */}
            <filter id="sp-noise">
              <feTurbulence type="fractalNoise" baseFrequency="0.9" numOctaves="4" result="noise" />
              <feComposite in="SourceGraphic" in2="noise" operator="in" />
            </filter>

            {/* Glow filter */}
            <filter id="sp-barglow" x="-50%" y="-50%" width="200%" height="200%">
              <feGaussianBlur in="SourceGraphic" stdDeviation="1.5" result="blur" />
              <feMerge>
                <feMergeNode in="blur" />
                <feMergeNode in="SourceGraphic" />
              </feMerge>
            </filter>
          </defs>

          {/* Background grid */}
          <g stroke="#1a1a1e" strokeWidth="0.5" opacity="0.5">
            {Array.from({ length: 5 }).map((_, i) => (
              <line key={`h${i}`} x1="0" y1={i * 20} x2="200" y2={i * 20} />
            ))}
            {Array.from({ length: 9 }).map((_, i) => (
              <line key={`v${i}`} x1={i * 25} y1="0" x2={i * 25} y2="80" />
            ))}
          </g>

          {/* Spectrum bars */}
          <g filter="url(#sp-barglow)">
            {Array.from({ length: bars }).map((_, i) => {
              const height = getBarHeight(i, Date.now() / 1000);
              const barHeight = height * 65;
              const x = (i / bars) * 190 + 5;
              const width = 190 / bars - 2;

              return (
                <g key={i}>
                  {/* Main bar */}
                  <rect
                    x={x}
                    y={75 - barHeight}
                    width={width}
                    height={barHeight}
                    fill="url(#sp-bar-gradient)"
                    rx="1"
                  >
                    <animate
                      attributeName="height"
                      values={`${barHeight};${barHeight * 0.85};${barHeight}`}
                      dur={`${0.3 + (i % 5) * 0.1}s`}
                      repeatCount="indefinite"
                    />
                    <animate
                      attributeName="y"
                      values={`${75 - barHeight};${75 - barHeight * 0.85};${75 - barHeight}`}
                      dur={`${0.3 + (i % 5) * 0.1}s`}
                      repeatCount="indefinite"
                    />
                  </rect>

                  {/* Top glow */}
                  <rect
                    x={x}
                    y={75 - barHeight - 3}
                    width={width}
                    height={6}
                    fill="url(#sp-glow-gradient)"
                    opacity={height > 0.5 ? (height - 0.5) * 2 : 0}
                  >
                    <animate
                      attributeName="y"
                      values={`${75 - barHeight - 3};${75 - barHeight * 0.85 - 3};${75 - barHeight - 3}`}
                      dur={`${0.3 + (i % 5) * 0.1}s`}
                      repeatCount="indefinite"
                    />
                  </rect>

                  {/* Reflection below */}
                  <rect
                    x={x}
                    y={77}
                    width={width}
                    height={barHeight * 0.15}
                    fill="url(#sp-reflect-gradient)"
                    opacity="0.4"
                  />
                </g>
              );
            })}
          </g>

          {/* Center peak line */}
          <line x1="5" y1="10" x2="195" y2="10" stroke="#a04050" strokeWidth="0.5" opacity="0.4">
            <animate attributeName="opacity" values="0.2;0.5;0.2" dur="2s" repeatCount="indefinite" />
          </line>

          {/* Scan lines overlay */}
          <rect width="200" height="80" fill="url(#sp-scanlines)" opacity="0.4" />

          {/* Edge vignette */}
          <rect width="200" height="80" fill="none" stroke="#000" strokeWidth="8" opacity="0.3" />
        </svg>

        {/* Animated energy streaks */}
        <div className="energy-streak streak-1" />
        <div className="energy-streak streak-2" />
        <div className="energy-streak streak-3" />
      </div>
    </div>
  );
}

function CarouselArrow({ direction, onClick, disabled }: {
  direction: 'left' | 'right';
  onClick: () => void;
  disabled: boolean;
}) {
  return (
    <button
      className={`carousel-arrow ${direction} ${disabled ? 'disabled' : ''}`}
      onClick={onClick}
      disabled={disabled}
    >
      <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        {direction === 'left'
          ? <polyline points="15,18 9,12 15,6" />
          : <polyline points="9,6 15,12 9,18" />
        }
      </svg>
    </button>
  );
}

// Cathode Controls - Warm vintage layout
function CathodeControls({ drive, tone, output, outputLevel }: {
  drive: any;
  tone: any;
  output: any;
  inputLevel?: number;
  outputLevel: number;
}) {
  return (
    <div className="controls-wrapper">
      <div className="controls-panel cathode-panel">
        <Knob
          value={drive.value}
          onChange={drive.setValue}
          onDragStart={drive.onDragStart}
          onDragEnd={drive.onDragEnd}
          label="Drive"
          size="large"
        />

        <CathodeVUMeter level={outputLevel} />

        <Knob
          value={output.value}
          onChange={output.setValue}
          onDragStart={output.onDragStart}
          onDragEnd={output.onDragEnd}
          label="Output"
          size="large"
        />

        <div className="control-divider" />

        <Knob
          value={tone.value}
          onChange={tone.setValue}
          onDragStart={tone.onDragStart}
          onDragEnd={tone.onDragEnd}
          label="Tone"
          size="medium"
        />
      </div>

      <div className="effects-strip cathode">
        <span className="effects-label">Effects</span>
        <div className="effects-row">
          {CATHODE_EFFECTS.map(effect => (
            <EffectModule
              key={effect.id}
              paramId={effect.id}
              name={effect.name}
              theme="cathode"
            />
          ))}
        </div>
      </div>
    </div>
  );
}

// Filament Controls - Sleek digital layout with oscilloscope
function FilamentControls({ drive, tone, output, inputLevel, outputLevel }: {
  drive: any;
  tone: any;
  output: any;
  inputLevel: number;
  outputLevel: number;
}) {
  return (
    <div className="controls-wrapper">
      <div className="controls-panel filament-panel">
        {/* Main controls row */}
        <div className="filament-main-row">
          {/* Input section */}
          <div className="filament-section">
            <Knob
              value={drive.value}
              onChange={drive.setValue}
              onDragStart={drive.onDragStart}
              onDragEnd={drive.onDragEnd}
              label="Input"
              size="large"
            />
            <FilamentMiniMeter level={inputLevel} label="In" />
          </div>

          {/* Center oscilloscope - shows real output level and tone shape */}
          <FilamentOscilloscope level={outputLevel} color={tone.value} />

          {/* Output section */}
          <div className="filament-section">
            <FilamentMiniMeter level={outputLevel} label="Out" />
            <Knob
              value={output.value}
              onChange={output.setValue}
              onDragStart={output.onDragStart}
              onDragEnd={output.onDragEnd}
              label="Output"
              size="large"
            />
          </div>
        </div>

        {/* Color slider below - affects waveform shape */}
        <div className="filament-color-row">
          <FilamentSlider
            value={tone.value}
            onChange={tone.setValue}
            onDragStart={tone.onDragStart}
            onDragEnd={tone.onDragEnd}
            label="Color"
          />
        </div>
      </div>

      <div className="effects-strip filament">
        <span className="effects-label">Effects</span>
        <div className="effects-row">
          {FILAMENT_EFFECTS.map(effect => (
            <EffectModule
              key={effect.id}
              paramId={effect.id}
              name={effect.name}
              theme="filament"
            />
          ))}
        </div>
      </div>
    </div>
  );
}

// Steel Plate Controls - Wide industrial layout with visualizer
function SteelPlateControls({ drive, tone, output, outputLevel }: {
  drive: any;
  tone: any;
  output: any;
  inputLevel?: number;
  outputLevel: number;
}) {
  return (
    <div className="controls-wrapper">
      <div className="controls-panel steelplate-panel">
        <Knob
          value={drive.value}
          onChange={drive.setValue}
          onDragStart={drive.onDragStart}
          onDragEnd={drive.onDragEnd}
          label="Gain"
          size="large"
        />

        <Knob
          value={tone.value}
          onChange={tone.setValue}
          onDragStart={tone.onDragStart}
          onDragEnd={tone.onDragEnd}
          label="Grit"
          size="medium"
        />

        <Knob
          value={output.value}
          onChange={output.setValue}
          onDragStart={output.onDragStart}
          onDragEnd={output.onDragEnd}
          label="Level"
          size="large"
        />

        <div className="steelplate-divider" />

        <SteelPlateVisualizer level={outputLevel} />
      </div>

      <div className="effects-strip steelplate">
        <span className="effects-label">Effects</span>
        <div className="effects-row">
          {STEELPLATE_EFFECTS.map(effect => (
            <EffectModule
              key={effect.id}
              paramId={effect.id}
              name={effect.name}
              theme="steelplate"
            />
          ))}
        </div>
      </div>
    </div>
  );
}

function App() {
  // Activation state - driven by useActivation hook (communicates with C++ backend)
  const activation = useActivation();
  // Once activation screen has been shown, track it so it stays mounted during animation
  const [activationScreenShown, setActivationScreenShown] = useState(false);
  // Track when to actually unmount the activation screen (after fade-out animation completes)
  const [activationScreenRemoved, setActivationScreenRemoved] = useState(false);

  const [currentIndex, setCurrentIndex] = useState(0);
  const [isTransitioning, setIsTransitioning] = useState(false);
  const [transitionDirection, setTransitionDirection] = useState<'left' | 'right'>('right');
  const [displayIndex, setDisplayIndex] = useState(0);
  const [presetChanging, setPresetChanging] = useState(false);

  // Track current preset per preamp
  const [presets, setPresets] = useState<Record<string, Preset>>(() => ({
    cathode: getDefaultPresetForPreamp('cathode'),
    filament: getDefaultPresetForPreamp('filament'),
    steelplate: getDefaultPresetForPreamp('steelplate'),
  }));

  // All hooks must be called before any conditional returns
  const drive = useSliderParam('drive', { defaultValue: 0.25 });
  const tone = useSliderParam('tone', { defaultValue: 0.5 });
  const output = useSliderParam('output', { defaultValue: 0.5 });
  const bypass = useToggleParam('bypass', { defaultValue: false });
  const audioLevels = useAudioLevels();

  // Once C++ tells us activation is needed, mark it as shown so it stays mounted
  useEffect(() => {
    if (activation.showActivation && !activationScreenShown) {
      setActivationScreenShown(true);
    }
  }, [activation.showActivation, activationScreenShown]);

  // Handle activation animation complete - called by ActivationScreen when fade starts
  const handleActivated = useCallback(() => {
    // Delay unmounting to allow the CSS fade-out animation to complete
    setTimeout(() => {
      setActivationScreenRemoved(true);
    }, 600);
  }, []);

  // Show activation screen if it was shown and hasn't been removed yet
  const shouldShowActivation = activationScreenShown && !activationScreenRemoved;

  const currentPreamp = PREAMPS[displayIndex];
  const currentPreset = presets[currentPreamp.id] || null;

  // Get effect IDs for current preamp
  const getEffectIds = useCallback((preampId: string): string[] => {
    switch (preampId) {
      case 'cathode':
        return CATHODE_EFFECTS.map(e => e.id);
      case 'filament':
        return FILAMENT_EFFECTS.map(e => e.id);
      case 'steelplate':
        return STEELPLATE_EFFECTS.map(e => e.id);
      default:
        return [];
    }
  }, []);

  // Handle preset change with smooth parameter animation
  const handlePresetChange = useCallback((preset: Preset) => {
    setPresetChanging(true);

    // Update preset state
    setPresets(prev => ({
      ...prev,
      [preset.preamp]: preset
    }));

    // Build batch updates for all parameters
    const effectIds = getEffectIds(preset.preamp);
    const updates = [
      { paramId: 'drive', value: preset.values.drive },
      { paramId: 'tone', value: preset.values.tone },
      { paramId: 'output', value: preset.values.output },
      ...preset.values.effects.map((value, index) => ({
        paramId: effectIds[index],
        value
      }))
    ];

    // Update local state immediately for smooth UI
    drive.setValue(preset.values.drive);
    tone.setValue(preset.values.tone);
    output.setValue(preset.values.output);

    // Apply batch updates to JUCE with slight stagger for visual effect
    applyBatchUpdates(updates, {
      staggerMs: 15,
      onComplete: () => {
        setTimeout(() => setPresetChanging(false), 100);
      }
    });
  }, [drive, tone, output, getEffectIds]);

  // Use real audio levels from the processor (clamped to 0-1 range for display)
  const inputLevel = Math.min(1, audioLevels.inputLevel);
  const outputLevel = Math.min(1, audioLevels.outputLevel);

  const navigateTo = useCallback((newIndex: number) => {
    if (isTransitioning || newIndex === currentIndex) return;
    if (newIndex < 0 || newIndex >= PREAMPS.length) return;

    setTransitionDirection(newIndex > currentIndex ? 'right' : 'left');
    setIsTransitioning(true);
    setCurrentIndex(newIndex);

    // Fast transition - swap content at midpoint
    setTimeout(() => {
      setDisplayIndex(newIndex);
    }, 80);

    // Complete transition
    setTimeout(() => {
      setIsTransitioning(false);
    }, 200);
  }, [currentIndex, isTransitioning]);

  const goNext = () => navigateTo(currentIndex + 1);
  const goPrev = () => navigateTo(currentIndex - 1);

  const renderBackground = () => {
    switch (currentPreamp.theme) {
      case 'nightfall':
        return <NightfallBackground />;
      case 'steelplate':
        return <SteelplateBackground />;
      default:
        return <HearthglowBackground />;
    }
  };

  const renderControls = () => {
    const controlProps = { drive, tone, output, inputLevel, outputLevel };
    switch (currentPreamp.id) {
      case 'filament':
        return <FilamentControls {...controlProps} />;
      case 'steelplate':
        return <SteelPlateControls {...controlProps} />;
      default:
        return <CathodeControls {...controlProps} />;
    }
  };

  return (
    <>
      {/* Main plugin UI - always rendered so it's visible behind the activation screen */}
      <div className={`plugin-container ${currentPreamp.theme} ${bypass.value ? 'bypassed' : ''} ${shouldShowActivation ? 'behind-activation' : ''}`}>
        <div className={`plugin-background ${isTransitioning ? `transitioning-${transitionDirection}` : ''}`}>
          {renderBackground()}
        </div>

        <div className={`plugin-content ${isTransitioning ? `content-transitioning-${transitionDirection}` : ''} ${presetChanging ? 'preset-changing' : ''}`}>
          <header className="plugin-header">
            <PreampTooltipTrigger preampId={currentPreamp.id as 'cathode' | 'filament' | 'steelplate'}>
              <div className="brand">
                <h1 className="brand-name">{currentPreamp.name}</h1>
                <span className="brand-subtitle">{currentPreamp.subtitle}</span>
              </div>
            </PreampTooltipTrigger>

            <PresetSelector
              preampId={currentPreamp.id as 'cathode' | 'filament' | 'steelplate'}
              currentPreset={currentPreset}
              onPresetChange={handlePresetChange}
            />

            <button
              className={`bypass-toggle ${bypass.value ? 'bypassed' : 'active'}`}
              onClick={bypass.toggle}
            >
              <div className="bypass-light" />
              <span className="bypass-text">{bypass.value ? 'Off' : 'On'}</span>
            </button>
          </header>

          <section className="controls-section">
            <CarouselArrow
              direction="left"
              onClick={goPrev}
              disabled={currentIndex === 0 || isTransitioning}
            />

            {renderControls()}

            <CarouselArrow
              direction="right"
              onClick={goNext}
              disabled={currentIndex === PREAMPS.length - 1 || isTransitioning}
            />
          </section>

          <footer className="plugin-footer">
            <span className="footer-text">Dre DiMura Audio</span>

            <div className="carousel-indicators">
              {PREAMPS.map((_, i) => (
                <button
                  key={i}
                  className={`indicator ${i === currentIndex ? 'active' : ''}`}
                  onClick={() => navigateTo(i)}
                  disabled={isTransitioning}
                />
              ))}
            </div>

            <span className="footer-text">v1.0</span>
          </footer>
        </div>
      </div>

      {/* Activation screen overlay - rendered on top until animation completes */}
      {shouldShowActivation && (
        <ActivationScreen
          onActivated={handleActivated}
          isActivated={activation.isActivated}
          isLoading={activation.isLoading}
          lastError={activation.lastError}
          onActivate={activation.activate}
          onClearError={activation.clearError}
        />
      )}
    </>
  );
}

export default App;
