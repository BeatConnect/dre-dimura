import React, { useState, useRef, useEffect, useCallback } from 'react';

interface ActivationScreenProps {
  onActivated: () => void;
  isActivated: boolean;
  isLoading: boolean;
  lastError?: string;
  onActivate: (code: string) => void;
  onClearError: () => void;
}

export function ActivationScreen({
  onActivated,
  isActivated,
  isLoading,
  lastError,
  onActivate,
  onClearError,
}: ActivationScreenProps) {
  const [code, setCode] = useState('');
  const [isUnlocking, setIsUnlocking] = useState(false);
  const [isFadingOut, setIsFadingOut] = useState(false);
  const [shakeKey, setShakeKey] = useState(0);
  const inputRef = useRef<HTMLInputElement>(null);
  const hasTriggeredUnlock = useRef(false);

  // Focus input on mount
  useEffect(() => {
    setTimeout(() => inputRef.current?.focus(), 500);
  }, []);

  // Play unlock animation when activation succeeds
  const triggerUnlockSequence = useCallback(() => {
    if (hasTriggeredUnlock.current) return;
    hasTriggeredUnlock.current = true;

    setIsUnlocking(true);

    // After gate opens, start fade out and notify parent
    setTimeout(() => {
      setIsFadingOut(true);
      onActivated(); // Parent will delay unmount until fade completes
    }, 1600);
  }, [onActivated]);

  // Watch for activation success
  useEffect(() => {
    if (isActivated && !isUnlocking) {
      triggerUnlockSequence();
    }
  }, [isActivated, isUnlocking, triggerUnlockSequence]);

  // Watch for activation errors
  useEffect(() => {
    if (lastError && !isLoading) {
      setShakeKey(prev => prev + 1);
      setCode('');
      inputRef.current?.focus();
    }
  }, [lastError, isLoading]);

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!code.trim() || isLoading || isUnlocking) return;

    onClearError();
    onActivate(code.trim());
  };

  const handleCodeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setCode(e.target.value.toUpperCase());
    onClearError();
  };

  return (
    <div className={`activation-screen ${isUnlocking ? 'unlocking' : ''} ${isFadingOut ? 'fading-out' : ''}`}>
      {/* Ambient background effects */}
      <div className="activation-ambient">
        <div className="ambient-glow glow-1" />
        <div className="ambient-glow glow-2" />
        <div className="ambient-glow glow-3" />
      </div>

      {/* The Gate */}
      <div className="gate-container">
        {/* Left Gate Door */}
        <div className="gate-door gate-left">
          <div className="gate-panel">
            <div className="gate-texture" />
            <div className="gate-rivets">
              {[...Array(6)].map((_, i) => (
                <div key={`l-${i}`} className="rivet" />
              ))}
            </div>
            <div className="gate-edge" />
          </div>
        </div>

        {/* Right Gate Door */}
        <div className="gate-door gate-right">
          <div className="gate-panel">
            <div className="gate-texture" />
            <div className="gate-rivets">
              {[...Array(6)].map((_, i) => (
                <div key={`r-${i}`} className="rivet" />
              ))}
            </div>
            <div className="gate-edge" />
          </div>
        </div>

        {/* Center Lock Mechanism */}
        <div className={`lock-mechanism ${isLoading ? 'validating' : ''} ${isUnlocking ? 'unlocked' : ''}`}>
          <div className="lock-outer-ring">
            <div className="lock-inner-ring">
              <div className="lock-core">
                <div className="lock-keyhole">
                  <div className="keyhole-top" />
                  <div className="keyhole-bottom" />
                </div>
              </div>
            </div>
            {/* Rotating segments */}
            <div className="lock-segments">
              {[...Array(8)].map((_, i) => (
                <div key={i} className="lock-segment" style={{ '--i': i } as React.CSSProperties} />
              ))}
            </div>
          </div>

          {/* Lock status indicator */}
          <div className="lock-status">
            {isUnlocking ? (
              <svg viewBox="0 0 24 24" className="lock-icon unlocked">
                <path d="M12 17a2 2 0 0 0 2-2v-2a2 2 0 0 0-4 0v2a2 2 0 0 0 2 2zm6-7V7a6 6 0 0 0-12 0v1h2V7a4 4 0 0 1 8 0v3H6a2 2 0 0 0-2 2v8a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-8a2 2 0 0 0-2-2h-2z"/>
              </svg>
            ) : (
              <svg viewBox="0 0 24 24" className="lock-icon locked">
                <path d="M12 17a2 2 0 0 0 2-2v-2a2 2 0 0 0-4 0v2a2 2 0 0 0 2 2zm6-9V7a6 6 0 0 0-12 0v1a2 2 0 0 0-2 2v8a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-8a2 2 0 0 0-2-2zM8 7a4 4 0 0 1 8 0v1H8V7z"/>
              </svg>
            )}
          </div>
        </div>

        {/* Gate frame */}
        <div className="gate-frame">
          <div className="frame-top" />
          <div className="frame-left" />
          <div className="frame-right" />
          <div className="frame-corner tl" />
          <div className="frame-corner tr" />
        </div>
      </div>

      {/* Content behind the gate (revealed on unlock) */}
      <div className="gate-interior">
        <div className="interior-glow" />
        <h1 className="interior-title">DRE DIMURA</h1>
        <span className="interior-subtitle">Welcome</span>
      </div>

      {/* Input Area - In front of gate */}
      <div className={`activation-input-area ${isUnlocking ? 'hidden' : ''}`}>
        <div className="brand-header">
          <h1 className="activation-brand">DRE DIMURA</h1>
          <span className="activation-tagline">Audio Systems</span>
        </div>

        <form onSubmit={handleSubmit} className="activation-form">
          <div className={`input-container ${lastError ? 'error' : ''} ${isLoading ? 'validating' : ''}`} key={shakeKey}>
            <input
              ref={inputRef}
              type="text"
              value={code}
              onChange={handleCodeChange}
              placeholder="Enter activation code"
              className="activation-input"
              disabled={isLoading || isUnlocking}
              maxLength={48}
              autoComplete="off"
              spellCheck={false}
            />
            <div className="input-glow" />
            <div className="input-border" />
          </div>

          {lastError && (
            <div className="activation-error">
              <svg viewBox="0 0 24 24" fill="currentColor">
                <path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/>
              </svg>
              {lastError}
            </div>
          )}

          <button
            type="submit"
            className={`activation-button ${isLoading ? 'loading' : ''}`}
            disabled={!code.trim() || isLoading || isUnlocking}
          >
            {isLoading ? (
              <>
                <span className="button-spinner" />
                Validating
              </>
            ) : (
              <>
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                  <path d="M15 3h4a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-4M10 17l5-5-5-5M13.8 12H3"/>
                </svg>
                Activate
              </>
            )}
          </button>
        </form>

        <div className="activation-footer">
          <span>Need a license? Visit </span>
          <a href="https://beatconnect.io" target="_blank" rel="noopener noreferrer">beatconnect.io</a>
        </div>
      </div>

      {/* Unlock particles */}
      {isUnlocking && (
        <div className="unlock-particles">
          {[...Array(20)].map((_, i) => (
            <div key={i} className="particle" style={{ '--i': i } as React.CSSProperties} />
          ))}
        </div>
      )}
    </div>
  );
}
