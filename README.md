# Hidden Execute

Use this utility to run a program without showing a command prompt, without installing anything or messing with the registry.

## Quick instructions

- The executable looks for a INI file with the same name. So you may change
  the executable file and the INI file name at the same time to, say, run.exe
  and run.ini. This way you may have several utilities in the same folder, one
  for each application.
- The name of the data section in the INI file must be always "h_exec".
- "run" is the path and name of the executable, usually <path>\some.exe. It
  can be relative to the current path or an absolute path.
- "run" can also be the name of a non-executable file with a known association
  like readme.txt, readme.chm or faq.html.
- "param" is the name of the script to run, like, say, script.php.
- Use searchpath=1 to prepend the path of the h_exec.exe utility to the name
  of the application to run. This is generally not necessary.
- See the included INI file examples.
