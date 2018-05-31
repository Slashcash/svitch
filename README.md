## What is this?
Svitch is a Nintendo Switch savefile extractor, it just scans your system searching for savefiles and lists them at startup, you can then export them to your sd card into a convenient file format called .svi

## The .svi file format
After a succesful extraction you will find a .svi files into the *export* folder on your sd card named after the id of the game you extracted, this is nothing more than a glorified archive which contains your savefile and can be opened natively with every archive manager (such as WinZip, WinRar, 7Zip and so on).

This archive will also contain a *svitch_saveheader.svh* file. It is not part of your savefile, it contains human-readable additional informations about the save itself and it will also be used by Svitch to import the save back in the system (as soon as the function will be ready).

If you wish to import the .svi files back into your system with the use of a different tool you can easily decompress the archive, scrap away *svitch_saveheader.svh* and re-import it normally.

## What this will do soon?
Svitch will soon have a properly functioning importing function to reimport back your extracted saves. It will also feature a graphical ui instead of the current text-only interface. Svitch will hopefully become a full-fledged savefile manager for Nintendo Switch.

## How can i use it?
Just launch it as every other Nintendo Switch's homebrew, you can control it with:
  * +/- scrolls through all the savefiles.
  * Y exports the savefile.
  * B exits the software.

## Can i compile Svitch from source?
You are encouraged to do so, the *master* branch provides a convenient Makefile, just clone the repository and launch a make. It also provides a Code::Blocks .cbp file if you like it. You will need:
  * The switch devkitpro toolchain installed on your system
  * The freetype switch portlib

## Can i help?
Of course you can! I appreciate external helping, you can provide help by
  * Reviewing and improving the source code, you are encouraged to fork and request.
  * Testing the software intensively to search for bugs.
  * **Help me designing the UI:** i will soon need some graphical elements for the gui. Icons, buttons, some basic ui elements. And i'm not confident with graphics. If you do you can contact me.

## Can i test this on an emulator?
Although Nintendo Switch emulator currently exists they don't have savefiles support yet and they tend to crash when requesting a savefile. But luckily enough you can compile a version of Svitch that will succesfully launch in an emulator.
Compile the source with the command *make EMULATOR=1* and you will obtain a binary that can be launched on emulators. You can even scan for (fake) saves. You can contact me for additional information on this. 

## What if this crashes?
Svitch is still under **HEAVY TESTING**. As of now a lot of things can go wrong or don't work at all. I appreciate bug reporting: you can find additional informations about failures in a file called *log.txt* written in the software's root. This will help me debugging the error so please provide it when reporting.

## Disclaimer
Svitch *will* operate on your save file partition, as of now it just reads it (without writing to it) but it is a dangerous operation in any way. It may result in lost or damaged savefiles. For this reason i suggest you to backup your saves with another tool before trying this, this is still in **TESTING** 

# How can i download this?
As of now the official Svitch's download location is its Github Release page.
