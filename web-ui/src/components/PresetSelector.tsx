import React, { useState, useRef, useEffect, useCallback } from 'react';
import { Preset, getPresetsForPreamp } from '../data/presets';

interface PresetSelectorProps {
  preampId: 'cathode' | 'filament' | 'steelplate';
  currentPreset: Preset | null;
  onPresetChange: (preset: Preset) => void;
}

export function PresetSelector({ preampId, currentPreset, onPresetChange }: PresetSelectorProps) {
  const [isOpen, setIsOpen] = useState(false);
  const [isAnimating, setIsAnimating] = useState(false);
  const dropdownRef = useRef<HTMLDivElement>(null);
  const buttonRef = useRef<HTMLButtonElement>(null);

  const presets = getPresetsForPreamp(preampId);
  const currentIndex = currentPreset ? presets.findIndex(p => p.id === currentPreset.id) : 0;

  // Close dropdown when clicking outside
  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (dropdownRef.current && !dropdownRef.current.contains(event.target as Node)) {
        setIsOpen(false);
      }
    }

    if (isOpen) {
      document.addEventListener('mousedown', handleClickOutside);
      return () => document.removeEventListener('mousedown', handleClickOutside);
    }
  }, [isOpen]);

  // Close dropdown when preamp changes
  useEffect(() => {
    setIsOpen(false);
  }, [preampId]);

  const handlePresetSelect = useCallback((preset: Preset) => {
    if (preset.id === currentPreset?.id) {
      setIsOpen(false);
      return;
    }

    setIsAnimating(true);
    setIsOpen(false);
    onPresetChange(preset);

    // Reset animation state after transition
    setTimeout(() => {
      setIsAnimating(false);
    }, 350);
  }, [currentPreset, onPresetChange]);

  const navigatePreset = useCallback((direction: 'prev' | 'next') => {
    const newIndex = direction === 'prev'
      ? Math.max(0, currentIndex - 1)
      : Math.min(presets.length - 1, currentIndex + 1);

    if (newIndex !== currentIndex) {
      handlePresetSelect(presets[newIndex]);
    }
  }, [currentIndex, presets, handlePresetSelect]);

  const handleKeyDown = useCallback((e: React.KeyboardEvent) => {
    if (e.key === 'Escape') {
      setIsOpen(false);
    } else if (e.key === 'ArrowUp' || e.key === 'ArrowLeft') {
      e.preventDefault();
      navigatePreset('prev');
    } else if (e.key === 'ArrowDown' || e.key === 'ArrowRight') {
      e.preventDefault();
      navigatePreset('next');
    }
  }, [navigatePreset]);

  return (
    <div
      className={`preset-selector ${preampId} ${isAnimating ? 'animating' : ''}`}
      ref={dropdownRef}
      onKeyDown={handleKeyDown}
    >
      {/* Left Arrow */}
      <button
        className={`preset-nav-arrow left ${currentIndex === 0 ? 'disabled' : ''}`}
        onClick={() => navigatePreset('prev')}
        disabled={currentIndex === 0}
        aria-label="Previous preset"
      >
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <polyline points="15,18 9,12 15,6" />
        </svg>
      </button>

      {/* Main Dropdown Button */}
      <button
        ref={buttonRef}
        className={`preset-button ${isOpen ? 'open' : ''}`}
        onClick={() => setIsOpen(!isOpen)}
        aria-expanded={isOpen}
        aria-haspopup="listbox"
      >
        <span className="preset-button-name">
          {currentPreset?.name || 'Select Preset'}
        </span>
        <svg className="preset-button-chevron" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <polyline points="6,9 12,15 18,9" />
        </svg>
      </button>

      {/* Right Arrow */}
      <button
        className={`preset-nav-arrow right ${currentIndex === presets.length - 1 ? 'disabled' : ''}`}
        onClick={() => navigatePreset('next')}
        disabled={currentIndex === presets.length - 1}
        aria-label="Next preset"
      >
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <polyline points="9,6 15,12 9,18" />
        </svg>
      </button>

      {/* Dropdown Menu */}
      {isOpen && (
        <div className="preset-dropdown" role="listbox">
          <div className="preset-dropdown-content">
            {presets.map((preset, index) => (
              <button
                key={preset.id}
                className={`preset-item ${preset.id === currentPreset?.id ? 'active' : ''}`}
                onClick={() => handlePresetSelect(preset)}
                role="option"
                aria-selected={preset.id === currentPreset?.id}
                style={{ animationDelay: `${index * 0.03}s` }}
              >
                <div className="preset-item-content">
                  <span className="preset-item-number">{String(index + 1).padStart(2, '0')}</span>
                  <div className="preset-item-text">
                    <span className="preset-item-name">{preset.name}</span>
                    <span className="preset-item-description">{preset.description}</span>
                  </div>
                </div>
                {preset.id === currentPreset?.id && (
                  <div className="preset-item-active-indicator" />
                )}
              </button>
            ))}
          </div>
        </div>
      )}
    </div>
  );
}
