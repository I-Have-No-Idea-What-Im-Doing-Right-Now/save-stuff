#include "../src/version.h" 
[Setup]
AppName=savestuff
AppVersion={#SAVESTUFF_VERSION}
AppVerName="savestuff {#SAVESTUFF_VERSION}"
DefaultDirName="{autopf}\savestuff"
DefaultGroupName="savestuff"
DisableProgramGroupPage=yes
ChangesEnvironment=yes
OutputBaseFilename="savestuff setup"

[Files]
Source: "savestuff.exe"; DestDir: "{app}"; Flags: ignoreversion

[Tasks]
Name: modifypath; Description: "Add savestuff to your system PATH environment variable"

[Code]
// 1. Define the configuration constants modpath.iss expects
const
    ModPathName = 'modifypath';
    ModPathType = 'system'; // Options are 'user' or 'system'

// 2. Define the path directory array properly
function ModPathDir(): TArrayOfString;
begin
    setArrayLength(Result, 1);
    Result[0] := ExpandConstant('{app}'); 
end;

// 3. Include the extension script safely (it triggers everything automatically)
#include "modpath.iss"
