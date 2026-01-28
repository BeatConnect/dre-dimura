/**
 * React Hook for BeatConnect Activation System
 *
 * Manages license activation state and communication with C++ backend.
 * When running outside JUCE (browser dev), activation is skipped entirely.
 */

import { useState, useEffect, useCallback } from 'react';
import { isInJuceWebView, addCustomEventListener } from '../lib/juce-bridge';

// Activation status codes (match C++ ActivationStatus enum)
export type ActivationStatus =
  | 'valid'
  | 'invalid'
  | 'revoked'
  | 'max_reached'
  | 'network_error'
  | 'server_error'
  | 'not_configured'
  | 'already_active'
  | 'not_activated'
  | 'unknown';

export interface ActivationInfo {
  activationCode: string;
  machineId: string;
  activatedAt: string;
  expiresAt?: string;
  currentActivations: number;
  maxActivations: number;
  isValid: boolean;
}

export interface ActivationState {
  isConfigured: boolean;      // SDK configured (activation enabled in build)
  isActivated: boolean;       // Currently activated
  isLoading: boolean;         // Network operation in progress
  lastStatus?: ActivationStatus;
  lastError?: string;
  info?: ActivationInfo;
}

// Status message helper
function getStatusMessage(status: ActivationStatus): string {
  switch (status) {
    case 'valid': return 'Activation successful';
    case 'invalid': return 'Invalid activation code';
    case 'revoked': return 'This license has been revoked';
    case 'max_reached': return 'Maximum activations reached for this license';
    case 'network_error': return 'Network error — please check your connection';
    case 'server_error': return 'Server error — please try again later';
    case 'not_configured': return 'Activation not configured';
    case 'already_active': return 'Already activated';
    case 'not_activated': return 'Not activated';
    default: return 'Unknown error';
  }
}

/**
 * Hook for managing license activation state.
 * Communicates with the C++ backend via JUCE WebView events.
 * When not in JUCE (browser dev), activation is skipped.
 */
export function useActivation() {
  const [state, setState] = useState<ActivationState>({
    isConfigured: false,
    isActivated: false,
    isLoading: false,
  });

  // Whether the activation screen should be shown
  const showActivation = state.isConfigured && !state.isActivated;

  // Initialize listeners for C++ events
  useEffect(() => {
    if (!isInJuceWebView()) {
      // Outside JUCE: skip activation, allow full access
      console.log('[Activation] Not in JUCE WebView - activation skipped');
      setState(s => ({ ...s, isConfigured: false, isActivated: true }));
      return;
    }

    console.log('[Activation] Setting up C++ event listeners');

    // Listen for activation state updates from C++
    const unsubState = addCustomEventListener('activationState', (data: unknown) => {
      console.log('[Activation] activationState event received:', data);
      const eventData = data as {
        isConfigured: boolean;
        isActivated: boolean;
        info?: ActivationInfo;
      };
      setState(s => ({
        ...s,
        isConfigured: eventData.isConfigured,
        isActivated: eventData.isActivated,
        info: eventData.info,
      }));
    });

    // Listen for activation results
    const unsubResult = addCustomEventListener('activationResult', (data: unknown) => {
      const result = data as { status: ActivationStatus; info?: ActivationInfo };
      console.log('[Activation] activationResult:', result);

      const isSuccess = result.status === 'valid' || result.status === 'already_active';
      setState(s => ({
        ...s,
        isLoading: false,
        lastStatus: result.status,
        isActivated: isSuccess,
        info: result.info,
        lastError: isSuccess ? undefined : getStatusMessage(result.status),
      }));
    });

    // Listen for deactivation results
    const unsubDeactivate = addCustomEventListener('deactivationResult', (data: unknown) => {
      const result = data as { status: ActivationStatus };
      console.log('[Activation] deactivationResult:', result);

      const isSuccess = result.status === 'valid';
      setState(s => ({
        ...s,
        isLoading: false,
        lastStatus: result.status,
        isActivated: !isSuccess,
        info: isSuccess ? undefined : s.info,
        lastError: isSuccess ? undefined : getStatusMessage(result.status),
      }));
    });

    // Request initial state from C++
    console.log('[Activation] Requesting initial activation status');
    window.__JUCE__!.backend.emitEvent('getActivationStatus', {});

    return () => {
      unsubState();
      unsubResult();
      unsubDeactivate();
    };
  }, []);

  // Activate with license code
  const activate = useCallback((code: string) => {
    if (!isInJuceWebView()) return;

    setState(s => ({ ...s, isLoading: true, lastError: undefined }));
    window.__JUCE__!.backend.emitEvent('activateLicense', { code });
  }, []);

  // Deactivate current license
  const deactivate = useCallback(() => {
    if (!isInJuceWebView()) return;

    setState(s => ({ ...s, isLoading: true }));
    window.__JUCE__!.backend.emitEvent('deactivateLicense', {});
  }, []);

  // Clear error state
  const clearError = useCallback(() => {
    setState(s => ({ ...s, lastError: undefined }));
  }, []);

  return {
    // State
    isConfigured: state.isConfigured,
    isActivated: state.isActivated,
    isLoading: state.isLoading,
    lastStatus: state.lastStatus,
    lastError: state.lastError,
    info: state.info,
    showActivation,

    // Actions
    activate,
    deactivate,
    clearError,
  };
}
