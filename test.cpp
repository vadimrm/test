#include "stdafx.h"
const wchar_t* ProgramTitle = L"Dichotic Harmony Files Converter";
const wchar_t* ProgramCopyright = L"Copyright (c) 2010 Vadim Madgazin";
const wchar_t* VER_NUM = L"1.00"; // версия от 4 октября 2010 г.
/*
  =Сделано:
  -работает с libjdksmidi версии 5!
  -консольный конвертер daccords -> midi файл (формата 0 или 1), строка запуска:
   2h_converter  INFILE[.daccords]  [midi_tracks_number(1-4)  [clks_per_beat(100-1000)]]
   по умолчанию midi_tracks_number = 4 и clks_per_beat = 1000
  -в отличии от midi кода DHAG тут возможна смена номера инструмента у каждого аккорда!
  -некоторые файлы моего стандартного набора изменены, даже те, которые не используются
   в этом проекте, однако есть в директории проекта!
  -все Dichotic структуры и операции вынесены в отдельные файлы!
  -найден ещё один баг в библиотеке libjdksmidi, из-за которого формат миди файла не мог
   быть нулевым даже при наличии одного трека, см код в jdksmidi_filewritemultitrack.cpp
   bool MIDIFileWriteMultiTrack::Write ( int num_tracks, int division )
    //  writer.WriteFileHeader ( ( num_tracks > 0 ), num_tracks, division ); // JDK
        writer.WriteFileHeader ( ( num_tracks > 1 )? 1:0, num_tracks, division ); // @VRM!
  -найден баг в Сибелиусе вер. 4:
   если в одном аккорде есть 2 одинаковые ноты (напр. с разной панорамой), то они читаются
   неверно из midi файла формата 1 когда обе ноты расположены в одном треке - вместо двух
   нот нормальной длительности появляется одна минимально возможной длительности и пауза!
   но то же самое из midi файла с форматом 0 читается нормально!

  =Надо:
  -завести проект libjdkmidi, скопировать последний код из libjdksmidi!
  -возможен ли двойной язык комментариев в doxygen?
  -апдейт кода libjdkmidi:
   все мои исправления и улучшения, новый демо-пример, также исправить старые варнингсы!
  -перенести все канальные события в нотные треки: уст. панорамы и тембра!
   так Сибелиус правильно воспроизводит тембр нот трека!
   проверить чтобы rewrite_midifile не меняло файл!

  =Следующая версия:
  -апдейт кода libjdkmidi:
  -сделать варианты всех соотв. функций с wchar_t строками, в т.ч. текстовых строк внутри
   midi файлов - с Unicode заголовком! написать в midi.org ???

  -консольный конвертер midi в .daccords файл!
  -написать рекомендации для 2-го формата файла аккордов:
   изменить формат старых и добавить новые данные, обычно присутствующие в midi-партитуре
   Сибелиуса - размер такта, название иструмента...
   видимо надо для каждого голоса предусмотреть индивид-й номер инструмента и громкость, а
   также номер и стадию "легато" процесса (начало, продолжение, конец): номер легато нужен
   чтобы отличить в остальном полностью одинаковые ноты друг от друга!

  =Потом:
  -на основе кода этой программы проапдейтить код DHAG!
  -в midi может быть любая панорама - сделать её float в диапазоне -1.0 ... +1.0!

  =Важно:
  -текст META_INSTRUMENT_NAME в треке 0 Сибелиус использует как заголовок музыкального произведения!
  -текст META_INSTRUMENT_NAME в треках 1,2,3 Сибелиус использует как левый заголовок строки нотоносца!
  -текст META_LYRIC_TEXT в треках 1,2,3 Сибелиус использует как подстрочный текст строки нотоносца!
*/


// число треков в выходном midi файле, если 1 то формат midi файла=0, если 2 и больше, то формат=1
int midi_tracks_number = 4; // 1...4: 1-й трек служебный, а ноты в треках 2-4 по 3-м точкам панорамы

// число миди тиков в четверти (число в пределах 1...32767)
int clks_per_beat = 1000; // обычно 100...1000
// при tempo_times_32=60*32 разрешение времени (1 midi тик) будет = (1000/clks_per_beat) миллисекунд
// т.е. 1 мсек/тик при clks_per_beat=1000, и 10 мсек/тик при clks_per_beat=100
// обычно хватает и 100, но для очень быстрых произведений лучше 1000, т.к. длительность отдельных нот
// может составлять всего 50 миллисекунд и 10-мсек на тик даст погрешность этой длительности в 20%...


void main(int argc, char **argv)
{
  // получаем хендл и дескриптор окна консоли
  HWND chwnd = GetConsoleWindow();
  HINSTANCE chinst = GetModuleHandle(0);
/*
  // другой вариант:
  char title[500];  // to hold title
  GetConsoleTitleA( title, 500 );  // get title of console window
  HWND hwndConsole = FindWindowA( NULL, title );  // get HWND of console, based on its title
  HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwndConsole, GWL_HINSTANCE); // get HINSTANCE of console, based on HWND
*/
  MboxHWND = chwnd; // хендл родительского окна для Mbox() - окон
  MboxTitle = ProgramTitle; // их титульная строка

  // разбираемся с аргументами
  if ( argc < 2 )
  {
    Mbox(
      ProgramTitle, L"version", VER_NUM,
      L"\n\nUsage:\n\n2h_converter  INFILE[.daccords]  [midi_tracks_number(1-4)  [clks_per_beat(100-1000)]]",
      UNI_SPACE
      );
    return;
  }
  // else argc >= 2

  wstring2 infilew = argv[1]; // имя входного файла
  wstring2 outfilew = infilew + L".mid"; // имя выходного файла

  if ( argc >= 3 ) midi_tracks_number = atoi(argv[2]);
  if ( argc >= 4 ) clks_per_beat      = atoi(argv[3]);

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

