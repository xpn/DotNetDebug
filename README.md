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
* launch - Launches a new process an attaches the debugger on start

For example:

```
DotNetDebug.exe launch C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe
```

## Remember your COMPlus

If you are struggling to get things attached, remember that there are 2 COMPlus variables which can come in handy:

```
set COMPlus_ZapDisable=1
set COMPlus_JITMinOpts=1
```

Set both of these before launching DotNetDebug.exe, seems to solve some issues when injecting into a x86 process.