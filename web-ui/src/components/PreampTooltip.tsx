import React, { useState, useRef, useEffect } from 'react';

interface TooltipContent {
  title: string;
  description: string;
}

const TOOLTIP_CONTENT: Record<'cathode' | 'filament' | 'steelplate', TooltipContent> = {
  cathode: {
    title: 'Tube Saturation',
    description: 'Asymmetric harmonic saturation modeled on classic tube preamps. Low drive adds subtle even-order warmth; higher settings introduce musical compression and soft clipping. The effect chain emphasizes tape-style modulation and dark, diffuse reverbs.'
  },
  filament: {
    title: 'Digital Precision',
    description: 'Clean, linear gain staging with high headroom and transparent dynamics. Optimized for sources that need clarity without coloration. The effect chain features pristine delays, shimmer reverbs, and through-zero modulation for modern production.'
  },
  steelplate: {
    title: 'Transformer Drive',
    description: 'Aggressive solid-state saturation with hard clipping and odd-order harmonics. Adds weight and presence to drums, bass, and distorted sources. The effect chain includes gated reverb, bit-reduction, and resonant filtering for industrial textures.'
  }
};

interface PreampTooltipProps {
  preampId: 'cathode' | 'filament' | 'steelplate';
  children: React.ReactNode;
}

export function PreampTooltipTrigger({ preampId, children }: PreampTooltipProps) {
  const [isVisible, setIsVisible] = useState(false);
  const [isAnimatingOut, setIsAnimatingOut] = useState(false);
  const triggerRef = useRef<HTMLDivElement>(null);
  const timeoutRef = useRef<ReturnType<typeof setTimeout> | null>(null);

  const content = TOOLTIP_CONTENT[preampId];

  const handleMouseEnter = () => {
    if (timeoutRef.current) {
      clearTimeout(timeoutRef.current);
    }
    setIsAnimatingOut(false);
    timeoutRef.current = setTimeout(() => {
      setIsVisible(true);
    }, 150); // Small delay before showing
  };

  const handleMouseLeave = () => {
    if (timeoutRef.current) {
      clearTimeout(timeoutRef.current);
    }
    setIsAnimatingOut(true);
    timeoutRef.current = setTimeout(() => {
      setIsVisible(false);
      setIsAnimatingOut(false);
    }, 200); // Animation duration
  };

  useEffect(() => {
    return () => {
      if (timeoutRef.current) {
        clearTimeout(timeoutRef.current);
      }
    };
  }, []);

  // Hide tooltip when preamp changes
  useEffect(() => {
    setIsVisible(false);
    setIsAnimatingOut(false);
  }, [preampId]);

  return (
    <div
      className="preamp-tooltip-trigger"
      ref={triggerRef}
      onMouseEnter={handleMouseEnter}
      onMouseLeave={handleMouseLeave}
    >
      {children}

      {isVisible && (
        <div className={`preamp-tooltip ${preampId} ${isAnimatingOut ? 'exiting' : 'entering'}`}>
          <div className="tooltip-accent-line" />
          <h3 className="tooltip-title">{content.title}</h3>
          <p className="tooltip-description">{content.description}</p>
          <div className="tooltip-corner tl" />
          <div className="tooltip-corner tr" />
          <div className="tooltip-corner bl" />
          <div className="tooltip-corner br" />
        </div>
      )}
    </div>
  );
}
