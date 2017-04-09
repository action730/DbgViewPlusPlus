# DbgViewPlusPlus
DbgViewPlusPlus is a utility for capturing OutputDebugStrings from Windwos applications similar to System Internals DbgView. However, DbgViewPlusPlus is a command line utility to ease automation such as scripting or Continuous Integration testing. It is also designed to capture all DbgOut or to target a specified process, eliminating the manual filtering required by DbgView. DbgViewPlusPlus writes to StdOut by default, but can easily be redirected anywhere you'd like it to go.

## Examples
#### Capture All OutputDebugStrings
```
> DbgViewPlusPlus.exe
```

#### Specify a target process by partial name
```
> DbgViewPlusPlus.exe -name generator
```

#### Specify a target process by PID
```
> DbgViewPlusPlus.exe -pid 457
```

If a target process is specified, DbgViewPlusPlus will filter for the first process that meets the criteria and ignore later matches. It will also stop capturing and exit once the target process has exited.
