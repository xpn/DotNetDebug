# DotNetDebug

A simple POC to show the power of .NET debugging capabilities when looking to inject code without an unmanaged stub.

## Building

It is recommended that you build this POC in x64 mode for best results. This can be done via Visual Studio or MSBuild using the following:

```
msbuild.exe DotNetDebug.sln /property:Platform=x64 
```

## Running

This tool can be run in the following modes:

* attach - Attaches to a running process by name
* attachpid - Attached to a running process via PID
* launch - Launches a new process and attaches the debugger on start

By default, the injected .NET payload will attempt to load an assembly from C:\Windows\Temp\inject.exe and execute its entrypoint.

For example:

```
DotNetDebug.exe launch C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe
```

Note: Ensure that if you are running the x64 version of the tool, you are only targeting x64 compiled .NET processes.

## Remember your COMPlus

If you are struggling to get things attached, remember that there are 2 COMPlus variables which can come in handy:

```
set COMPlus_ZapDisable=1
set COMPlus_JITMinOpts=1
```

Set both of these before launching DotNetDebug.exe, seems to solve some issues when injecting into a x86 process.