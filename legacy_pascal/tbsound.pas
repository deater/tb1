{       SMIX is Copyright 1995 by Ethan Brodsky.  All rights reserved.       }
program MixTest;
  uses
    CRT,
    Detect,
    SMix;
  const
    XMSRequired   = 190;    {XMS memory required to load the sounds (KBytes) }
    SharedEMB     = true;
      {TRUE:   All sounds will be stored in a shared EMB}
      {FALSE:  Each sound will be stored in a separate EMB}
    NumSounds = 8;
  var
    BaseIO: word; IRQ, DMA, DMA16: byte;
    Sound: array[0..NumSounds-1] of PSound;
    i: byte;
    Counter: LongInt;
    InKey: char;
    Stop: boolean;
    Num: byte;
    Temp: integer;
    OldExitProc: pointer;

  function HexW(W: word): string; {Word}
    const
      HexChars: array [0..$F] of Char = '0123456789ABCDEF';
    begin
      HexW :=
        HexChars[(W and $F000) shr 12] +
        HexChars[(W and $0F00) shr 8]  +
        HexChars[(W and $00F0) shr 4]  +
        HexChars[(W and $000F)];
    end;

  procedure OurExitProc; far;
   {If the program terminates with a runtime error before the extended memory}
   {is deallocated, then the memory will still be allocated, and will be lost}
   {until the next reboot.  This exit procedure is ALWAYS called upon program}
   {termination and will deallocate extended memory if necessary.            }
    var
      i: byte;
    begin
      for i := 0 to NumSounds-1 do
        if Sound[i] <> nil then FreeSound(Sound[i]);
      if SharedEMB then ShutdownSharing;
      ExitProc := OldExitProc; {Chain to next exit procedure}
    end;

  procedure Init;
    begin
      Randomize;
      writeln;
      writeln('-------------------------------------------');
      writeln('Sound Mixing Library v1.27 by Ethan Brodsky');
      if not(GetSettings(BaseIO, IRQ, DMA, DMA16))
        then
          begin
            writeln('Error initializing:  Invalid or non-existant BLASTER environment variable');
            Halt(1); {BLASTER environment variable invalid or non-existant}
          end
        else
          begin
            if not(InitSB(BaseIO, IRQ, DMA, DMA16))
              then
                begin
                  writeln('Error initializing sound card');
                  writeln('Incorrect base IO address, sound card not installed, or broken');
                  Halt(2); {Sound card could not be initialized}
                end;
            if SixteenBit
              then writeln('BaseIO=', HexW(BaseIO), 'h    IRQ', IRQ, '    DMA8=', DMA, '    DMA16=', DMA16)
              else writeln('BaseIO=', HexW(BaseIO), 'h        IRQ', IRQ, '        DMA8=', DMA);
          end;
      write('DSP version ', DSPVersion:0:2, ':  ');
      if SixteenBit
        then write('16-bit, ')
        else write('8-bit, ');
      if AutoInit
        then writeln('Auto-initialized')
        else writeln('Single-cycle');
      if not(InitXMS)
        then
          begin
            writeln('Error initializing extended memory');
            writeln('HIMEM.SYS must be installed');
            Halt(3); {XMS driver not installed}
          end
        else
          begin
            writeln('Extended memory succesfully initialized');
            write('Free XMS memory:  ', GetFreeXMS, 'k  ');
            if GetFreeXMS < XMSRequired
              then
                begin
                  writeln('Insufficient free XMS');
                  writeln('You are probably running MIXTEST from the protected mode IDE');
                  writeln('Run it from the command line or read the documentation');
                  Halt(4); {Insufficient XMS memory}
                end
              else
                begin
                  writeln('Loading sounds');
                  if SharedEMB then InitSharing;

                                    OpenSoundResourceFile('sounds.tb1');
                  LoadSound(Sound[0], 'CLICK');
                  LoadSound(Sound[1], 'AHH');
                  LoadSound(Sound[2], 'ZOOP');
                  LoadSound(Sound[3], 'KAPOW');
                  LoadSound(Sound[4], 'CC');
                  LoadSound(Sound[5], 'BONK');
                  LoadSound(sound[6], 'OW');
                  LoadSound(sound[7], 'SCREAM');
                  CloseSoundResourceFile;

                  {OpenSoundResourceFile('MIXTEST.SND');
                  LoadSound(Sound[0], 'JET');
                  LoadSound(Sound[1], 'GUN');
                  LoadSound(Sound[2], 'CRASH');
                  LoadSound(Sound[3], 'CANNON');
                  LoadSound(Sound[4], 'LASER');
                  LoadSound(Sound[5], 'GLASS');
                  LoadSOund(Sound
                  CloseSoundResourceFile;}

                  OldExitProc := ExitProc;
                  ExitProc := @OurExitProc;
                end
          end;
      InitMixing;
      writeln;
    end;

  procedure Shutdown;
    begin
      ShutdownMixing;
      ShutdownSB5;

      for i := 0 to NumSounds-1 do
        FreeSound(Sound[i]);
      if SharedEMB then ShutdownSharing;
      writeln;
    end;

  begin
    Init;



    writeln('Press:');
    writeln('  0)  Click');
    writeln('  1)  Ahh... Mr. Bombem');
    writeln('  2)  Zhhip');
    writeln('  3)  Explosion');
    writeln('  4)  Cachoo');
    writeln('  5)  Bonk');
    writeln('  6)  Ow');
    writeln('  7)  ARRRGH!!!');
    writeln('  Q)  Quit');

    Stop := false;
    Counter := 0;

    repeat
     {Display counters}
      Inc(Counter);
      write(Counter:8, IntCount:8, VoiceCount:4);
      GotoXY(1, WhereY);


     {Maybe start a random sound}
      {if Random(64000) = 0
        then
          begin
            Num := Random(NumSounds-1)+1;
            StartSound(Sound[Num], Num, false);
          end;}

     {Start a sound if a key is pressed}
      if KeyPressed
        then
          begin
            InKey := ReadKey;
              case InKey
                of
                  '0'..'9':
                    begin
                      Val(InKey, Num, Temp);
                      if Num < NumSounds
                        then
                          StartSound(Sound[Num], Num, false);
                    end;
                  else
                    Stop := true;
                end;
          end;
    until Stop;

    writeln;



    Shutdown;
  end.
