import { useState, useEffect, useCallback, useRef } from 'react';
import { getSliderState, getToggleState, SliderState, ToggleState } from '../lib/juce-bridge';

export function useSliderParam(paramId: string, defaultValue = 0.5) {
  const [value, setValue] = useState(defaultValue);
  const sliderRef = useRef<SliderState | undefined>(undefined);

  useEffect(() => {
    const slider = getSliderState(paramId);
    sliderRef.current = slider;

    if (slider) {
      setValue(slider.getNormalisedValue());

      const handleChange = () => {
        setValue(slider.getNormalisedValue());
      };

      slider.addEventListener('valueChanged', handleChange);
      return () => slider.removeEventListener('valueChanged', handleChange);
    }
  }, [paramId]);

  const setParamValue = useCallback((newValue: number) => {
    setValue(newValue);
    sliderRef.current?.setNormalisedValue(newValue);
  }, []);

  const dragStart = useCallback(() => {
    sliderRef.current?.sliderDragStarted();
  }, []);

  const dragEnd = useCallback(() => {
    sliderRef.current?.sliderDragEnded();
  }, []);

  return { value, setValue: setParamValue, dragStart, dragEnd };
}

export function useToggleParam(paramId: string, defaultValue = false) {
  const [value, setValue] = useState(defaultValue);
  const toggleRef = useRef<ToggleState | undefined>(undefined);

  useEffect(() => {
    const toggle = getToggleState(paramId);
    toggleRef.current = toggle;

    if (toggle) {
      setValue(toggle.getValue());

      const handleChange = () => {
        setValue(toggle.getValue());
      };

      toggle.addEventListener('valueChanged', handleChange);
      return () => toggle.removeEventListener('valueChanged', handleChange);
    }
  }, [paramId]);

  const setParamValue = useCallback((newValue: boolean) => {
    setValue(newValue);
    toggleRef.current?.setValue(newValue);
  }, []);

  const toggle = useCallback(() => {
    const newValue = !value;
    setValue(newValue);
    toggleRef.current?.setValue(newValue);
  }, [value]);

  return { value, setValue: setParamValue, toggle };
}
