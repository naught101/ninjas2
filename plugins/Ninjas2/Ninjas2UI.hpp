/*
 * Ninjas 2
 *
 * Copyright (C) 2018 Clearly Broken Software
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef NINJAS_UI_HPP_INCLUDED
#define NINJAS_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "Window.hpp"
#include "sndfile.hh"
#include "DistrhoPlugin.hpp"
#include "iterator"
#include "algorithm"
#include "array"
#include "aubio.h"
#include <math.h>
#include <string>
#include <cstdlib>
#include <samplerate.h>

//Wolf Widgets
#include "VolumeKnob.hpp"
#include "BipolarModeSwitch.hpp"
#include "GlowingLabelsBox.hpp"
#include "RemoveDCSwitch.hpp"
#include "SpinBox.hpp"
#include "TextButton.hpp"
#include "PlayModeSwitch.hpp"
#include "StateSwitch.hpp"

#include "Ninjas2Resources.hpp"
#include "Ninjas2Plugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class NinjasUI : public UI,
     public NanoKnob::Callback,
     public NanoSwitch::Callback,
     public NanoSpinBox::Callback,
     public NanoButton::Callback,
     public IdleCallback {
public:
     NinjasUI();

protected:
     void positionWidgets();

     // -------------------------------------------------------------------
     // DSP Callbacks

     void parameterChanged ( uint32_t index, float value ) override;
     void uiFileBrowserSelected ( const char* filename ) override;
     void stateChanged ( const char* key, const char* value ) override;

     // -------------------------------------------------------------------
     // Widget Callbacks

     void onNanoDisplay() override;
     void idleCallback() override;
     void nanoKnobValueChanged ( NanoKnob* nanoKnob, const float value ) override;
     void nanoSpinBoxValueChanged ( NanoSpinBox *nanoSpinBox, const float value ) override;
     void nanoSwitchClicked ( NanoSwitch* nanoSwitch, const MouseEvent &ev ) override;
     void nanoButtonClicked ( NanoButton* nanoButton ) override;
     bool onMouse ( const MouseEvent& ) override;
     bool onScroll ( const ScrollEvent& ) override;
     bool onMotion ( const MotionEvent& ) override;

private:
     ScopedPointer<VolumeKnob> fKnobAttack, fKnobDecay, fKnobSustain, fKnobRelease, fKnobSliceSensitivity, fKnobPitchbendDepth;
     ScopedPointer<SpinBox> fSpinBox;
     ScopedPointer<BipolarModeSwitch> fSliceModeSlider;
     ScopedPointer<GlowingLabelsBox> fLabelsBoxSliceModeSlider, fLabelsBoxSliceSensitivity, fLabelsBoxLoadSample;
     ScopedPointer<PlayModeSwitch> fSwitchFwd, fSwitchRev, fSwitchLoopFwd, fSwitchLoopRev;
     ScopedPointer<RemoveDCSwitch> fSwitchLoadSample;
     ScopedPointer<StateSwitch> fGrid[16];
     ScopedPointer<TextButton> fSliceButton;

     const NinjasPlugin * plugin;
     void loadSample ( bool fromUser );
     std::vector<short int> waveform;
     void createSlicesRaw ();
     void createSlicesOnsets ();
     void removeSlice(const int slice);
     void insertSlice(const int slice, const int position);
     int64_t find_nearest ( std::vector<uint_t> & haystack, uint_t needle );
     void recallSliceSettings ( int slice );
     void drawWaveform();
     void drawRuler();
     void drawOnsets();
     void drawSliceMarkers();
     void drawCurrentSlice();
     void drawPlayheads();
     void getVisibleSlices ( int &firstSice, int &lastSlice );
     void selectSlice();
     void editCurrentSlice();
     void editSlice();
     bool sampleIsInSlice ( long unsigned int sample );
     void setProgramGrid ( int program );
     void ProgramGrid ( int grid );
     void getProgram ( int program );
     void getOnsets();

     float p_Attack[128], p_Decay[128], p_Sustain[128], p_Release[128];

     std::string dirnameOf ( const std::string& fname );
     std::string toTime ( double time, double round_up );
     int programNumber; // 0 - 15

     //
     bool mouseDragging;
     uint mouseX;
     uint mouseY;
     int mouseDistance;
     int lastClick;

     enum SLICEMODE {
          RAW,
          ONSETS,
          MANUAL
     };

     SLICEMODE enum_slicemode;
     //temp FIXME
     int slicemode;
     bool slicemodeChanged;

     enum slicePlayMode {
          ONE_SHOT_FWD ,
          ONE_SHOT_REV,
          LOOP_FWD,
          LOOP_REV
     };
     slicePlayMode p_playMode[128];
     bool fwd,rev,loop,loop_rev;


     struct Slice {
          unsigned long int sliceStart;
          unsigned long int sliceEnd;
          slicePlayMode playmode;
          Rectangle<int> bothHitBox = Rectangle<int> ( 0,0,20,20 );
          Rectangle<int> startHitBox = Rectangle<int> ( 0,0,10,10 );
          Rectangle<int> endHitBox  = Rectangle<int> ( 0,0,10,10 );
     };

     struct program {
          int program_slices; // number of slices ..
          int program_currentslice;
          Slice program_a_slices[128];
          float program_Attack[128];
          float program_Decay[128];
          float program_Sustain[128];
          float program_Release[128];
          float program_OneShotFwd[128];
          float program_OneShotRev[128];
          float program_LoopFwd[128];
          float program_LoopRev[128];
          bool program_isEmpty;
     };
     program Programs[16];

     bool slicemethod;
     int currentSlice;
     enum editslicestartend {
          start = 0,
          end,
          both
     };

     editslicestartend editSliceStartEnd;
     bool mouseEditSlice;
     int slices;
     int tempSlices;
     Slice a_slices[128];
     Slice currentEditSlice;
     std::vector<uint_t>onsets;

     struct WaveView {
          unsigned long int start;
          unsigned long int end;
          float zoom; // sample lenght / display width
          float max_zoom;
     };

     WaveView waveView;
     uint64_t sampleSize;
     int sampleChannels;
     std::vector<float>sampleVector;
     bool sample_is_loaded;
     double samplerate;
     Window::FileBrowserOptions filebrowseropts;
     std::string directory;
     std::string stateSlice;
     Rectangle<int> display;
     bool mouseMoveWaveform;
     //  int currentProgram;
     uint programGrid;
     bool sliceButton;
     bool sig_LoadProgram {false};

     // need static constexpr apparently because of std::array ..

     static constexpr unsigned int display_left = 30;
     static constexpr unsigned int display_width = 1140;
     static constexpr unsigned int display_right = 30 + display_width;
     static constexpr unsigned int display_top = 600 - 200 - 350;
     static constexpr unsigned int display_bottom = 600-200 ;
     static constexpr unsigned int display_center = ( display_bottom - display_top ) / 2 + display_top;
     static constexpr unsigned int display_length = display_right - display_left;
     static constexpr unsigned int display_height = ( display_bottom - display_top ) /2;
     //static constexpr unsigned int waveform_length = display_length * 2;

     // font
     FontId fNanoFont;
     FontId Labels;

     NanoImage imgNinjasLogo,imgClearlyBroken;

     DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ( NinjasUI )
     void initParameters();
     void initSlices();

};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // 
