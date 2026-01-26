import React, { useState } from 'react';
import { useActivation } from '../hooks/useActivation';

export function ActivationDialog() {
  const { activate, isLoading, error } = useActivation();
  const [code, setCode] = useState('');

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (code.trim()) {
      activate(code.trim());
    }
  };

  return (
    <div className="activation-overlay">
      <div className="activation-dialog">
        <h2>Activate Dre-Dimura</h2>
        <p>Enter your license key to activate the plugin.</p>

        <form onSubmit={handleSubmit}>
          <input
            type="text"
            value={code}
            onChange={(e) => setCode(e.target.value)}
            placeholder="XXXX-XXXX-XXXX-XXXX"
            disabled={isLoading}
            className="activation-input"
            autoFocus
          />

          {error && <div className="activation-error">{error}</div>}

          <button
            type="submit"
            disabled={isLoading || !code.trim()}
            className="activation-button"
          >
            {isLoading ? 'Activating...' : 'Activate'}
          </button>
        </form>

        <div className="activation-footer">
          <a href="https://beatconnect.io" target="_blank" rel="noopener noreferrer">
            Get a license
          </a>
        </div>
      </div>
    </div>
  );
}
