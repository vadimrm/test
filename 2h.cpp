#include "stdafx.h"
const wchar_t* ProgramTitle = L"Dichotic Harmony Files Converter";
const wchar_t* ProgramCopyright = L"Copyright (c) 2010 Vadim Madgazin";
const wchar_t* VER_NUM = L"1.01"; // верси€ от 12 окт€бр€ 2010 г.


void main(int argc, char **argv)
{

  wstring2 infilew = argv[1]; // им€ входного файла
  wstring2 outfilew = infilew + L".mid"; // им€ выходного файла


  // читаем daccords файл
  DaccordsFile dfile;
  if ( !dfile.Read(infilew) )
  {
    Mbox(L"Error reading or processing file", infilew);
    return;
  }
  // Mbox( dfile.header_comment(), L"„исло аккордов в файле:", dfile.arr_accords().elements() );

  // создаЄм midi файл
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

