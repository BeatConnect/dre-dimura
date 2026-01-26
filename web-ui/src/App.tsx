import { useSliderParam, useToggleParam } from './hooks/useJuceParam';
import { useActivation } from './hooks/useActivation';
import { ActivationDialog } from './components/ActivationDialog';
import { Knob } from './components/Knob';

function App() {
  const { showDialog } = useActivation();

  // Parameter hooks - IDs must match C++ ParameterIDs exactly
  const drive = useSliderParam('drive', 0.25);
  const tone = useSliderParam('tone', 0.5);
  const output = useSliderParam('output', 0.5);
  const bypass = useToggleParam('bypass', false);

  return (
    <div className={`plugin-container ${bypass.value ? 'bypassed' : ''}`}>
      <header className="plugin-header">
        <h1>DRE-DIMURA</h1>
        <span className="plugin-subtitle">Vintage Preamp Coloration</span>
      </header>

      <main className="plugin-controls">
        <Knob
          value={drive.value}
          onChange={drive.setValue}
          onDragStart={drive.dragStart}
          onDragEnd={drive.dragEnd}
          label="Drive"
          color="#e85d04"
        />

        <Knob
          value={tone.value}
          onChange={tone.setValue}
          onDragStart={tone.dragStart}
          onDragEnd={tone.dragEnd}
          label="Tone"
          color="#c9a227"
        />

        <Knob
          value={output.value}
          onChange={output.setValue}
          onDragStart={output.dragStart}
          onDragEnd={output.dragEnd}
          label="Output"
          color="#4cc9f0"
        />
      </main>

      <footer className="plugin-footer">
        <button
          className={`bypass-button ${bypass.value ? 'active' : ''}`}
          onClick={bypass.toggle}
        >
          {bypass.value ? 'BYPASSED' : 'ACTIVE'}
        </button>
      </footer>

      {showDialog && <ActivationDialog />}
    </div>
  );
}

export default App;
