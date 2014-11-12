What this program does?
  - Translates files written in utf8 to utf32
  - Translates files written in utf32 to utf8

How it works?
  - In order to translate from utf8 to utf32 you can create a main in c which open two files, the files that you want to 
be translated and the file in which you want to be written the translation. Open it with fopen and pass to conv8_32 both 
pointers (first the file to be translated and then the file to be written) and the order that you want to be written 
('L' for little ending and 'B' for big ending).The function conv8_32 can be found in utfconv8.c and utfconv.c
  - In order to translate from utf8 to utf32 you can create a main in c which open two files, the files that you want to 
be translated and the file in which you want to be written the translation. Open it with fopen and pass to conv8_32 both 
pointers (first the file to be translated and then the file to be written). The function conv8_32 can be found in
utfconv32.c and utfconv.c
