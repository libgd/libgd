# LibGD CI

LibGD uses github actions for CI.

The platforms used are:
- Ubuntu 20.04LTS, Intel
    - https://github.com/actions/virtual-environments/blob/main/images/win/Windows2019-Readme.md
    - GCC (x64)
    - CLang (x64)
- Ubuntu 20.04LTS, ARM (graviton2)
    - AWS Ubuntu Image
    - GCC (64bit)
    - CLang (64bit)
- Windows Server 2019, x64
    - https://github.com/actions/virtual-environments/blob/main/images/win/Windows2019-Readme.md
    - Visual Studio Enterprise 2019 (x64, x86 and arm64 cross compiliations)
    - MingW GCC (x86, x64 and ucrt64)
- MacOS 11, Intel
    - https://github.com/actions/virtual-environments/blob/main/images/macos/macos-11-Readme.md
    - GCC x64
- MacOS 11, M1 in place but we don't manage to get M1 runners

The CI configurations and script can be found in

```
.gihub/workflows
```

## Where are they used

The actions work on:
- master push
- any new pull requests
- GD3 (branch surface/surface)

Please do not merge pull requests can be merged if any of the CI failed.

## ci_ubuntu

This file contains all Linux/Ubuntu configurations. The dependencies are installed using apt. Some libraries may use non official Ubuntu repositories to get the latest versions.

## ci_windows.yml

It contains all jobs and matrices for VS builds (cross compilations included).

Dependencies are installed using archived VCPKG libraries, from:

https://github.com/libgd/libgd-windows-vcpkg-prebuild

For x64, x86 and arm64.

## ci_windows.yml

## ci_macos.yml

All configs for MacOS. Dependencies are installed using homebrew (for those not available already in the image).

Important note:

Github actions MacOS images contain Mono. Mono somehow messes up with the various paths and it is not worth figuring out how to work around it. We use a script to simply remove Mono from the system, the script can be found in .github\scripts\removemono.sh

## shellcheck.yml

CI to valid shell scripts syntax, portability or other possible issues. It uses https://www.shellcheck.net/.

If a script is needed with some shell specific features which are raised as error by shellcheck, please use the ignore rules in the top of the script or at the required line(s). For an example, removemono.sh contains such rules.
