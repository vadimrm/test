#include "stdafx.h"
const wchar_t* ProgramTitle = L"Dichotic Harmony Files Converter";
const wchar_t* ProgramCopyright = L"Copyright (c) 2010 Vadim Madgazin";
const wchar_t* VER_NUM = L"1.01"; // ������ �� 12 ������� 2010 �.


void main(int argc, char **argv)
{

  // ������ daccords ����
  DaccordsFile dfile;
  if ( !dfile.Read(infilew) )
  {
    Mbox(L"Error reading or processing file", infilew);
    return;
  }
  // Mbox( dfile.header_comment(), L"����� �������� � �����:", dfile.arr_accords().elements() );

  // ������ midi ����
  MidiFile mfile;
  bool res = mfile.DaccordsToMidi(dfile, midi_tracks_number, clks_per_beat);
  if (!res)
  {
    Mbox(L"Error in DaccordsToMidi() converter !");
    return;
  }

  // write the output midi file
  if ( !mfile.Write(outfilew) ) Mbox(L"Error writing file ", outfilew);
}

