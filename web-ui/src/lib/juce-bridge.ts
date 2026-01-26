/**
 * JUCE WebView Bridge
 *
 * Type definitions and utilities for communicating with JUCE 8's
 * WebBrowserComponent relay system.
 */

declare global {
  interface Window {
    __JUCE__?: {
      backend: {
        addEventListener(event: string, callback: (data: unknown) => void): void;
        removeEventListener(event: string, callback: (data: unknown) => void): void;
        emitEvent(event: string, data: unknown): void;
      };
      initialisationData?: unknown;
      getSliderState(name: string): SliderState | undefined;
      getToggleState(name: string): ToggleState | undefined;
    };
  }
}

export interface SliderState {
  name: string;
  getValue(): number;
  getNormalisedValue(): number;
  setValue(value: number): void;
  setNormalisedValue(value: number): void;
  getScaledValue(): number;
  setScaledValue(value: number): void;
  addEventListener(event: 'valueChanged', callback: () => void): void;
  removeEventListener(event: 'valueChanged', callback: () => void): void;
  sliderDragStarted(): void;
  sliderDragEnded(): void;
  properties: {
    start: number;
    end: number;
    interval: number;
    skew: number;
  };
}

export interface ToggleState {
  name: string;
  getValue(): boolean;
  setValue(value: boolean): void;
  addEventListener(event: 'valueChanged', callback: () => void): void;
  removeEventListener(event: 'valueChanged', callback: () => void): void;
}

export function getSliderState(name: string): SliderState | undefined {
  return window.__JUCE__?.getSliderState(name);
}

export function getToggleState(name: string): ToggleState | undefined {
  return window.__JUCE__?.getToggleState(name);
}

export function emitToJuce(event: string, data: unknown = {}): void {
  window.__JUCE__?.backend.emitEvent(event, data);
}

export function onJuceEvent(event: string, callback: (data: unknown) => void): () => void {
  window.__JUCE__?.backend.addEventListener(event, callback);
  return () => {
    window.__JUCE__?.backend.removeEventListener(event, callback);
  };
}

export function isInJuce(): boolean {
  return typeof window.__JUCE__ !== 'undefined';
}
