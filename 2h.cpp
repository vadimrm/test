#include "stdafx.h"
const wchar_t* ProgramTitle = L"Dichotic Harmony Files Converter";
const wchar_t* ProgramCopyright = L"Copyright (c) 2010 Vadim Madgazin";
const wchar_t* VER_NUM = L"1.01"; // версия от 12 октября 2010 г.


void main(int argc, char **argv)
{

  // читаем daccords файл
  DaccordsFile dfile;
  if ( !dfile.Read(infilew) )
  {
    Mbox(L"Error reading or processing file", infilew);
    return;
  }
  // Mbox( dfile.header_comment(), L"Число аккордов в файле:", dfile.arr_accords().elements() );

  // создаём midi файл
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

