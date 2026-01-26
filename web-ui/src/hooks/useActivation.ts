import { useState, useEffect, useCallback } from 'react';
import { onJuceEvent, emitToJuce, isInJuce } from '../lib/juce-bridge';

export interface ActivationInfo {
  activationCode: string;
  machineId: string;
  activatedAt: string;
  currentActivations: number;
  maxActivations: number;
  isValid: boolean;
}

export interface ActivationState {
  isConfigured: boolean;
  isActivated: boolean;
  info?: ActivationInfo;
}

export function useActivation() {
  const [state, setState] = useState<ActivationState>({
    isConfigured: false,
    isActivated: false,
  });
  const [showDialog, setShowDialog] = useState(false);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (!isInJuce()) return;

    emitToJuce('getActivationStatus');

    const unsubState = onJuceEvent('activationState', (data) => {
      const d = data as ActivationState;
      setState(d);
      if (d.isConfigured && !d.isActivated) {
        setShowDialog(true);
      }
    });

    const unsubResult = onJuceEvent('activationResult', (data) => {
      const d = data as { status: string; info?: ActivationInfo };
      setIsLoading(false);
      if (d.status === 'Valid' && d.info) {
        setState({ isConfigured: true, isActivated: true, info: d.info });
        setShowDialog(false);
        setError(null);
      } else {
        setError(getErrorMessage(d.status));
      }
    });

    const unsubDeactivate = onJuceEvent('deactivationResult', (data) => {
      const d = data as { status: string };
      setIsLoading(false);
      if (d.status === 'Valid' || d.status === 'Deactivated') {
        setState({ isConfigured: true, isActivated: false });
        setShowDialog(true);
      }
    });

    return () => {
      unsubState();
      unsubResult();
      unsubDeactivate();
    };
  }, []);

  const activate = useCallback((code: string) => {
    setIsLoading(true);
    setError(null);
    emitToJuce('activateLicense', { code });
  }, []);

  const deactivate = useCallback(() => {
    setIsLoading(true);
    setError(null);
    emitToJuce('deactivateLicense', {});
  }, []);

  return {
    ...state,
    showDialog,
    isLoading,
    error,
    activate,
    deactivate,
    closeDialog: () => setShowDialog(false),
  };
}

function getErrorMessage(status: string): string {
  switch (status) {
    case 'Invalid': return 'Invalid activation code';
    case 'Expired': return 'This license has expired';
    case 'MaxActivationsReached': return 'Maximum activations reached';
    case 'NetworkError': return 'Network error. Check your connection.';
    case 'ServerError': return 'Server error. Try again later.';
    default: return 'Activation failed';
  }
}
