import React, { useRef, useCallback } from 'react';

interface KnobProps {
  value: number;
  onChange: (value: number) => void;
  onDragStart?: () => void;
  onDragEnd?: () => void;
  label: string;
  min?: number;
  max?: number;
  size?: number;
  color?: string;
}

export function Knob({
  value,
  onChange,
  onDragStart,
  onDragEnd,
  label,
  min = 0,
  max = 1,
  size = 80,
  color = '#c9a227',
}: KnobProps) {
  const knobRef = useRef<HTMLDivElement>(null);
  const isDragging = useRef(false);
  const startY = useRef(0);
  const startValue = useRef(0);

  const normalizedValue = (value - min) / (max - min);
  const rotation = -135 + normalizedValue * 270;

  const handleMouseDown = useCallback(
    (e: React.MouseEvent) => {
      isDragging.current = true;
      startY.current = e.clientY;
      startValue.current = value;
      onDragStart?.();

      const handleMouseMove = (moveEvent: MouseEvent) => {
        if (!isDragging.current) return;

        const deltaY = startY.current - moveEvent.clientY;
        const sensitivity = 0.005;
        const newValue = Math.max(
          min,
          Math.min(max, startValue.current + deltaY * sensitivity)
        );
        onChange(newValue);
      };

      const handleMouseUp = () => {
        isDragging.current = false;
        onDragEnd?.();
        window.removeEventListener('mousemove', handleMouseMove);
        window.removeEventListener('mouseup', handleMouseUp);
      };

      window.addEventListener('mousemove', handleMouseMove);
      window.addEventListener('mouseup', handleMouseUp);
    },
    [value, min, max, onChange, onDragStart, onDragEnd]
  );

  const displayValue = Math.round(normalizedValue * 100);

  return (
    <div className="knob-container" style={{ width: size }}>
      <div
        ref={knobRef}
        className="knob"
        style={{
          width: size,
          height: size,
          transform: `rotate(${rotation}deg)`,
          cursor: 'grab',
        }}
        onMouseDown={handleMouseDown}
      >
        <div className="knob-indicator" style={{ backgroundColor: color }} />
      </div>
      <div className="knob-value">{displayValue}%</div>
      <div className="knob-label">{label}</div>
    </div>
  );
}
